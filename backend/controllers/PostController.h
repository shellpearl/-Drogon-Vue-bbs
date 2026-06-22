#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class PostController : public HttpController<PostController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PostController::createPost, "/api/post/create", Post, "TokenFilter");
        ADD_METHOD_TO(PostController::getPostsByBoard, "/api/post/board/{boardId}", Get, "TokenFilter");
        ADD_METHOD_TO(PostController::getPostDetail, "/api/post/{postId}", Get, "TokenFilter");
        ADD_METHOD_TO(PostController::deletePost, "/api/post/{postId}", Delete, "TokenFilter");
        ADD_METHOD_TO(PostController::getPostsByAuthor, "/api/post/author/{authorId}", Get, "TokenFilter");
    METHOD_LIST_END

    void createPost(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
    void getPostsByBoard(const HttpRequestPtr& req,
                         std::function<void(const HttpResponsePtr&)>&& callback,
                         int boardId);
    void getPostDetail(const HttpRequestPtr& req,
                       std::function<void(const HttpResponsePtr&)>&& callback,
                       int postId);
    void deletePost(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback,
                    int postId);
    void getPostsByAuthor(const HttpRequestPtr& req,
                          std::function<void(const HttpResponsePtr&)>&& callback,
                          int authorId);
};