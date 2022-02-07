#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <memory>

template<class T, class N> std::unique_ptr<T> factory_create_object(N arg);

#endif  // _FACTORY_H_
