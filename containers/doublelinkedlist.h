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
    DLLNode() : LLNode<T>(), m_pPrev(nullptr) {}
    DLLNode(T data, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr)
        : LLNode<T>(data, ref, pNext), m_pPrev(pPrev) {}

    Node* getPrev() const { return m_pPrev; }
    Node*& getPrevRef() { return m_pPrev; }
    void   setPrev(Node* pPrev) { m_pPrev = pPrev; }
};
template <typename T>
ostream &operator<<(ostream &os, const DLLNode<T> &node){
    return os << node.ToString();
}
template <typename T>
istream &operator>>(istream &is, DLLNode<T> &node){
    return node.fromIstream(is);
}

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
class DoubleLinkedList: public LinkedList<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

//private:
//    Node *m_pHead;
//    Node *m_pTail;
//    size_t m_size;
//    mutex  m_mtx;
protected:
    void on_node_inserted(Node* pNewNode, Node* pParent, Node* pNextNode) override {

        pNewNode->setPrev(pParent);

        if (pNextNode != nullptr) {
            pNextNode->setPrev(pNewNode);
        }
    }
    void on_node_removed(Node* pRemovedNode, Node* pParent, Node* pNextNode) override {
        if (pParent != nullptr) {
            pParent->setNext(pNextNode);
        }
        if (pNextNode != nullptr) {
            pNextNode->setPrev(pParent);
        }
    }
public:
    // Se delega la inicialización de m_pRoot, m_pTail, m_size al constructor base
    DoubleLinkedList() : LinkedList<Traits>() {}
    
    DoubleLinkedList(const DoubleLinkedList &other) : LinkedList<Traits>(other) {
        //scoped_lock<mutex> lock(other.m_mtx);
        //Node* pTemp = other.m_pRoot;
        //while (pTemp != nullptr) {
        //    push_back(pTemp->getData(), pTemp->getRef());
        //    pTemp = pTemp->getNext();
        //}
    }
    DoubleLinkedList(DoubleLinkedList &&other): LinkedList<Traits>(std::move(other)) {
        //scoped_lock<mutex> lock(other.m_mtx);
        //m_pRoot = exchange(other.m_pRoot, nullptr);
        //m_pTail = exchange(other.m_pTail, nullptr);
        //m_size = exchange(other.m_size, 0);
        //m_comp = move(other.m_comp);
    }
    //~DoubleLinkedList() {
    //   
    //}
    
    size_t size () const { return this->m_size; }
    bool isEmpty() const { return this->m_pRoot == nullptr; } // m_pRoot viene de la base
    
    void insert(const value_type &value, Ref ref) override {
        // TODO: insertar la el nodo hacia adelante (como en la LinkedList)
        // adicionalmente conectar el nodo anterior con su nuevo siguiente
        // usar internal insert pero debe devolver el nuevo nodo creado y 
        // el puntero al lnodo anterior
        //Nonde find_node_ref(pPrev, value);
        
        // Se reemplaza m_pHead por this->m_pRoot para respetar la clase base
        this->internal_insert(nullptr, this->m_pRoot, value, ref);
    }
    
    //void push_back(value_type value, Ref ref) override;
    //virtual auto pop_back() -> pair<value_type, Ref> override;



    //forward_iterator begin()   { return forward_iterator(this, this->m_pRoot); }
    //forward_iterator end()     { return forward_iterator(this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_pTail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

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
};
template <typename Traits>
ostream &operator<<(ostream &os, DoubleLinkedList<Traits> &list){
    return os << list.toString();
}
template <typename Traits>
istream &operator>>(istream &is, DoubleLinkedList<Traits> &list){
    return list.fromIstream(is);
}



#endif // __DOUBLELINKEDLIST_H__