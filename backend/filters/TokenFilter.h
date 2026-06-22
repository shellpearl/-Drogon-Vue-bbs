#pragma once
#include <drogon/HttpFilter.h>

using namespace drogon;

class TokenFilter : public HttpFilter<TokenFilter> {
public:
    void doFilter(const HttpRequestPtr& req,
                  FilterCallback&& fcb,
                  FilterChainCallback&& fccb) override;
};