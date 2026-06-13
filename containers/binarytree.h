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

#define L 0
#define R 1

template <typename Container>
class BinaryTreeIterator : public general_iterator<Container, BinaryTreeIterator<Container>> {
public:
    using MySelf = BinaryTreeIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
    using NodePtr = Node*;
    
    // Tipo para el puntero a método de navegación simétrica del nodo
    using AdvanceMethod = NodePtr (Node::*)(size_t);

    // Heredamos los constructores base de general_iterator
    using Parent::Parent;

private:
    AdvanceMethod m_advance = nullptr;
    size_t        m_side    = 0;

public:
    // Constructor completo para inicializar la estrategia de recorrido
    BinaryTreeIterator(Container* pContainer, Node* pNode, AdvanceMethod advance, size_t side)
        : Parent(pContainer, pNode), m_advance(advance), m_side(side) {}

    // Operador++ requerido por el patrón CRTP de general_iterator
    MySelf& operator++() {
        if (this->m_pNode != nullptr && m_advance != nullptr) {
            // Llama al método simétrico del nodo actual pasando la dirección
            this->m_pNode = (this->m_pNode->*m_advance)(m_side);
        }
        return *this;
    }
};

template <typename Traits>
class BinaryTree{
public:
    class BinaryTreeNode;
    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;
    using Node       = BinaryTreeNode;
    using NodePtr    = Node*;
    using MySelf     = BinaryTree<Traits>;

    using Iterator = BinaryTreeIterator<MySelf>;
protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutable mutex    m_mtx;

    virtual NodePtr createNode(const value_type &value, const Ref ref){
        return new Node(value, ref);
    }
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor
        scoped_lock<mutex> lock(other.m_mtx);
        if(other.m_pRoot != nullptr)
            m_pRoot = new Node(*other.m_pRoot);
    };
    BinaryTree(BinaryTree &&other){ // Move constructor
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
    };

    ~BinaryTree() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;   // destructor del nodo borra todo el subárbol
        m_pRoot = nullptr;
    }

    void insert(const value_type &value, Ref ref){
        internal_insert(m_pRoot, value, ref, nullptr);
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args){
        ::ForEach(inorder_begin(), inorder_end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    Iterator FirstThat(Func func, Args&&... args){
        return ::FirstThat(inorder_begin(), inorder_end(), func, std::forward<Args>(args)...);
    }

    void clear(){
        delete m_pRoot;   // destructor del nodo borra todo el subárbol
        m_pRoot = nullptr;
    }

    Iterator inorder_begin() {
        NodePtr p = m_pRoot;
        if (p == nullptr) return inorder_end();
        while(p->getChild(L) != nullptr) p = p->getChild(L);
        return Iterator(this, p, &Node::nextInorder, R);
    }
    Iterator inorder_end() {
        return Iterator(this, nullptr, nullptr, R);
    }

    Iterator reverse_inorder_begin() {
        NodePtr p = m_pRoot;
        if (p == nullptr) return reverse_inorder_end();
        while(p->getChild(R) != nullptr) p = p->getChild(R);
        return Iterator(this, p, &Node::nextInorder, L);
    }
    Iterator reverse_inorder_end() {
        return Iterator(this, nullptr, nullptr, L);
    }

    Iterator preorder_begin() {
        if (m_pRoot == nullptr) return preorder_end();
        return Iterator(this, m_pRoot, &Node::nextPreorder, L);
    }
    Iterator preorder_end() {
        return Iterator(this, nullptr, nullptr, L);
    }

    Iterator reverse_preorder_begin() {
        NodePtr p = m_pRoot;
        if (p == nullptr) return reverse_preorder_end();
        while(p->getChild(R) != nullptr) p = p->getChild(R);
        return Iterator(this, p, &Node::nextPreorder, R);
    }
    Iterator reverse_preorder_end() {
        return Iterator(this, nullptr, nullptr, R);
    }

    Iterator postorder_begin() {
        NodePtr p = m_pRoot;
        if (p == nullptr) return postorder_end();
        while(p != nullptr && (p->getChild(L) != nullptr || p->getChild(R) != nullptr)){
            if(p->getChild(L) != nullptr) p = p->getChild(L);
            else                          p = p->getChild(R);
        }
        return Iterator(this, p, &Node::nextPostorder, L);
    }
    Iterator postorder_end() {
        return Iterator(this, nullptr, nullptr, L);
    }

    Iterator reverse_postorder_begin() {
        if (m_pRoot == nullptr) return reverse_postorder_end();
        return Iterator(this, m_pRoot, &Node::nextPostorder, R);
    }
    Iterator reverse_postorder_end() {
        return Iterator(this, nullptr, nullptr, R);
    }

protected:
    virtual NodePtr internal_insert(NodePtr &pNode, const value_type &value, Ref ref, NodePtr pParent){
        if( !pNode ){
            pNode = createNode(value, ref);
            pNode->setParent(pParent);
            return pNode;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
        pNode->setChild(pos, internal_insert(pNode->getChildRef(pos), value, ref, pNode));
        return pNode;
    }
};

template <typename Traits>
class BinaryTree<Traits>::BinaryTreeNode{
public:
    using value_type = typename Traits::value_type;
    using Node       = BinaryTreeNode;
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
    {
        scoped_lock<mutex> lock(other.m_mtx);
        m_data = move(other.m_data);
        m_ref = move(other.m_ref);
        m_pParent = move(other.m_pParent);

        m_pChild[0] = exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = exchange(other.m_pChild[1], nullptr);
    }

    // NOTA: Sin lock. Si otro hilo espera el mutex mientras este destructor
    // destruye la clase, despertará en memoria vacía causando un crasheo (Use-After-Free).
    ~BinaryTreeNode() {
        scoped_lock<mutex> lock(m_mtx);
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
    NodePtr         getParent()          const { return m_pParent; }
    void            setParent(NodePtr pParent) { m_pParent = pParent; };

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

    NodePtr nextInorder(size_t side) {
        size_t opposite = 1 - side;
        if (m_pChild[side] != nullptr) {
            NodePtr p = m_pChild[side];
            while (p->m_pChild[opposite] != nullptr) {
                p = p->m_pChild[opposite];
            }
            return p;
        }
        NodePtr p = this;
        NodePtr parent = m_pParent;
        while (parent != nullptr && parent->m_pChild[side] == p) {
            p = parent;
            parent = parent->m_pParent;
        }
        return parent;
    }

    NodePtr nextPreorder(size_t side) {
        size_t opposite = 1 - side;
        if (m_pChild[side] != nullptr) return m_pChild[side];
        if (m_pChild[opposite] != nullptr) return m_pChild[opposite];

        NodePtr node = this;
        NodePtr parent = m_pParent;
        while (parent != nullptr) {
            if (parent->m_pChild[side] == node && parent->m_pChild[opposite] != nullptr) {
                return parent->m_pChild[opposite];
            }
            node = parent;
            parent = node->m_pParent;
        }
        return nullptr;
    }

    NodePtr nextPostorder(size_t side) {
        size_t opposite = 1 - side;
        NodePtr parent = m_pParent;
        if (parent == nullptr) return nullptr;

        if (parent->m_pChild[side] == this && parent->m_pChild[opposite] != nullptr) {
            NodePtr node = parent->m_pChild[opposite];
            while (true) {
                if (node->m_pChild[side])          node = node->m_pChild[side];
                else if (node->m_pChild[opposite]) node = node->m_pChild[opposite];
                else break;
            }
            return node;
        }
        return parent;
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, BinaryTree<Traits>& tree){
    tree.ForEach([&os](typename BinaryTree<Traits>::Node &node){
        os << node << endl;
    });
    return os;
}

template <typename Traits>
istream& operator>>(istream& is, BinaryTree<Traits>& tree){
    tree.clear();

    using value_type = typename Traits::value_type;
    value_type value;
    Ref ref;

    while(is >> value >> ref){
        tree.insert(value, ref);
    }

    return is;
}


#endif // __BINARY_TREE_H__