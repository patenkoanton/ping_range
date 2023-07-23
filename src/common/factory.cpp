#include <iostream>
#include <string>
#include <functional>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "orchestrator.h"
#include "output_stream.h"
#include "custom_exception.h"

/***** Private declarations *****/
static CustomException factory_get_custom_exception(std::string info, const std::exception &exc);
template<class T, class... Args>
static T make_object(std::function<T(Args...)> allocate, Args... args);

/***** Public definitions *****/

// Return class <T> object wrapped in unique_ptr.
template<class T, class... Args>
std::unique_ptr<T> Factory::make_unique(Args... args)
{
    std::function<std::unique_ptr<T>(Args...)> allocate = [](Args... allocate_args) {
        return std::make_unique<T>(allocate_args...);
    };

    return make_object<std::unique_ptr<T>, Args...>(allocate, args...);
}

// Return class <T> object wrapped in shared_ptr.
template<class T, class... Args>
std::shared_ptr<T> Factory::make_shared(Args... args)
{
    std::function<std::shared_ptr<T>(Args...)> allocate = [](Args... allocate_args) {
        return std::make_shared<T>(allocate_args...);
    };

    return make_object<std::shared_ptr<T>, Args...>(allocate, args...);
}

/***** Private definitions *****/

// Template factory used by both make_unique and make_shared.
// Throws when allocation fails.
template<class T, class... Args>
static T make_object(std::function<T(Args...)> allocate, Args... args)
{
    try {
        return allocate(args...);
    } catch (std::bad_alloc &exc) {     // thrown by std::make_xxxxx
        throw factory_get_custom_exception("failed to allocate memory for application", exc);
    } catch (CustomException &exc) {
        throw factory_get_custom_exception("internal error", exc);
    } catch (std::exception &exc) {
        throw factory_get_custom_exception("unexpected error", exc);
    }
}

// Returns custom exception containing properly formatted info.
static CustomException factory_get_custom_exception(std::string info, const std::exception &exc)
{
    // Make sure info does not repeat itself.
    // Comes in use when factory is called recursively.
    std::string exc_info = exc.what();
    if (exc_info.find(info) == std::string::npos) {
        auto full_info = "ERROR: " + info + ". ";
        if (exc_info.empty() == false) {
            full_info += exc_info + ".";
        }
        return CustomException(full_info);
    }

    return CustomException(exc_info);
}