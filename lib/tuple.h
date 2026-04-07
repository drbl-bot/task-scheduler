#pragma once

#include "remove_cv_ref.h"
#include <cstddef>


template <typename ...Args>
class Tuple;

template <typename Head, typename ...Tail>
class Tuple<Head, Tail...> {
public:
    template<typename FwHeadType, typename... FwTailType>
    Tuple(FwHeadType&& head, FwTailType&&... tails)
    : head_(std::forward<FwHeadType>(head)), tail_{std::forward<FwTailType>(tails)...} {  }

private:
    template <size_t Ind, typename ...Args>
    friend decltype(auto) Get(Tuple<Args...>& tuple_val);

    template <size_t Ind, typename ...Args>
    friend decltype(auto) Get(Tuple<Args...>&& tuple_val);

    Head head_;
    Tuple<Tail...> tail_;
};

template<typename FwHeadType, typename... FwTailType>
Tuple(FwHeadType&& head, FwTailType&&... tails)
  -> Tuple<RemoveCVRefT<FwHeadType>, RemoveCVRefT<FwTailType>...>;


template <typename Head>
class Tuple<Head> {
public:
    template<typename FwHeadType>
    Tuple(FwHeadType&& head)
    : head_(std::forward<FwHeadType>(head)) {  }

private:
    template <size_t Ind, typename ...Args>
    friend decltype(auto) Get(Tuple<Args...>& tuple_val);

    template <size_t Ind, typename ...Args>
    friend decltype(auto) Get(Tuple<Args...>&& tuple_val);

    template <size_t Ind, typename ...Args>
    friend decltype(auto) Get(const Tuple<Args...>& tuple_val) ;

    Head head_;
};

template<typename FwHeadType>
Tuple(FwHeadType&& head)
  -> Tuple<RemoveCVRefT<FwHeadType>>;


template<>
class Tuple<>;

template <size_t Ind, typename ...Args>
decltype(auto) Get(Tuple<Args...>& tuple_val) {
    if constexpr (Ind == 0) 
        return tuple_val.head_;
    else 
        return Get<Ind - 1>(tuple_val.tail_);
}

template <size_t Ind, typename ...Args>
decltype(auto) Get(Tuple<Args...>&& tuple_val) {
    if constexpr (Ind == 0) 
        return tuple_val.head_;
    else 
        return Get<Ind - 1>(tuple_val.tail_);
}

template <size_t Ind, typename ...Args>
decltype(auto) Get(const Tuple<Args...>& tuple_val) {
    if constexpr (Ind == 0) 
        return tuple_val.head_;
    else 
        return Get<Ind - 1>(tuple_val.tail_);
}

namespace detail {

    template<typename ...Args>
    struct Size;
    
    template <typename ...Args>
    struct Size<Tuple<Args...>> { 
        static constexpr auto value = sizeof...(Args);
    };

}

template <typename T>
inline static constexpr auto TupleSize = detail::Size<RemoveCVRefT<T>>::value;
