#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H
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

    Node*  getNext() const { return static_cast<Node*>(this->m_pNext); }
    Node*& getNextRef() { return reinterpret_cast<Node*&>(this->m_pNext); }
    void   setNext(Node* pNext) { this->m_pNext = pNext; }
    Node*  getPrev() const { return m_pPrev; } // Devuelve el nodo anterior
    Node*& getPrevRef() { return m_pPrev; } // Devuelve una referencia al puntero del nodo anterior para poder modificarlo
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; } // Permite actualizar el nodo anterior
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
public:
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using Parent     = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = typename Parent::forward_iterator;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

// private:
//     Node *m_pHead;
//     Node *m_pTail;
//     size_t m_size;
//     mutex  m_mtx;

public:
    DoubleLinkedList() = default;

    //COPY CONSTRUCTOR
    DoubleLinkedList(const DoubleLinkedList &other){
        scoped_lock<mutex> lock(other.m_mtx);
        Node* pTemp = other.m_pRoot;

        while(pTemp != nullptr){
            push_back(pTemp -> getData(), pTemp -> getRef());
            pTemp = pTemp->getNext();
        }
    }

    DoubleLinkedList(DoubleLinkedList &&other){
        scoped_lock<mutex> lock(other.m_mtx);
        this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
        this->m_pTail = std::exchange(other.m_pTail, nullptr);
        this->m_size = std::exchange(other.m_size, 0);
    }

    // DESTRUCTOR
    ~DoubleLinkedList() override = default;

    
    size_t size () const override { return this->m_size; }
    bool isEmpty() const { return this->m_pRoot == nullptr; }
    
    //INSERT 
    void insert(const value_type &value, Ref ref) override{
        // TODO: insertar el nodo hacia adelante (como en la LinkedList)
        // adicionalmente conectar el nodo anterior con su nuevo siguiente
        // usar internal insert pero debe devolver el nuevo nodo creado y 
        // el puntero al lnodo anterior

        scoped_lock<mutex> lock(this->m_mtx);
        Parent::insert(value, ref); // Reutilizamos la función de inserción de LinkedList

        Node* current = this->m_pRoot; 
        Node* prev= nullptr;

        while(current != nullptr){ 
            current->setPrev(prev);
            prev = current; //

            if(current->getNext() == nullptr)
                this->m_pTail = current;

            prev = current;
            current = current->getNext();
        }
    }

    void push_back(value_type value, Ref ref) override{ // Insertar al final de la lista
        scoped_lock<mutex> lock(this->m_mtx);
        
        Node* pTemp = new Node(value, ref, nullptr, this->m_pTail);  //Como es el ultimo nodo no apunta a nada

        if (this->m_size == 0){
            this->m_pRoot = pTemp;
            this->m_pTail = pTemp; 
        } else {
            this->m_pTail->setNext(pTemp);
            pTemp->setPrev(this->m_pTail);
            this->m_pTail = pTemp;
        }

        ++this->m_size;
    };

    auto pop_back() -> pair<value_type, Ref> override{
        return pair<value_type, Ref>();
    }

    forward_iterator begin()   { return forward_iterator(this, this->m_pRoot); }
    forward_iterator end()     { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_pTail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(rbegin(), rend(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        return ::FirstThat(rbegin(), rend(), func, std::forward<Args>(args)...);
    }

};

    //Operator<< para imprimir la lista
    template <typename Traits>
    ostream& operator<<(ostream& os, DoubleLinkedList<Traits>& list){
        return os << static_cast<LinkedList<Traits>&>(list); // Reutilizamos el operador << de LinkedList para imprimir la parte común de la lista
    }
    //Operator>> para leer la lista
    template <typename Traits>
    istream& operator>>(istream& is, DoubleLinkedList<Traits>& list){
        return is >> static_cast<LinkedList<Traits>&>(list); // Reutilizamos el operador >> de LinkedList para leer la parte común de la lista
    }

#endif