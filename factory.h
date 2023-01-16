#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <memory>

template<class T, class... Args> std::shared_ptr<T> factory_create_object(Args... args);

#endif  // _FACTORY_H_
