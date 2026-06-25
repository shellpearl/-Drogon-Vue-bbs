#include "TokenFilter.h"
#include "TokenHelper.h"
#include "utils/ResponseUtil.h"

void TokenFilter::doFilter(const HttpRequestPtr& req,
                           FilterCallback&& fcb,
                           FilterChainCallback&& fccb) {
    auto path = req->path();

    std::string role;
    bool tokenValid = TokenHelper::validateToken(req, role);

    if (TokenHelper::isPublicPath(path)) {
        fccb();
        return;
    }

    if (tokenValid) {
        fccb();
    } else {
        auto resp = ResponseUtil::error("Unauthorized", 401);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
    }
}