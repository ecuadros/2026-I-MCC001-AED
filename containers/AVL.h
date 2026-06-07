#ifndef __AVL_H__
#define __AVL_H__
#include "binarytree.h"
#include <algorithm>

template<typename T>
class AVLNode : public BinaryTreeNode<T>{
public:
    using Parent = BinaryTreeNode<T>;
    using Node = AVLNode<T>;
    using NodePtr = Node*;
    using value_type = T;
protected:
    T m_height;
public:
    AVLNode(const value_type& data, const Ref& ref, NodePtr left = nullptr, NodePtr right = nullptr) : Parent(data, ref, left, right), m_height(1) {}
    T getHeight() const{return m_height;}
    void setHeight(T h){m_height = h;}
    virtual NodePtr clone() const override{return new Node(*this);}
};
template<typename T>
struct AVLTreeTrait :
    public BaseContainerTrait<T, AVLNode<T>>,
    public AscendingTrait<T>
{
};
template<typename Traits>
class AVLTree : public BinaryTree<Traits>{
public:
    using Parent = BinaryTree<Traits>;
    using value_type = typename Parent::value_type;
    using Node = typename Parent::Node;
    using NodePtr = typename Parent::NodePtr;
public:
    virtual void insert(const value_type& value, Ref ref) override
    {
        Parent::insert(value, ref);
        NodePtr pNode = static_cast<NodePtr>(this->findNode(value));
        rebalance(pNode);
    }
protected:
    value_type height(NodePtr pNode) const{
        return pNode ? pNode->getHeight() : 0;
    }
    void updateHeight(NodePtr pNode){
        if(!pNode)
            return;
        pNode->setHeight(1 + std::max(height(static_cast<NodePtr>(pNode->getChild(0))),height(static_cast<NodePtr>(pNode->getChild(1)))));
    }
    value_type balanceFactor(NodePtr pNode) const{
        if(!pNode)
            return 0;
        return
            height(static_cast<NodePtr>(pNode->getChild(0))) - height(static_cast<NodePtr>(pNode->getChild(1)));
    }
    void rotateLeft(NodePtr x){
        NodePtr y = static_cast<NodePtr>(x->getChild(1));
        NodePtr B = static_cast<NodePtr>(y->getChild(0));
        y->m_pParent = x->m_pParent;
        if(!x->m_pParent)
            this->rootRef() = y;
        else{
            size_t pos = x->childPosition();
            x->m_pParent->setChild(pos, y);
        }
        y->setChild(0, x);
        x->setChild(1, B);
        updateHeight(x);
        updateHeight(y);
    }
    void rotateRight(NodePtr y){
        NodePtr x = static_cast<NodePtr>(y->getChild(0));
        NodePtr B = static_cast<NodePtr>(x->getChild(1));
        x->m_pParent = y->m_pParent;
        if(!y->m_pParent)
            this->rootRef() = x;
        else{
            size_t pos = y->childPosition();
            y->m_pParent->setChild(pos, x);
        }
        x->setChild(1, y);
        y->setChild(0, B);
        updateHeight(y);
        updateHeight(x);
    }
    void rebalance(NodePtr pNode){
        while(pNode){
            updateHeight(pNode);
            value_type bf = balanceFactor(pNode);
            if(bf > 1){
                NodePtr left = static_cast<NodePtr>(pNode->getChild(0));
                if(balanceFactor(left) < 0)
                    rotateLeft(left);
                rotateRight(pNode);
            }
            else if(bf < -1){
                NodePtr right = static_cast<NodePtr>(pNode->getChild(1));
                if(balanceFactor(right) > 0)
                    rotateRight(right);
                rotateLeft(pNode);
            }
            pNode = static_cast<NodePtr>(pNode->m_pParent);
        }
    }
};
#endif
