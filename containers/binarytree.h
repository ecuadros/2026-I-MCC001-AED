#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>
#include "general_iterator.h"
#include "basetrait.h"
#include "../foreach.h"
#include "../types.h"

struct ForwardInorderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->inOrderSuccessor(pNode);
    }
};

struct BackwardInorderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->inOrderPredecessor(pNode);
    }
};

struct ForwardPreOrderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->preOrderSuccessor(pNode);
    }
};

struct BackwardPreOrderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->preOrderPredecessor(pNode);
    }
};

struct ForwardPostOrderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->postOrderSuccessor(pNode);
    }
};

struct BackwardPostOrderIterator {
    template <typename Container>
    static auto next(Container* pContainer, typename Container::Node* pNode) {
        return pContainer->postOrderPredecessor(pNode);
    }
};

// La clase BinaryTreeWalkIterator es un iterador genérico para recorrer un árbol binario utilizando una política de avance 
// específica (in-order, pre-order, post-order). Hereda de general_iterator y define el operador de incremento para avanzar 
// al siguiente nodo según la política (AdvancePolicy) de avance proporcionada.
template <typename Container, typename AdvancePolicy>
class BinaryTreeWalkIterator : public general_iterator<Container,
                                 BinaryTreeWalkIterator<Container, AdvancePolicy>>{
    using MySelf = BinaryTreeWalkIterator<Container, AdvancePolicy>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if (this->m_pNode != nullptr)
            this->m_pNode = AdvancePolicy::template next<Container>(this->m_pContainer, this->m_pNode);
        return *this;
    }
};

template <typename Container>
using BinaryTreeForwardInorderIterator = BinaryTreeWalkIterator<Container, ForwardInorderIterator>;

template <typename Container>
using BinaryTreeBackwardInorderIterator = BinaryTreeWalkIterator<Container, BackwardInorderIterator>;

template <typename Container>
using BinaryTreeForwardPreOrderIterator = BinaryTreeWalkIterator<Container, ForwardPreOrderIterator>;

template <typename Container>
using BinaryTreeBackwardPreOrderIterator = BinaryTreeWalkIterator<Container, BackwardPreOrderIterator>;

template <typename Container>
using BinaryTreeForwardPostOrderIterator = BinaryTreeWalkIterator<Container, ForwardPostOrderIterator>;

template <typename Container>
using BinaryTreeBackwardPostOrderIterator = BinaryTreeWalkIterator<Container, BackwardPostOrderIterator>;

template <typename Iterator>
struct IteratorRange {
    Iterator m_begin;
    Iterator m_end;

    Iterator begin() { return m_begin; }
    Iterator end() { return m_end; }
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
    NodePtr    m_pParent;   
    //NodePtr    m_right;

private:
    static NodePtr cloneSubtree(NodePtr pNode, NodePtr pParent = nullptr) { // Clona un subárbol dado un nodo raíz y establece el padre de los nodos clonados
        if (pNode == nullptr) return nullptr;

        NodePtr newNode = new Node(pNode->getData(), pNode->getRef()); // Clona el nodo actual
        newNode->setParent(pParent); // Establece el padre del nuevo nodo

        newNode->setLeftChild(cloneSubtree(pNode->getLeftChild(), newNode)); // Clona el hijo izquierdo
        newNode->setRightChild(cloneSubtree(pNode->getRightChild(), newNode)); // Clona el hijo derecho

        return newNode;
    }
public:
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr)
        : m_data(data), m_ref(ref), m_pParent(nullptr)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;

        if (m_pChild[0] != nullptr) m_pChild[0]->setParent(this);
        if (m_pChild[1] != nullptr) m_pChild[1]->setParent(this);
    }

    //Copy Constructor
    BinaryTreeNode(const BinaryTreeNode& other)
        : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr)
    {
        m_pChild[0] = cloneSubtree(other.m_pChild[0], this);
        m_pChild[1] = cloneSubtree(other.m_pChild[1], this);
    }
    
    //Move Constructor
    BinaryTreeNode(BinaryTreeNode&& other) noexcept
        : m_data(std::move(other.m_data)), m_ref(std::move(other.m_ref)), m_pParent(nullptr)
    {
        m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
        
        if(m_pChild[0] != nullptr) 
            m_pChild[0]->setParent(this);
        if(m_pChild[1] != nullptr)
            m_pChild[1]->setParent(this);

    }

    // Destructor
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
    void            setChild(size_t pos, NodePtr pChild) { 
        m_pChild[pos] = pChild; 
        if (pChild != nullptr) {
            pChild->setParent(this);
        }
    }

    NodePtr         getLeftChild() const { return m_pChild[0]; }
    NodePtr&        getLeftChildRef()    { return m_pChild[0]; }
    void            setLeftChild(NodePtr pChild) { 
        m_pChild[0] = pChild; 
        if (pChild != nullptr) {
            pChild->setParent(this);
        }
    }

    NodePtr         getRightChild() const { return m_pChild[1]; }
    NodePtr&        getRightChildRef()    { return m_pChild[1]; }
    void            setRightChild(NodePtr pChild) { 
        m_pChild[1] = pChild; 
        if (pChild != nullptr) {
            pChild->setParent(this);
        }
    }

    NodePtr         getParent() const { return m_pParent; }
    NodePtr&        getParentRef()    { return m_pParent; }
    void            setParent(NodePtr pParent) { m_pParent = pParent; }

    TS to_string() const {
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
        TS line;
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
    using NodePtr    = Node*;

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator  = BinaryTreeForwardPreOrderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreOrderIterator<MySelf>;
    using forward_postorder_iterator  = BinaryTreeForwardPostOrderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostOrderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr; // Raíz del árbol
    Comp     m_comp; // Función de comparación para mantener el orden del árbol
    mutable mutex    m_mutex; // Mutex para proteger el acceso concurrente al árbol

public:
    BinaryTree() {}

    BinaryTree(const BinaryTree &other): m_pRoot(nullptr), m_comp(other.m_comp){ // Copy constructor
        scoped_lock lock(other.m_mutex);
        if  (other.m_pRoot != nullptr) {
            m_pRoot = new Node(*other.m_pRoot);
        }
    };

    BinaryTree(BinaryTree &&other) noexcept : m_pRoot(nullptr), m_comp(std::move(other.m_comp)){ // Move constructor
        scoped_lock lock(other.m_mutex);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
    };

    //Destructor
    ~BinaryTree() {
        scoped_lock lock(m_mutex);
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    virtual void insert(const value_type &value, Ref ref){
        scoped_lock lock(m_mutex);
        internal_insert(m_pRoot, nullptr, value, ref);
    }

    TS toString() const {
        scoped_lock<mutex> lock(m_mutex);
        
        stringstream ss;
        TB first = true;
        ss << "[";
        ToStringInOrder(m_pRoot, ss, first);
        ss << "]";
        return ss.str();
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        scoped_lock<mutex> lock(m_mutex);
        ForEachInOrder(m_pRoot, func, std::forward<Args>(args)...
        );
    }

    template <typename Func, typename... Args>
    NodePtr FirstThat(Func func, Args &&...  args){
        scoped_lock lock(m_mutex);
        return FirstThatInOrder(m_pRoot, func, std::forward<Args>(args)...);
    }

    NodePtr leftmost(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;
        
        while (pNode->getLeftChild() != nullptr) {
            pNode = asNodePtr(pNode->getLeftChild());
        }
        return pNode;
    }

    NodePtr rightmost(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;
        
        while (pNode->getRightChild() != nullptr) {
            pNode = asNodePtr(pNode->getRightChild());
        }
        return pNode;
    }

    NodePtr inOrderSuccessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        // 1. Si el nodo tiene un hijo derecho, el sucesor es el nodo más a la izquierda del subárbol derecho
        if (pNode->getRightChild() != nullptr) {
            return leftmost(asNodePtr(pNode->getRightChild()));
        }

        // 2. Si no tiene hijo derecho, el sucesor es el primer ancestro para el cual el nodo es un descendiente izquierdo
        NodePtr pParent = asNodePtr(pNode->getParent());
        while (pParent != nullptr && pNode == pParent->getRightChild()) {
            pNode = pParent;
            pParent = asNodePtr(pParent->getParent());
        }
        return pParent;
    }

    NodePtr inOrderPredecessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        // 1. Si el nodo tiene un hijo izquierdo, el predecesor es el nodo más a la derecha del subárbol izquierdo
        if (pNode->getLeftChild() != nullptr) {
            return rightmost(asNodePtr(pNode->getLeftChild()));
        }

        // 2. Si no tiene hijo izquierdo, el predecesor es el primer ancestro para el cual el nodo es un descendiente derecho
        NodePtr pParent = asNodePtr(pNode->getParent());
        while (pParent != nullptr && pNode == pParent->getLeftChild()) {
            pNode = pParent;
            pParent = asNodePtr(pParent->getParent());
        }
        return pParent;
    }

    NodePtr preOrderFirst(NodePtr pNode) const {
        return pNode;
    }

    NodePtr preOrderLast(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        while (true) {
            if (pNode->getRightChild() != nullptr) {
                pNode = asNodePtr(pNode->getRightChild());
            } else if (pNode->getLeftChild() != nullptr) {
                pNode = asNodePtr(pNode->getLeftChild());
            } else {
                break;
            }
        }
        return pNode;
    }

    NodePtr preOrderSuccessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        if (pNode->getLeftChild() != nullptr) 
            return asNodePtr(pNode->getLeftChild());

        if (pNode->getRightChild() != nullptr) 
            return asNodePtr(pNode->getRightChild());

        NodePtr pParent = asNodePtr(pNode->getParent());
        while (pParent != nullptr) {
            if (pNode == pParent->getLeftChild() && pParent->getRightChild() != nullptr) {
                return asNodePtr(pParent->getRightChild());    
            }

            pNode = pParent;
            pParent = asNodePtr(pParent->getParent());
        }
        return nullptr;
    }

    NodePtr preOrderPredecessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        NodePtr pParent = asNodePtr(pNode->getParent());
        if (pParent == nullptr) return nullptr; // El nodo es la raíz, no tiene predecesor

        //Si el nodo es el hijo izquierdo de su padre, el predecesor es el padre mismo
        if (pNode == pParent->getLeftChild()) {
            return pParent;
        }

        //Si el nodo es el hijo derecho de su padre, el predecesor es el hijo izquierdo del padre (si existe), o el padre mismo si no tiene hijo izquierdo
        if (pNode == pParent->getRightChild()) {
            if (pParent->getLeftChild() != nullptr) {
                return preOrderLast(asNodePtr(pParent->getLeftChild()));
            }
            return pParent;
        }

        return nullptr;
    }

    NodePtr postOrderFirst(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        while (true) {
            if (pNode->getLeftChild() != nullptr) {
                pNode = asNodePtr(pNode->getLeftChild());
            } else if (pNode->getRightChild() != nullptr) {
                pNode = asNodePtr(pNode->getRightChild());
            } else {
                break;
            }
        }
        return pNode;
    }

    NodePtr postOrderLast(NodePtr pNode) const {
        return pNode;
    }   

    NodePtr postOrderSuccessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        NodePtr pParent = asNodePtr(pNode->getParent());
        if (pParent == nullptr) return nullptr; // El nodo es la raíz, no tiene sucesor

        // Si el nodo es el hijo derecho de su padre, el sucesor es el padre mismo
        if (pNode == pParent->getRightChild() || pParent->getRightChild() == nullptr) {
            return pParent;
        }

        // Si el nodo es el hijo izquierdo de su padre, el sucesor es el hijo derecho del padre (si existe), o el padre mismo si no tiene hijo derecho
        return postOrderFirst(asNodePtr(pParent->getRightChild()));
    }

    NodePtr postOrderPredecessor(NodePtr pNode) const {
        if (pNode == nullptr) return nullptr;

        // Si el nodo tiene un hijo derecho, el predecesor es el último nodo visitado en el subárbol derecho
        if (pNode->getRightChild() != nullptr) 
            return asNodePtr(pNode->getRightChild());

        // Si no tiene hijo derecho pero tiene un hijo izquierdo, el predecesor es el último nodo visitado en el subárbol izquierdo
        if (pNode->getLeftChild() != nullptr)
            return asNodePtr(pNode->getLeftChild());

        // Si no tiene hijos, el predecesor es el primer ancestro para el cual el nodo es un descendiente derecho
        NodePtr pParent = asNodePtr(pNode->getParent());
        while (pParent != nullptr) {
            if (pNode == pParent->getRightChild() && pParent->getLeftChild() != nullptr)
                return asNodePtr(pParent->getLeftChild());

            pNode = pParent;
            pParent = asNodePtr(pParent->getParent());
        }
        return nullptr;
    }


    forward_inorder_iterator begin() {
        return forward_inorder_iterator(this, leftmost(m_pRoot));
    }

    forward_inorder_iterator end() {
        return forward_inorder_iterator(this, nullptr);
    }

    backward_inorder_iterator rbegin() {
        return backward_inorder_iterator(this, rightmost(m_pRoot));
    }

    backward_inorder_iterator rend() {
        return backward_inorder_iterator(this, nullptr);
    }

    forward_preorder_iterator preorder_begin() {
        return forward_preorder_iterator(this, preOrderFirst(m_pRoot));
    }

    forward_preorder_iterator preorder_end() {
        return forward_preorder_iterator(this, nullptr);
    }

    backward_preorder_iterator preorder_rbegin() {
        return backward_preorder_iterator(this, preOrderLast(m_pRoot));
    }

    backward_preorder_iterator preorder_rend() {
        return backward_preorder_iterator(this, nullptr);
    }

    forward_postorder_iterator postorder_begin() {
        return forward_postorder_iterator(this, postOrderFirst(m_pRoot));
    }

    forward_postorder_iterator postorder_end() {
        return forward_postorder_iterator(this, nullptr);
    }

    backward_postorder_iterator postorder_rbegin() {
        return backward_postorder_iterator(this, postOrderLast(m_pRoot));
    }

    backward_postorder_iterator postorder_rend() {
        return backward_postorder_iterator(this, nullptr);
    }

    auto inorder() {
        return IteratorRange<forward_inorder_iterator>{begin(), end()};
    }

    auto rinorder() {
        return IteratorRange<backward_inorder_iterator>{rbegin(), rend()};
    }

    auto preorder() {
        return IteratorRange<forward_preorder_iterator>{preorder_begin(), preorder_end()};
    }

    auto rpreorder() {
        return IteratorRange<backward_preorder_iterator>{preorder_rbegin(), preorder_rend()};
    }

    auto postorder() {
        return IteratorRange<forward_postorder_iterator>{postorder_begin(), postorder_end()};
    }

    auto rpostorder() {
        return IteratorRange<backward_postorder_iterator>{postorder_rbegin(), postorder_rend()};
    }

    template <typename TTraits>
    friend ostream& operator<<(ostream& os, const BinaryTree<TTraits>& tree);

    template <typename TTraits>
    friend istream& operator>>(istream& is, BinaryTree<TTraits>& tree);

protected:
    NodePtr internal_insert(NodePtr &pNode, NodePtr pParent, const value_type &value, Ref ref){
        if( !pNode ){
            pNode = new Node(value, ref);
            pNode->setParent(pParent);
            return pNode;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
        return internal_insert(reinterpret_cast<NodePtr&>(pNode->getChildRef(pos)), pNode, value, ref);
    }

private:
    static NodePtr asNodePtr(BinaryTreeNode<value_type>* pNode) { // Helper para convertir un puntero de tipo BinaryTreeNode a NodePtr (que es un puntero a BinaryTreeNode en este caso)
        return static_cast<NodePtr>(pNode);
    }

    void ToStringInOrder(NodePtr pNode, stringstream &ss, TB &first) const { // Función recursiva para generar la representación en cadena del árbol en orden
        if (pNode == nullptr) return;
        ToStringInOrder(asNodePtr(pNode->getLeftChild()), ss, first);

        if (!first) ss << ",";
        ss << *pNode;
        first = false;
        ToStringInOrder(asNodePtr(pNode->getRightChild()), ss, first);
    }

    template <typename Func, typename... Args>
    void ForEachInOrder(NodePtr pNode, Func func, Args &&...  args){
        if (pNode == nullptr) return;
        ForEachInOrder(asNodePtr(pNode->getLeftChild()), func, std::forward<Args>(args)...);
        func(*pNode, std::forward<Args>(args)...);
        ForEachInOrder(asNodePtr(pNode->getRightChild()), func, std::forward<Args>(args)...);
    }   

    template <typename Func, typename... Args>
    NodePtr FirstThatInOrder(NodePtr pNode, Func func, Args &&...  args){
        if (pNode == nullptr) return nullptr;

        NodePtr left = FirstThatInOrder(asNodePtr(pNode->getLeftChild()), func, std::forward<Args>(args)...);
        if (left != nullptr) return left;

        if (func(*pNode, std::forward<Args>(args)...)) return pNode;

        return FirstThatInOrder(asNodePtr(pNode->getRightChild()), func, std::forward<Args>(args)...);
    }

    void WritePreOrderWithNulls(NodePtr pNode, ostream &os) const { // Funcion recursiva, escribe el nodo actual, luego el hijo izquierdo y luego el derecho. Si un nodo es nullptr, escribe "#" para indicar un nodo nulo
        if (pNode == nullptr) {
            os << "#\n";
            return;
        }
        os << pNode->getData() << " " << pNode->getRef() << "\n";
        WritePreOrderWithNulls(asNodePtr(pNode->getLeftChild()), os);
        WritePreOrderWithNulls(asNodePtr(pNode->getRightChild()), os);
    } 

    NodePtr ReadPreOrderWithNulls(istream &is, NodePtr pParent = nullptr) { // Función recursiva, lee un nodo de la entrada, si el nodo es "#", retorna nullptr. Si no, crea un nuevo nodo con los datos leídos y luego llama recursivamente para leer el hijo izquierdo y derecho
        TS line;
        if (!getline(is, line)) return nullptr;

        if (line == "#") return nullptr;

        value_type data;
        Ref ref;

        stringstream ss(line);
        ss >> data >> ref;
        is >> ws; // Consumir cualquier espacio en blanco restante

        NodePtr pNode = new Node(data, ref);
        pNode->setParent(pParent);

        pNode->setLeftChild(ReadPreOrderWithNulls(is, pNode));
        pNode->setRightChild(ReadPreOrderWithNulls(is, pNode));

        return pNode;
    }

};

    template <typename TTraits>
    ostream& operator<<(ostream& os, const BinaryTree<TTraits>& tree) {
        scoped_lock lock(tree.m_mutex);
        tree.WritePreOrderWithNulls(tree.m_pRoot, os);
        return os;
    }

    template <typename TTraits>
    istream& operator>>(istream& is, BinaryTree<TTraits>& tree) {
        scoped_lock lock(tree.m_mutex);

        delete tree.m_pRoot;

        tree.m_pRoot = nullptr;

        tree.m_pRoot = tree.ReadPreOrderWithNulls(is);
        return is;
    }
;

#endif // __BINARY_TREE_H__
