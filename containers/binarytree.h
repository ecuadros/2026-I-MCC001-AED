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
        if(!this->m_pNode)
            return *this;
        if(this->m_pNode->getChild(1)){
            this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(1));
            while(this->m_pNode->getChild(0))
    			this->m_pNode =static_cast<typename Container::Node*>(this->m_pNode->getChild(0));
            return *this;
    	}
        auto pParent = static_cast<typename Container::Node*>(this->m_pNode->m_pParent);
        while(pParent && this->m_pNode == pParent->getChild(1))
        {
            this->m_pNode = pParent;
            pParent = static_cast<typename Container::Node*>(pParent->m_pParent);
        }
        this->m_pNode = pParent;
        return *this;
    }
};

// Backward iterator inorder
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
        if(!this->m_pNode)
        	return *this;
    	if(this->m_pNode->getChild(0)){
        	this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(0));
        	while(this->m_pNode->getChild(1))
            	this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(1));
        	return *this;
    	}
    	auto pParent = this->m_pNode->m_pParent;
    	while(pParent &&
          	this->m_pNode == pParent->getChild(0))
    	{
        	this->m_pNode = pParent;
        	pParent = pParent->m_pParent;
    	}
    	this->m_pNode = pParent;
    	return *this;
    }
};

// Preorder iterator forward
template <typename Container>
class BinaryTreeForwardPreorderIterator :
    public general_iterator<Container,
    BinaryTreeForwardPreorderIterator<Container>>
{
    using MySelf = BinaryTreeForwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(!this->m_pNode)
            return *this;
        if(this->m_pNode->getChild(0)){
            this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(0));
    		return *this;
        }
        if(this->m_pNode->getChild(1)){
            this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(1));
    		return *this;
        }
        auto pParent = this->m_pNode->m_pParent;
        while(pParent){
            if(this->m_pNode == pParent->getChild(0) &&
               pParent->getChild(1))
            {
                this->m_pNode = pParent->getChild(1);
                return *this;
            }
            this->m_pNode = pParent;
            pParent = pParent->m_pParent;
        }
        this->m_pNode = nullptr;
        return *this;
    }
};

// Preorder iterator backwards
template <typename Container>
class BinaryTreeBackwardPreorderIterator :
    public general_iterator<Container,
    BinaryTreeBackwardPreorderIterator<Container>>
{
    using MySelf = BinaryTreeBackwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(!this->m_pNode)
            return *this;
        vector<typename Container::Node*> nodes;
        this->m_pContainer->build_preorder(
            this->m_pContainer->m_pRoot,
            nodes
        );
        auto it = find(nodes.begin(), nodes.end(), this->m_pNode);
        if(it == nodes.begin())
            this->m_pNode = nullptr;
        else{
            --it;
            this->m_pNode = *it;
        }
        return *this;
    }
};

// Postorder iterator forward
template <typename Container>
class BinaryTreeForwardPostorderIterator :
    public general_iterator<Container,
    BinaryTreeForwardPostorderIterator<Container>>
{
    using MySelf = BinaryTreeForwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(!this->m_pNode)
            return *this;
        auto pParent = this->m_pNode->m_pParent;
        if(!pParent){
            this->m_pNode = nullptr;
            return *this;
        }
        if(this->m_pNode == pParent->getChild(0) &&
           pParent->getChild(1))
        {
            this->m_pNode = pParent->getChild(1);
            while(true){
                if(this->m_pNode->getChild(0)){
                    this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(0));
                }
                else if(this->m_pNode->getChild(1)){
                    this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getChild(1));
                }
                else{
                    break;
                }
            }
            return *this;
        }
        this->m_pNode = pParent;
        return *this;
    }
};

// Postorder iterator backwards
template <typename Container>
class BinaryTreeBackwardPostorderIterator :
    public general_iterator<Container,
    BinaryTreeBackwardPostorderIterator<Container>>
{
    using MySelf = BinaryTreeBackwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(!this->m_pNode)
            return *this;
        vector<typename Container::Node*> nodes;
        this->m_pContainer->build_postorder(
            this->m_pContainer->m_pRoot,
            nodes
        );
        auto it = find(nodes.begin(), nodes.end(), this->m_pNode);
        if(it == nodes.begin())
            this->m_pNode = nullptr;
        else{
            --it;
            this->m_pNode = *it;
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
public:
    NodePtr    m_pParent; 
public:
    BinaryTreeNode(const value_type& data, const Ref& ref, 
        NodePtr left = nullptr, NodePtr right = nullptr)
        : m_data(data), m_ref(ref), m_pParent(nullptr)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;
        if(left)
            left->m_pParent = this;
        if(right)
            right->m_pParent = this;
    }
    // copy constructor
    BinaryTreeNode(const BinaryTreeNode& other) : m_data(), m_ref(), m_pParent(nullptr)
    {
        //scoped_lock<mutex> lock(other.m_mtx);
        m_data = other.m_data;
    	m_ref  = other.m_ref;
        if(other.m_pChild[0]){
    		m_pChild[0] = other.m_pChild[0]->clone();
    		m_pChild[0]->m_pParent = this;
		}

		if(other.m_pChild[1]){
    		m_pChild[1] = other.m_pChild[1]->clone();
    		m_pChild[1]->m_pParent = this;
		}
    }
    // Corregir con exchange, Move onstructor
    BinaryTreeNode(BinaryTreeNode&& other) noexcept : m_data(), m_ref(), m_pParent(nullptr)
    {
        scoped_lock<mutex> lock(other.m_mtx);
        m_data = std::move(other.m_data);
		m_ref  = std::move(other.m_ref);
        m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
        if (m_pChild[0]) m_pChild[0]->m_pParent = this;
        if (m_pChild[1]) m_pChild[1]->m_pParent = this;
    }
    
	BinaryTreeNode& operator=(const BinaryTreeNode& other){
    	if(this == &other)
        	return *this;
    	delete m_pChild[0];
    	delete m_pChild[1];
    	m_data = other.m_data;
    	m_ref  = other.m_ref;
    	m_pChild[0] = nullptr;
    	m_pChild[1] = nullptr;
    	m_pParent   = nullptr;
    	if(other.m_pChild[0]){
    		m_pChild[0] = other.m_pChild[0]->clone();
   	 		m_pChild[0]->m_pParent = this;
		}

		if(other.m_pChild[1]){
   			m_pChild[1] = other.m_pChild[1]->clone();
    		m_pChild[1]->m_pParent = this;
		}
    	return *this;
	}
	BinaryTreeNode& operator=(BinaryTreeNode&& other) noexcept{
    	if(this == &other)
        	return *this;
    	delete m_pChild[0];
    	delete m_pChild[1];
    	m_data = std::move(other.m_data);
    	m_ref  = std::move(other.m_ref);
    	m_pChild[0] = std::exchange(other.m_pChild[0], nullptr);
    	m_pChild[1] = std::exchange(other.m_pChild[1], nullptr);
    	m_pParent = nullptr;
    	if(m_pChild[0])
        	m_pChild[0]->m_pParent = this;
    	if(m_pChild[1])
        	m_pChild[1]->m_pParent = this;
    	return *this;
	}
    
    // Destructor
    virtual ~BinaryTreeNode() {
    	scoped_lock<mutex> lock(m_mtx);
        delete m_pChild[0];
        delete m_pChild[1];
    };

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }

    NodePtr         getChild(size_t pos) const {return m_pChild[pos];}
    bool isLeaf() const {return !m_pChild[0] && !m_pChild[1];}
    NodePtr&        getChildRef(size_t pos) {return m_pChild[pos];}
    size_t childPosition() const { return m_pParent && m_pParent->getChild(1) == this; }
    void            setChild(size_t pos, NodePtr pChild) { m_pChild[pos] = pChild; if(pChild) pChild->m_pParent = this;}

    string to_string() const {
        stringstream ss;
        ss << "Node(data: " << m_data << ", ref: " << m_ref << ")";
        return ss.str();
    }
    virtual NodePtr clone() const{
    	return new Node(*this);
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
	template <typename>
    friend class BinaryTreeBackwardPostorderIterator;
    template <typename>
    friend class BinaryTreeBackwardPreorderIterator;
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using NodePtr    = Node*;
    using Comp       = typename Traits::Comp;
    using MySelf     = BinaryTree<Traits>;
    using forward_inorder_iterator  = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;

protected:
    NodePtr m_pRoot = nullptr;
    Comp    m_comp;
    mutable mutex   m_mtx;
protected:
    // Funcion de impresion
    void internal_print(NodePtr pNode, int depth) const{
        if(!pNode)
            return;
        internal_print(reinterpret_cast<NodePtr>(pNode->getChild(1)), depth + 1);
        for(int i = 0; i < depth; i++)
            cout << "        ";
        cout << pNode->getData() << "(" << pNode->getRef() << ")" << endl;
        internal_print(reinterpret_cast<NodePtr>(pNode->getChild(0)), depth + 1);
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
        auto& child = reinterpret_cast<NodePtr&>(pNode->getChildRef(pos));
        internal_insert(reinterpret_cast<NodePtr&>(pNode->getChildRef(pos)), value, ref);
        pNode->getChild(pos)->m_pParent = pNode;
    }
    
public:
    void printTree() const{
        scoped_lock<mutex> lock(m_mtx);
        internal_print(m_pRoot, 0);
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
    template <typename Func, typename... Args>
    forward_inorder_iterator
    FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
		return ::FirstThat(begin(), end(), func, std::forward<Args>(args)... );
    }    
};
#endif // __BINARY_TREE_H__
