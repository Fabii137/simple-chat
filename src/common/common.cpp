#include "common/common.hpp"

Common::Common() {

}

Common::~Common() {

}

void Common::trim(std::string &s) {
    const size_t start = s.find_first_not_of(" \t\n\r");
    const size_t end = s.find_last_not_of(" \t\n\r");

    if (start == std::string::npos) {
        s.clear();
    } else {
        s = s.substr(start, end - start + 1);
    }
}

std::string Common::encrypt(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.length(); i++) {
    //    result +=
    }
    return result;
}

std::string Common::decrypt(const std::string& s) {

}


