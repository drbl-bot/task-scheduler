#pragma once

#include <utility>


namespace detail {

    template<typename T>
    T RemoveCVRefF(T val) {}
    
    template<typename T>
    struct RemoveCVRef {
        using type = decltype(RemoveCVRefF(std::declval<T>()));
    };

}
    
template<typename T>
using RemoveCVRefT = detail::RemoveCVRef<T>::type;
