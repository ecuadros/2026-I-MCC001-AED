#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // forward
#include <type_traits> // invoke_result_t, is_void_v
#include <functional> // invoke

using namespace std;

template <typename Iterator, typename Func, typename... Args>
decltype(auto) Traverse(Iterator begin, Iterator end, Func func, Args &&... args){
    using func_ret_type = invoke_result_t<Func, decltype(*begin), Args...>;

    if constexpr(is_void_v<func_ret_type>){
        for (auto it = begin; it != end; ++it)
            invoke(func, *it, forward<Args>(args)...);
        return;
    } else{
        for (auto it = begin; it != end; ++it){
            if (invoke(func, *it, forward<Args>(args)...))
                return it;
        }
        return end;
    }
}

template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    Traverse(begin, end, func, forward<Args>(args)...);
}

// Variadic templates: template <typename ...Args>
// Variadic templates allow a function or class to accept an arbitrary
// number of arguments.
// Example: template <typename ...Args> func() { // ... }
template <typename Iterator, typename Func, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    return Traverse(begin, end, func, forward<Args>(args)...);
}

template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}

#endif // __FOREACH_H__