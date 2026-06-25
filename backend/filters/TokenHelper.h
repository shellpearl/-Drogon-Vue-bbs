#pragma once
#include <drogon/HttpRequest.h>
#include <string>

using namespace drogon;

class TokenHelper {
public:
    static bool validateToken(const HttpRequestPtr& req, std::string& outRole);
    
    static bool isPublicPath(const std::string& path);
};