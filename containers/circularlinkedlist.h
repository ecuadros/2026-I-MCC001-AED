#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>     // mutex
#include "general_iterator.h"
#include "../util.h"
#include "../types.h"
#include "../foreach.h"
#include "basetrait.h"

using namespace std;

// Forward iterator
template <typename Container>
class CircularLinkedListForwardIterator : public general_iterator<Container, 
                                    CircularLinkedListForwardIterator<Container>>{
    using MySelf = CircularLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        if(this->m_pNode)
			this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Circular Linked List Node
template <typename T>
class CLLNode{
public:
    using value_type = T;
    using Node       = CLLNode<T>;
protected:
    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext;
public:
    CLLNode(){}
    CLLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~CLLNode() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
};

template <typename T>
ostream &operator<<(ostream &os, const CLLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}

template <typename T>
struct BaseCircularLinkedListTrait : public BaseContainerTrait<T, CLLNode<T>>{

};

template <typename T>
struct AscendingCircularLinkedListTrait : public BaseCircularLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingCircularLinkedListTrait : public BaseCircularLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Traits>
class CircularLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = CircularLinkedList<Traits>;

    using forward_iterator = CircularLinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutex m_mtx;
public:
    CircularLinkedList() {}
    CircularLinkedList(const CircularLinkedList &other){ // Copy constructor
        Node* pTemp = other.m_pRoot;
        for(size_t i = 0; i < other.m_size; ++i){
            push_back(pTemp->getData(), pTemp->getRef());
            pTemp = pTemp->getNext();
        }
    }
    
    CircularLinkedList(CircularLinkedList &&other){ // Move constructor
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);

    }
    CircularLinkedList& operator=(const CircularLinkedList &other){ 
    }
    CircularLinkedList& operator=(CircularLinkedList &&other){
    }
    
    virtual ~CircularLinkedList() {
        scoped_lock<mutex> lock(m_mtx);
        if(m_pRoot){
            Node* pTemp = m_pRoot;
            for(size_t i = 0; i < m_size; ++i){
                Node* pNext = pTemp->getNext();
                delete pTemp;
                pTemp = pNext;
            }
        }
        m_pRoot = nullptr;
        m_pTail = nullptr;
        m_size  = 0;
    }

    virtual void   push_front(value_type value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        if(m_size == 0){
        	Node* pTemp = new Node(value, ref);
            pTemp->setNext(pTemp);
            m_pRoot = pTemp;
            m_pTail = pTemp;
        }
        else{
            Node* pTemp = new Node(value, ref, m_pRoot);
            m_pRoot = pTemp;
            m_pTail->setNext(m_pRoot);
        }
        ++m_size;
    }
    
    virtual auto    pop_front() -> pair<value_type, Ref>{ 
        scoped_lock<mutex> lock(m_mtx);
        if(!m_pRoot)
            throw out_of_range("pop_front(): empty list");
        Node* pDelete = m_pRoot;
        auto data = make_pair(
            pDelete->getData(),
            pDelete->getRef()
        );
        if(m_size == 1){
            delete pDelete;
            m_pRoot = nullptr;
            m_pTail = nullptr;
        }
        else{
            m_pRoot = m_pRoot->getNext();
            m_pTail->setNext(m_pRoot);
            delete pDelete;
        }
        --m_size;
        return data;
    }
    virtual void    push_back(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        if(m_size == 0){
            Node* pTemp = new Node(value, ref);
            pTemp->setNext(pTemp);
            m_pRoot = pTemp;
            m_pTail = pTemp;
        }
        else{
            Node* pTemp = new Node(value, ref, m_pRoot);
            m_pTail->setNext(pTemp);
            m_pTail = pTemp;
        }
        ++m_size;
    }
    
    virtual auto    pop_back() -> pair<value_type, Ref>{
        scoped_lock<mutex> lock(m_mtx);
        if(!m_pRoot)
            throw out_of_range("pop_back(): empty list");
        if(m_size == 1){
            auto pDelete = make_pair(
                m_pTail->getData(),
                m_pTail->getRef()
            );
            delete m_pTail;
            m_pRoot = nullptr;
            m_pTail = nullptr;
            --m_size;
            return pDelete;
        }
        Node *pTemp = m_pRoot;
        while(pTemp->getNext() != m_pTail)
            pTemp = pTemp->getNext();
        auto pDelete = make_pair(
            m_pTail->getData(),
            m_pTail->getRef()
        );
        delete m_pTail;
        m_pTail = pTemp;
        m_pTail->setNext(m_pRoot);
        --m_size;
        return pDelete;
    }
    
private:
    void internal_insert(Node* &pPrev, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual Node& operator[](const size_t index) const{
        
        if (index >= m_size)
        	throw out_of_range("Index out of range");

        Node* pTemp = m_pRoot;
        for (size_t i = 0; i < index; ++i){
            pTemp = pTemp -> getNext();
        }
        return *pTemp;
    }

    virtual size_t  size() const { return m_size; }
    virtual string  toString();
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<mutex> lock(m_mtx);
        Node* pTemp = m_pRoot;
        for(size_t i = 0; i < m_size; ++i){
            func(*pTemp, std::forward<Args>(args)...);
            pTemp = pTemp->getNext();
        }
    }

    //Agregar FirstThat
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&...  args){
        Node* pTemp = m_pRoot;
        for(size_t i = 0; i < m_size; ++i){
            if(func(*pTemp, std::forward<Args>(args)...))
                return forward_iterator(this, pTemp);
            pTemp = pTemp->getNext();
        }
        return end();
    }
};

template <typename Traits>
void CircularLinkedList<Traits>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!m_pRoot){
        m_pRoot = new Node(value, ref);
        m_pRoot->setNext(m_pRoot);
        m_pTail = m_pRoot;
        ++m_size;
        return;
    }
    if(m_comp(value, m_pRoot->getDataRef())){
        Node* pNew = new Node(value, ref, m_pRoot);
        m_pRoot = pNew;
        m_pTail->setNext(m_pRoot);
        ++m_size;
        return;
    }
    if(pPrev == m_pTail ||
       m_comp(value, pPrev->getNext()->getDataRef())){
        Node* pNew = new Node(value, ref, pPrev->getNext());
        pPrev->setNext(pNew);
        if(pPrev == m_pTail)
            m_pTail = pNew;
        ++m_size;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Traits>
void CircularLinkedList<Traits>::insert(const value_type &value, Ref ref){
    internal_insert(m_pRoot, value, ref);
}

template <typename Traits>
string CircularLinkedList<Traits>::toString(){
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    for(size_t i = 0; i < m_size; ++i){
        ss << *pNode;
        if(i != m_size - 1)
            ss << ",";
        pNode = pNode->getNext();
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, CircularLinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
istream& operator>>(istream& is, CircularLinkedList<Traits>& list){
    using value_type = typename CircularLinkedList<Traits>::value_type;
    string line;

    getline(is, line);

    for (char& c : line){
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',')
            c = ' ';
    }

    value_type value;
    Ref ref;
    stringstream ss(line);

    while (ss >> value >> ref){
        list.push_back(value, ref);
    }

    return is;
}

#endif // __CIRCULARLINKEDLIST_H__
