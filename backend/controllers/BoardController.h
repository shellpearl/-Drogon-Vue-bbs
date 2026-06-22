#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class BoardController : public HttpController<BoardController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(BoardController::listBoards, "/api/board/list", Get);
    METHOD_LIST_END

    static void listBoards(const HttpRequestPtr& req,
                           std::function<void(const HttpResponsePtr&)>&& callback);
};