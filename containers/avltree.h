#ifndef __AVLTREE_H__
#define __AVLTREE_H__
#include "binarytree.h"

template <typename Traits>
class AVLTree : public BinaryTree<Traits>{
private:
    using BaseNodePtr = typename BinaryTree<Traits>::NodePtr;
    using value_type = typename BinaryTree<Traits>::value_type;
protected:
    BaseNodePtr createNode(const value_type &value, const Ref ref) override {
        return new AVLTreeNode(value, ref);
    }

    TI getNodeHeight(BaseNodePtr pNode) const {
        if (pNode == nullptr)
            return 0;
        return static_cast<AVLTreeNode*>(pNode)->getHeight();
    }

    TI getBalanceFactor(BaseNodePtr pNode) const {
        if (pNode == nullptr)
            return 0;
        return getNodeHeight(pNode->getChild(L)) - getNodeHeight(pNode->getChild(R));
    }

    void updateHeight(BaseNodePtr pNode) {
        if (pNode != nullptr) {
            TI leftH = getNodeHeight(pNode->getChild(L));
            TI rightH = getNodeHeight(pNode->getChild(R));
            static_cast<AVLTreeNode*>(pNode)->setHeight(1 + max(leftH, rightH));
        }
    }

    void rotateRight(BaseNodePtr &pNode) {
        cout << "Rotación derecha en nodo: " << pNode->to_string() << endl;
        BaseNodePtr pLeftChild = pNode->getChild(L);
        BaseNodePtr pGrandChildR = pLeftChild->getChild(R);

        pLeftChild->setChild(R, pNode);
        pNode->setChild(L, pGrandChildR);

        pLeftChild->setParent(pNode->getParent());
        pNode->setParent(pLeftChild);
        if (pGrandChildR != nullptr)
            pGrandChildR->setParent(pNode);

        updateHeight(pNode);
        updateHeight(pLeftChild);

        pNode = pLeftChild;
    }

    void rotateLeft(BaseNodePtr &pNode) {
        cout << "Rotación izquierda en nodo: " << pNode->to_string() << endl;
        BaseNodePtr pRightChild = pNode->getChild(R);
        BaseNodePtr pGrandChildL = pRightChild->getChild(L);

        pRightChild->setChild(L, pNode);
        pNode->setChild(R, pGrandChildL);

        pRightChild->setParent(pNode->getParent());
        pNode->setParent(pRightChild);
        if (pGrandChildL != nullptr)
            pGrandChildL->setParent(pNode);

        updateHeight(pNode);
        updateHeight(pRightChild);

        pNode = pRightChild;
    }

    void balance(BaseNodePtr &pNode) {
        cout << "Balanceando nodo: " << pNode->to_string() << endl;
        if (pNode == nullptr)
            return;

        updateHeight(pNode);
        TI balanceFactor = getBalanceFactor(pNode);

        if (balanceFactor > 1) {
            if (getBalanceFactor(pNode->getChild(L)) < 0) {
                rotateLeft(pNode->getChildRef(L));
            }
            rotateRight(pNode);
        } else if (balanceFactor < -1) {
            if (getBalanceFactor(pNode->getChild(R)) > 0) {
                rotateRight(pNode->getChildRef(R));
            }
            rotateLeft(pNode);
        }
    }

private:
    BaseNodePtr internal_insert(BaseNodePtr &pNode, const value_type &value, Ref ref, BaseNodePtr pParent) override {
        cout << "AVL insert: " << value << endl;
        pNode = BinaryTree<Traits>::internal_insert(pNode, value, ref, pParent);
        balance(pNode);
        return pNode;
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