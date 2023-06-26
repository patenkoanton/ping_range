#include <iostream>
#include <string>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "orchestrator.h"
#include "output_stream.h"
#include "custom_exception.h"

// Private helpers (declarations).
static CustomException factory_get_custom_exception(std::string info, const std::exception &exc);

// Return class <T> object wrapped in unique_ptr.
// Throw if allocation fails.
template<class T, class... Args> std::unique_ptr<T> Factory::make_unique(Args... args)
{
    try {
        return std::make_unique<T>(args...);
    } catch (std::bad_alloc &exc) {     // thrown by std::make_unique
        throw factory_get_custom_exception("failed to allocate memory for application", exc);
    } catch (CustomException &exc) {
        throw factory_get_custom_exception("internal error", exc);
    } catch (std::exception &exc) {
        throw factory_get_custom_exception("unexpected error", exc);
    }
}


// Return class <T> object wrapped in shared_ptr.
// Throw if allocation fails.
template<class T, class... Args> std::shared_ptr<T> Factory::make_shared(Args... args)
{
    try {
        return std::make_shared<T>(args...);
    } catch (std::bad_alloc &exc) {     // thrown by std::make_shared
        throw factory_get_custom_exception("failed to allocate memory for application", exc);
    } catch (CustomException &exc) {
        throw factory_get_custom_exception("internal error", exc);
    } catch (std::exception &exc) {
        throw factory_get_custom_exception("unexpected error", exc);
    }
}

// Private helpers (definitions).

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