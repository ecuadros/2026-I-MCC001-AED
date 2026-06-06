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
class BinaryTreeForwardInorderIterator : public general_iterator<Container, 
                                         BinaryTreeForwardInorderIterator<Container>>{
    using MySelf = BinaryTreeForwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        if(node->getChild(R) != nullptr){                           // node tiene un hijo a su derecha
            node = node->getChild(R);
            while(node->getChild(L) != nullptr){                    // node tiene un hijo a su izquierda
                node = node->getChild(L);
            }
            this->m_pNode = node;
            return *this;
        }

        auto* parent = this->m_pNode->getParent();
        while(parent != nullptr && parent->getChild(R) == node){    // node es el hijo derecho de su padre
            node = parent;
            parent =  node->getParent();
        }

        this->m_pNode = parent;
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
    MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        if(node->getChild(L) != nullptr){                           // node tiene un hijo a su izquierda
            node = node->getChild(L);
            while(node->getChild(R) != nullptr){                    // node tiene un hijo a su derecha
                node = node->getChild(R);
            }
            this->m_pNode = node;
            return *this;
        }

        auto* parent = this->m_pNode->getParent();
        while(parent != nullptr && parent->getChild(L) == node){    // node es el hijo izquierdo de su padre
            node = parent;
            parent =  node->getParent();
        }

        this->m_pNode = parent;
        return *this;
    }
};

template <typename Container>
class BinaryTreeForwardPreorderIterator : public general_iterator<Container, 
                                         BinaryTreeForwardPreorderIterator<Container>>{
    using MySelf = BinaryTreeForwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        if(node->getChild(L) != nullptr){                           // node tiene un hijo a su izquierda
            this->m_pNode = node->getChild(L);
            return *this;
        }
        if(node->getChild(R) != nullptr){                           // node tiene un hijo a su derecha
            this->m_pNode = node->getChild(R);
            return *this;
        }

        auto* parent = this->m_pNode->getParent();
        while(parent){
            if(parent->getChild(L) == node && parent->getChild(R) != nullptr){    // node es el hijo izquierdo y tiene un hermano a su derecha
                this->m_pNode = parent->getChild(R);
                return *this;
            }
            node = parent;
            parent =  node->getParent();
        }

        this->m_pNode = nullptr;
        return *this;
    }
};

template <typename Container>
class BinaryTreeBackwardPreorderIterator : public general_iterator<Container, 
                                         BinaryTreeBackwardPreorderIterator<Container>>{
    using MySelf = BinaryTreeBackwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        auto* parent = this->m_pNode->getParent();

        if(parent == nullptr){
            this->m_pNode = nullptr;
            return *this;
        }

        if(parent->getChild(R) == node && parent->getChild(L) != nullptr){    // node es el hijo derecho y tiene un hermano a su izquierda
            node = parent->getChild(L);

            while(true)
            {
                if(node->getChild(R))
                    node = node->getChild(R);
                else if(node->getChild(L))
                    node = node->getChild(L);
                else
                    break;
            }

            this->m_pNode = node;
            return *this;
        }

        this->m_pNode = parent;
        return *this;
    }
};

template <typename Container>
class BinaryTreeForwardPostorderIterator : public general_iterator<Container, 
                                         BinaryTreeForwardPostorderIterator<Container>>{
    using MySelf = BinaryTreeForwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        auto* parent = this->m_pNode->getParent();

        if(parent == nullptr){
            this->m_pNode = nullptr;
            return *this;
        }

        if(parent->getChild(L) == node && parent->getChild(R) != nullptr){    // node es el hijo izquierdo y tiene un hermano a su derecha
            node = parent->getChild(R);

            while(true)
            {
                if(node->getChild(L))
                    node = node->getChild(L);
                else if(node->getChild(R))
                    node = node->getChild(R);
                else
                    break;
            }

            this->m_pNode = node;
            return *this;
        }

        this->m_pNode = parent;
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

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator = BinaryTreeForwardPostorderIterator<MySelf>;
protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;

    virtual NodePtr createNode(const value_type &value, const Ref ref){
        return new Node(value, ref);
    }
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor

    };
    BinaryTree(BinaryTree &&other){ // Move constructor

    };

    void insert(const value_type &value, Ref ref){
        internal_insert(m_pRoot, value, ref, nullptr);
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args){
        ::ForEach(inorder_begin(), inorder_end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_inorder_iterator FirstThat(Func func, Args&&... args){
        return ::FirstThat(inorder_begin(), inorder_end(), func, std::forward<Args>(args)...);
    }

    void clear(){
        delete m_pRoot;   // destructor del nodo borra todo el subárbol
        m_pRoot = nullptr;
    }

    forward_inorder_iterator inorder_begin() {
        NodePtr p = m_pRoot;
        while(p->getChild(L) != nullptr){
            p = p->getChild(L);
        }
        return forward_inorder_iterator(this, p);
    }

    forward_inorder_iterator inorder_end(){
        return forward_inorder_iterator(this, nullptr);
    }

    backward_inorder_iterator reverse_inorder_begin() {
        NodePtr p = m_pRoot;
        while(p!= nullptr && p->getChild(R) != nullptr){
            p = p->getChild(R);
        }
        return backward_inorder_iterator(this, p);
    }

    backward_inorder_iterator reverse_inorder_end(){
        return backward_inorder_iterator(this, nullptr);   
    }

    forward_preorder_iterator preorder_begin() {
        return forward_preorder_iterator(this, m_pRoot);
    }

    forward_preorder_iterator preorder_end(){
        return forward_preorder_iterator(this, nullptr);
    }

    backward_preorder_iterator reverse_preorder_begin() {
        NodePtr p = m_pRoot;
        while(p!= nullptr && p->getChild(R) != nullptr){
            p = p->getChild(R);
        }
        return backward_preorder_iterator(this, p);
    }

    backward_preorder_iterator reverse_preorder_end(){
        return backward_preorder_iterator(this, nullptr);
    }

    forward_postorder_iterator postorder_begin() {
        NodePtr p = m_pRoot;
        while(p!= nullptr && (p->getChild(L) != nullptr || p->getChild(R) != nullptr)){
            if(p->getChild(L) != nullptr)
                p = p->getChild(L);
            else
                p = p->getChild(R);
        }
        return forward_postorder_iterator(this, p);
    }

    forward_postorder_iterator postorder_end(){
        return forward_postorder_iterator(this, nullptr);
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