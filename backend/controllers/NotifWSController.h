#pragma once
#include <drogon/WebSocketController.h>
#include <unordered_map>
#include <mutex>

using namespace drogon;

class NotifWSController
    : public WebSocketController<NotifWSController> {
public:
    WS_PATH_LIST_BEGIN
        WS_PATH_ADD("/api/notifications/ws");
    WS_PATH_LIST_END

    void handleNewConnection(const HttpRequestPtr& req,
                             const WebSocketConnectionPtr& conn) override;
    void handleNewMessage(const WebSocketConnectionPtr& conn,
                          std::string &&message,
                          const WebSocketMessageType &type) override;
    void handleConnectionClosed(const WebSocketConnectionPtr& conn) override;

    static void pushNotification(int userId, const Json::Value& notification);

private:
    static std::unordered_map<int, WebSocketConnectionPtr> connections_;
    static std::mutex connections_mutex_;

    void sendHistoryNotifications(int userId, const WebSocketConnectionPtr& conn);
};