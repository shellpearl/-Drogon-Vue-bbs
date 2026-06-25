#include "AdminFilter.h"
#include "TokenHelper.h"
#include "utils/ResponseUtil.h"

void AdminFilter::doFilter(const HttpRequestPtr& req,
                           FilterCallback&& fcb,
                           FilterChainCallback&& fccb) {
    std::string role;
    bool valid = TokenHelper::validateToken(req, role);

    if (!valid) {
        auto resp = ResponseUtil::error("Unauthorized", 401);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
        return;
    }

    if (role == "admin") {
        fccb();
    } else {
        auto resp = ResponseUtil::error("Forbidden", 403);
        resp->setStatusCode(k403Forbidden);
        fcb(resp);
    }
}