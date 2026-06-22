#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class AdminController : public HttpController<AdminController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AdminController::createBoard, "/api/admin/board/create", Post, "TokenFilter", "AdminFilter");
        ADD_METHOD_TO(AdminController::deleteBoard, "/api/admin/board/{boardId}", Delete, "TokenFilter", "AdminFilter");
        ADD_METHOD_TO(AdminController::getAllPosts, "/api/admin/posts", Get, "TokenFilter", "AdminFilter");
        ADD_METHOD_TO(AdminController::deleteAnyPost, "/api/admin/post/{postId}", Delete, "TokenFilter", "AdminFilter");
        ADD_METHOD_TO(AdminController::createAdminPost, "/api/admin/post/create", Post, "TokenFilter", "AdminFilter");
        ADD_METHOD_TO(AdminController::getContactInfo, "/api/admin/contact", Get);
    METHOD_LIST_END

    void createBoard(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    void deleteBoard(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     int boardId);
    void getAllPosts(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    void deleteAnyPost(const HttpRequestPtr& req,
                       std::function<void(const HttpResponsePtr&)>&& callback,
                       int postId);
    void createAdminPost(const HttpRequestPtr& req,
                         std::function<void(const HttpResponsePtr&)>&& callback);
    void getContactInfo(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};