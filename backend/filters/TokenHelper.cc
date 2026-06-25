#include "TokenHelper.h"
#include "utils/JwtUtils.h"

bool TokenHelper::validateToken(const HttpRequestPtr& req, std::string& outRole) {
    auto auth = req->getHeader("Authorization");
    if (auth.size() > 7 && auth.substr(0, 7) == "Bearer ") {
        std::string token = auth.substr(7);
        int id;
        std::string role;
        if (JwtUtils::verifyToken(token, id, role)) {
            req->attributes()->insert("userId", std::to_string(id));
            req->attributes()->insert("userRole", role);
            outRole = role;
            return true;
        }
    }
    outRole.clear();
    return false;
}

bool TokenHelper::isPublicPath(const std::string& path) {
    return (path.find("/api/post/") == 0 ||
            path.find("/api/board/") == 0 ||
            path == "/api/user/profile" ||
            path.find("/api/user/public/") == 0 ||
            path.find("/api/auth/") == 0);
}