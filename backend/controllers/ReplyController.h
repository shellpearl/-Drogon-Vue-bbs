#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class ReplyController : public HttpController<ReplyController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ReplyController::createReply, "/api/reply/create", Post, "TokenFilter");
        ADD_METHOD_TO(ReplyController::deleteReply, "/api/reply/{replyId}", Delete, "TokenFilter");
        ADD_METHOD_TO(ReplyController::getMyReplies, "/api/reply/my", Get, "TokenFilter");
    METHOD_LIST_END

    void createReply(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    void deleteReply(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     int replyId);
    void getMyReplies(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);
};