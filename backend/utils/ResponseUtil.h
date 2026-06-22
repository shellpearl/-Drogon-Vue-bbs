#pragma once
#include <drogon/HttpResponse.h>
#include <json/json.h>

class ResponseUtil {
public:
    static drogon::HttpResponsePtr success(const Json::Value& data = Json::nullValue, const std::string& msg = "success");
    static drogon::HttpResponsePtr error(const std::string& msg = "error", int code = 1);
};