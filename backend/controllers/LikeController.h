#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class LikeController : public HttpController<LikeController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(LikeController::toggleLike, "/api/like/toggle", Post, "TokenFilter");
        ADD_METHOD_TO(LikeController::getLikeCount, "/api/like/count/{postId}", Get);
        ADD_METHOD_TO(LikeController::checkLike, "/api/like/check/{postId}", Get, "TokenFilter");
        ADD_METHOD_TO(LikeController::getMyLikes, "/api/like/my", Get, "TokenFilter");
    METHOD_LIST_END

    void toggleLike(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
    void getLikeCount(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback,
                      int postId);
    void checkLike(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback,
                   int postId);
    void getMyLikes(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};