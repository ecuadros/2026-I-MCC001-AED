#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // forward

using namespace std;

template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    Traverse(begin, end, func, std::forward<Args>(args)...);
    // for (auto it = begin; it != end; ++it)
    //     func(*it, std::forward<Args>(args)...);
    // cout<<endl;
}

// Variadic templates: template <typename ...Args>
// Variadic templates allow a function or class to accept an arbitrary
// number of arguments.
// Example: template <typename ...Args> func() { // ... }
template <typename Iterator, typename Func, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    // for (auto it = begin; it != end; ++it){
    //     if (func(*it, std::forward<Args>(args)...))
    //         return it;
    // }
    // return end;
    return Traverse(begin, end, func, std::forward<Args>(args)...);
}

template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}

//TODO: Implementar ForEach y FirstThat
template <typename Iterator, typename Func, typename... Args>
decltype(auto) Traverse(Iterator begin, Iterator end, Func func, Args &&... args){
    // Si la func retorna void (Foreach), y si retorna bool (FirstThat), entonces se puede usar decltype(auto) para deducir el tipo de retorno de la función.
    using func_return_type = invoke_result_t<Func, decltype(*begin), Args...>;

    if constexpr (is_void_v<func_return_type>) {
        for (auto it = begin; it != end; ++it)
            invoke(func, *it, std::forward<Args>(args)...);
    } else {
        for (auto it = begin; it != end; ++it) {
            if(invoke(func, *it, std::forward<Args>(args)...)) {
                return it;
            }
        }
        return end;
    }

}

#endif // __FOREACH_H__