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

template <typename Container, TB Reverse> // InOrder: Izquireda - Raiz - Derecha
class BinaryTreeInorderIterator : public general_iterator<Container, 
                                         BinaryTreeInorderIterator<Container, Reverse>>{
    using MySelf = BinaryTreeInorderIterator<Container, Reverse>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if (!this->m_pNode) return *this;
        // Reverse ---- True (Backward), False (Forward)
        size_t left_idx = Reverse ? 1 : 0; 
        size_t right_idx = Reverse ? 0 : 1;

        if (this->m_pNode->getChild(right_idx) != nullptr) { // tiene hijo a la derecha?
            this->m_pNode = this->m_pNode->getChild(right_idx);
            while (this->m_pNode->getChild(left_idx) != nullptr) { // recursion a la izquierda
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

// Recorrido Forward y Backward con el argumento reverse
template <typename Container>
using BinaryTreeForwardInorderIterator = BinaryTreeInorderIterator<Container, false>;

template <typename Container>
using BinaryTreeBackwardInorderIterator = BinaryTreeInorderIterator<Container, true>;

template <typename Container, TB Reverse> // Preorder: Raiz - Izquierda - Derecha
class BinaryTreePreorderIterator : public general_iterator<Container, BinaryTreePreorderIterator<Container, Reverse>> {
    using MySelf = BinaryTreePreorderIterator<Container, Reverse>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++() {
        if (!this->m_pNode) return *this;

        size_t left_idx = Reverse ? 1 : 0;
        size_t right_idx = Reverse ? 0 : 1;

        if (this->m_pNode->getChild(left_idx) != nullptr)
            this->m_pNode = this->m_pNode->getChild(left_idx);
         
        else if (this->m_pNode->getChild(right_idx) != nullptr) 
            this->m_pNode = this->m_pNode->getChild(right_idx);
         
        else {
            auto parent = this->m_pNode->getParent();
            while (parent != nullptr && (this->m_pNode == parent->getChild(right_idx) || parent->getChild(right_idx) == nullptr)) {
                this->m_pNode = parent;
                parent = parent->getParent();
            }
            if (parent != nullptr)
                this->m_pNode = parent->getChild(right_idx);
            else 
                this->m_pNode = nullptr;
        }
        return *this;
    }
};

template <typename Container>
using BinaryTreeForwardPreorderIterator = BinaryTreePreorderIterator<Container, false>;

template <typename Container>
using BinaryTreeBackwardPreorderIterator = BinaryTreePreorderIterator<Container, true>;

template <typename Container, TB Reverse> //PosOrder: Izquierda - Derecha - Raiz
class BinaryTreePostorderIterator : public general_iterator<Container, BinaryTreePostorderIterator<Container, Reverse>> {
    using MySelf = BinaryTreePostorderIterator<Container, Reverse>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++() {
        if (!this->m_pNode) return *this;

        size_t left_idx = Reverse ? 1 : 0;
        size_t right_idx = Reverse ? 0 : 1;

        auto parent = this->m_pNode->getParent();
        
        if (parent == nullptr) 
            this->m_pNode = nullptr; 

        else if (this->m_pNode == parent->getChild(right_idx) || parent->getChild(right_idx) == nullptr) 
            this->m_pNode = parent;
         
        else {
            auto curr = parent->getChild(right_idx);
            while (curr->getChild(left_idx) != nullptr || curr->getChild(right_idx) != nullptr) {
                if (curr->getChild(left_idx) != nullptr) curr = curr->getChild(left_idx);
                else curr = curr->getChild(right_idx);
            }
            this->m_pNode = curr;
        }
        return *this;
    }
};

template <typename Container>
using BinaryTreeForwardPostorderIterator = BinaryTreePostorderIterator<Container, false>;

template <typename Container>
using BinaryTreeBackwardPostorderIterator = BinaryTreePostorderIterator<Container, true>;

template <typename T>
//struct BaseBinaryTreeListTrait : public BaseContainerTrait<T, BinaryTreeNode<T>>{
struct BaseBinaryTreeListTrait{
    using value_type = T;
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


enum class TraversalMode{ // Para modos de impresion <<
    PreOrder,
    InOrder,
    PostOrder,
};

template <typename Traits>
class BinaryTree{
public:
    using value_type = typename Traits::value_type;
    //using Node       = typename Traits::Node;
    //using NodePtr    = typename Traits::NodePtr;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;

    class BinaryTreeNode{
    public:
        using NodePtr = BinaryTreeNode*;

    protected:
        value_type m_data;
        Ref        m_ref;
        NodePtr    m_pChild[2] = {nullptr, nullptr};
        NodePtr    m_pParent;

    public:
        BinaryTreeNode(const value_type& data, const Ref& ref, NodePtr left = nullptr, NodePtr right = nullptr, NodePtr parent = nullptr)
            : m_data(data), m_ref(ref), m_pParent(parent)
        {
            m_pChild[0] = left;
            m_pChild[1] = right;
        }

        ~BinaryTreeNode() {
            delete m_pChild[0];
            delete m_pChild[1];
        }

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
            ss << "(" << m_data << ", " << m_ref << ")";
            return ss.str();
        }

        friend ostream& operator<<(ostream& os, const BinaryTreeNode& node) {
            os << node.to_string();
            return os;
        }

        friend istream& operator>>(istream& is, BinaryTreeNode& node) {
            string line;
            if (getline(is, line)) {
                stringstream ss(line);
                ss >> node.m_data >> node.m_ref;
            }
            return is;
        }
    };

    using Node       = BinaryTreeNode;
    using NodePtr    = typename Node::NodePtr;


    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;

    using forward_preorder_iterator   = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator  = BinaryTreeBackwardPreorderIterator<MySelf>;

    using forward_postorder_iterator  = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutable mutex   m_mtx;

public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other) : m_comp(other.m_comp){ // Copy constructor
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = clone_tree(other.m_pRoot, nullptr);
    };
    BinaryTree(BinaryTree &&other) noexcept 
        : m_comp(std::move(other.m_comp)){ // Move constructor
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
    };

    ~BinaryTree() {
        delete m_pRoot;
        m_pRoot = nullptr;
    }

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



    NodePtr clone_tree(NodePtr pOtherNode, NodePtr pMyParent) {
        if (!pOtherNode) return nullptr;

        NodePtr newNode = new Node(pOtherNode->getData(), pOtherNode->getRef(), nullptr, nullptr, pMyParent);

        newNode->getChildRef(0) = clone_tree(pOtherNode->getChild(0), newNode);
        newNode->getChildRef(1) = clone_tree(pOtherNode->getChild(1), newNode);

        return newNode;
    }

    void print_recursive(NodePtr pNode, ostream& os, TraversalMode mode) const {
        if ( !pNode ) return;

        if (mode == TraversalMode::PreOrder) os << pNode->getData() << " ";
        print_recursive(pNode->getChild(0), os, mode);

        if (mode == TraversalMode::InOrder) os << pNode->getData() << " ";
        print_recursive(pNode->getChild(1), os, mode);

        if (mode == TraversalMode::PostOrder) os << pNode->getData() << " ";

    };

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

    forward_preorder_iterator begin_preorder() {
        return forward_preorder_iterator(this, m_pRoot);
    }
    forward_preorder_iterator end_preorder() {
        return forward_preorder_iterator(this, nullptr);
    }

    backward_preorder_iterator rbegin_preorder() {
        return backward_preorder_iterator(this, m_pRoot);
    }
    backward_preorder_iterator rend_preorder() {
        return backward_preorder_iterator(this, nullptr);
    }

    forward_postorder_iterator begin_postorder() {
        NodePtr p = m_pRoot;
        if (p) {
            // Prioridad a la izquierda (índice 0)
            while (p->getChild(0) != nullptr || p->getChild(1) != nullptr) {
                if (p->getChild(0) != nullptr) p = p->getChild(0);
                else p = p->getChild(1);
            }
        }
        return forward_postorder_iterator(this, p);
    }
    forward_postorder_iterator end_postorder() {
        return forward_postorder_iterator(this, nullptr);
    }

    backward_postorder_iterator rbegin_postorder() {
        NodePtr p = m_pRoot;
        if (p) {
            // Prioridad a la derecha (índice 1) para el reverso
            while (p->getChild(1) != nullptr || p->getChild(0) != nullptr) {
                if (p->getChild(1) != nullptr) p = p->getChild(1);
                else p = p->getChild(0);
            }
        }
        return backward_postorder_iterator(this, p);
    }
    backward_postorder_iterator rend_postorder() {
        return backward_postorder_iterator(this, nullptr);
    }

    // implementando modos de impresion
    struct TreePrinter {
        const BinaryTree& tree;
        TraversalMode mode;
        
        friend ostream& operator<<(ostream& os, const TreePrinter& printer) {
            scoped_lock<mutex> lock(printer.tree.m_mtx);
            os << "[ ";
            printer.tree.print_recursive(printer.tree.m_pRoot, os, printer.mode);
            os << "]";
            return os;
        }
    };

    TreePrinter print(TraversalMode mode) const {
        return TreePrinter{*this, mode};
    }

    // por defecto InOrder
    friend ostream& operator<<(ostream& os, const BinaryTree& tree) {
        return os << tree.print(TraversalMode::InOrder);
    }

    friend istream& operator>>(istream& is, BinaryTree& tree) {
        string token;
        
        while (is >> token) {
            if (token == "[" || token == "]") continue;
            
            stringstream ss(token);
            value_type val;
            if (ss >> val) {
                tree.insert(val, 0); 
            }
        }
        return is;
    }
};

void DemoBinaryTree();



#endif // __BINARY_TREE_H__