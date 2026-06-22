#include "AuthController.h"
#include "utils/ResponseUtil.h"
#include "utils/BcryptUtils.h"
#include "utils/JwtUtils.h"

using namespace drogon;

void handleLoginResult(const orm::Result& res, const std::string& password,
                       const std::string& role,
                       const std::function<void(const HttpResponsePtr&)>& callback) {
    if (res.empty() || !BcryptUtils::verify(password, res[0]["password"].as<std::string>())) {
        callback(ResponseUtil::error("用户名或密码错误"));
        return;
    }
    int userId = res[0]["id"].as<int>();
    std::string token = JwtUtils::generateToken(userId, role);
    Json::Value data;
    data["token"] = token;
    data["role"] = role;
    data["userId"] = userId;
    callback(ResponseUtil::success(data, "登录成功"));
}

void AuthController::registerUser(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error("Invalid JSON"));
        return;
    }
    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();
    std::string name = (*json)["name"].asString();
    std::string gender = (*json)["gender"].asString();
    std::string birthday = (*json)["birthday"].asString();
    std::string major = (*json)["major"].asString();

    auto db = app().getDbClient();
    auto doInsert = [callback, db, username, name, gender, birthday, major](
        const std::string& hashedPassword
    ) {
        db->execSqlAsync(
            "INSERT INTO student (username, password, name, gender, birthday, major) "
            "VALUES (?, ?, ?, ?, ?, ?)",
            [callback](const orm::Result&) {
                callback(ResponseUtil::success(Json::nullValue, "注册成功"));
            },
            [callback](const orm::DrogonDbException& e) {
                callback(ResponseUtil::error("数据库错误"));
            },
            username, hashedPassword, name, gender, birthday, major
        );
    };
    db->execSqlAsync(
        "SELECT id FROM student WHERE username = ?",
        [callback, password, doInsert](const drogon::orm::Result& res) {
            if (!res.empty()) {
                callback(ResponseUtil::error("用户名已存在"));
                return;
            }
            std::string hashed = BcryptUtils::hash(password);
            doInsert(hashed);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            callback(ResponseUtil::error("数据库错误"));
        },
        username   // 查询参数
    );
}

void AuthController::login(const HttpRequestPtr& req,
                           std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error("Invalid JSON"));
        return;
    }
    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();
    std::string role = (*json).get("role", "student").asString();

    if (role != "admin" && role != "student") {
        callback(ResponseUtil::error("Invalid role"));
        return;
    }

    auto db = app().getDbClient();

    if (role == "admin") {
        db->execSqlAsync(
            "SELECT id, password FROM admin WHERE admin_name = ?",
            [callback, password, role](const orm::Result& res) {
                handleLoginResult(res, password, role, callback);
            },
            [callback](const orm::DrogonDbException& e) {
                callback(ResponseUtil::error("数据库错误"));
            },
            username
        );
    } else {
        db->execSqlAsync(
            "SELECT id, password FROM student WHERE account = ?",
            [callback, password, role](const orm::Result& res) {
                handleLoginResult(res, password, role, callback);
            },
            [callback](const orm::DrogonDbException& e) {
                callback(ResponseUtil::error("数据库错误"));
            },
            username
        );
    }
}