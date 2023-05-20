#include "custom_exception.h"

const char* CustomException::what() const noexcept
{
    try {
        return this->info.c_str();
    } catch (...) {     // ensure nothing is thrown
        return (const char *)"";
    }
}