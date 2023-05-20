#pragma once

#include <exception>
#include <string>

class CustomException : public std::exception {
    std::string info;
public:
    CustomException(const std::string info) noexcept
    try : info(info) { /* constructor */ }
    catch (...) {
        // Exception occured in constructor. Should never happen, but we want to be safe.
        this->info = "";
    };

    // Returns a C-style character string describing the general cause of the current error.
    virtual const char* what() const noexcept;
};