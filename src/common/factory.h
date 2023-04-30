#pragma once

#include <memory>

template<class T, class... Args> std::unique_ptr<T> factory_create_object(Args... args);
