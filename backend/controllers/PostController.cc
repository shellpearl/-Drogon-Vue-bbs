#include "PostController.h"
#include "utils/ResponseUtil.h"

void PostController::createPost(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("boardId") || !json->isMember("title") || !json->isMember("content")) {
        callback(ResponseUtil::error("参数不足"));
        return;
    }
    int boardId = (*json)["boardId"].asInt();
    std::string title = (*json)["title"].asString();
    std::string content = (*json)["content"].asString();
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");

    auto db = app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO post (board_id, author_id, author_type, title, content) VALUES (?, ?, ?, ?, ?)",
        [callback](const orm::Result &) {
            callback(ResponseUtil::success(Json::nullValue, "发帖成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("发帖失败"));
        },
        boardId, userId, role, title, content);
}

void PostController::getPostsByBoard(const HttpRequestPtr& req,
                                     std::function<void(const HttpResponsePtr&)>&& callback,
                                     int boardId) {
    int currentUserId = 0;
    try {
        currentUserId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    } catch (...) {}

    auto db = app().getDbClient();
    std::string sql = R"(
        SELECT p.id, p.title, p.post_time, p.author_id, p.author_type,
        CASE WHEN p.author_type='student' THEN s.name ELSE a.admin_name END as author_name,
        CASE WHEN p.author_type='student' THEN s.avatar ELSE '' END as author_avatar,
        EXISTS (
            SELECT 1 FROM user_follows u
            WHERE u.follower_id = ? AND u.followee_id = p.author_id AND u.followee_type = p.author_type
        ) AS is_following,
        (SELECT COUNT(*) FROM post_like WHERE post_id = p.id) AS like_count,
        EXISTS (
            SELECT 1 FROM post_like WHERE user_id = ? AND post_id = p.id
        ) AS is_liked
        FROM post p
        LEFT JOIN student s ON (p.author_type='student' AND p.author_id=s.id)
        LEFT JOIN admin a ON (p.author_type='admin' AND p.author_id=a.id)
        WHERE p.board_id=? AND p.is_deleted=0
        ORDER BY p.post_time DESC
    )";
    db->execSqlAsync(
        sql,
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value post;
                post["id"] = row["id"].as<int>();
                post["title"] = row["title"].as<std::string>();
                post["post_time"] = row["post_time"].as<std::string>();
                post["author_id"] = row["author_id"].as<int>();
                post["author_type"] = row["author_type"].as<std::string>();
                post["author_name"] = row["author_name"].as<std::string>();
                post["author_avatar"] = row["author_avatar"].as<std::string>();
                post["is_following"] = row["is_following"].as<bool>();
                post["like_count"] = row["like_count"].as<int>();
                post["is_liked"] = row["is_liked"].as<bool>();
                data.append(post);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("查询失败"));
        },
        currentUserId, currentUserId, boardId);
}

void PostController::getPostDetail(const HttpRequestPtr& req,
                                   std::function<void(const HttpResponsePtr&)>&& callback,
                                   int postId) {
    int userId = 0;
    std::string role;
    try {
        userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
        role = req->getAttributes()->get<std::string>("userRole");
    } catch (...) {
    }

    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT p.*, "
        "CASE WHEN p.author_type='student' THEN s.name ELSE a.admin_name END as author_name, "
        "CASE WHEN p.author_type='student' THEN s.avatar ELSE '' END as author_avatar, "
        "(SELECT COUNT(*) FROM post_like WHERE post_id = p.id) AS like_count, "
        "EXISTS (SELECT 1 FROM post_like WHERE user_id = ? AND post_id = p.id) AS is_liked "
        "FROM post p "
        "LEFT JOIN student s ON (p.author_type='student' AND p.author_id=s.id) "
        "LEFT JOIN admin a ON (p.author_type='admin' AND p.author_id=a.id) "
        "WHERE p.id=? AND p.is_deleted=0",
        [callback, postId, userId, role](const orm::Result &result) {
            if (result.empty()) {
                callback(ResponseUtil::error("帖子不存在"));
                return;
            }
            const auto &row = result[0];
            Json::Value post;
            post["id"] = row["id"].as<int>();
            post["board_id"] = row["board_id"].as<int>();
            post["title"] = row["title"].as<std::string>();
            post["content"] = row["content"].as<std::string>();
            post["post_time"] = row["post_time"].as<std::string>();
            post["author_id"] = row["author_id"].as<int>();
            post["author_type"] = row["author_type"].as<std::string>();
            post["author_name"] = row["author_name"].as<std::string>();
            post["author_avatar"] = row["author_avatar"].as<std::string>();
            post["like_count"] = row["like_count"].as<int>();
            post["is_liked"] = row["is_liked"].as<bool>();

            if (!role.empty() && role == "student" && userId > 0) {
                auto db2 = app().getDbClient();
                db2->execSqlAsync(
                    "INSERT INTO browse_history (user_id, post_id, browse_time) VALUES (?, ?, NOW()) "
                    "ON DUPLICATE KEY UPDATE browse_time = NOW()",
                    [](const orm::Result &) {
                    },
                    [](const orm::DrogonDbException &e) {
                    },
                    userId, postId);
            } else {
            }

            auto db2 = app().getDbClient();
            db2->execSqlAsync(
                "SELECT r.id, r.content, r.reply_time, s.name as student_name, s.avatar as student_avatar "
                "FROM reply r JOIN student s ON r.student_id=s.id "
                "WHERE r.post_id=? ORDER BY r.reply_time ASC",
                [callback, post](const orm::Result &replyResult) {
                    Json::Value replies(Json::arrayValue);
                    for (const auto &replyRow : replyResult) {
                        Json::Value rep;
                        rep["id"] = replyRow["id"].as<int>();
                        rep["content"] = replyRow["content"].as<std::string>();
                        rep["reply_time"] = replyRow["reply_time"].as<std::string>();
                        rep["student_name"] = replyRow["student_name"].as<std::string>();
                        rep["student_avatar"] = replyRow["student_avatar"].as<std::string>();
                        replies.append(rep);
                    }
                    Json::Value data;
                    data["post"] = post;
                    data["replies"] = replies;
                    callback(ResponseUtil::success(data));
                },
                [callback](const orm::DrogonDbException &e) {
                    callback(ResponseUtil::error("获取回复失败"));
                },
                postId);
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("查询帖子失败"));
        },
        userId, postId);
}

void PostController::deletePost(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback,
                                int postId) {
    int userId = std::stoi(req->getAttributes()->get<std::string>("userId"));
    std::string role = req->getAttributes()->get<std::string>("userRole");
    auto db = app().getDbClient();
    std::string sql;
    if (role == "admin") {
        sql = "UPDATE post SET is_deleted=1 WHERE id=?";
        db->execSqlAsync(sql,
            [callback](const orm::Result &) { callback(ResponseUtil::success(Json::nullValue, "删除成功")); },
            [callback](const orm::DrogonDbException &e) { callback(ResponseUtil::error("删除失败")); },
            postId);
    } else {
        sql = "UPDATE post SET is_deleted=1 WHERE id=? AND author_id=? AND author_type='student'";
        db->execSqlAsync(sql,
            [callback](const orm::Result &result) {
                if (result.affectedRows() == 0)
                    callback(ResponseUtil::error("无权删除此帖子或帖子不存在"));
                else
                    callback(ResponseUtil::success(Json::nullValue, "删除成功"));
            },
            [callback](const orm::DrogonDbException &e) { callback(ResponseUtil::error("删除失败")); },
            postId, userId);
    }
}

    void PostController::getPostsByAuthor(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback,
                                      int authorId) {
        auto db = app().getDbClient();
        db->execSqlAsync(
            "SELECT id, title, post_time FROM post WHERE author_id=? AND is_deleted=0 ORDER BY post_time DESC",
            [callback](const orm::Result &result) {
                Json::Value data(Json::arrayValue);
                for (const auto &row : result) {
                    Json::Value post;
                    post["id"] = row["id"].as<int>();
                    post["title"] = row["title"].as<std::string>();
                    post["post_time"] = row["post_time"].as<std::string>();
                    data.append(post);
                }
                callback(ResponseUtil::success(data));
            },
            [callback](const orm::DrogonDbException &e) {
                callback(ResponseUtil::error("获取帖子失败"));
            },
            authorId);
    }