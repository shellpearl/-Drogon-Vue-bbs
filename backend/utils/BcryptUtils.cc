#include "BcryptUtils.h"
#include "../third_party/bcrypt/bcrypt.h"  // 直接包含

std::string BcryptUtils::hash(const std::string& password) {
    return bcrypt::generateHash(password);
}

bool BcryptUtils::verify(const std::string& password, const std::string& hash) {
    return bcrypt::validatePassword(password, hash);
}