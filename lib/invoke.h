#pragma once

#include<utility>

template <typename Func, typename Obj, typename... Args>
auto Invoke(Func&& f, Obj&& obj, Args&&... args) -> decltype((std::declval<Obj>().*std::declval<Func>())(std::declval<Args>()...)) {
    return (std::forward<Obj>(obj).*std::forward<Func>(f))(std::forward<Args>(args)...);
}

template <typename Func, typename... Args>
auto Invoke(Func&& f, Args&&... args) -> decltype(std::declval<Func>()(std::declval<Args>()...)) {
    return std::forward<Func>(f)(std::forward<Args>(args)...);
}

template <typename Func, typename Obj>
auto Invoke(Func&& f, Obj&& obj) -> decltype(std::declval<Obj>().*std::declval<Func>()()) {
    return (std::forward<Obj>(obj).*std::forward<Func>(f))();
}
