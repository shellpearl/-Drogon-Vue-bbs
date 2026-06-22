#include "NotificationController.h"
#include "utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

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