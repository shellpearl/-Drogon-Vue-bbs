#include "ReplyController.h"
#include "utils/ResponseUtil.h"

void ReplyController::createReply(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("postId") || !json->isMember("content")) {
        callback(ResponseUtil::error("请提供postId和content"));
        return;
    }
    int postId = (*json)["postId"].asInt();
    std::string content = (*json)["content"].asString();
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT author_id, author_type, title FROM post WHERE id=?",
        [callback, db, postId, userId, role, content](const orm::Result &result) {
            if (result.empty()) {
                callback(ResponseUtil::error("帖子不存在"));
                return;
            }
            auto authorId = result[0]["author_id"].as<int>();
            auto authorType = result[0]["author_type"].as<std::string>();
            auto postTitle = result[0]["title"].as<std::string>();

            db->execSqlAsync(
                "INSERT INTO reply (post_id, student_id, content) VALUES (?, ?, ?)",
                [callback, db, postId, userId, role, authorId, authorType, postTitle, content](const orm::Result &res) {
                    int replyId = res.insertId();
                    if (!(userId == authorId && role == authorType)) {
                        std::string senderName;
                        if (role == "student") {
                            db->execSqlAsync(
                                "SELECT name FROM student WHERE id=?",
                                [callback, db, authorId, authorType, postId, replyId, postTitle, userId, role](const orm::Result &r) {
                                    std::string senderName = r[0]["name"].as<std::string>();
                                    std::string notifContent = senderName + " 回复了你的帖子《" + postTitle + "》";
                                    db->execSqlAsync(
                                        "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, reply_id, type, content) "
                                        "VALUES (?, ?, ?, ?, ?, ?, 'reply', ?)",
                                        [callback, authorId, userId, role, postId, replyId, notifContent](const orm::Result &notifResult) {
                                            Json::Value notif;
                                            notif["id"] = notifResult.insertId();
                                            notif["sender_id"] = userId;
                                            notif["sender_type"] = role;
                                            notif["post_id"] = postId;
                                            notif["reply_id"] = replyId;
                                            notif["type"] = "reply";
                                            notif["content"] = notifContent;
                                            notif["is_read"] = false;
                                            notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            LOG_ERROR << "插入通知失败: " << e.base().what();
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        authorId, authorType, userId, role, postId, replyId, notifContent);
                                },
                                [callback, db, postId, replyId, authorId, authorType, postTitle, userId, role](const orm::DrogonDbException &e) {
                                    std::string notifContent = "有人回复了你的帖子《" + postTitle + "》";
                                    db->execSqlAsync(
                                        "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, reply_id, type, content) "
                                        "VALUES (?, ?, ?, ?, ?, ?, 'reply', ?)",
                                        [callback, authorId, userId, role, postId, replyId, notifContent](const orm::Result &notifResult) {
                                            Json::Value notif;
                                            notif["id"] = notifResult.insertId();
                                            notif["sender_id"] = userId;
                                            notif["sender_type"] = role;
                                            notif["post_id"] = postId;
                                            notif["reply_id"] = replyId;
                                            notif["type"] = "reply";
                                            notif["content"] = notifContent;
                                            notif["is_read"] = false;
                                            notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        authorId, authorType, userId, role, postId, replyId, notifContent);
                                },
                                userId);
                        } else {
                            db->execSqlAsync(
                                "SELECT admin_name FROM admin WHERE id=?",
                                [callback, db, authorId, authorType, postId, replyId, postTitle, userId, role](const orm::Result &r) {
                                    std::string senderName = r[0]["admin_name"].as<std::string>();
                                    std::string notifContent = senderName + " 回复了你的帖子《" + postTitle + "》";
                                    db->execSqlAsync(
                                        "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, reply_id, type, content) "
                                        "VALUES (?, ?, ?, ?, ?, ?, 'reply', ?)",
                                        [callback, authorId, userId, role, postId, replyId, notifContent](const orm::Result &notifResult) {
                                            Json::Value notif;
                                            notif["id"] = notifResult.insertId();
                                            notif["sender_id"] = userId;
                                            notif["sender_type"] = role;
                                            notif["post_id"] = postId;
                                            notif["reply_id"] = replyId;
                                            notif["type"] = "reply";
                                            notif["content"] = notifContent;
                                            notif["is_read"] = false;
                                            notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        authorId, authorType, userId, role, postId, replyId, notifContent);
                                },
                                [callback, db, authorId, authorType, postId, replyId, postTitle, userId, role](const orm::DrogonDbException &e) {
                                    std::string notifContent = "管理员回复了你的帖子《" + postTitle + "》";
                                    db->execSqlAsync(
                                        "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, reply_id, type, content) "
                                        "VALUES (?, ?, ?, ?, ?, ?, 'reply', ?)",
                                        [callback, authorId, userId, role, postId, replyId, notifContent](const orm::Result &notifResult) {
                                            Json::Value notif;
                                            notif["id"] = notifResult.insertId();
                                            notif["sender_id"] = userId;
                                            notif["sender_type"] = role;
                                            notif["post_id"] = postId;
                                            notif["reply_id"] = replyId;
                                            notif["type"] = "reply";
                                            notif["content"] = notifContent;
                                            notif["is_read"] = false;
                                            notif["created_at"] = trantor::Date::now().toFormattedString("%Y-%m-%d %H:%M:%S");
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        [callback](const orm::DrogonDbException &e) {
                                            callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                                        },
                                        authorId, authorType, userId, role, postId, replyId, notifContent);
                                },
                                userId);
                        }
                    } else {
                        callback(ResponseUtil::success(Json::nullValue, "回复成功"));
                    }
                },
                [callback](const orm::DrogonDbException &e) {
                    LOG_ERROR << "插入回复失败: " << e.base().what();
                    callback(ResponseUtil::error("回复失败"));
                },
                postId, userId, content);
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "查询帖子失败: " << e.base().what();
            callback(ResponseUtil::error("帖子不存在"));
        },
        postId);
}

void ReplyController::deleteReply(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback,
                                  int replyId) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    if (role == "admin") {
        db->execSqlAsync("DELETE FROM reply WHERE id=?",
            [callback](const orm::Result &) { callback(ResponseUtil::success(Json::nullValue, "删除成功")); },
            [callback](const orm::DrogonDbException &e) { callback(ResponseUtil::error("删除失败")); },
            replyId);
    } else {
        db->execSqlAsync("DELETE FROM reply WHERE id=? AND student_id=?",
            [callback](const orm::Result &result) {
                if (result.affectedRows() == 0)
                    callback(ResponseUtil::error("无权删除此回复或回复不存在"));
                else
                    callback(ResponseUtil::success(Json::nullValue, "删除成功"));
            },
            [callback](const orm::DrogonDbException &e) { callback(ResponseUtil::error("删除失败")); },
            replyId, userId);
    }
}

void ReplyController::getMyReplies(const HttpRequestPtr& req,
                                   std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT r.id, r.content, r.reply_time, p.title as post_title, p.id as post_id "
        "FROM reply r JOIN post p ON r.post_id = p.id "
        "WHERE r.student_id = ? AND p.is_deleted = 0 "
        "ORDER BY r.reply_time DESC",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value reply;
                reply["id"] = row["id"].as<int>();
                reply["content"] = row["content"].as<std::string>();
                reply["reply_time"] = row["reply_time"].as<std::string>();
                reply["post_title"] = row["post_title"].as<std::string>();
                reply["post_id"] = row["post_id"].as<int>();
                data.append(reply);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取回复失败"));
        },
        userId);
}