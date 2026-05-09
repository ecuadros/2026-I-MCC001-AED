#include "linkedlist.h"

template class DLLNode : public LLNode{ public: using value_type = T; using Node = DLLNode; private: Node *m_pPrev; public: DLLNode(T data, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr) : LLNode(data, ref, pNext), m_pPrev(pPrev){}

Node*  getPrev() const { return m_pPrev; }
Node*& getPrevRef() { return m_pPrev; }
void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};

template struct BaseDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode>{

};

template struct AscendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait{ using Comp = less; };

template struct DescendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait{ using Comp = greater; };

// Reutiizar el LinkedListForwardIterator de la linked list template using DoubleLinkedListForwardIterator = LinkedListForwardIterator;

// Backward iterator template class DoubleLinkedListBackwardIterator : public general_iterator<Container, DoubleLinkedListBackwardIterator>{ using MySelf = DoubleLinkedListBackwardIterator; using Parent = general_iterator<Container, MySelf>; using Parent::Parent; public: MySelf& operator++(){ this->m_pNode = this->m_pNode->getPrev(); return *this; } };

template class DoubleLinkedList : public LinkedList{ public: using value_type = typename Traits::value_type; using Node = typename Traits::Node; using Comp = typename Traits::Comp; using MySelf = DoubleLinkedList;

using forward_iterator  = DoubleLinkedListForwardIterator<MySelf>;
using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

// reutilizar atributos heredados
private: using LinkedList::m_pRoot; using LinkedList::m_pTail; using LinkedList::m_size; using LinkedList::m_comp; using LinkedList::m_mtx;

public: // reutilizar constructor base DoubleLinkedList() : LinkedList() {}

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
    LinkedList<Traits>::insert(value, ref);
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
