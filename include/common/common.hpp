#pragma once
#include <string>

class Common {
public:
    Common();
    ~Common();

    static void trim(std::string& s);
    static std::string encrypt(const std::string& s);
    static std::string decrypt(const std::string& s);
private:

};