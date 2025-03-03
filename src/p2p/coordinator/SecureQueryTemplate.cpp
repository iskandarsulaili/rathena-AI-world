#include "SecureDatabaseSync.hpp"
#include <regex>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <openssl/sha.h>

namespace rathena {
namespace p2p {

SecureQueryTemplate SecureQueryTemplate::create(const std::string& pattern) {
    SecureQueryTemplate templ;
    templ.pattern_ = pattern;
    
    // Extract parameter types
    std::regex param_regex("\\?|:\\w+");
    std::regex type_regex("int|float|string|timestamp");
    
    auto param_begin = std::sregex_iterator(pattern.begin(), pattern.end(), param_regex);
    auto param_end = std::sregex_iterator();
    
    size_t pos = 0;
    for (std::sregex_iterator i = param_begin; i != param_end; ++i) {
        // Find parameter type hint in comment
        size_t type_pos = pattern.find("/*", pos);
        if (type_pos != std::string::npos) {
            size_t type_end = pattern.find("*/", type_pos);
            if (type_end != std::string::npos) {
                std::string type_hint = pattern.substr(type_pos + 2, type_end - type_pos - 2);
                std::smatch type_match;
                if (std::regex_search(type_hint, type_match, type_regex)) {
                    templ.parameter_types_.push_back(type_match.str());
                } else {
                    templ.parameter_types_.push_back("string");  // Default type
                }
            }
        } else {
            templ.parameter_types_.push_back("string");  // Default type
        }
        pos = i->position() + i->length();
    }
    
    // Generate hash for the template
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, pattern.c_str(), pattern.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }
    templ.hash_ = ss.str();
    
    return templ;
}

bool SecureQueryTemplate::validate_parameters(
    const std::vector<std::string>& params) const {
    
    if (params.size() != parameter_types_.size()) {
        return false;
    }
    
    for (size_t i = 0; i < params.size(); i++) {
        if (!validate_parameter_type(params[i], parameter_types_[i])) {
            return false;
        }
    }
    
    return true;
}

std::string SecureQueryTemplate::build_query(
    const std::vector<std::string>& params) const {
    
    if (!validate_parameters(params)) {
        throw std::runtime_error("Invalid parameters for query template");
    }
    
    std::string query = pattern_;
    size_t param_index = 0;
    
    // Replace placeholders with sanitized parameters
    std::regex placeholder_regex("\\?|:\\w+");
    std::string result;
    std::string::const_iterator start = query.begin();
    std::sregex_iterator it(query.begin(), query.end(), placeholder_regex);
    std::sregex_iterator end;
    
    while (it != end) {
        if (param_index >= params.size()) {
            throw std::runtime_error("Parameter count mismatch");
        }
        
        // Append text before the match
        result.append(start, query.begin() + it->position());
        
        // Append sanitized parameter
        result.append(sanitize_parameter(params[param_index],
                                      parameter_types_[param_index]));
        
        // Update iterators
        start = query.begin() + it->position() + it->length();
        param_index++;
        ++it;
    }
    
    // Append remaining text
    result.append(start, query.end());
    return result;
}

bool SecureQueryTemplate::validate_parameter_type(
    const std::string& value, const std::string& type) const {
    
    try {
        if (type == "int") {
            // Validate integer
            std::regex int_regex("^-?\\d+$");
            return std::regex_match(value, int_regex);
            
        } else if (type == "float") {
            // Validate float
            std::regex float_regex("^-?\\d*\\.?\\d+$");
            return std::regex_match(value, float_regex);
            
        } else if (type == "timestamp") {
            // Validate timestamp format (YYYY-MM-DD HH:MM:SS)
            std::regex timestamp_regex(
                "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");
            return std::regex_match(value, timestamp_regex);
            
        } else if (type == "string") {
            // String can be anything, but check for maximum length
            return value.length() <= 65535;  // MySQL text field limit
        }
        
    } catch (const std::regex_error& e) {
        // Log regex error
        std::cerr << "Regex error: " << e.what() << std::endl;
        return false;
    }
    
    return false;
}

std::string SecureQueryTemplate::sanitize_parameter(
    const std::string& value, const std::string& type) const {
    
    if (type == "int" || type == "float") {
        return value;  // Numeric values don't need escaping
        
    } else if (type == "timestamp") {
        return "'" + value + "'";  // Wrap timestamp in quotes
        
    } else {
        // Escape string value
        std::string escaped;
        escaped.reserve(value.length() * 2);
        
        for (char c : value) {
            switch (c) {
                case '\'': escaped += "''"; break;    // Escape single quotes
                case '\\': escaped += "\\\\"; break;  // Escape backslashes
                case '\0': escaped += "\\0"; break;   // Escape null bytes
                default: escaped += c;
            }
        }
        
        return "'" + escaped + "'";
    }
}

} // namespace p2p
} // namespace rathena