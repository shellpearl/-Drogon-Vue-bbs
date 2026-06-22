#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class FollowController : public HttpController<FollowController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(FollowController::toggleFollow, "/api/follow/toggle", Post, "TokenFilter");
        ADD_METHOD_TO(FollowController::getFollowing, "/api/follow/following", Get, "TokenFilter");
        ADD_METHOD_TO(FollowController::getFollowers, "/api/follow/followers", Get, "TokenFilter");
        ADD_METHOD_TO(FollowController::getCounts, "/api/follow/count/{userId}", Get, "TokenFilter");
        ADD_METHOD_TO(FollowController::checkFollow, "/api/follow/check/{userId}", Get, "TokenFilter");
    METHOD_LIST_END

    void toggleFollow(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void getFollowing(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void getFollowers(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void getCounts(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback,
                   int userId);
    void checkFollow(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     int userId);
};