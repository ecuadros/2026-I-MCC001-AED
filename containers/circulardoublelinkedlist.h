#include "circularlinkedlist.h"

template <typename T>
class CDLLNode : public CLLNode<T>{
public:
    using value_type = T;
    using Node       = CDLLNode<T>;
private:
    Node *m_pPrev;
public:
    CDLLNode(T data, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr)
        : CLLNode<T>(data, ref, pNext), m_pPrev(pPrev){}

    Node*  getPrev() const { return m_pPrev; }
    Node*& getPrevRef() { return m_pPrev; }
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};

template <typename T>
struct BaseCircularDoubleLinkedListTrait : public BaseContainerTrait<T, CDLLNode<T>>{

};

template <typename T>
struct AscendingCircularDoubleLinkedListTrait : public BaseCircularDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingCircularDoubleLinkedListTrait : public BaseCircularDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

// reutiliza iterator de CircularLinkedList
template <typename Container>
using CircularDoubleLinkedListForwardIterator =
    CircularLinkedListForwardIterator<Container>;
    
// Backward iterator
template <typename Container>
class CircularDoubleLinkedListBackwardIterator : public general_iterator<Container, 
                                        CircularDoubleLinkedListBackwardIterator<Container>>{
    using MySelf = CircularDoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        if(this->m_pNode)
            this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

template <typename Traits>
class CircularDoubleLinkedList : public CircularLinkedList<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = CircularDoubleLinkedList<Traits>;

    using forward_iterator  = CircularDoubleLinkedListForwardIterator<MySelf>;
    using backward_iterator = CircularDoubleLinkedListBackwardIterator<MySelf>;

    // reutilizar atributos heredados
private:
    using CircularLinkedList<Traits>::m_pRoot;
    using CircularLinkedList<Traits>::m_pTail;
    using CircularLinkedList<Traits>::m_size;
    using CircularLinkedList<Traits>::m_comp;
    using CircularLinkedList<Traits>::m_mtx;

public:
	// reutilizar constructor base
    CircularDoubleLinkedList() : CircularLinkedList<Traits>() {}
    
    // copy constructor
    CircularDoubleLinkedList(const CircularDoubleLinkedList &other): CircularLinkedList<Traits>() {
        Node *pTemp = other.m_pRoot;
        for(size_t i = 0; i < other.m_size; ++i){
            this->push_back(
                pTemp->getData(),
                pTemp->getRef()
            );
            pTemp = pTemp->getNext();
        }
    }
    
    // move constructor
    CircularDoubleLinkedList(CircularDoubleLinkedList &&other): CircularLinkedList<Traits>() {
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);
    }
    
    // reutiliza destructor de CircularLinkedList
    ~CircularDoubleLinkedList(){}
    
    size_t size () const { return m_size; }
    bool isEmpty() const { return m_pRoot == nullptr; }
         
    void insert(value_type value, Ref ref){
    	scoped_lock<mutex> lock(m_mtx);
        CircularLinkedList<Traits>::insert(value, ref);
    }
    
    void push_back(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        Node *pNew = new Node(value, ref, nullptr, m_pTail);
        if(m_size == 0){
            Node *pNew = new Node(value, ref);
            pNew->setNext(pNew);
            pNew->setPrev(pNew);
            m_pRoot = pNew;
            m_pTail = pNew;
        }
        else{
            Node *pNew = new Node( value, ref, m_pRoot, m_pTail);
            m_pTail->setNext(pNew);
            m_pRoot->setPrev(pNew);
            m_pTail = pNew;
        }
        ++m_size;
    }
    
    auto pop_back() -> pair<value_type, Ref>{
        scoped_lock<mutex> lock(m_mtx);
        if(m_pTail == nullptr)
            throw out_of_range("pop_back(): empty list");
        auto data = make_pair(
            m_pTail->getData(),
            m_pTail->getRef()
        );
        if(m_pRoot == m_pTail){
            delete m_pTail;
            m_pRoot = nullptr;
            m_pTail = nullptr;
        }
        else{
            Node *pDelete = m_pTail;
            m_pTail = m_pTail->getPrev();
            m_pTail->setNext(m_pRoot);
            m_pRoot->setPrev(m_pTail);
            delete pDelete;
        }
        --m_size;
        return data;
    }

    forward_iterator begin()   { return forward_iterator(this, m_pRoot); }
    forward_iterator end()     { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, m_pTail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        Node *pTemp = m_pRoot;
        for(size_t i = 0; i < m_size; ++i){
            func(*pTemp, forward<Args>(args)...);
            pTemp = pTemp->getNext();
        }
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        Node *pTemp = m_pTail;
        for(size_t i = 0; i < m_size; ++i){
            func(*pTemp, forward<Args>(args)...);
            pTemp = pTemp->getPrev();
        }
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        Node *pTemp = m_pRoot;
        for(size_t i = 0; i < m_size; ++i){
            if(func(*pTemp, forward<Args>(args)...)){
                return forward_iterator(this, pTemp);
            }
            pTemp = pTemp->getNext();
        }
        return end();
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        Node *pTemp = m_pTail;
        for(size_t i = 0; i < m_size; ++i){
            if(func(*pTemp, forward<Args>(args)...)){
                return backward_iterator(this, pTemp);
            }
            pTemp = pTemp->getPrev();
        }
        return rend();
    }

	// operador <<
    friend ostream& operator<<(ostream& os, CircularDoubleLinkedList& list){
        return os << list.toString();
    }
	
	// operador >>
    friend istream& operator>>(istream& is, CircularDoubleLinkedList& list){
        using value_type =
            typename CircularDoubleLinkedList::value_type;
        string line;
        getline(is, line);
        for(char& c : line){
            if(c == '[' || c == ']' || c == '(' || c == ')' || c == ','){
                c = ' ';
            }
        }
        value_type value;
        Ref ref;
        stringstream ss(line);
        while(ss >> value >> ref){
            list.push_back(value, ref);
        }
        return is;
    }
};
