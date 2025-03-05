#ifndef P2P_SECURITY_HPP
#define P2P_SECURITY_HPP

#include <string>
#include <random>

namespace rathena {
namespace p2p {

class Security {
public:
    static std::string generate_auth_token() {
        const std::string charset = "0123456789"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz";
        const size_t token_length = 32;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, charset.length() - 1);
        
        std::string token;
        token.reserve(token_length);
        
        for (size_t i = 0; i < token_length; ++i) {
            token += charset[dis(gen)];
        }
        
        return token;
    }
    
    static bool verify_token(const std::string& token) {
        // Basic validation for now
        return token.length() == 32;
    }
};

} // namespace p2p
} // namespace rathena

#endif // P2P_SECURITY_HPP
