#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__


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

// Backward iterator
template <typename Container>
class DoubleLinkedListBackwardIterator : public general_iterator<Container, 
                                        DoubleLinkedListBackwardIterator<Container>>{
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    using Node   = typename Container::Node; // Allias para el DLLNode
public:
    MySelf& operator++(){
    if(this->m_pNode){
        this->m_pNode = static_cast<Node*>(this->m_pNode->getPrev());
    }
    //this->m_pNode = this->m_pNode->getPrev();
    return *this;
    }

    MySelf operator++(int){
        MySelf temp = *this;
        ++(*this);
        return temp;
    }
};

template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;
    
    forward_iterator begin()   { return forward_iterator(this, this->m_pRoot); }
    forward_iterator end()     { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_pTail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

private:
    void internal_insert(Node* &pCurrent, Node* pPrev, const value_type &value, Ref ref);


public:
    DoubleLinkedList(){
        this->m_pRoot = nullptr;
        this->m_pTail = nullptr;
        this->m_size = 0;
    }
    
    // Copy constructor
    DoubleLinkedList(DoubleLinkedList &other){
        this->m_pRoot = this->m_pTail = nullptr;
        this->m_size = 0;

        scoped_lock<mutex> lock(const_cast<mutex&>(other.m_mtx));

        Node* pTemp = static_cast<Node*>(other.m_pRoot); 
        while (pTemp != nullptr) {
            this->push_back(pTemp->getData(), pTemp->getRef());
            pTemp = static_cast<Node*>(pTemp->getNext());
        }
    }

    DoubleLinkedList(DoubleLinkedList &&other){
        scoped_lock<mutex> lock(this->m_mtx);
        this->m_pRoot = exchange(other.m_pRoot, nullptr);
        this->m_pTail = exchange(other.m_pTail, nullptr);
        this->m_size = exchange(other.m_size, 0);
    }
    
    virtual ~DoubleLinkedList() {
        scoped_lock<mutex> lock(this->m_mtx);
        Node* pCurrent = static_cast<Node*>(this->m_pRoot);
        while (pCurrent != nullptr) {
            Node* pNext = static_cast<Node*>(pCurrent->getNext());
            delete pCurrent;
            pCurrent = pNext;
        }
        this->m_pRoot = nullptr;
        this->m_pTail = nullptr;
        this->m_size = 0;
    }
    
    size_t size () const { return this->m_size; }
    bool isEmpty() const { return this->m_pRoot == nullptr; }
    
    void insert(value_type value, Ref ref);
        // TODO: insertar la el nodo hacia adelante (como en la LinkedList)
        // adicionalmente conectar el nodo anterior con su nuevo siguiente
        // usar internal insert pero debe devolver el nuevo nodo creado y 
        // el puntero al lnodo anterior
    void push_back(value_type value, Ref ref);
    //virtual auto pop_back() -> pair<value_type, Ref> override;

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }

    template <typename U>
    friend istream& operator>>(istream& is, DoubleLinkedList<U>& list);

    virtual string toString() override {
        stringstream ss;
        auto* pNode = this->m_pRoot; 
        
        ss << "[";
        if (this->m_size > 0) {
            while (pNode != nullptr) {
                ss << *pNode; 
                pNode = static_cast<Node*>(pNode->getNext());
                if (pNode != nullptr) ss << ",";
            }
        }
        ss << "]";
        return ss.str();
    }
};

template <typename Traits>
void DoubleLinkedList<Traits>::internal_insert(Node* &pCurrent, Node* pPrev, const value_type &value, Ref ref) {
    if (!pCurrent || this->m_comp(value, pCurrent->getDataRef())) {
        Node* pNew = new Node(value, ref, pCurrent, pPrev);
        if (pCurrent) {
            pCurrent->setPrev(pNew);
        } else {
            this->m_pTail = pNew;
        }
        pCurrent = pNew;
        this->m_size++;
        return;
    }

    Node* &pNext = reinterpret_cast<Node*&>(pCurrent->getNextRef());
    internal_insert(pNext, pCurrent, value, ref);
}

template <typename Traits>
void DoubleLinkedList<Traits>::insert(value_type value, Ref ref) {
    scoped_lock<mutex> lock(this->m_mtx);
    internal_insert(this->m_pRoot, nullptr, value, ref);
}

template <typename Traits>
istream& operator>>(istream& is, DoubleLinkedList<Traits>& list) {
    using value_type = typename Traits::value_type;
    string line;
    if (!getline(is, line) || line.empty()) {
        return is;
    }
    for (char& c : line) {
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',' || c == ';') {
            c = ' ';
        }
    }
    //Extracción de datos
    stringstream ss(line);
    value_type value;
    Ref ref;
    while (ss >> value >> ref) {
        list.insert(value, ref); 
    }
    return is;
}

template <typename Traits>
void DoubleLinkedList<Traits>::push_back(value_type value, Ref ref) {
    scoped_lock<mutex> lock(this->m_mtx);
    // Creamos el nuevo nodo. Su 'prev' será el actual m_pTail.
    Node* pNew = new Node(value, ref, nullptr, static_cast<Node*>(this->m_pTail));

    if (this->m_pTail == nullptr) {
        // Si la lista estaba vacía
        this->m_pRoot = this->m_pTail = pNew;
    } else {
        // Conectamos el viejo tail con el nuevo nodo
        this->m_pTail->setNext(pNew);
        // Actualizamos el tail de la lista
        this->m_pTail = pNew;
    }
    this->m_size++;
}

#endif // __DOUBLELINKEDLIST_H__