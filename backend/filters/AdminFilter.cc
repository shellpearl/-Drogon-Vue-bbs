#include "AdminFilter.h"
#include "utils/ResponseUtil.h"

void AdminFilter::doFilter(const HttpRequestPtr& req,
                           FilterCallback&& fcb,
                           FilterChainCallback&& fccb) {
    // 先调用父类验证 token
    TokenFilter::doFilter(req, std::move(fcb), [this, req, fcb, fccb]() {
        std::string role = req->getAttributes()->get<std::string>("userRole");
        if (role == "admin") {
            fccb();
        } else {
            auto resp = ResponseUtil::error("Forbidden", 403);
            resp->setStatusCode(k403Forbidden);
            fcb(resp);
        }
    });
}