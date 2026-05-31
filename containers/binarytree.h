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
        MySelf& operator++(){
        if(this->m_pNode == nullptr)
            return *this;

        // Caso 1: Existe subárbol derecho
        auto* node = this->m_pNode;
        if(node->getChild(1) != nullptr){
            node = node->getChild(1);
            while(node->getChild(0) != nullptr)
                node = node->getChild(0);
            this->m_pNode = node;
            return *this;
        }

        // Caso 2: No existe subárbol derecho
        auto* parent = node->getParent();
        while(parent != nullptr && parent->getChild(1) == node){
            node = parent;
            parent = parent->getParent();
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

        // Caso 1: existe subárbol izquierdo
        auto* node = this->m_pNode;
        if(node->getChild(0) != nullptr){
            node = node->getChild(0);
            while(node->getChild(1) != nullptr)
                node = node->getChild(1);
            this->m_pNode = node;
            return *this;
        }

        // Caso 2: subir por los padres
        auto* parent = node->getParent();
        while(parent != nullptr && parent->getChild(0) == node){
            node = parent;
            parent = parent->getParent();
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

        // Caso 1: hijo izquierdo
        if(node->getChild(0)){
            this->m_pNode = node->getChild(0);
            return *this;
        }

        // Caso 2: hijo derecho
        if(node->getChild(1)){
            this->m_pNode = node->getChild(1);
            return *this;
        }

        // Caso 3: subir
        auto* parent = node->getParent();

        while(parent){
            // venimos del hijo izquierdo y existe hijo derecho sin visitar
            if(parent->getChild(0) == node && parent->getChild(1) != nullptr){
                this->m_pNode = parent->getChild(1);
                return *this;
            }

            node = parent;
            parent = parent->getParent();
        }

        this->m_pNode = nullptr;
        return *this;
    }
};

template <typename Container>
class BinaryTreeBackwardPreorderIterator :
    public general_iterator<
        Container,
        BinaryTreeBackwardPreorderIterator<Container>>
{
    using MySelf = BinaryTreeBackwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

public:

    MySelf& operator++()
    {
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        auto* parent = node->getParent();

        // raíz
        if(parent == nullptr)
        {
            this->m_pNode = nullptr;
            return *this;
        }

        // soy hijo derecho
        if(parent->getChild(1) == node &&
           parent->getChild(0) != nullptr)
        {
            node = parent->getChild(0);

            while(true)
            {
                if(node->getChild(1))
                    node = node->getChild(1);
                else if(node->getChild(0))
                    node = node->getChild(0);
                else
                    break;
            }

            this->m_pNode = node;
            return *this;
        }

        // soy hijo izquierdo
        this->m_pNode = parent;
        return *this;
    }
};

template <typename Container>
class BinaryTreeForwardPostorderIterator :
    public general_iterator<
        Container,
        BinaryTreeForwardPostorderIterator<Container>>
{
    using MySelf = BinaryTreeForwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    using Parent::Parent;

private:

    static auto first_postorder(auto* node)
    {
        while(node)
        {
            if(node->getChild(0))
                node = node->getChild(0);
            else if(node->getChild(1))
                node = node->getChild(1);
            else
                break;
        }

        return node;
    }

public:

    MySelf& operator++()
    {
        if(this->m_pNode == nullptr)
            return *this;

        auto* node = this->m_pNode;
        auto* parent = node->getParent();

        if(parent == nullptr)
        {
            this->m_pNode = nullptr;
            return *this;
        }

        if(parent->getChild(0) == node &&
           parent->getChild(1) != nullptr)
        {
            this->m_pNode =
                first_postorder(parent->getChild(1));
        }
        else
        {
            this->m_pNode = parent;
        }

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

    NodePtr         getParent() const {return m_pParent; }
    void            setParent(NodePtr parent){m_pParent = parent; }

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
    using NodePtr    = typename Node::NodePtr; 

    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator = BinaryTreeForwardPostorderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor

    };
    BinaryTree(BinaryTree &&other){ // Move constructor

    };

    void insert(const value_type &value, Ref ref){
        internal_insert(m_pRoot, value, ref, nullptr);
    }
private:
    void internal_insert(NodePtr &pNode, const value_type &value, Ref ref, NodePtr pParent){
        if( !pNode ){
            pNode = new Node(value, ref);
            pNode->setParent(pParent);
            return;
        }

        size_t pos = !m_comp(value, pNode->getDataRef());
        internal_insert(pNode->getChildRef(pos), value, ref, pNode);
    }

    static Node* first_postorder(Node* node){
        while(node){
            if(node->getChild(0))
                node = node->getChild(0);
            else if(node->getChild(1))
                node = node->getChild(1);
            else
                break;
        }

        return node;
    }

public:
    void clear(){
        delete m_pRoot;   // destructor del nodo borra todo el subárbol
        m_pRoot = nullptr;
    }

    forward_inorder_iterator begin(){
        NodePtr p = m_pRoot;
        while(p && p->getChild(0))
            p = p->getChild(0);
        return forward_inorder_iterator(this, p);
    }

    forward_inorder_iterator end(){
        return forward_inorder_iterator(this, nullptr);
    }

    backward_inorder_iterator rbegin(){
        NodePtr p = m_pRoot;
        while(p && p->getChild(1))
            p = p->getChild(1);
        return backward_inorder_iterator(this, p);
    }

    backward_inorder_iterator rend(){
        return backward_inorder_iterator(this, nullptr);
    }

    forward_preorder_iterator preorder_begin(){
        return forward_preorder_iterator(this, m_pRoot);
    }

    forward_preorder_iterator preorder_end(){
        return forward_preorder_iterator(this, nullptr);
    }

    backward_preorder_iterator preorder_rbegin(){
        NodePtr p = m_pRoot;

        if(!p)
            return backward_preorder_iterator(this, nullptr);

        while(true){
            if(p->getChild(1))
                p = p->getChild(1);
            else if(p->getChild(0))
                p = p->getChild(0);
            else
                break;
        }

        return backward_preorder_iterator(this, p);
    }

    backward_preorder_iterator preorder_rend(){
        return backward_preorder_iterator(this, nullptr);
    }

    forward_postorder_iterator postorder_begin(){
        return forward_postorder_iterator(this, first_postorder(m_pRoot));
    }

    forward_postorder_iterator postorder_end(){
        return forward_postorder_iterator(this, nullptr);
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args){
        ::ForEach(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_inorder_iterator FirstThat(Func func, Args&&... args){
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, BinaryTree<Traits>& tree){
    auto it = tree.begin(); // inorder

    bool first = true;
    for(; it != tree.end(); ++it){
        if(!first) os << ", ";
        first = false;
        os << *it;
    }

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