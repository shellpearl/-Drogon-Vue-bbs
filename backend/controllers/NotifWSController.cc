#include "NotifWSController.h"
#include <drogon/orm/DbClient.h>
#include "utils/JwtUtils.h"

std::unordered_map<int, WebSocketConnectionPtr>
    NotifWSController::connections_;
std::mutex NotifWSController::connections_mutex_;

void NotifWSController::handleNewConnection(
    const HttpRequestPtr& req,
    const WebSocketConnectionPtr& conn) {
    std::string token = req->getParameter("token");
    if (token.empty()) {
        conn->send("Unauthorized", WebSocketMessageType::Close);
        return;
    }
    int userId = 0;
    std::string role;
    if (!JwtUtils::verifyToken(token, userId, role)) {
        conn->send("Unauthorized", WebSocketMessageType::Close);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_[userId] = conn;
    }
    sendHistoryNotifications(userId, conn);
}

void NotifWSController::handleNewMessage(
    const WebSocketConnectionPtr& conn,
    std::string &&message,
    const WebSocketMessageType &type) {
    if (message == "ping") {
        conn->send("pong");
    } else {
        conn->send("{\"msg\":\"ok\"}");
    }
}

void NotifWSController::handleConnectionClosed(
    const WebSocketConnectionPtr& conn) {
    int userId = 0;
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        for (auto &pair : connections_) {
            if (pair.second == conn) {
                userId = pair.first;
                break;
            }
        }
        if (userId != 0) {
            connections_.erase(userId);
        }
    }
    if (userId != 0) {
    }
}

void NotifWSController::sendHistoryNotifications(
    int userId,
    const WebSocketConnectionPtr& conn) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id, sender_id, sender_type, post_id, reply_id, type, content, "
        "is_read, created_at FROM notification WHERE receiver_id=? ORDER BY created_at DESC",
        [conn](const orm::Result &result) {
            Json::Value history(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value n;
                n["id"] = row["id"].as<int>();
                n["sender_id"] = row["sender_id"].as<int>();
                n["sender_type"] = row["sender_type"].as<std::string>();
                n["post_id"] = row["post_id"].as<int>();
                n["reply_id"] = row["reply_id"].as<int>();
                n["type"] = row["type"].as<std::string>();
                n["content"] = row["content"].as<std::string>();
                n["is_read"] = row["is_read"].as<bool>();
                n["created_at"] = row["created_at"].as<std::string>();
                history.append(n);
            }
            Json::Value msg;
            msg["type"] = "history";
            msg["data"] = history;
            conn->send(msg.toStyledString());
        },
        [conn](const orm::DrogonDbException &e) {
            Json::Value msg;
            msg["type"] = "history";
            msg["data"] = Json::arrayValue;
            conn->send(msg.toStyledString());
        },
        userId);
}

void NotifWSController::pushNotification(int userId,
                                         const Json::Value& notification) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = connections_.find(userId);
    if (it != connections_.end()) {
        Json::Value msg;
        msg["type"] = "new";
        msg["data"] = notification;
        it->second->send(msg.toStyledString());
        LOG_DEBUG << "✅ New notification pushed to user " << userId;
    } else {
        LOG_DEBUG << "❌ User " << userId << " not connected, notification stored in DB only";
    }
}