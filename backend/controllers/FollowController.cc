#include "FollowController.h"
#include "utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

void FollowController::toggleFollow(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("followee_id") || !json->isMember("followee_type")) {
        callback(ResponseUtil::error("参数不足"));
        return;
    }
    int followerId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    int followeeId = (*json)["followee_id"].asInt();
    std::string followeeType = (*json)["followee_type"].asString();

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id FROM user_follows WHERE follower_id=? AND followee_id=? AND followee_type=?",
        [callback, followerId, followeeId, followeeType, db](const orm::Result &result) {
            if (result.empty()) {
                db->execSqlAsync(
                    "INSERT INTO user_follows (follower_id, followee_id, followee_type) VALUES (?, ?, ?)",
                    [callback](const orm::Result &) {
                        callback(ResponseUtil::success(Json::nullValue, "关注成功"));
                    },
                    [callback](const orm::DrogonDbException &e) {
                        callback(ResponseUtil::error("关注失败"));
                    },
                    followerId, followeeId, followeeType);
            } else {
                db->execSqlAsync(
                    "DELETE FROM user_follows WHERE follower_id=? AND followee_id=? AND followee_type=?",
                    [callback](const orm::Result &) {
                        callback(ResponseUtil::success(Json::nullValue, "取消关注成功"));
                    },
                    [callback](const orm::DrogonDbException &e) {
                        callback(ResponseUtil::error("取消关注失败"));
                    },
                    followerId, followeeId, followeeType);
            }
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        followerId, followeeId, followeeType);
}

void FollowController::getFollowing(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    int currentUserId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string userIdParam = req->getParameter("userId");
    int targetUserId = userIdParam.empty() ? currentUserId : std::stoi(userIdParam);

    auto db = app().getDbClient();
    std::string sql = R"(
        SELECT u.followee_id as id, u.followee_type as type,
               CASE WHEN u.followee_type='student' THEN s.name ELSE a.admin_name END as username,
               CASE WHEN u.followee_type='student' THEN s.avatar ELSE '' END as avatar,
               1 AS is_following
        FROM user_follows u
        LEFT JOIN student s ON (u.followee_type='student' AND u.followee_id=s.id)
        LEFT JOIN admin a ON (u.followee_type='admin' AND u.followee_id=a.id)
        WHERE u.follower_id = ?
    )";
    db->execSqlAsync(
        sql,
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value user;
                user["id"] = row["id"].as<int>();
                user["type"] = row["type"].as<std::string>();
                user["username"] = row["username"].as<std::string>();
                user["avatar"] = row["avatar"].as<std::string>();
                user["is_following"] = true;
                data.append(user);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取关注列表失败"));
        },
        targetUserId);
}

void FollowController::getFollowers(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    int currentUserId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string userIdParam = req->getParameter("userId");
    int targetUserId = userIdParam.empty() ? currentUserId : std::stoi(userIdParam);

    auto db = app().getDbClient();
    std::string sql = R"(
        SELECT s.id, s.name as username, s.avatar,
               EXISTS (
                   SELECT 1 FROM user_follows u2
                   WHERE u2.follower_id = ? AND u2.followee_id = s.id AND u2.followee_type = 'student'
               ) AS is_following
        FROM user_follows u
        JOIN student s ON u.follower_id = s.id
        WHERE u.followee_id = ?
    )";
    db->execSqlAsync(
        sql,
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value user;
                user["id"] = row["id"].as<int>();
                user["username"] = row["username"].as<std::string>();
                user["avatar"] = row["avatar"].as<std::string>();
                user["type"] = "student";
                user["is_following"] = row["is_following"].as<bool>();
                data.append(user);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取粉丝列表失败"));
        },
        currentUserId, targetUserId);
}

void FollowController::getCounts(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback,
                                 int userId) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT "
        "  (SELECT COUNT(*) FROM user_follows WHERE follower_id=?) AS following_count, "
        "  (SELECT COUNT(*) FROM user_follows WHERE followee_id=? AND (followee_type='student' OR followee_type='admin')) AS follower_count",
        [callback](const orm::Result &result) {
            Json::Value data;
            data["following_count"] = result[0]["following_count"].as<int>();
            data["follower_count"] = result[0]["follower_count"].as<int>();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取数量失败"));
        },
        userId, userId);
}

void FollowController::checkFollow(const HttpRequestPtr& req,
                                   std::function<void(const HttpResponsePtr&)>&& callback,
                                   int userId) {
    int currentUserId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string followeeType = req->getParameter("followee_type");
    if (followeeType.empty()) {
        callback(ResponseUtil::error("缺少 followee_type 参数"));
        return;
    }
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT id FROM user_follows WHERE follower_id=? AND followee_id=? AND followee_type=?",
        [callback](const orm::Result &result) {
            Json::Value data;
            data["is_following"] = !result.empty();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("检查失败"));
        },
        currentUserId, userId, followeeType);
}