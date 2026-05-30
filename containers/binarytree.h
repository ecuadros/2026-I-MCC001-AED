#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <utility>  // exchange
#include <mutex>    // mutex
#include "general_iterator.h"
#include "../types.h"

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
        return *this;
    }
};

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
    NodePtr    m_pParent = nullptr;
private:
    mutex m_mtx;
public:
    // NOTA: Sin lock. El objeto apenas está naciendo en este hilo y nadie más conoce
    // su dirección de memoria todavía; es imposible que ocurra una condición de carrera.
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr)
        : m_data(data), m_ref(ref)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;

        if (left != nullptr)
            left->m_pParent = this;
        if (right != nullptr)
            right->m_pParent = this;
    }
    
    BinaryTreeNode(const BinaryTreeNode& other)
    {
        scoped_lock<mutex> lock(other.m_mtx);
        m_data = other.m_data;
        m_ref  = other.m_ref;
        m_pParent = other.m_pParent;

        for(size_t i = 0; i < 2; ++i){
            if(other.m_pChild[i] != nullptr){
                m_pChild[i] = new Node(*other.m_pChild[i]);
                m_pChild[i]->m_pParent = this;
            }else{
                m_pChild[i] = nullptr;
            }
        }
    }
    
    // NOTA: Sin lock. Si otro hilo espera el mutex, despertará cuando el movimiento
    // termine y leerá un objeto ya vacío (nullptr), causando fallos lógicos o crasheos.
    BinaryTreeNode(BinaryTreeNode&& other) noexcept
        : m_data(std::move(other.m_data)), m_ref(std::move(other.m_ref))
    {
        m_pChild[0] = exchange(other.m_pChild[0], nullptr);

        m_pChild[1] = exchange(other.m_pChild[1], nullptr);
    }

    // NOTA: Sin lock. Si otro hilo espera el mutex mientras este destructor
    // destruye la clase, despertará en memoria vacía causando un crasheo (Use-After-Free).
    ~BinaryTreeNode() {
        // Desconectar el padre del nodo
        if(m_pParent != nullptr){
            if(m_pParent->m_pChild[0] == this)
                m_pParent->m_pChild[0] = nullptr;
            else if(m_pParent->m_pChild[1] == this)
                m_pParent->m_pChild[1] = nullptr;
        }

        // Desconectar los hijos del nodo
        if (m_pChild[0] != nullptr) m_pChild[0]->m_pParent = nullptr;
        if (m_pChild[1] != nullptr) m_pChild[1]->m_pParent = nullptr;
        
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
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr;
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor

    };
    BinaryTree(BinaryTree &&other){ // Move constructor

    };

    void insert(const value_type &value, Ref ref){
        internal_insert(m_pRoot, value, ref);
    }
private:
    void internal_insert(NodePtr &pNode, const value_type &value, Ref ref){
        if( !pNode ){
            pNode = new Node(value, ref);
            return;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
        internal_insert(pNode->m_pChild[pos], value, ref);
    }
};




#endif // __BINARY_TREE_H__