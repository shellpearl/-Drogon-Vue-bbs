#pragma once
#include "TokenFilter.h"

class AdminFilter : public TokenFilter {
public:
    void doFilter(const HttpRequestPtr& req,
                  FilterCallback&& fcb,
                  FilterChainCallback&& fccb) override;
};