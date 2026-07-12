#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include "linkedlist.h"

template <typename T>
struct BaseCircularLinkedListTrait : public BaseContainerTrait<T, LLNode<T>>{};

template <typename T>
struct AscendingCircularLinkedListTrait : public BaseCircularLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingCircularLinkedListTrait : public BaseCircularLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Container>
class CircularLinkedListForwardIterator
    : public general_iterator<Container, CircularLinkedListForwardIterator<Container>>
{
    using MySelf = CircularLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Parent::Parent;

    MySelf& operator++(){
        this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getNext());
        if (this->m_pNode == this->m_pContainer->getRoot())
            this->m_pNode = nullptr;     // cerró el ciclo → end()
        return *this;
    }
};

template <typename Traits>
class CircularLinkedList : public LinkedList<Traits>
{
public:
    using value_type       = typename Traits::value_type;
    using Node             = typename Traits::Node;
    using Comp             = typename Traits::Comp;
    using MySelf           = CircularLinkedList<Traits>;
    using forward_iterator = CircularLinkedListForwardIterator<MySelf>;

    Node* getRoot() const { return this->m_pRoot; }

protected:
    // Cierra el anillo tail → root después de cada inserción
    void on_node_inserted(Node*, Node*, Node*) override {
        if (this->m_pTail && this->m_pRoot)
            this->m_pTail->setNext(this->m_pRoot);
    }

    // Re-cierra el anillo después de cada eliminación
    void on_node_removed(Node*, Node*, Node*) override {
        if (this->m_pTail && this->m_pRoot)
            this->m_pTail->setNext(this->m_pRoot);
        else if (this->m_pTail)
            this->m_pTail->setNext(nullptr);
    }

public:
    CircularLinkedList() : LinkedList<Traits>() {}

    // Copia: recorre la lista circular completa
    CircularLinkedList(const CircularLinkedList& other) {
        if (!other.m_pRoot) return;
        Node* pTemp = other.m_pRoot;
        do {
            push_back(pTemp->getData(), pTemp->getRef());
            pTemp = static_cast<Node*>(pTemp->getNext());
        } while (pTemp != other.m_pRoot);
    }

    CircularLinkedList(CircularLinkedList&& other) : LinkedList<Traits>(std::move(other)) {}

    // Abre el anillo antes de que el destructor base libere los nodos,
    // para evitar un bucle infinito al recorrer next hasta nullptr.
    ~CircularLinkedList() override {
        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_pTail)
            this->m_pTail->setNext(nullptr);
    }

    void push_back(value_type value, Ref ref) override {
        Node* pNew = new Node(value, ref, this->m_pRoot);

        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_size == 0){
            pNew->setNext(pNew);          // auto-referencia: un solo elemento
            this->m_pRoot = pNew;
            this->m_pTail = pNew;
        } else {
            this->m_pTail->setNext(pNew);
            this->m_pTail = pNew;
            this->m_pTail->setNext(this->m_pRoot);
        }
        ++this->m_size;
    }

    auto pop_back() -> pair<value_type, Ref> override {
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_pRoot)
            throw out_of_range("pop_back(): empty circular list");

        auto result = make_pair(this->m_pTail->getData(), this->m_pTail->getRef());

        if (this->m_size == 1){
            delete this->m_pRoot;
            this->m_pRoot = this->m_pTail = nullptr;
            --this->m_size;
            return result;
        }

        Node* pTemp = this->m_pRoot;
        while (static_cast<Node*>(pTemp->getNext()) != this->m_pTail)
            pTemp = static_cast<Node*>(pTemp->getNext());

        delete this->m_pTail;
        this->m_pTail = pTemp;
        this->m_pTail->setNext(this->m_pRoot);
        --this->m_size;
        return result;
    }

    void insert(const value_type& value, Ref ref) override {
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_size == 0){
            Node* pNew = new Node(value, ref, nullptr);
            pNew->setNext(pNew);
            this->m_pRoot = this->m_pTail = pNew;
            ++this->m_size;
            return;
        }

        // Abre el anillo para que internal_insert termine en nullptr
        this->m_pTail->setNext(nullptr);
        this->internal_insert(nullptr, this->m_pRoot, value, ref);
        // on_node_inserted re-cierra el anillo vía el hook
    }

    //string toString() override {
    //    if (this->m_size == 0) return "[]";
    //    stringstream ss;
    //    ss << "[";
    //    Node* pNode = this->m_pRoot;
    //    for (size_t i = 0; i < this->m_size - 1; ++i){
    //        ss << *pNode << ",";
    //        pNode = static_cast<Node*>(pNode->getNext());
    //    }
    //    ss << *pNode << "]";
    //    return ss.str();
    //}

    //forward_iterator begin() { return forward_iterator(this, this->m_pRoot); }
    //forward_iterator end()   { return forward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args){
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args){
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, CircularLinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
istream& operator>>(istream& is, CircularLinkedList<Traits>& list){
    return list.fromIstream(is);
}

#endif // __CIRCULARLINKEDLIST_H__
