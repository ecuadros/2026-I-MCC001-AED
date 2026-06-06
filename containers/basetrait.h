#ifndef __BASE_TRAIT_H__
#define __BASE_TRAIT_H__
#include <functional> // less, greater

using namespace std;

template <typename _T>
struct AscendingTrait{
    using value_type = _T;
    using Comp = less<_T>;
};

template <typename _T>
struct DescendingTrait{
    using value_type = _T;
    using Comp = greater<_T>;
};

template <typename _Node>
struct AscendingContainerTrait: public AscendingTrait<typename _Node::value_type>{
    using Node       = _Node;
};

template <typename _Node>
struct DescendingContainerTrait: public DescendingTrait<typename _Node::value_type>{
    using Node       = _Node;
};

#endif // __BASE_TRAIT_H__