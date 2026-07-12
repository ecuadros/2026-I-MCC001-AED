#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <utility>
#include <stack>
#include "general_iterator.h"
#include "basetrait.h"
#include "../types.h"
#include "basenode.h"
#include "../foreach.h"
#include <mutex> 


template <typename Container>
class BinaryTreeForwardInorderIterator : public general_iterator<Container, 
                                         BinaryTreeForwardInorderIterator<Container>>{
    using MySelf = BinaryTreeForwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    using Node = typename Container::Node;

public:
    // TODO: Completar el operator++
    //Tenemos que pasar el primer nodo mas izquierdo del arbol
    // LNR
    MySelf& operator++(){
        // this->m_pNode = this->m_pNode->getNext();
        
        if(!this->m_pNode) return *this; // end()
        
        // Caso que hay nodo derecho
        if(this->m_pNode->GetChild(1)){
            this->m_pNode = this->m_pNode->GetChild(1);
            while(this->m_pNode->GetChild(0)){
                this->m_pNode = this->m_pNode->GetChild(0);
            }
        }else{
            //subimos
            Node* parent = this->m_pNode->GetParent();
            while(parent && this->m_pNode == parent->GetChild(1)){
                this->m_pNode = parent;
                parent = parent->GetParent();
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
    using Node = typename Container::Node;
public:
    // RNL
    // TODO: Completar el operator++
    MySelf& operator++(){
        if (!this->m_pNode) return *this; // rend()
        if (this->m_pNode->GetChild(0)) {
            this->m_pNode = this->m_pNode->GetChild(0);
            while (this->m_pNode->GetChild(1)) {
                this->m_pNode = this->m_pNode->GetChild(1);
            }
        } else {
            
            Node* parent = this->m_pNode->GetParent();
            while (parent && this->m_pNode == parent->GetChild(0)) {
                this->m_pNode = parent;
                parent = parent->GetParent();
            }
            this->m_pNode = parent;
        }
        return *this;
    }
};

// NLR:
// con pilas
template <typename Container>
class BinaryTreeForwardPreorderIterator : public general_iterator<Container,
                                         BinaryTreeForwardPreorderIterator<Container>>{
    using MySelf = BinaryTreeForwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
    stack<Node*> m_stack;
public:
    BinaryTreeForwardPreorderIterator(Container* container, Node* node)
        : Parent(container, node)
    {
        if (node) {
            if (node->GetChild(1)) m_stack.push(node->GetChild(1)); // derecho primero
            if (node->GetChild(0)) m_stack.push(node->GetChild(0)); // izquierdo encima
        }
    }
    MySelf& operator++() {
        if (m_stack.empty()) {
            this->m_pNode = nullptr;
        } else {
            this->m_pNode = m_stack.top(); 
            m_stack.pop();
            if (this->m_pNode->GetChild(1)) m_stack.push(this->m_pNode->GetChild(1));
            if (this->m_pNode->GetChild(0)) m_stack.push(this->m_pNode->GetChild(0));
        }
        return *this;
    }
};

// Reverse de NLR = RLN:
// derecha, izquierda, nodo
template <typename Container>
class BinaryTreeBackwardPreorderIterator : public general_iterator<Container,
                                         BinaryTreeBackwardPreorderIterator<Container>>{
    using MySelf = BinaryTreeBackwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
    stack<Node*> m_stack;
    Node*        m_last = nullptr;

    void push_right_spine(Node* node) {
        while (node) { m_stack.push(node); node = node->GetChild(1); }
    }

    void advance() {
        while (!m_stack.empty()) {
            Node* top  = m_stack.top();
            Node* left = top->GetChild(0);
            if (left && left != m_last) {
                push_right_spine(left);
            } else {
                m_stack.pop();
                m_last = top;
                this->m_pNode = top;
                return;
            }
        }
        this->m_pNode = nullptr;
    }
public:
    BinaryTreeBackwardPreorderIterator(Container* container, Node* node)
        : Parent(container, nullptr), m_last(nullptr)
    {
        if (node) {
            push_right_spine(node);
            advance();
        }
    }
    MySelf& operator++() {
        advance();
        return *this;
    }
};

// LRN:

template <typename Container>
class BinaryTreeForwardPostorderIterator : public general_iterator<Container,
                                         BinaryTreeForwardPostorderIterator<Container>>{
    using MySelf = BinaryTreeForwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
    stack<Node*> m_stack;
    Node*        m_last = nullptr; // ultimo nodo visitado

    // empuja toda la espina izquierda a partir de node
    void push_left_spine(Node* node) {
        while (node) { m_stack.push(node); node = node->GetChild(0); }
    }

    // avanza al siguiente nodo postorder
    void advance() {
        while (!m_stack.empty()) {
            Node* top   = m_stack.top();
            Node* right = top->GetChild(1);
            if (right && right != m_last) {
                // sub-arbol derecho aun no visitado: descender por el
                push_left_spine(right);
            } else {
                // ambos sub-arboles ya visitados: visitar este nodo
                m_stack.pop();
                m_last = top;
                this->m_pNode = top;
                return;
            }
        }
        this->m_pNode = nullptr;
    }
public:
    BinaryTreeForwardPostorderIterator(Container* container, Node* root)
        : Parent(container, nullptr), m_last(nullptr)
    {
        if (root) {
            push_left_spine(root);
            advance(); // posicionarse en el primer nodo (hoja mas izquierda)
        }
    }
    MySelf& operator++() {
        advance();
        return *this;
    }
};

// Reverse de LRN = NRL:
// nodo, derecha, izquierda

template <typename Container>
class BinaryTreeBackwardPostorderIterator : public general_iterator<Container,
                                         BinaryTreeBackwardPostorderIterator<Container>>{
    using MySelf = BinaryTreeBackwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
    stack<Node*> m_stack;
public:
    BinaryTreeBackwardPostorderIterator(Container* container, Node* root)
        : Parent(container, root)
    {
        if (root) {
            if (root->GetChild(0)) m_stack.push(root->GetChild(0));
            if (root->GetChild(1)) m_stack.push(root->GetChild(1));
        }
    }
    MySelf& operator++() {
        if (m_stack.empty()) {
            this->m_pNode = nullptr;
        } else {
            this->m_pNode = m_stack.top();
            m_stack.pop();
            if (this->m_pNode->GetChild(0)) m_stack.push(this->m_pNode->GetChild(0));
            if (this->m_pNode->GetChild(1)) m_stack.push(this->m_pNode->GetChild(1));
        }
        return *this;
    }
};



template <typename T>
class BinaryTreeNode : public BaseNode<T> {
public:
    using Parent     = BaseNode<T>;
    using value_type = typename Parent::value_type;
    using Node       = BinaryTreeNode<T>;
    using NodePtr    = Node*;
private:
    static NodePtr CloneSubtree(NodePtr src, NodePtr parent) {
        if (!src) {
            return nullptr;
        }
        NodePtr copy = new Node(src->GetData(), src->GetRef(), nullptr, nullptr, parent);
        copy->m_pChild[0] = CloneSubtree(src->m_pChild[0], copy);
        copy->m_pChild[1] = CloneSubtree(src->m_pChild[1], copy);
        return copy;
    }
protected:
    NodePtr    m_pChild[2] = {nullptr, nullptr};
    NodePtr    m_pParent;   
public:
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr, NodePtr parent = nullptr)
        : Parent(data, ref)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;
        m_pParent = parent;
    }
    // Copy constructor: deep copy del subarbol
    BinaryTreeNode(const BinaryTreeNode& other)
        : Parent(other)
    {
        m_pParent   = nullptr;
        m_pChild[0] = CloneSubtree(other.m_pChild[0], this);
        m_pChild[1] = CloneSubtree(other.m_pChild[1], this);
    }
    // Move constructor: transfiere ownership de hijos
    BinaryTreeNode(BinaryTreeNode&& other) noexcept
        : Parent(std::move(other))
    {
        m_pChild[0] = std::exchange( other.m_pChild[0], nullptr );
        m_pChild[1] = std::exchange( other.m_pChild[1], nullptr );
        m_pParent   = std::exchange( other.m_pParent, nullptr );
        if (m_pChild[0]) m_pChild[0]->m_pParent = this;
        if (m_pChild[1]) m_pChild[1]->m_pParent = this;
    }

    Node& operator=(const Node& other) {
        if (this == &other) {
            return *this;
        }
        Parent::operator=(other);
        delete m_pChild[0];
        delete m_pChild[1];
        m_pChild[0] = CloneSubtree(other.m_pChild[0], this);
        m_pChild[1] = CloneSubtree(other.m_pChild[1], this);
        m_pParent   = nullptr;
        return *this;
    }

    Node& operator=(Node&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        Parent::operator=(std::move(other));
        delete m_pChild[0];
        delete m_pChild[1];
        m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
        m_pParent   = std::exchange(other.m_pParent, nullptr);
        if (m_pChild[0]) m_pChild[0]->m_pParent = this;
        if (m_pChild[1]) m_pChild[1]->m_pParent = this;
        return *this;
    }

    ~BinaryTreeNode() {
        delete m_pChild[0];
        delete m_pChild[1];
    };

    //value_type      getData() const { return m_data; }
    //value_type&     getDataRef()    { return m_data; }
    //void            setData(value_type data) { m_data = data; }
    //Ref             getRef() const  { return m_ref; }
    //Ref&            getRefRef()     { return m_ref; }
    //void            setRef(Ref ref) { m_ref = ref; }

    NodePtr         GetChild(size_t pos) const { return m_pChild[pos]; }
    NodePtr&        GetChildRef(size_t pos)    { return m_pChild[pos]; }
    void            SetChild(size_t pos, NodePtr pChild) { m_pChild[pos] = pChild; }
    void            SetParent(NodePtr pParent) { m_pParent = pParent; }
    NodePtr         GetParent() const { return m_pParent; }
    //string to_string() const {
    //    stringstream ss;
    //    ss << "Node(data: " << m_data << ", ref: " << m_ref << ")";
    //    return ss.str();
    //}
    //// Cuidado: en el disco hay posiciones dentro del archivo,
    ////          en memoria hay punteros
    //friend ostream& operator<<(ostream& os, 
    //    const BinaryTreeNode& node) {
    //    os << node.ToString();
    //    return os;
    //}

    //// Cuidado: en el disco hay posiciones dentro del archivo,
    ////          en memoria hay punteros
    //friend istream& operator>>(istream& is, 
    //    BinaryTreeNode& node) {
    //    string line;
    //    if (getline(is, line)) {
    //        stringstream ss(line);
    //        ss >> node.m_data >> node.m_ref;
    //    }
    //    return is;
    //}
    //std::string ToString() const {
    //    std::ostringstream oss;
    //    oss << "(" << this->m_data << ", " << this->m_ref << ")";
    //    if (m_pParent) {
    //        oss << "<-" << m_pParent->GetDataRef();
    //    }
    //    return oss.str();
    //}
};

template <typename T>
using BaseBinaryTreeListTrait = BaseNodeContainerTrait<T, BinaryTreeNode>;

template <typename T>
using AscendingBinaryTreeListTrait = AscendingNodeContainerTrait<T, BinaryTreeNode>;

template <typename T>
using DescendingBinaryTreeListTrait = DescendingNodeContainerTrait<T, BinaryTreeNode>;





// Proxy que expone begin()/end() para poder usar range-for: for(auto& n : tree.preorder())
template <typename Iterator>
struct IteratorRange {
    Iterator m_begin, m_end;
    Iterator begin() { return m_begin; }
    Iterator end()   { return m_end; }
};
// Hay mejora una estrcutrua abstracta que defina un contrato común para todas las estructuras
template <typename Traits>
class BinaryTree{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using NodePtr    = Node*;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator    = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator   = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator   = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator  = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator  = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;
    using forward_iterator            = forward_inorder_iterator;

protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutex   m_mtx;
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other) // Copy constructor (deep copy)
        : m_pRoot(nullptr), m_comp(other.m_comp)
    {
        std::stringstream ss;
        other.serialize(other.m_pRoot, ss);
        if (!deserialize(m_pRoot, ss)) {
            clear();
        }
    }
    BinaryTree(BinaryTree &&other) noexcept // Move constructor
        : m_pRoot(nullptr), m_comp(std::move(other.m_comp))
    {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
    }

    ~BinaryTree(){
        clear();
    }

    void clear() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    void insert(const value_type &value, Ref ref){
        internal_insert(m_pRoot, value, ref);
    }

    bool empty() const {
        return m_pRoot == nullptr;
    }

    const Node* root() const {
        return m_pRoot;
    }
    forward_inorder_iterator begin() { 
        NodePtr current = m_pRoot;
        while(current && current->GetChild(0)){
            current = current->GetChild(0);
        }    
        return forward_inorder_iterator(this, current); 
    }
    forward_inorder_iterator end()   { return forward_inorder_iterator(this, nullptr); }
    backward_inorder_iterator rbegin() {
        NodePtr current = m_pRoot;
        while (current && current->GetChild(1)) {
            current = current->GetChild(1);
        }
        return backward_inorder_iterator(this, current);
    }
    backward_inorder_iterator rend() { return backward_inorder_iterator(this, nullptr); }

    // Iteradores preorder (NLR / NRL)
    forward_preorder_iterator  preorder_begin()  { return forward_preorder_iterator(this, m_pRoot); }
    forward_preorder_iterator  preorder_end()    { return forward_preorder_iterator(this, nullptr); }
    backward_preorder_iterator rpreorder_begin() { return backward_preorder_iterator(this, m_pRoot); }
    backward_preorder_iterator rpreorder_end()   { return backward_preorder_iterator(this, nullptr); }

    // Iteradores postorder (LRN / RLN)
    forward_postorder_iterator  postorder_begin()  { return forward_postorder_iterator(this, m_pRoot); }
    forward_postorder_iterator  postorder_end()    { return forward_postorder_iterator(this, nullptr); }
    backward_postorder_iterator rpostorder_begin() { return backward_postorder_iterator(this, m_pRoot); }
    backward_postorder_iterator rpostorder_end()   { return backward_postorder_iterator(this, nullptr); }

    //  (auto& node : tree.preorder())
    auto inorder()    { return IteratorRange<forward_inorder_iterator>   {begin(),   end()}; }
    auto rinorder()   { return IteratorRange<backward_inorder_iterator> {rbegin(),  rend()}; }
    auto preorder()   { return IteratorRange<forward_preorder_iterator>  {preorder_begin(),   preorder_end()}; }
    auto rpreorder()  { return IteratorRange<backward_preorder_iterator> {rpreorder_begin(),  rpreorder_end()}; }
    auto postorder()  { return IteratorRange<forward_postorder_iterator> {postorder_begin(),  postorder_end()}; }
    auto rpostorder() { return IteratorRange<backward_postorder_iterator>{rpostorder_begin(), rpostorder_end()}; }

    std::string toString() const {
        std::stringstream ss;
        serialize(m_pRoot, ss);
        return ss.str();
    }
    std::istream& fromIstream(std::istream &is) {
        clear();
        if (!deserialize(m_pRoot, is)) {
            clear();
            is.setstate(std::ios::failbit);
        }
        return is;
    }
    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }

    //Agregar FirstThat
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&...  args){
        scoped_lock<mutex> lock(m_mtx);
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)... );
    }
private:
    void internal_insert(NodePtr &pNode, const value_type &value, Ref ref){
        if( !pNode ){
            pNode = new Node(value, ref);
            return;
        }
        size_t pos = !m_comp(value, pNode->GetDataRef());
        internal_insert(pNode->GetChildRef(pos), value, ref);
        pNode->GetChild(pos)->SetParent(pNode);
    }
    void serialize(NodePtr node, std::stringstream& ss) const {
        if (!node) {
            ss << "# ";
            return;
        }
        ss << *node << " ";
        serialize(node->GetChild(0), ss);
        serialize(node->GetChild(1), ss); 
    }
    bool parseNodeToken(const std::string& token, value_type& value, Ref& ref) {
        std::istringstream ts(token);
        BaseNode<value_type> tmp;
        if (!(ts >> tmp)) {
            return false;
        }
        ts >> std::ws;
        if (ts.peek() != std::char_traits<char>::eof()) {
            return false;
        }
        value = tmp.GetData();
        ref   = tmp.GetRef();
        return true;
    }

    // reconstruccion por preorder con # como null
    bool deserialize(NodePtr &node, std::istream& is, NodePtr parent = nullptr) {
        std::string token;
        if (!(is >> token)) {
            node = nullptr;
            return false;
        }
        if (token == "#") {
            node = nullptr;
            return true;
        }

        value_type value{};
        Ref ref{};
        if (!parseNodeToken(token, value, ref)) {
            node = nullptr;
            return false;
        }
        
        node = new Node(value, ref);
        node->SetParent(parent);

        if (!deserialize(node->GetChildRef(0), is, node)) {
            return false;
        }
        if (!deserialize(node->GetChildRef(1), is, node)) {
            return false;
        }
        return true;
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, const BinaryTree<Traits>& tree){
    os << "TREE ";
    return os << tree.toString();
}

template <typename Traits>
istream& operator>>(istream& is, BinaryTree<Traits>& tree){
    std::string header;
    if (!(is >> header) || header != "TREE") {
        is.setstate(std::ios::failbit);
        return is;
    }
    return tree.fromIstream(is);
}


#endif // __BINARY_TREE_H__