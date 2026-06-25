#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class NotificationController : public HttpController<NotificationController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(NotificationController::getNotifications, "/api/notifications", Get, "TokenFilter");
        ADD_METHOD_TO(NotificationController::markAsRead, "/api/notifications/{id}/read", Put, "TokenFilter");
        ADD_METHOD_TO(NotificationController::markAllRead, "/api/notifications/read-all", Put, "TokenFilter");
        ADD_METHOD_TO(NotificationController::getUnreadCount, "/api/notifications/unread-count", Get, "TokenFilter");
        ADD_METHOD_TO(NotificationController::deleteBatch, "/api/notifications/batch", Delete, "TokenFilter");
    METHOD_LIST_END

    void getNotifications(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void markAsRead(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback,
                    int id);
    void markAllRead(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    void getUnreadCount(const HttpRequestPtr& req,
                        std::function<void(const HttpResponsePtr&)>&& callback);
    void deleteBatch(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
};