#include "UserController.h"
#include "utils/ResponseUtil.h"
#include "utils/BcryptUtils.h"
#include <filesystem>

#include "utils/JwtUtils.h"

void UserController::getInfo(const HttpRequestPtr& req,
                             std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    std::string sql;
    if (role == "admin") {
        sql = "SELECT id, admin_name as account, admin_name as username, '' as name, '' as gender, '' as birthday, '' as major, '' as avatar FROM admin WHERE id = ?";
    } else {
        sql = "SELECT id, account, username, name, gender, birthday, major, avatar FROM student WHERE id = ?";
    }
    db->execSqlAsync(
        sql,
        [callback](const orm::Result &result) {
            if (result.empty()) {
                callback(ResponseUtil::error("用户不存在"));
                return;
            }
            const auto &row = result[0];
            Json::Value data;
            data["id"] = row["id"].as<int>();
            data["account"] = row["account"].as<std::string>();
            data["username"] = row["username"].as<std::string>();
            data["name"] = row["name"].as<std::string>();
            data["gender"] = row["gender"].as<std::string>();
            data["birthday"] = row["birthday"].as<std::string>();
            data["major"] = row["major"].as<std::string>();
            data["avatar"] = row["avatar"].as<std::string>();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        userId);
}

void UserController::updateInfo(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error("无效的JSON"));
        return;
    }
    std::string username = (*json)["username"].asString();
    std::string name = (*json)["name"].asString();
    std::string gender = (*json)["gender"].asString();
    std::string birthday = (*json)["birthday"].asString();
    std::string major = (*json)["major"].asString();

    auto db = app().getDbClient();
    db->execSqlAsync(
        "UPDATE student SET username=?, name=?, gender=?, birthday=?, major=? WHERE id=?",
        [callback](const orm::Result &) {
            callback(ResponseUtil::success(Json::nullValue, "更新成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        username, name, gender, birthday, major, userId);
}

void UserController::changePassword(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto json = req->getJsonObject();
    if (!json || !json->isMember("old_password") || !json->isMember("new_password")) {
        callback(ResponseUtil::error("请提供旧密码和新密码"));
        return;
    }
    std::string oldPwd = (*json)["old_password"].asString();
    std::string newPwd = (*json)["new_password"].asString();

    std::string table = (role == "admin") ? "admin" : "student";
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT password FROM " + table + " WHERE id=?",
        [callback, oldPwd, newPwd, userId, table](const orm::Result &result) {
            if (result.empty()) {
                callback(ResponseUtil::error("用户不存在"));
                return;
            }
            std::string storedHash = result[0]["password"].as<std::string>();
            if (!BcryptUtils::verify(oldPwd, storedHash)) {
                callback(ResponseUtil::error("原密码错误"));
                return;
            }
            std::string newHash = BcryptUtils::hash(newPwd);
            auto db2 = app().getDbClient();
            db2->execSqlAsync(
                "UPDATE " + table + " SET password=? WHERE id=?",
                [callback](const orm::Result &) {
                    callback(ResponseUtil::success(Json::nullValue, "密码修改成功"));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(ResponseUtil::error("数据库错误"));
                },
                newHash, userId);
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        userId);
}

void UserController::uploadAvatar(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    MultiPartParser fileParser;
    if (fileParser.parse(req) != 0) {
        callback(ResponseUtil::error("解析上传文件失败"));
        return;
    }
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto &files = fileParser.getFiles();
    if (files.empty()) {
        callback(ResponseUtil::error("请选择文件"));
        return;
    }
    auto &file = files[0];
    std::string ext(file.getFileExtension());
    if (ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "gif") {
        callback(ResponseUtil::error("只允许图片格式 (jpg/png/gif)"));
        return;
    }

    std::string avatarName = "avatar_" + std::to_string(userId) + "_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) +
                             "." + ext;
    std::string savePath = "./uploads/avatars/" + avatarName;
    std::filesystem::create_directories("./uploads/avatars");

    try {
        file.saveAs(savePath);
    } catch (const std::exception &e) {
        LOG_ERROR << "保存头像文件失败: " << e.what();
        callback(ResponseUtil::error("文件保存失败"));
        return;
    }

    std::string avatarUrl = "/static/avatars/" + avatarName;
    auto db = app().getDbClient();
    db->execSqlAsync(
        "UPDATE student SET avatar=? WHERE id=?",
        [callback, avatarUrl](const orm::Result &) {
            Json::Value data;
            data["avatar_url"] = avatarUrl;
            callback(ResponseUtil::success(data, "头像上传成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "更新数据库头像URL失败: " << e.base().what();
            callback(ResponseUtil::error("数据库更新失败"));
        },
        avatarUrl, userId);
}

void UserController::getMyPosts(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT p.id, p.title, p.content, p.post_time, b.board_name "
        "FROM post p JOIN board b ON p.board_id=b.id "
        "WHERE p.author_id=? AND p.author_type='student' AND p.is_deleted=0 "
        "ORDER BY p.post_time DESC",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value post;
                post["id"] = row["id"].as<int>();
                post["title"] = row["title"].as<std::string>();
                post["content"] = row["content"].as<std::string>();
                post["post_time"] = row["post_time"].as<std::string>();
                post["board_name"] = row["board_name"].as<std::string>();
                data.append(post);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取帖子失败"));
        },
        userId);
}

void UserController::getPublicProfile(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback,
                                      int targetUserId) {

    int currentUserId = 0;
    std::string currentRole;
    auto auth = req->getHeader("Authorization");
    if (auth.size() > 7 && auth.substr(0, 7) == "Bearer ") {
        std::string token = auth.substr(7);
        int id;
        std::string role;
        if (JwtUtils::verifyToken(token, id, role)) {
            currentUserId = id;
            currentRole = role;
            LOG_INFO << "Authenticated user: " << currentUserId << ", role: " << currentRole;
        } else {
            LOG_INFO << "Token invalid";
        }
    } else {
        LOG_INFO << "No Authorization header";
    }

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT 'student' as type, id, username, name, gender, birthday, major, avatar FROM student WHERE id=?",
        [callback, targetUserId, currentUserId, currentRole, db](const orm::Result &result) {
            if (!result.empty()) {
                const auto &row = result[0];
                Json::Value data;
                data["id"] = row["id"].as<int>();
                data["type"] = row["type"].as<std::string>();
                data["username"] = row["username"].as<std::string>();
                data["name"] = row["name"].as<std::string>();
                data["gender"] = row["gender"].as<std::string>();
                data["birthday"] = row["birthday"].as<std::string>();
                data["major"] = row["major"].as<std::string>();
                data["avatar"] = row["avatar"].as<std::string>();

                std::string targetType = row["type"].as<std::string>();

                if (currentUserId != targetUserId) {
                    bool isGuest = (currentUserId == 0);

                    auto db2 = app().getDbClient();
                    db2->execSqlAsync(
                        "INSERT INTO profile_view (viewer_id, target_user_id, target_user_type, viewed_at) "
                        "VALUES (?, ?, ?, NOW())",
                        [targetUserId, targetType, currentUserId, currentRole, isGuest](const orm::Result &) {

                            if (isGuest) {
                                std::string content = "有游客查看了您的主页";
                                auto db4 = app().getDbClient();
                                db4->execSqlAsync(
                                    "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                    "VALUES (?, ?, ?, ?, NULL, 'visit', ?)",
                                    [](const orm::Result &) {
                                    },
                                    [](const orm::DrogonDbException &e) {
                                    },
                                    targetUserId, targetType, 0, "student", content);
                            } else {
                                std::string table = (currentRole == "admin") ? "admin" : "student";
                                std::string nameField = (currentRole == "admin") ? "admin_name" : "name";
                                auto db3 = app().getDbClient();
                                db3->execSqlAsync(
                                    "SELECT " + nameField + " as name FROM " + table + " WHERE id=?",
                                    [targetUserId, targetType, currentUserId, currentRole](const orm::Result &r2) {
                                        std::string viewerName = r2[0]["name"].as<std::string>();
                                        std::string content = viewerName + " 查看了您的主页";
                                        auto db4 = app().getDbClient();
                                        db4->execSqlAsync(
                                            "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                            "VALUES (?, ?, ?, ?, NULL, 'visit', ?)",
                                            [](const orm::Result &) {
                                            },
                                            [](const orm::DrogonDbException &e) {
                                            },
                                            targetUserId, targetType, currentUserId, currentRole, content);
                                    },
                                    [](const orm::DrogonDbException &e) {
                                    },
                                    currentUserId);
                            }
                        },
                        [](const orm::DrogonDbException &e) {
                        },
                        currentUserId, targetUserId, targetType);
                } else {
                }

                db->execSqlAsync(
                    "SELECT "
                    "  (SELECT COUNT(*) FROM user_follows WHERE follower_id=?) AS following_count, "
                    "  (SELECT COUNT(*) FROM user_follows WHERE followee_id=? AND (followee_type='student' OR followee_type='admin')) AS follower_count",
                    [callback, data](const orm::Result &r) {
                        Json::Value ret = data;
                        ret["following_count"] = r[0]["following_count"].as<int>();
                        ret["follower_count"] = r[0]["follower_count"].as<int>();
                        callback(ResponseUtil::success(ret));
                    },
                    [callback](const orm::DrogonDbException &e) {
                        callback(ResponseUtil::error("获取数量失败"));
                    },
                    targetUserId, targetUserId);
                return;
            }

            db->execSqlAsync(
                "SELECT 'admin' as type, id, admin_name as username, '' as name, '' as gender, '' as birthday, '' as major, '' as avatar FROM admin WHERE id=?",
                [callback, targetUserId, currentUserId, currentRole, db](const orm::Result &r) {
                    if (r.empty()) {
                        callback(ResponseUtil::error("用户不存在"));
                        return;
                    }
                    const auto &row = r[0];
                    Json::Value data;
                    data["id"] = row["id"].as<int>();
                    data["type"] = row["type"].as<std::string>();
                    data["username"] = row["username"].as<std::string>();
                    data["name"] = "";
                    data["gender"] = "";
                    data["birthday"] = "";
                    data["major"] = "";
                    data["avatar"] = "";

                    std::string targetType = row["type"].as<std::string>();

                    if (currentUserId != targetUserId) {
                        bool isGuest = (currentUserId == 0);

                        auto db2 = app().getDbClient();
                        db2->execSqlAsync(
                            "INSERT INTO profile_view (viewer_id, target_user_id, target_user_type, viewed_at) "
                            "VALUES (?, ?, ?, NOW())",
                            [targetUserId, targetType, currentUserId, currentRole, isGuest](const orm::Result &) {

                                if (isGuest) {
                                    std::string content = "有游客查看了您的主页";
                                    auto db4 = app().getDbClient();
                                    db4->execSqlAsync(
                                        "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                        "VALUES (?, ?, ?, ?, NULL, 'visit', ?)",
                                        [](const orm::Result &) {
                                        },
                                        [](const orm::DrogonDbException &e) {
                                        },
                                        targetUserId, targetType, 0, "student", content);
                                } else {
                                    std::string table = (currentRole == "admin") ? "admin" : "student";
                                    std::string nameField = (currentRole == "admin") ? "admin_name" : "name";
                                    auto db3 = app().getDbClient();
                                    db3->execSqlAsync(
                                        "SELECT " + nameField + " as name FROM " + table + " WHERE id=?",
                                        [targetUserId, targetType, currentUserId, currentRole](const orm::Result &r2) {
                                            std::string viewerName = r2[0]["name"].as<std::string>();
                                            std::string content = viewerName + " 查看了您的主页";
                                            auto db4 = app().getDbClient();
                                            db4->execSqlAsync(
                                                "INSERT INTO notification (receiver_id, receiver_type, sender_id, sender_type, post_id, type, content) "
                                                "VALUES (?, ?, ?, ?, NULL, 'visit', ?)",
                                                [](const orm::Result &) {
                                                },
                                                [](const orm::DrogonDbException &e) {
                                                },
                                                targetUserId, targetType, currentUserId, currentRole, content);
                                        },
                                        [](const orm::DrogonDbException &e) {
                                        },
                                        currentUserId);
                                }
                            },
                            [](const orm::DrogonDbException &e) {
                            },
                            currentUserId, targetUserId, targetType);
                    } else {
                    }

                    db->execSqlAsync(
                        "SELECT "
                        "  (SELECT COUNT(*) FROM user_follows WHERE follower_id=?) AS following_count, "
                        "  (SELECT COUNT(*) FROM user_follows WHERE followee_id=? AND (followee_type='student' OR followee_type='admin')) AS follower_count",
                        [callback, data](const orm::Result &r2) {
                            Json::Value ret = data;
                            ret["following_count"] = r2[0]["following_count"].as<int>();
                            ret["follower_count"] = r2[0]["follower_count"].as<int>();
                            callback(ResponseUtil::success(ret));
                        },
                        [callback](const orm::DrogonDbException &e) {
                            callback(ResponseUtil::error("获取数量失败"));
                        },
                        targetUserId, targetUserId);
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(ResponseUtil::error("用户不存在"));
                },
                targetUserId);
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        targetUserId);
}
