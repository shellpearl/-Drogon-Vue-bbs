#include "AdminController.h"
#include "utils/ResponseUtil.h"

void AdminController::createBoard(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("board_name") || !json->isMember("description")) {
        callback(ResponseUtil::error("请提供板块名称和描述"));
        return;
    }
    std::string name = (*json)["board_name"].asString();
    std::string desc = (*json)["description"].asString();

    auto db = app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO board (board_name, description) VALUES (?, ?)",
        [callback](const orm::Result &) {
            callback(ResponseUtil::success(Json::nullValue, "板块创建成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("创建失败，可能是板块名重复"));
        },
        name, desc);
}

void AdminController::deleteBoard(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback,
                                  int boardId) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "DELETE FROM board WHERE id=?",
        [callback](const orm::Result &result) {
            if (result.affectedRows() == 0)
                callback(ResponseUtil::error("板块不存在"));
            else
                callback(ResponseUtil::success(Json::nullValue, "板块删除成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("删除失败"));
        },
        boardId);
}

void AdminController::getAllPosts(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT p.id, p.title, p.content, p.post_time, p.is_deleted, b.board_name, "
        "CASE WHEN p.author_type='student' THEN s.name ELSE a.admin_name END as author_name "
        "FROM post p "
        "LEFT JOIN student s ON (p.author_type='student' AND p.author_id=s.id) "
        "LEFT JOIN admin a ON (p.author_type='admin' AND p.author_id=a.id) "
        "JOIN board b ON p.board_id=b.id ORDER BY p.post_time DESC",
        [callback](const orm::Result &result) {
            Json::Value data(Json::arrayValue);
            for (const auto &row : result) {
                Json::Value post;
                post["id"] = row["id"].as<int>();
                post["title"] = row["title"].as<std::string>();
                post["content"] = row["content"].as<std::string>();
                post["post_time"] = row["post_time"].as<std::string>();
                post["is_deleted"] = row["is_deleted"].as<bool>();
                post["board_name"] = row["board_name"].as<std::string>();
                post["author_name"] = row["author_name"].as<std::string>();
                data.append(post);
            }
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("获取帖子失败"));
        });
}

void AdminController::deleteAnyPost(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback,
                                    int postId) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "UPDATE post SET is_deleted=1 WHERE id=?",
        [callback](const orm::Result &result) {
            if (result.affectedRows() == 0)
                callback(ResponseUtil::error("帖子不存在"));
            else
                callback(ResponseUtil::success(Json::nullValue, "帖子已删除"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("删除失败"));
        },
        postId);
}

void AdminController::createAdminPost(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("boardId") || !json->isMember("title") || !json->isMember("content")) {
        callback(ResponseUtil::error("参数不足"));
        return;
    }
    int boardId = (*json)["boardId"].asInt();
    std::string title = (*json)["title"].asString();
    std::string content = (*json)["content"].asString();
    int adminId = std::stoi(req->getAttributes()->get<std::string>("userId"));

    auto db = app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO post (board_id, author_id, author_type, title, content) VALUES (?, ?, 'admin', ?, ?)",
        [callback](const orm::Result &) {
            callback(ResponseUtil::success(Json::nullValue, "管理员帖子发布成功"));
        },
        [callback](const orm::DrogonDbException &e) {
            callback(ResponseUtil::error("发布失败"));
        },
        boardId, adminId, title, content);
}

void AdminController::getContactInfo(const HttpRequestPtr& req,
                                     std::function<void(const HttpResponsePtr&)>&& callback) {
    auto db = app().getDbClient();
    db->execSqlAsync(
        "SELECT admin_name, email FROM admin LIMIT 1", // 获取第一个管理员
        [callback](const orm::Result &result) {
            if (result.empty()) {
                // 如果没有管理员，返回默认信息（或错误）
                Json::Value data;
                data["name"] = "系统管理员";
                data["email"] = "admin@example.com";
                callback(ResponseUtil::success(data));
                return;
            }
            const auto &row = result[0];
            Json::Value data;
            data["name"] = row["admin_name"].as<std::string>();
            data["email"] = row["email"].as<std::string>();
            callback(ResponseUtil::success(data));
        },
        [callback](const orm::DrogonDbException &e) {
            LOG_ERROR << "获取管理员信息失败: " << e.base().what();
            // 返回默认信息
            Json::Value data;
            data["name"] = "系统管理员";
            data["email"] = "admin@example.com";
            callback(ResponseUtil::success(data));
        });
}