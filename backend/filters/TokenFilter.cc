#include "TokenFilter.h"
#include "utils/ResponseUtil.h"
#include "utils/JwtUtils.h"

void TokenFilter::doFilter(const HttpRequestPtr& req,
                           FilterCallback&& fcb,
                           FilterChainCallback&& fccb) {
    auto path = req->path();
    bool isPublic = (path.find("/api/post/") == 0 ||
                     path.find("/api/board/") == 0 ||
                     path == "/api/user/profile" ||
                     path.find("/api/auth/") == 0);

    auto auth = req->getHeader("Authorization");
    if (auth.size() > 7 && auth.substr(0, 7) == "Bearer ") {
        std::string token = auth.substr(7);
        int id;
        std::string role;
        if (JwtUtils::verifyToken(token, id, role)) {
            req->attributes()->insert("userId", std::to_string(id));
            req->attributes()->insert("userRole", role);
            fccb();
            return;
        }
        if (isPublic) {
            fccb();
            return;
        }
        auto resp = ResponseUtil::error("Unauthorized", 401);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
        return;
    }
    if (isPublic) {
        fccb();
    } else {
        auto resp = ResponseUtil::error("Unauthorized", 401);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
    }
}