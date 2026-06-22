#pragma once
#include <drogon/HttpController.h>
using namespace drogon;

class BrowseHistoryController : public HttpController<BrowseHistoryController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(BrowseHistoryController::getHistory, "/api/browse/history", Get, "TokenFilter");
    METHOD_LIST_END

    void getHistory(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};