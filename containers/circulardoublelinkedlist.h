#ifndef __CIRCULAR_DOUBLE_LINKED_LIST_H__
#define __CIRCULAR_DOUBLE_LINKED_LIST_H__

#include "doublelinkedlist.h"
#include "circularlinkedlist.h" // reusa CircularLLForwardIterator

// Iterador circular hacia atrás: análogo al CircularLLForwardIterator pero
// avanza con getPrev(); termina cuando regresa al nodo de inicio (tail).
template <typename Container>
class CircularDLLBackwardIterator : public general_iterator<Container,
                                            CircularDLLBackwardIterator<Container>>{
    using MySelf = CircularDLLBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    Node *m_pStart;
public:
    CircularDLLBackwardIterator(Container *pContainer, Node *pNode, Node *pStart)
        : Parent(pContainer, pNode), m_pStart(pStart) {}

    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getPrev();
        if(this->m_pNode == m_pStart)
            this->m_pNode = nullptr;
        return *this;
    }
};

template <typename Traits>
class CircularDoubleLinkedList : public DoubleLinkedList<Traits>{
public:
    using Base       = DoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using MySelf     = CircularDoubleLinkedList<Traits>;

    using forward_iterator  = CircularLLForwardIterator<MySelf>;
    using backward_iterator = CircularDLLBackwardIterator<MySelf>;

private:
    void open_circle(){
        if(this->m_pTail){
            static_cast<Node*>(this->m_pTail)->setNext(nullptr);
        }
        if(this->m_pRoot){
            static_cast<Node*>(this->m_pRoot)->setPrev(nullptr);
        }
    }
    void close_circle(){
        if(this->m_pTail && this->m_pRoot){
            Node* pTail = static_cast<Node*>(this->m_pTail);
            Node* pRoot = static_cast<Node*>(this->m_pRoot);
            pTail->setNext(pRoot);
            pRoot->setPrev(pTail);
        }
    }

public:
    CircularDoubleLinkedList() = default;

    CircularDoubleLinkedList(const CircularDoubleLinkedList &other){
        Node* p = static_cast<Node*>(other.m_pRoot);
        if(!p) return;
        do {
            push_back(p->getData(), p->getRef());
            p = p->getNext();
        } while(p != other.m_pRoot);
    }

    CircularDoubleLinkedList(CircularDoubleLinkedList &&other) noexcept
        : Base(std::move(other)) {}

    virtual ~CircularDoubleLinkedList() override {
        open_circle(); // permitir que el destructor de la base recorra hasta nullptr
    }

    // Adaptación de la inserción
    void push_back(value_type value, Ref ref) override {
        open_circle();
        Base::push_back(value, ref);
        close_circle();
    }

    void push_front(value_type value, Ref ref) override {
        open_circle();
        Base::push_front(value, ref);
        close_circle();
    }

    void insert(const value_type &value, Ref ref) override {
        open_circle();
        Base::insert(value, ref);
        close_circle();
    }

    // Adaptación del recorrido
    forward_iterator  begin()  { return forward_iterator (this, static_cast<Node*>(this->m_pRoot), static_cast<Node*>(this->m_pRoot)); }
    forward_iterator  end()    { return forward_iterator (this, nullptr,                              static_cast<Node*>(this->m_pRoot)); }
    backward_iterator rbegin() { return backward_iterator(this, static_cast<Node*>(this->m_pTail), static_cast<Node*>(this->m_pTail)); }
    backward_iterator rend()   { return backward_iterator(this, nullptr,                              static_cast<Node*>(this->m_pTail)); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        ::ForEach(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        ::ForEach(rbegin(), rend(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        return ::FirstThat(rbegin(), rend(), func, std::forward<Args>(args)...);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, CircularDoubleLinkedList<Traits>& list){
    return os << static_cast<LinkedList<Traits>&>(list);
}

#endif // __CIRCULAR_DOUBLE_LINKED_LIST_H__
