#include "linkedlist.h"

template <typename T>
class DLLNode :  public LLNode<T>{
public:
    using value_type = T;
    using Node       = DLLNode<T>;
private:
    Node *m_pPrev;
public:
    DLLNode(T data, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr)
        : LLNode<T>(data, ref, pNext), m_pPrev(pPrev){}

    Node*  getPrev() const { return m_pPrev; }
    Node*& getPrevRef() { return m_pPrev; }
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};

template <typename T>
struct BaseDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>>{

};

template <typename T>
struct AscendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

// Reutiizar el LinkedListForwardIterator de la linked list
template <typename Container>
using DoubleLinkedListForwardIterator =
    LinkedListForwardIterator<Container>;
    
// Backward iterator
template <typename Container>
class DoubleLinkedListBackwardIterator : public general_iterator<Container, 
                                        DoubleLinkedListBackwardIterator<Container>>{
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = DoubleLinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

    // reutilizar atributos heredados
private:
    using LinkedList<Traits>::m_pRoot;
    using LinkedList<Traits>::m_pTail;
    using LinkedList<Traits>::m_size;
    using LinkedList<Traits>::m_comp;
    using LinkedList<Traits>::m_mtx;

public:
	// reutilizar constructor base
    DoubleLinkedList() : LinkedList<Traits>() {}
    
    // copy constructor
    DoubleLinkedList(DoubleLinkedList &other): LinkedList<Traits>() {
        Node *pTemp = other.m_pRoot;
        while(pTemp != nullptr){
            this->insert(pTemp->getData(), pTemp->getRef());
            pTemp = pTemp->getNext();
        }
    }
    
    // move constructor
    DoubleLinkedList(DoubleLinkedList &&other): LinkedList<Traits>() {
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);
    }
    
    // reutilizar destructor de LinkedList
    ~DoubleLinkedList() {}
    
    size_t size () const { return m_size; }
    bool isEmpty() const { return m_pRoot == nullptr; }
         
    void insert(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        Node* pNew = new Node(value, ref, nullptr, nullptr);
        if(m_pRoot == nullptr){
            m_pRoot = pNew;
            m_pTail = pNew;
        }
        else if(m_comp(value, m_pRoot->getData())){
            pNew->setNext(m_pRoot);
            pNew->setPrev(nullptr);
            m_pRoot->setPrev(pNew);
            m_pRoot = pNew;
        }
        else{
            Node* pPrev = m_pRoot;
            Node* pCurr = m_pRoot->getNext();
            while(pCurr != nullptr &&
                  !m_comp(value, pCurr->getData())){
                pPrev = pCurr;
                pCurr = pCurr->getNext();
            }
            pNew->setNext(pCurr);
            pNew->setPrev(pPrev);
            pPrev->setNext(pNew);
            if(pCurr != nullptr){
                pCurr->setPrev(pNew);
            }
            else{
                m_pTail = pNew;
            }
        }
        ++m_size;
    }
    
    void push_back(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        Node *pNew = new Node(value, ref, nullptr, m_pTail);
        if(m_size == 0){
            m_pRoot = pNew;
            m_pTail = pNew;
        }
        else{
            m_pTail->setNext(pNew);
            pNew->setPrev(m_pTail);
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
            m_pTail->setNext(nullptr);
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
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(m_mtx);
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }

	// operador >>
    friend ostream& operator<<(ostream& os, DoubleLinkedList& list){
        return LinkedList<Traits>::operator<<(os, list);
    }
	
	// operador >>
    friend istream& operator>>(istream& is, DoubleLinkedList& list){
        return LinkedList<Traits>::operator>>(is, list);
	}
};
