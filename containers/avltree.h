#ifndef __AVLTREE_H__
#define __AVLTREE_H__
#include "binarytree.h"

template <typename Traits>
class AVLTree : public BinaryTree<Traits>{
private:
    using BaseNodePtr = typename BinaryTree<Traits>::NodePtr;
    using value_type = typename BinaryTree<Traits>::value_type;
protected:
    BaseNodePtr CreateNode(const value_type &value, const Ref ref) override {
        return new AVLTreeNode(value, ref);
    }
public:
    class AVLTreeNode : public BinaryTree<Traits>::BinaryTreeNode{
    private:
        TI height;
    public:
        AVLTreeNode(typename BinaryTree<Traits>::value_type data, Ref ref)
            : BinaryTree<Traits>::BinaryTreeNode(data, ref), height(1) {}

        TI getHeight() const { return height; }
        void setHeight(TI h) { height = h; }
    };
};

#endif // __AVLTREE_H__