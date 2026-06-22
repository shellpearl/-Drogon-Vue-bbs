#include "ResponseUtil.h"

drogon::HttpResponsePtr ResponseUtil::success(const Json::Value& data, const std::string& msg) {
    Json::Value ret;
    ret["code"] = 0;
    ret["msg"] = msg;
    if (!data.isNull())
        ret["data"] = data;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(drogon::k200OK);
    return resp;
}

drogon::HttpResponsePtr ResponseUtil::error(const std::string& msg, int code) {
    Json::Value ret;
    ret["code"] = code;
    ret["msg"] = msg;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(drogon::k400BadRequest);
    return resp;
}