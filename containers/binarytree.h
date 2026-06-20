#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__
#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <mutex>
#include <stack>
#include <vector>
#include <utility>
#include <algorithm>
#include "general_iterator.h"
#include "../types.h"
#include "../foreach.h"
#include "basetrait.h"

enum class Traversal{Inorder,Preorder,Postorder};
enum class Direction{Forward,Backward};

template<Direction Dir>
struct DirectionTraits;

template<>
struct DirectionTraits<Direction::Forward>{
    static constexpr size_t Next = 1;
    static constexpr size_t Prev = 0;
};

template<>
struct DirectionTraits<Direction::Backward>{
    static constexpr size_t Next = 0;
    static constexpr size_t Prev = 1;
};

template<Direction Dir>
struct InorderAdapter;

template<Direction Dir>
struct PreorderAdapter;

template<Direction Dir>
struct PostorderAdapter;

template<typename Container, typename Adapter>
class BinaryTreeIterator :
    public general_iterator<Container, BinaryTreeIterator<Container,Adapter>>
{
    using MySelf = BinaryTreeIterator<Container,Adapter>;
    using Parent = general_iterator<Container,MySelf>;
public:
    using Parent::Parent;
    MySelf& operator++(){
        if(this->m_pNode)
            this->m_pNode = Adapter::next(this->m_pNode, this->m_pContainer);
        return *this;
    }
};

template<Traversal Order, Direction Dir>
struct TraversalAdapter;

template<>
struct TraversalAdapter<Traversal::Inorder, Direction::Forward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->begin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->end();}
};

template<>
struct TraversalAdapter<Traversal::Inorder, Direction::Backward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->rbegin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->rend();}
};

template<>
struct TraversalAdapter<Traversal::Preorder, Direction::Forward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->pbegin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->pend();}
};

template<>
struct TraversalAdapter<Traversal::Preorder, Direction::Backward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->prbegin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->prend();}
};

template<>
struct TraversalAdapter<Traversal::Postorder, Direction::Forward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->postbegin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->postend();}
};

template<>
struct TraversalAdapter<Traversal::Postorder, Direction::Backward>
{
    template<typename Tree>
    static auto begin(Tree* tree){return tree->postrbegin();}
    template<typename Tree>
    static auto end(Tree* tree){return tree->postrend();}
};

template<Direction Dir>
struct InorderAdapter{
    template<typename Tree>
    static typename Tree::Node* next(typename Tree::Node* node, Tree* tree);
};

template<Direction Dir>
struct PreorderAdapter{
    template<typename Tree>
    static typename Tree::Node* next(typename Tree::Node* node, Tree* tree);
};

template<Direction Dir>
struct PostorderAdapter{
    template<typename Tree>
    static typename Tree::Node* next(typename Tree::Node* node, Tree* tree);
};

template<typename Tree, Traversal Order, Direction Dir>
class TraversalRange{
    Tree* m_tree;
public:
    TraversalRange(Tree* tree) : m_tree(tree) {}
    auto begin(){return TraversalAdapter<Order,Dir>::begin(m_tree);}
    auto end(){return TraversalAdapter<Order,Dir>::end(m_tree);}
};

template<Direction Dir>
template<typename Tree>
typename Tree::Node*
InorderAdapter<Dir>::next(typename Tree::Node* node, Tree*)
{
    constexpr size_t Next = DirectionTraits<Dir>::Next;
    constexpr size_t Prev = DirectionTraits<Dir>::Prev;
    if(node->getChild(Next)){
        node = node->getChild(Next);
        while(node->getChild(Prev))
            node = node->getChild(Prev);
        return node;
    }
    auto parent = node->m_pParent;
    while(parent && node == parent->getChild(Next)){
        node = parent;
        parent = parent->m_pParent;
    }
    return parent;
}

template<Direction Dir>
template<typename Tree>
typename Tree::Node*
PreorderAdapter<Dir>::next(typename Tree::Node* node, Tree* tree)
{
    constexpr size_t Next = DirectionTraits<Dir>::Next;
    constexpr size_t Prev = DirectionTraits<Dir>::Prev;
    if constexpr(Dir == Direction::Forward){
        if(node->getChild(Prev))
            return node->getChild(Prev);
        if(node->getChild(Next))
            return node->getChild(Next);
        auto parent = node->m_pParent;
        while(parent){
            if(node == parent->getChild(Prev) && parent->getChild(Next))
                return parent->getChild(Next);
            node = parent;
            parent = parent->m_pParent;
        }
        return nullptr;
    }
    else{
        vector<typename Tree::Node*> nodes;
        tree->build_preorder(tree->m_pRoot, nodes);
        auto it = find(nodes.begin(), nodes.end(), node);
        if(it == nodes.begin())
            return nullptr;
        --it;
        return *it;
    }
}

template<Direction Dir>
template<typename Tree>
typename Tree::Node*
PostorderAdapter<Dir>::next(typename Tree::Node* node, Tree* tree)
{
    constexpr size_t Next = DirectionTraits<Dir>::Next;
    constexpr size_t Prev = DirectionTraits<Dir>::Prev;
    if constexpr(Dir == Direction::Forward){
        auto parent = node->m_pParent;
        if(!parent)
            return nullptr;
        if(node == parent->getChild(Prev) && parent->getChild(Next))
        {
            node = parent->getChild(Next);
            while(true){
                if(node->getChild(Prev))
                    node = node->getChild(Prev);
                else if(node->getChild(Next))
                    node = node->getChild(Next);
                else
                    break;
            }
            return node;
        }
        return parent;
    }
    else{
        vector<typename Tree::Node*> nodes;
        tree->build_postorder(tree->m_pRoot, nodes);
        auto it = find(nodes.begin(), nodes.end(), node);
        if(it == nodes.begin())
            return nullptr;
        --it;
        return *it;
    }
}

template <typename T>
struct BaseBinaryTreeListTrait{
    using value_type = T;
};

template <typename T>
struct AscendingBinaryTreeListTrait :
    public BaseBinaryTreeListTrait<T>,
    public AscendingTrait<T>{};

template <typename T>
struct DescendingBinaryTreeListTrait :
    public BaseBinaryTreeListTrait<T>,
    public DescendingTrait<T>{};

template <typename Traits>
class BinaryTree{
    template<typename,typename>
    friend class BinaryTreeIterator;
    template<Direction>
    friend struct InorderAdapter;
    template<Direction>
    friend struct PreorderAdapter;
    template<Direction>
    friend struct PostorderAdapter;
public:
    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;
public:
    class Node{
    protected:
        value_type m_data;
        Ref        m_ref;
        Node*      m_pChild[2] = {nullptr,nullptr};
    public:
        Node* m_pParent = nullptr;
        mutable mutex m_mtx;
    public:
        Node(const value_type& data, const Ref& ref, Node* left = nullptr, Node* right = nullptr) : m_data(data), m_ref(ref)
        {
            m_pChild[0] = left;
            m_pChild[1] = right;
            if(left)
                left->m_pParent = this;
            if(right)
                right->m_pParent = this;
        }
        Node(const Node& other) : m_data(), m_ref(), m_pParent(nullptr)
        {
            scoped_lock<mutex> lock(other.m_mtx);
            m_data = other.m_data;
            m_ref  = other.m_ref;
            if(other.m_pChild[0]){
                m_pChild[0] = new Node(*other.m_pChild[0]);
                m_pChild[0]->m_pParent = this;
            }
            if(other.m_pChild[1]){
                m_pChild[1] = new Node(*other.m_pChild[1]);
                m_pChild[1]->m_pParent = this;
            }
        }
        Node(Node&& other) noexcept : m_data(std::move(other.m_data)), m_ref(std::move(other.m_ref))
        {
            scoped_lock<mutex> lock(other.m_mtx);
            m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
            m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
            if(m_pChild[0])
                m_pChild[0]->m_pParent = this;
            if(m_pChild[1])
                m_pChild[1]->m_pParent = this;
        }
        Node& operator=(const Node& other){
            if(this == &other)
                return *this;
            scoped_lock<mutex> lock(other.m_mtx);
            delete m_pChild[0];
            delete m_pChild[1];
            m_pChild[0] = nullptr;
            m_pChild[1] = nullptr;
            m_data = other.m_data;
            m_ref  = other.m_ref;
            if(other.m_pChild[0]){
                m_pChild[0] = new Node(*other.m_pChild[0]);
                m_pChild[0]->m_pParent = this;
            }
            if(other.m_pChild[1]){
                m_pChild[1] = new Node(*other.m_pChild[1]);
                m_pChild[1]->m_pParent = this;
            }
            return *this;
        }
        Node& operator=(Node&& other) noexcept{
            if(this == &other)
                return *this;
            scoped_lock<mutex> lock(other.m_mtx);
            delete m_pChild[0];
            delete m_pChild[1];
            m_data = std::move(other.m_data);
            m_ref  = std::move(other.m_ref);
            m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
            m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
            if(m_pChild[0])
                m_pChild[0]->m_pParent = this;
            if(m_pChild[1])
                m_pChild[1]->m_pParent = this;
            return *this;
        }
        ~Node(){
            delete m_pChild[0];
            delete m_pChild[1];
        }
        value_type getData() const { return m_data; }
        value_type& getDataRef() { return m_data; }
        Ref getRef() const { return m_ref; }
        Ref& getRefRef() { return m_ref; }
        Node* getChild(size_t pos) const {
            return m_pChild[pos];
        }
        Node*& getChildRef(size_t pos) {
            return m_pChild[pos];
        }
        void setChild(size_t pos, Node* child){
            m_pChild[pos] = child;
            if(child)
                child->m_pParent = this;
        }
        bool isLeaf() const{
            return !m_pChild[0] && !m_pChild[1];
        }
    };
public:
    using NodePtr = Node*;
protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutable mutex m_mtx;
public:
    using forward_inorder_iterator   = BinaryTreeIterator<MySelf, InorderAdapter<Direction::Forward>>;
    using backward_inorder_iterator  = BinaryTreeIterator<MySelf, InorderAdapter<Direction::Backward>>;
    using forward_preorder_iterator  = BinaryTreeIterator<MySelf, PreorderAdapter<Direction::Forward>>;
    using backward_preorder_iterator = BinaryTreeIterator<MySelf, PreorderAdapter<Direction::Backward>>;
    using forward_postorder_iterator = BinaryTreeIterator<MySelf, PostorderAdapter<Direction::Forward>>;
    using backward_postorder_iterator= BinaryTreeIterator<MySelf, PostorderAdapter<Direction::Backward>>;
protected:
    // Funcion de impresion
	void internal_print(NodePtr pNode, TI depth, ostream& os) const{
    	if(!pNode)
        	return;
    	internal_print(pNode->getChild(1), depth + 1, os);
    	vector<TI> tabs(depth);
    	::ForEach(tabs.begin(), tabs.end(), [&](auto&){os << "        ";});
    	os << pNode->getData() << "(" << pNode->getRef() << ")" << endl;
    	internal_print(pNode->getChild(0), depth + 1, os);
	}
    void build_postorder(NodePtr pNode, vector<NodePtr>& nodes) const{
        if(!pNode)
            return;
        build_postorder(pNode->getChild(0), nodes);
        build_postorder(pNode->getChild(1), nodes);
        nodes.push_back(pNode);
    }
    void build_preorder(NodePtr pNode, vector<NodePtr>& nodes) const{
    if(!pNode)
        return;
    nodes.push_back(pNode);
    build_preorder(pNode->getChild(0), nodes);
    build_preorder(pNode->getChild(1), nodes);
	}
public:
    BinaryTree() {}
    BinaryTree(const BinaryTree &other){ // Copy constructor
    	scoped_lock<mutex> lock(other.m_mtx);
        if(other.m_pRoot)
            m_pRoot = new Node(*other.m_pRoot);
    };
    BinaryTree(BinaryTree &&other){ // Move constructor
    	scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
    }
    
	BinaryTree& operator=(const BinaryTree& other){
    	if(this == &other)
        	return *this;
    	scoped_lock<mutex> lock(m_mtx, other.m_mtx);
    	delete m_pRoot;
    	m_pRoot = nullptr;
    	if(other.m_pRoot)
        	m_pRoot = new Node(*other.m_pRoot);
    	return *this;
	}
	BinaryTree& operator=(BinaryTree&& other){
    	if(this == &other)
        	return *this;
    	scoped_lock<mutex> lock(m_mtx, other.m_mtx);
    	delete m_pRoot;
    	m_pRoot = std::exchange(other.m_pRoot, nullptr);
    	return *this;
	}

	// Destructor
    virtual ~BinaryTree(){
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = nullptr;
    }
    
	protected:
    	virtual NodePtr minimum(NodePtr pNode) const{
        	while(pNode && pNode->getChild(0))
            	pNode = static_cast<NodePtr>(pNode->getChild(0));
        	return pNode;
    	}
    	virtual NodePtr maximum(NodePtr pNode) const{
        	while(pNode && pNode->getChild(1))
            	pNode = static_cast<NodePtr>(pNode->getChild(1));
        	return pNode;
    	}
    	NodePtr& rootRef(){
        	return m_pRoot;
    	}
    	virtual NodePtr findNode(const value_type& value) const{
        	NodePtr pNode = m_pRoot;
        	while(pNode){
            	if(pNode->getData() == value)
                	return pNode;
            	size_t pos =
                	!m_comp(value, pNode->getDataRef());
            	pNode = static_cast<NodePtr>(pNode->getChild(pos));
        	}
        	return nullptr;
    	}

public:
    // Insert
    virtual void insert(const value_type &value, Ref ref){
    	scoped_lock<mutex> lock(m_mtx);
        internal_insert(m_pRoot, value, ref);
    }
protected:
    void internal_insert(NodePtr& pNode, const value_type& value, Ref ref){
        if( !pNode ){
            pNode = new Node(value, ref);
            return;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
		internal_insert(pNode->getChildRef(pos), value, ref);
		pNode->getChild(pos)->m_pParent = pNode;
    }
    
public:
	void printTree(ostream& os = cout) const{
    	scoped_lock<mutex> lock(m_mtx);
    	internal_print(m_pRoot, 0, os);
	}
    // Inorder
    forward_inorder_iterator begin(){
        Node* pNode = m_pRoot;
        if(!pNode)
            return forward_inorder_iterator(this, nullptr);
        while(pNode->getChild(0))
            pNode = reinterpret_cast<NodePtr>(pNode->getChild(0));
        return forward_inorder_iterator(this, pNode);
    }
    forward_inorder_iterator end(){
        return forward_inorder_iterator(this, nullptr);
    }
    
    // Reverse Inorder
    backward_inorder_iterator rbegin(){
        NodePtr pNode = m_pRoot;
        if(!pNode)
            return backward_inorder_iterator(this, nullptr);
        while(pNode->getChild(1))
            pNode = static_cast<Node*>(pNode->getChild(1));
        return backward_inorder_iterator(this, pNode);
    }
    backward_inorder_iterator rend(){
        return backward_inorder_iterator(this, nullptr);
    }
	
	// Preorder forward
    forward_preorder_iterator pbegin(){
        return forward_preorder_iterator(this, m_pRoot);
    }
    forward_preorder_iterator pend(){
        return forward_preorder_iterator(this, nullptr);
    }
    
    // Preorder backward
	backward_preorder_iterator prbegin(){
    	vector<NodePtr> nodes;
    	build_preorder(m_pRoot, nodes);
    	if(nodes.empty())
        	return backward_preorder_iterator(this, nullptr);
    	return backward_preorder_iterator(this, nodes.back());
	}
	backward_preorder_iterator prend(){
    	return backward_preorder_iterator(this, nullptr);
	}

	// Postorder backward
	backward_postorder_iterator postrbegin(){
    	vector<NodePtr> nodes;
    	build_postorder(m_pRoot, nodes);
    	if(nodes.empty())
        	return backward_postorder_iterator(this, nullptr);
    	return backward_postorder_iterator(this, nodes.back());
	}
	backward_postorder_iterator postrend(){
    	return backward_postorder_iterator(this, nullptr);
	}
	
    // Postorder forward
    forward_postorder_iterator postbegin(){
        NodePtr pNode = m_pRoot;
        if(!pNode)
            return forward_postorder_iterator(this, nullptr);
        while(true){
            if(pNode->getChild(0))
                pNode = static_cast<Node*>(pNode->getChild(0));
            else if(pNode->getChild(1))
                pNode = static_cast<Node*>(pNode->getChild(1));
            else
                break;
        }

        return forward_postorder_iterator(this, pNode);
    }
    forward_postorder_iterator postend(){
        return forward_postorder_iterator(this, nullptr);
    }
    
    // Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }

    // First that
	template<typename Iterator, typename Func, typename... Args>
	Iterator FirstThat(Iterator first, Iterator last, Func func, Args&&... args){
    	return ::FirstThat(first, last, func, std::forward<Args>(args)...);
	}
	
	auto inorder(){return TraversalRange<MySelf,Traversal::Inorder,Direction::Forward>(this);}
	auto rinorder(){return TraversalRange<MySelf,Traversal::Inorder,Direction::Backward>(this);}
	auto preorder(){return TraversalRange<MySelf,Traversal::Preorder,Direction::Forward>(this);}
	auto rpreorder(){return TraversalRange<MySelf,Traversal::Preorder,Direction::Backward>(this);}
	auto postorder(){return TraversalRange<MySelf,Traversal::Postorder,Direction::Forward>(this);}
	auto rpostorder(){return TraversalRange<MySelf,Traversal::Postorder,Direction::Backward>(this);}
};
#endif // __BINARY_TREE_H__
