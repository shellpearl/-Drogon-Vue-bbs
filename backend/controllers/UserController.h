#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class UserController : public HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::getInfo, "/api/user/info", Get, "TokenFilter");
        ADD_METHOD_TO(UserController::updateInfo, "/api/user/update", Post, "TokenFilter");
        ADD_METHOD_TO(UserController::changePassword, "/api/user/password", Post, "TokenFilter");
        ADD_METHOD_TO(UserController::uploadAvatar, "/api/user/avatar", Post, "TokenFilter");
        ADD_METHOD_TO(UserController::getMyPosts, "/api/user/posts", Get, "TokenFilter");
        ADD_METHOD_TO(UserController::getPublicProfile, "/api/user/public/{userId}", Get);
    METHOD_LIST_END

    void getInfo(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback);
    void updateInfo(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
    void changePassword(const HttpRequestPtr& req,
                        std::function<void(const HttpResponsePtr&)>&& callback);
    void uploadAvatar(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void getMyPosts(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback);
    void getPublicProfile(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback,
                      int userId);
};