#include "NotificationController.h"
#include "utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>
#include <json/json.h>

void NotificationController::getNotifications(const HttpRequestPtr& req,
                                              std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id, sender_id, sender_type, post_id, reply_id, type, content, is_read, created_at "
        "FROM notification WHERE receiver_id=? AND receiver_type=? ORDER BY created_at DESC",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
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
                data.append(n);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "DB error in getNotifications: " << e.base().what();
            callback(ResponseUtil::error("数据库错误"));
        },
        userId, role);
}

void NotificationController::markAsRead(const HttpRequestPtr& req,
                                        std::function<void(const HttpResponsePtr&)>&& callback,
                                        int id) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    db->execSqlAsync(
        "UPDATE notification SET is_read=1 WHERE id=? AND receiver_id=? AND receiver_type=?",
        [callback](const orm::Result &result) {
            if (result.affectedRows() == 0)
                callback(ResponseUtil::error("通知不存在或无权限"));
            else
                callback(ResponseUtil::success(Json::nullValue, "已标记为已读"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        id, userId, role);
}

void NotificationController::markAllRead(const HttpRequestPtr& req,
                                         std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    db->execSqlAsync(
        "UPDATE notification SET is_read=1 WHERE receiver_id=? AND receiver_type=? AND is_read=0",
        [callback](const orm::Result &result) {
            callback(ResponseUtil::success(Json::nullValue, "全部已读"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        userId, role);
}

void NotificationController::getUnreadCount(const HttpRequestPtr& req,
                                            std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT COUNT(*) as count FROM notification WHERE receiver_id=? AND receiver_type=? AND is_read=0",
        [callback](const orm::Result &result) {
            Json::Value data;
            data["count"] = result[0]["count"].as<int>();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        userId, role);
}

void NotificationController::deleteBatch(const HttpRequestPtr& req,
                                         std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error("无效的JSON格式"));
        return;
    }
    if (!json->isMember("ids") || !(*json)["ids"].isArray()) {
        callback(ResponseUtil::error("请提供消息ID数组"));
        return;
    }
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto ids = (*json)["ids"];
    if (ids.empty()) {
        callback(ResponseUtil::success(Json::nullValue, "没有要删除的消息"));
        return;
    }

    std::string sql = "DELETE FROM notification WHERE receiver_id = " + std::to_string(userId) +
                      " AND receiver_type = '" + role + "' AND id IN (";
    for (unsigned int i = 0; i < ids.size(); ++i) {
        if (i > 0) sql += ",";
        sql += std::to_string(ids[i].asInt());
    }
    sql += ")";

    auto db = app().getDbClient();
    db->execSqlAsync(
        sql,
        [callback](const orm::Result &) {
            callback(ResponseUtil::success(Json::nullValue, "删除成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "批量删除消息失败: " << e.base().what();
            callback(ResponseUtil::error("删除失败"));
        }
    );
}