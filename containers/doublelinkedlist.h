#ifndef __DOUBLE_LINKEDLIST_H__
#define __DOUBLE_LINKEDLIST_H__

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
        : LLNode(data, ref, pNext), m_pPrev(pPrev){}

    Node*  getPrev() const { return m_pPrev; }
    Node*& getPrevRef() { return m_pPrev; }
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};

template <typename T>
struct BaseDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>>{

};

template <typename T>
struct AscecndingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

// Reutiizar el LinkedListForwardIterator de la linked list

//////////////////////////////////////
//////////////////////////////////////
template <typename Container>
class DoubleLinkedListForwardIterator : public general_iterator<Container, 
                                            DoubleLinkedListForwardIterator<Container>>{
    using MySelf = DoubleLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
private:
     Container *m_pComp = nullptr;
     Node      *m_pNode = nullptr;
public:
     DoubleLinkedListForwardIterator(Container *pComp, Node *pNode)
             : m_pComp(pComp), m_pNode(pNode){}
     DoubleLinkedListForwardIterator(iterator &other)
             : m_pComp(other.m_pComp), m_pNode(other.m_pNode){}   
     TB operator==(iterator other){return m_pComp == other.m_pComp && m_pNode == other.m_pNode; }
     TB operator!=(iterator other){return !(*this == other); }

     iterator operator++(){ 
         if(m_pNode)
             m_pNode = m_pNode->GetNext();
         return *this;
     }
     value_type &operator*(){return m_pNode->GetDataRef(); }
};

//////////////////////////////////////
//////////////////////////////////////


// Backward iterator
template <typename Container>
class DoubleLinkedListBackwardIterator : public general_iterator<Container, 
                                        DoubleLinkedListBackwardIterator<Container>>{
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

 private:
     Container *m_pComp = nullptr;
     Node      *m_pNode = nullptr;
 public:
     DoubleLinkedListBackwardIterator(Container *pComp, Node *pNode)
             : m_pComp(pComp), m_pNode(pNode){}
     DoubleLinkedListBackwardIterator(iterator &other)
             : m_pComp(other.m_pComp), m_pNode(other.m_pNode){}   
     TB operator==(iterator other){ return m_pComp == other.m_pComp && m_pNode == other.m_pNode; }
     TB operator!=(iterator other){ return !(*this == other);    }

     iterator operator++(){ 
         if(m_pNode)
             m_pNode = m_pNode->GetPrev();
         return *this;
     }
     value_type &operator*(){    return m_pNode->GetDataRef();   }
};

template <typename Traits>
class CDoubleLinkedList public LinkedList<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = CDoubleLinkedList<Traits>;

    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

private:
    Node *m_pHead;
    Node *m_pTail;
    size_t m_size;
    mutex  m_mtx;

public:
    CDoubleLinkedList() : m_pHead(nullptr), m_pTail(nullptr), m_size(0) {}
    CDoubleLinkedList(CDoubleLinkedList &other){// Copy constructor
        Node* pTemp = other.m_pRoot;

        while(pTemp != nullptr){
            push_back(pTemp -> getData(), pTemp -> getRef());
            pTemp = pTemp->getNext();
        }

    }
    CDoubleLinkedList(CDoubleLinkedList &&other){
        scoped_lock<mutex> lock(m_mtx);
        m_pHead = exchange(other.m_pHead, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size = exchange(other.m_size, 0);
    }

    DoubleLinkedListForwardIterator& operator=(const cLinkedList &other){ // Copy assignment operator
    }
    DoubleLinkedList& operator=(LinkedList &&other){ // Move assignment operator
    }

    ~cDoubleLinkedList() {
        scoped_lock<mutex> lock(m_mtx);
        while (m_pHead != nullptr) {
            Node *pTemp = m_pHead;
            m_pHead = m_pHead->getNextRef();
            delete pTemp;
        }
        m_pHead = nullptr;
        m_pTail = nullptr;
        m_size = 0;
    }
    
    size_t size () const { return m_size; }
    bool isEmpty() const { return m_pHead == nullptr; }


    
    voidd insert(value_type value, Ref ref){
        // TODO: insertar la el nodo hacia adelante (como en la LinkedList)
        // adicionalmente conectar el nodo anterior con su nuevo siguiente
        // usar internal insert pero debe devolver el nuevo nodo creado y 
        // el puntero al lnodo anterior
    }
    void    push_back(value_type value, Ref ref){
        Node* pTemp = new Node(value, ref, nullptr);  //Como es el ultimo nodo no apunta a nada
        
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == 0){
            m_pRoot = pTemp;
            m_pTail = pTemp; 
        } else {
            m_pTail->setNext(pTemp);
            m_pTail = pTemp;
        }                  
        ++m_size;
    }


    value_type pop_back();

    forward_iterator begin()   { return forward_iterator(this, m_pHead); }
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

}

template <typename T>
ostream &operator<<(ostream &os, const DLLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}

template <typename Traits>
string  CDoubleLinkedList<Traits>::toString() {
    stringstream ss;
    Node *pNode = m_pHead;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size()-1 ; ++i ){
            ss << *pNode << ",";
            pNode = pNode->getNext();
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
istream& operator>>(istream& is, CDoubleLinkedList<Traits>& list){
    using value_type = typename CDoubleLinkedList<Traits>::value_type;
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

};


void DemoDoubleLinkedList();

#endif // __DOUBLE_LINKEDLIST_H__