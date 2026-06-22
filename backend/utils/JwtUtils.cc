#include "JwtUtils.h"
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/open-source-parsers-jsoncpp/defaults.h>
#include <json/json.h>
#include <chrono>
#include <trantor/utils/Logger.h>

const std::string JwtUtils::SECRET = "your-secret-key";

std::string JwtUtils::generateToken(int id, const std::string& role) {
    return jwt::create()
        .set_type("JWT")
        .set_issuer("bbs_server")
        .set_payload_claim("id", jwt::claim(id))
        .set_payload_claim("role", jwt::claim(role))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
        .sign(jwt::algorithm::hs256{SECRET});
}

bool JwtUtils::verifyToken(const std::string& token, int& id, std::string& role) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{SECRET})
            .with_issuer("bbs_server");
        verifier.verify(decoded);
        id = decoded.get_payload_claim("id").as_integer();
        role = decoded.get_payload_claim("role").as_string();
        return true;
    } catch (...) {
        return false;
    }
}