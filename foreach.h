#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // forward
#include <functional>
using namespace std;

template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    //for (auto it = begin; it != end; ++it)
    //    func(*it, forward<Args>(args)...);
    // cout<<endl;
    InvokeEach(begin, end, func, std::forward<Args>(args)...);
}

// Variadic templates: template <typename ...Args>
// Variadic templates allow a function or class to accept an arbitrary
// number of arguments.
// Example: template <typename ...Args> func() { // ... }
template <typename Iterator, typename Func, typename... Args>
requires (!std::is_void_v<std::invoke_result_t<Func&, decltype(*std::declval<Iterator>()), Args...>>)
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    //for (auto it = begin; it != end; ++it){
    //    if (func(*it, forward<Args>(args)...))
    //        return it;
    //}
    //return end;
    //using ReturnType = std::invoke_result_t<Func, decltype(*begin), Args...>;
    //if constexpr(std::is_void_v<ReturnType>) {
    //    throw std::runtime_error("Function returns void, cannot use FirstThat.");
    //}
    //static_assert(
    //    !std::is_void_v<ReturnType>,
    //    "FirstThat requires a callable returning bool or a bool-convertible type."
    //);
    

    return InvokeEach(begin, end, func, std::forward<Args>(args)...);
}
template <typename Iterator, typename Func, typename... Args>
auto InvokeEach(Iterator begin, Iterator end, Func func, Args &&... args){
    using ReturnType = std::invoke_result_t<Func, decltype(*std::declval<Iterator>()), Args...>;
    if constexpr (std::is_void_v<ReturnType>) {
        for (auto it = begin; it != end; ++it)
            //func(*it, std::forward<Args>(args)...); // no peude llamar llamar un putno a metodo
            std::invoke(func, *it, std::forward<Args>(args)...);
    } else {
        for (auto it = begin; it != end; ++it) {
            if (std::invoke(func, *it, std::forward<Args>(args)...)) {
                return it;
            }
            // Do something with the result if needed
        }
        return end;
    }

}

//template<typename Callable, typename... Args>
//decltype(auto) call(Callable op, Args&&... args)
//{
//  if constexpr(is_void_v<invoke_result_t<Callable, Args...>>)
//  { cout << "Function is returning: void!" << endl;
//    invoke(forward<Callable>(op), forward<Args>(args)...);
//    //...  // do something before we return
//    return;
//  }
//  else // return type is not void:
//  { auto ret = invoke(forward<Callable>(op), forward<Args>(args)...);
//    cout << "Function is returning: " << type_name<decltype(ret)>() << endl;
//    //...  // do something (with ret) before we return
//    return ret;
//  }
//}






template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}

#endif // __FOREACH_H__