#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>
#include "general_iterator.h"
#include "basetrait.h"
#include "../types.h"

/*
template <typename Container>
class BinaryTreeForwardInorderIterator : public general_iterator<Container, 
                                         BinaryTreeForwardInorderIterator<Container>>{
    using MySelf = BinaryTreeForwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        // this->m_pNode = this->m_pNode->getNext();
        if (!this->m_pNode) return *this;

        if (this->m_pNode->getChild(1) != nullptr) {
            this->m_pNode = this->m_pNode->getChild(1);
            while (this->m_pNode->getChild(0) != nullptr) {
                this->m_pNode = this->m_pNode->getChild(0);
            }
        }
        else {
            auto parent = this->m_pNode->getParent();
            while (parent != nullptr && this->m_pNode == parent->getChild(1)) {
                this->m_pNode = parent;
                parent = parent->getParent();
            }
            this->m_pNode = parent;
        }
        return *this;
    }
};

template <typename Container>
class BinaryTreeBackwardInorderIterator : public general_iterator<Container, 
                                         BinaryTreeBackwardInorderIterator<Container>>{
    using MySelf = BinaryTreeBackwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        // this->m_pNode = this->m_pNode->getNext();
        if (!this->m_pNode) return *this;

        if (this->m_pNode->getChild(0) != nullptr) {
            this->m_pNode = this->m_pNode->getChild(0);
            while (this->m_pNode->getChild(1) != nullptr) {
                this->m_pNode = this->m_pNode->getChild(1);
            }
        }
        else {
            auto parent = this->m_pNode->getParent();
            while (parent != nullptr && this->m_pNode == parent->getChild(0)) {
                this->m_pNode = parent;
                parent = parent->getParent();
            }
            this->m_pNode = parent;
        }
        return *this;
    }
};
*/
template <typename Container, bool Reverse>
class BinaryTreeInorderIterator : public general_iterator<Container, 
                                         BinaryTreeInorderIterator<Container, Reverse>>{
    using MySelf = BinaryTreeInorderIterator<Container, Reverse>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if (!this->m_pNode) return *this;

        size_t left_idx = Reverse ? 1 : 0;
        size_t right_idx = Reverse ? 0 : 1;

        if (this->m_pNode->getChild(right_idx) != nullptr) {
            this->m_pNode = this->m_pNode->getChild(right_idx);
            while (this->m_pNode->getChild(left_idx) != nullptr) {
                this->m_pNode = this->m_pNode->getChild(left_idx);
            }
        }
        else {
            auto parent = this->m_pNode->getParent();

            while (parent != nullptr && this->m_pNode == parent->getChild(right_idx)) {
                this->m_pNode = parent;
                parent = parent->getParent();
            }
            this->m_pNode = parent;
        }
        return *this;
    }

};  

template <typename Container>
using BinaryTreeForwardInorderIterator = BinaryTreeInorderIterator<Container, false>;

template <typename Container>
using BinaryTreeBackwardInorderIterator = BinaryTreeInorderIterator<Container, true>;


template <typename T>
class BinaryTreeNode{
public:
    using value_type = T;
    using Node       = BinaryTreeNode<T>;
    using NodePtr    = Node*;
protected:
    value_type m_data;
    Ref        m_ref;
    NodePtr    m_pChild[2] = {nullptr, nullptr};
    NodePtr    m_pParent;   
    //NodePtr    m_right; // Ya estamos usando m_pChild[2]
public:
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr, NodePtr parent = nullptr)
        : m_data(data), m_ref(ref), m_pParent(parent)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;
    }
    // copy constructor ... tiene error
    /*
    BinaryTreeNode(const BinaryTreeNode& other)
        : m_data(other.m_data), m_ref(other.m_ref)
    {
        m_pChild[0] = other.m_pChild[0];
        m_pChild[1] = other.m_pChild[1];
    }
    // Corregir con exchange
    BinaryTreeNode(BinaryTreeNode&& other) noexcept
        : m_data(std::move(other.m_data)), m_ref(std::move(other.m_ref))
    {
        m_pChild[0] = other.m_pChild[0];
        other.m_pChild[0] = nullptr;

        m_pChild[1] = other.m_pChild[1];
        other.m_pChild[1] = nullptr;
    }
    */
    ~BinaryTreeNode() {
        delete m_pChild[0];
        delete m_pChild[1];
    };

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }

    NodePtr         getChild(size_t pos) const { return m_pChild[pos]; }
    NodePtr&        getChildRef(size_t pos)    { return m_pChild[pos]; }
    void            setChild(size_t pos, NodePtr pChild) { m_pChild[pos] = pChild; }
    NodePtr         getParent() const { return m_pParent; }
    void            setParent(NodePtr pParent) { m_pParent = pParent; }



    string to_string() const {
        stringstream ss;
        ss << "Node(data: " << m_data << ", ref: " << m_ref << ")";
        return ss.str();
    }
    // Cuidado: en el disco hay posiciones dentro del archivo,
    //          en memoria hay punteros
    friend ostream& operator<<(ostream& os, 
        const BinaryTreeNode& node) {
        os << node.to_string();
        return os;
    }

    // Cuidado: en el disco hay posiciones dentro del archivo,
    //          en memoria hay punteros
    friend istream& operator>>(istream& is, 
        BinaryTreeNode& node) {
        string line;
        if (getline(is, line)) {
            stringstream ss(line);
            ss >> node.m_data >> node.m_ref;
        }
        return is;
    }
};

template <typename T>
struct BaseBinaryTreeListTrait : public BaseContainerTrait<T, BinaryTreeNode<T>>{

};

template <typename T>
struct AscendingBinaryTreeListTrait : public BaseBinaryTreeListTrait<T>,
                                      public AscendingTrait<T>
{
};

template <typename T>
struct DescendingBinaryTreeListTrait : public BaseBinaryTreeListTrait<T>,
                                       public DescendingTrait<T>
{
};

template <typename Traits>
class BinaryTree{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using NodePtr    = typename Traits::NodePtr;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutex   m_mtx;

public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor

    };
    BinaryTree(BinaryTree &&other){ // Move constructor

    };

    void insert(const value_type &value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        internal_insert(m_pRoot, nullptr, value, ref);
    }
private:
    void internal_insert(NodePtr &pNode, NodePtr pParent, const value_type &value, Ref ref){
        if( !pNode ){
            pNode = new Node(value, ref, nullptr, nullptr, pParent);
            return;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
        internal_insert(pNode->getChildRef(pos), pNode, value, ref);
    }

public:
    forward_inorder_iterator begin() {
        NodePtr p = m_pRoot;
        if (p) {
            while (p->getChild(0) != nullptr) {
                p = p->getChild(0);
            }
        }
        return forward_inorder_iterator(this, p);
    }

    forward_inorder_iterator end() {
        return forward_inorder_iterator(this, nullptr);
    }

    backward_inorder_iterator rbegin() {
        NodePtr p = m_pRoot;
        if (p) {
            while (p->getChild(1) != nullptr) {
                p = p->getChild(1);
            }
        }
        return backward_inorder_iterator(this, p);
    }

    backward_inorder_iterator rend() {
        return backward_inorder_iterator(this, nullptr);
    }
};

void DemoBinaryTree();



#endif // __BINARY_TREE_H__