#include "LikeController.h"
#include "utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

void LikeController::toggleLike(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("post_id")) {
        callback(ResponseUtil::error("缺少post_id"));
        return;
    }
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    int postId = (*json)["post_id"].asInt();

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id FROM post_like WHERE user_id=? AND post_id=?",
        [callback, userId, postId, db](const orm::Result &result) {
            if (result.empty()) {
                db->execSqlAsync(
                    "INSERT INTO post_like (user_id, post_id) VALUES (?, ?)",
                    [callback, userId, postId](const orm::Result &) {
                        auto db2 = app().getDbClient();
                        db2->execSqlAsync(
                            "SELECT author_id, author_type FROM post WHERE id=?",
                            [callback, userId, postId](const orm::Result &r) {
                                if (r.empty()) {
                                    callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                    return;
                                }
                                int authorId = r[0]["author_id"].as<int>();
                                std::string authorType = r[0]["author_type"].as<std::string>();
                                if (authorId == userId) {
                                    callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                    return;
                                }
                                auto db3 = app().getDbClient();
                                db3->execSqlAsync(
                                    "SELECT name FROM student WHERE id=?",
                                    [callback, authorId, userId, postId, authorType](const orm::Result &r2) {
                                        std::string senderName = r2[0]["name"].as<std::string>();
                                        std::string content = senderName + " 点赞了您的帖子";
                                        auto db4 = app().getDbClient();
                                        db4->execSqlAsync(
                                            "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                            "VALUES (?, ?, ?, ?, ?, 'like', ?)",
                                            [callback, authorId, userId, postId, content, authorType](const orm::Result &notifResult) {
                                                Json::Value notif;
                                                notif["id"] = notifResult.insertId();
                                                notif["sender_id"] = userId;
                                                notif["sender_type"] = "student";
                                                notif["post_id"] = postId;
                                                notif["reply_id"] = 0;
                                                notif["type"] = "like";
                                                notif["content"] = content;
                                                notif["is_read"] = false;
                                                notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                                callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                            },
                                            [callback](const orm::DrogonDbException &e) {
                                                callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                            },
                                            authorId, authorType, userId, "student", postId, content);
                                    },
                                    [callback, authorId, userId, postId, authorType](const orm::DrogonDbException &e) {
                                        std::string content = "有人点赞了您的帖子";
                                        auto db4 = app().getDbClient();
                                        db4->execSqlAsync(
                                            "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                            "VALUES (?, ?, ?, ?, ?, 'like', ?)",
                                            [callback, authorId, userId, postId, content](const orm::Result &notifResult) {
                                                Json::Value notif;
                                                notif["id"] = notifResult.insertId();
                                                notif["sender_id"] = userId;
                                                notif["sender_type"] = "student";
                                                notif["post_id"] = postId;
                                                notif["reply_id"] = 0;
                                                notif["type"] = "like";
                                                notif["content"] = content;
                                                notif["is_read"] = false;
                                                notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                                callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                            },
                                            [callback](const orm::DrogonDbException &e) {
                                                callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                                            },
                                            authorId, authorType, userId, "student", postId, content);
                                    },
                                    userId);
                            },
                            [callback](const orm::DrogonDbException &e) {
                                callback(ResponseUtil::success(Json::nullValue, "点赞成功"));
                            },
                            postId);
                    },
                    [callback](const orm::DrogonDbException &e) {
                        callback(ResponseUtil::error("点赞失败"));
                    },
                    userId, postId);
            } else {
                db->execSqlAsync(
                    "DELETE FROM post_like WHERE user_id=? AND post_id=?",
                    [callback](const orm::Result &) {
                        callback(ResponseUtil::success(Json::nullValue, "取消点赞成功"));
                    },
                    [callback](const orm::DrogonDbException &e) {
                        callback(ResponseUtil::error("取消点赞失败"));
                    },
                    userId, postId);
            }
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        userId, postId);
}

void LikeController::getLikeCount(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback,
                                  int postId) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT COUNT(*) as count FROM post_like WHERE post_id=?",
        [callback](const orm::Result &result) {
            Json::Value data;
            data["count"] = result[0]["count"].as<int>();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "获取点赞数失败: " << e.base().what();
            callback(ResponseUtil::error("获取点赞数失败"));
        },
        postId);
}

void LikeController::checkLike(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback,
                               int postId) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id FROM post_like WHERE user_id=? AND post_id=?",
        [callback](const orm::Result &result) {
            Json::Value data;
            data["is_liked"] = !result.empty();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "检查点赞状态失败: " << e.base().what();
            callback(ResponseUtil::error("检查失败"));
        },
        userId, postId);
}

void LikeController::getMyLikes(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT p.id, p.title, p.post_time, b.board_name "
        "FROM post_like l JOIN post p ON l.post_id = p.id "
        "JOIN board b ON p.board_id = b.id "
        "WHERE l.user_id = ? AND p.is_deleted = 0 "
        "ORDER BY l.created_at DESC",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value post;
                post["id"] = row["id"].as<int>();
                post["title"] = row["title"].as<std::string>();
                post["post_time"] = row["post_time"].as<std::string>();
                post["board_name"] = row["board_name"].as<std::string>();
                data.append(post);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "获取我的点赞失败: " << e.base().what();
            callback(ResponseUtil::error("获取失败"));
        },
        userId);
}