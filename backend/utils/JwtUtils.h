#pragma once
#include <string>

class JwtUtils {
public:
    static std::string generateToken(int id, const std::string& role);
    static bool verifyToken(const std::string& token, int& id, std::string& role);
private:
    static const std::string SECRET;
};