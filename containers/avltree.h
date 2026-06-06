#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__
#include "binarytree.h"
#include "../types.h"

template <typename T>
class AVLNode : public BinaryTreeNode<T> {
public:
    using value_type = T;
    using Parent     = BinaryTreeNode<T>;
    using Node       = AVLNode<T>;
    using NodePtr    = Node*;

protected:
    TI m_height = 1; // Altura del nodo, se inicializa en 1 para nodos hoja

private:
    static NodePtr cloneSubtree(NodePtr pNode, NodePtr pParent = nullptr) {
        if (pNode == nullptr)
            return nullptr;

        NodePtr pCopy = new Node(pNode->getData(), pNode->getRef());
        pCopy->m_height = pNode->m_height; // Copia la altura del nodo original
        pCopy->setParent(pParent);

        pCopy->setLeftChild(cloneSubtree(static_cast<NodePtr>(pNode->getLeftChild()), pCopy));
        pCopy->setRightChild(cloneSubtree(static_cast<NodePtr>(pNode->getRightChild()), pCopy));

        return pCopy;
    }

public:
    // Constructor
    AVLNode(const value_type& data, const Ref& ref,
        NodePtr left = nullptr, NodePtr right = nullptr)
        : Parent(data, ref, left, right) {}

    // Copy Constructor
    AVLNode(const AVLNode& other)
        : Parent(other.getData(), other.getRef()), m_height(other.m_height)
    {
        this->m_pChild[0] = cloneSubtree(static_cast<NodePtr>(other.getLeftChild()), this);
        this->m_pChild[1] = cloneSubtree(static_cast<NodePtr>(other.getRightChild()), this);
    }

    // Move Constructor
    AVLNode(AVLNode&& other) noexcept
        : Parent(std::move(other)),
          m_height(std::exchange(other.m_height, 1)) {}

    TI getHeight() const { return m_height; }
    void setHeight(TI height) { m_height = height; }
};

template <typename T>
struct BaseAVLTreeListTrait : public BaseContainerTrait<T, AVLNode<T>> {
};

template <typename T>
struct AscendingAVLTreeListTrait : public BaseAVLTreeListTrait<T>,
                                   public AscendingTrait<T> {
};

template <typename T>
struct DescendingAVLTreeListTrait : public BaseAVLTreeListTrait<T>,
                                    public DescendingTrait<T> {
};

template <typename Traits>
class AVLTree : public BinaryTree<Traits> {
public:
    using Parent     = BinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using NodePtr    = Node*;
    using Comp       = typename Traits::Comp;

    using Parent::begin;
    using Parent::end;
    using Parent::rbegin;
    using Parent::rend;
    using Parent::preorder_begin;
    using Parent::preorder_end;
    using Parent::preorder_rbegin;
    using Parent::preorder_rend;
    using Parent::postorder_begin;
    using Parent::postorder_end;
    using Parent::postorder_rbegin;
    using Parent::postorder_rend;
    using Parent::toString;
    using Parent::ForEach;
    using Parent::FirstThat;

    AVLTree() = default; // El constructor por defecto es suficiente, no necesitamos hacer nada especial para inicializar el árbol AVL
    AVLTree(const AVLTree&) = default; // El constructor de copia por defecto es suficiente, ya que el BinaryTree ya maneja la copia profunda de los nodos
    AVLTree(AVLTree&&) noexcept = default; // El constructor de movimiento por defecto es suficiente, ya que el BinaryTree ya maneja la transferencia de recursos correctamente

    void insert(const value_type& value, Ref ref) override {
        scoped_lock lock(this->m_mutex);
        NodePtr pInserted = static_cast<NodePtr>(this->internal_insert(this->m_pRoot, nullptr, value, ref));
        rebalanceUpwards(pInserted);
    }

private:
    static NodePtr left(NodePtr pNode) { // Helper para obtener el hijo izquierdo de un nodo
        return pNode == nullptr ? nullptr : static_cast<NodePtr>(pNode->getLeftChild());
    }

    static NodePtr right(NodePtr pNode) {
        return pNode == nullptr ? nullptr : static_cast<NodePtr>(pNode->getRightChild());
    }

    static NodePtr parent(NodePtr pNode) { 
        return pNode == nullptr ? nullptr : static_cast<NodePtr>(pNode->getParent());
    }

    TI height(NodePtr pNode) const {
        return pNode == nullptr ? 0 : pNode->getHeight();
    }

    void updateHeight(NodePtr pNode) { // Actualiza la altura de un nodo basándose en la altura de sus hijos
        if (pNode == nullptr)
            return;

        pNode->setHeight(1 + std::max(height(left(pNode)), height(right(pNode))));
    }

    TI balanceFactor(NodePtr pNode) const { // Calcula el factor de balance de un nodo, que es la diferencia entre la altura del subárbol izquierdo y la altura del subárbol derecho
        return pNode == nullptr ? 0 : height(left(pNode)) - height(right(pNode));
    }

    NodePtr rotateLeft(NodePtr pNode) { // Realiza una rotación a la izquierda en el nodo dado y devuelve el nuevo nodo raíz del subárbol rotado
        NodePtr pNewRoot = right(pNode);
        NodePtr pTransfer = left(pNewRoot);
        NodePtr pParent = parent(pNode);

        if (pParent == nullptr) {
            this->m_pRoot = pNewRoot;
            pNewRoot->setParent(nullptr);
        } else if (pParent->getLeftChild() == pNode) {
            pParent->setLeftChild(pNewRoot);
        } else {
            pParent->setRightChild(pNewRoot);
        }

        pNode->setRightChild(pTransfer);
        pNewRoot->setLeftChild(pNode);

        updateHeight(pNode);
        updateHeight(pNewRoot);

        return pNewRoot;
    }

    NodePtr rotateRight(NodePtr pNode) {
        NodePtr pNewRoot = left(pNode);
        NodePtr pTransfer = right(pNewRoot);
        NodePtr pParent = parent(pNode);

        if (pParent == nullptr) {
            this->m_pRoot = pNewRoot;
            pNewRoot->setParent(nullptr);
        } else if (pParent->getLeftChild() == pNode) {
            pParent->setLeftChild(pNewRoot);
        } else {
            pParent->setRightChild(pNewRoot);
        }

        pNode->setLeftChild(pTransfer);
        pNewRoot->setRightChild(pNode);

        updateHeight(pNode);
        updateHeight(pNewRoot);

        return pNewRoot;
    }

    void rebalanceUpwards(NodePtr pNode) {
        while (pNode != nullptr) {
            updateHeight(pNode);
            TI balance = balanceFactor(pNode);

            if (balance > 1) {
                if (balanceFactor(left(pNode)) < 0)
                    rotateLeft(left(pNode));
                pNode = rotateRight(pNode);
            } else if (balance < -1) {
                if (balanceFactor(right(pNode)) > 0)
                    rotateRight(right(pNode));
                pNode = rotateLeft(pNode);
            }

            pNode = parent(pNode);
        }
    }
};

#endif // __AVL_TREE_H__
