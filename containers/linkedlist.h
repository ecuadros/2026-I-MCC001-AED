#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

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
#include "basenode.h"

using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, 
                                            LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getNext());
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode : public BaseNode<T>{
public:
    using Parent = BaseNode<T>;
    using value_type = T;
    using Node       = LLNode<T>;
protected:
    Node      *m_pNext;
public:
    LLNode(): Parent(){}
    LLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : Parent(data, ref), m_pNext(pNext) {}
    virtual ~LLNode() {}

    value_type      getData() const { return this->m_data; }
    value_type&     getDataRef()    { return this->m_data; }
    void            setData(value_type data) { this->m_data = data; }
    Ref             getRef() const  { return this->m_ref; }
    Ref&            getRefRef()     { return this->m_ref; }
    void            setRef(Ref ref) { this->m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
    virtual string ToString() const {
        ostringstream oss;
        oss << "(" << this->m_data << ", " << this->m_ref << ")";
        return oss.str();
    }
    virtual istream& fromIstream(istream &is){
        char ch1, ch2, ch3;
        if (is >> ch1 && ch1 == '(' &&
            is >> this->m_data &&
            is >> ch2 && ch2 == ',' &&
            is >> this->m_ref &&
            is >> ch3 && ch3 == ')') {
            return is;
        }
        is.setstate(ios::failbit);
        return is;
    }

};

template <typename T>
ostream &operator<<(ostream &os, const LLNode<T> &node){
    return os << node.ToString();
}
template <typename T>
istream &operator>>(istream &is, LLNode<T> &node){
    
    
    return node.fromIstream(is);
}

template <typename T>
using BaseLinkedListTrait = BaseNodeContainerTrait<T, LLNode>;

template <typename T>
using AscendingLinkedListTrait = AscendingNodeContainerTrait<T, LLNode>;

template <typename T>
using DescendingLinkedListTrait = DescendingNodeContainerTrait<T, LLNode>;

template <typename Traits>
class LinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = LinkedList<Traits>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutex m_mtx;
    

public:
    LinkedList() {}
    LinkedList(const LinkedList &other){ // Copy constructor
        //scoped_lock<mutex> lock(other.m_mtx);
        Node* pTemp = other.m_pRoot;

        while(pTemp != nullptr){
            push_back(pTemp -> getData(), pTemp -> getRef());
            pTemp = static_cast<Node*>(pTemp->getNext());
        }
            
    }
    LinkedList(LinkedList &&other){ // Move constructor
        
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);

    }
    LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
    }
    LinkedList& operator=(LinkedList &&other){ // Move assignment operator
    }
    
    virtual        ~LinkedList() {
        
        scoped_lock<mutex> lock(m_mtx);
        Node* pTemp = m_pRoot;

        while (pTemp){
            Node* pNext =  static_cast<Node*>(pTemp->getNext());
            delete pTemp;
            pTemp = pNext;
        }

        m_pRoot = nullptr;
        m_pTail = nullptr;
        m_size  = 0;
    }

    virtual void   push_front(value_type value, Ref ref) {
        Node* pTemp = new Node(value, ref, m_pRoot);  //Se crea el Nodo temporal con los datos ingresados que apunta a m_pRoot
        
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = pTemp;                              //Se actualiza el nodo raiz
        if (m_size == 0)
            m_pTail = pTemp;
        on_node_inserted(pTemp, nullptr, m_pRoot);  //Si la lista esta vacia la cola tambien se debe actualizar
      
        ++m_size;
    }
    virtual auto    pop_front() -> pair<value_type, Ref>{ 
        
        scoped_lock<mutex> lock(m_mtx);
        if( m_pRoot ){
            Node* pTemp = m_pRoot;
            m_pRoot =  static_cast<Node*>(m_pRoot->getNext());
            --m_size;
            on_node_removed(pTemp, nullptr, m_pRoot);
            return make_pair(pTemp->getData(), pTemp->getRef());

        }else
            throw out_of_range("pop_front(): empty list");
    }
    virtual void    push_back(value_type value, Ref ref){
        Node* pTemp = new Node(value, ref, nullptr);  //Como es el ultimo nodo no apunta a nada
        
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == 0){
            m_pRoot = pTemp;
            m_pTail = pTemp; 
        } else {
            m_pTail->setNext(pTemp);
            m_pTail = pTemp;
        } 
        on_node_inserted(pTemp, m_pTail, nullptr);  //Si la lista esta vacia la raiz tambien se debe actualizar                 
        ++m_size;
    }
    virtual auto    pop_back() -> pair<value_type, Ref>{
        
        scoped_lock<mutex> lock(m_mtx);
        if( !m_pRoot )
            throw out_of_range("pop_back(): empty list");
        
        //Con un solo elemento
        if( m_pRoot == m_pTail){
            auto pDelete = make_pair(m_pTail->getData(), m_pTail->getRef());

            delete m_pTail;
            m_pRoot = nullptr;
            m_pTail = nullptr;
            
            --m_size;
            on_node_removed(m_pTail, nullptr, nullptr);
            return pDelete;
        }

        //Lista con varios elementos
        Node *pTemp = m_pRoot;

        while (pTemp->getNext() != m_pTail)             //Recorrer toda la lista hasta el penultimo elemento
            pTemp = static_cast<Node*>(pTemp->getNext());

        auto pDelete = make_pair(m_pTail->getData(), m_pTail->getRef());
        
        delete static_cast<Node*>(pTemp->getNext());
        pTemp->setNext(nullptr);

        m_pTail = pTemp;
        
        --m_size;
        on_node_removed(m_pTail, pTemp, nullptr);
        return pDelete;
    }
protected:
    Node* internal_insert(Node* pParent, Node* &pCurrent, const value_type &value, Ref ref);
    Node* find_node_ref(Node* &pPrev, const value_type &value);
    virtual void on_node_inserted(Node* pNewNode, Node* pParent, Node* pNextNode) {} 
    virtual void on_node_removed(Node* pRemovedNode, Node* pParent, Node* pNextNode) {}
    //virtual void on_node_
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual Node& operator[](const size_t index) const{
        
        if (index >= m_size)
        throw out_of_range("Index out of range");

        Node* pTemp = m_pRoot;
        for (size_t i = 0; i < index; ++i){
            pTemp =  static_cast<Node*>(pTemp -> getNext());
        }
        return *pTemp;
    };

    virtual size_t  size() const { return m_size; }
    virtual string  toString();
    virtual istream& fromIstream(istream &is){
        scoped_lock<mutex> lock(m_mtx);
        char ch;
        if(!(is >> ch) || ch != '['){
            is.setstate(ios::failbit);
            return is;
        }
        
        while (true){
            value_type value;
            Ref ref;
            
            if(!(is >> ch) || ch != '('){
                is.setstate(ios::failbit);
                return is;
            }
            if(!(is >> value)){
                is.setstate(ios::failbit);
                return is;
            }
            if(!(is >> ch) || ch != ','){
                is.setstate(ios::failbit);
                return is;
            }
            if(!(is >> ref)){
                is.setstate(ios::failbit);
                return is;
            }
            if(!(is >> ch) || ch != ')'){
                is.setstate(ios::failbit);
                return is;
            }

            insert(value, ref);

            is >> ch;

            if(ch == ']') break;
            if (ch != ',') {         
                is.setstate(ios::failbit);
                return is;
            }
        }
        return is;

    }
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

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
};
template <typename Traits>
typename LinkedList<Traits>::Node* LinkedList<Traits>::find_node_ref(Node* &pPrev, const value_type &value){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        return pPrev;
    }
    return find_node_ref(pPrev->getNextRef(), value);
}


template <typename Traits>
typename LinkedList<Traits>::Node* LinkedList<Traits>::internal_insert(Node* pParent, Node* &pCurrent, const value_type &value, Ref ref){
    //pPrev = find_node_ref(pPrev, value);
    //Node *newNode = new Node(value, ref, pPrev);
    //pPrev = newNode;
    //++m_size;
    //if(newNode->getNext() == nullptr){
    //    m_pTail = newNode;
    //}

    //if(!pPrev || m_comp(value, pPrev->getDataRef())){
    //    Node* newNode = new Node(value, ref, pPrev);
    //    pPrev = newNode;
    //    ++m_size;
    //    
    //    //if(pPrev == m_pRoot)
    //    //    m_pTail = pPrev;
    //    if(newNode->getNext() == nullptr ){
    //        m_pTail = newNode;
    //    }   
    //    
    //    return newNode;
    //}
    //return internal_insert(pPrev->getNextRef(), value, ref);


    if(!pCurrent || m_comp(value, pCurrent->getDataRef())){
        Node* newNode = new Node(value, ref, pCurrent); 
        
        on_node_inserted(newNode, pParent, pCurrent);
        
        pCurrent = newNode;
        ++m_size;
        
        if(newNode->getNext() == nullptr ){
            m_pTail = newNode;
        }   
        
        return newNode;
    }

    return internal_insert(pCurrent, reinterpret_cast<Node*&>(pCurrent->getNextRef()), value, ref);
}

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){
    internal_insert(nullptr,m_pRoot, value, ref);
}

template <typename Traits>
string  LinkedList<Traits>::toString() {
    scoped_lock<mutex> lock(m_mtx);
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size()-1 ; ++i ){
            ss << *pNode << ",";
            pNode =  static_cast<Node*>(pNode->getNext());
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, LinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
istream& operator>>(istream& is, LinkedList<Traits>& list){
    return list.fromIstream(is);
}

#endif // __LINKEDLIST_H__