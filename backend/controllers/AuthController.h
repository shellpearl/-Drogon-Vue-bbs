#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", Post);
        ADD_METHOD_TO(AuthController::login, "/api/auth/login", Post);
    METHOD_LIST_END

    void static registerUser(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);
    void static login(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);
};