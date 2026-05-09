#ifndef __CIRCULAR_LINKED_LIST_H__
#define __CIRCULAR_LINKED_LIST_H__

#include "linkedlist.h"

// ----------------------------------------------------------------------------
// Iterador forward circular: como en una CLL m_pTail->next == m_pRoot, no
// existe un nullptr terminal. Para reutilizar el contrato de end() == nullptr
// del general_iterator, el iterador "se apaga" (m_pNode = nullptr) cuando al
// avanzar volvería al nodo de inicio.
// ----------------------------------------------------------------------------
template <typename Container>
class CircularLLForwardIterator : public general_iterator<Container,
                                            CircularLLForwardIterator<Container>>{
    using MySelf = CircularLLForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    Node *m_pStart;
public:
    CircularLLForwardIterator(Container *pContainer, Node *pNode, Node *pStart)
        : Parent(pContainer, pNode), m_pStart(pStart) {}

    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        if(this->m_pNode == m_pStart)
            this->m_pNode = nullptr; // alcanzó el inicio: fin de un ciclo
        return *this;
    }
};

// Reusamos los traits de LinkedList: una CLL almacena los mismos nodos
// (LLNode<T>); la única diferencia es la invariante m_pTail->next = m_pRoot.

template <typename Traits>
class CircularLinkedList : public LinkedList<Traits>{
public:
    using Base       = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using MySelf     = CircularLinkedList<Traits>;

    using forward_iterator = CircularLLForwardIterator<MySelf>;

private:
    // Helpers para abrir/cerrar el ciclo durante operaciones que reutilizan
    // el código de LinkedList (que asume terminación con nullptr).
    void open_circle()  { if(this->m_pTail) this->m_pTail->setNext(nullptr); }
    void close_circle() { if(this->m_pTail) this->m_pTail->setNext(this->m_pRoot); }

public:
    CircularLinkedList() = default;

    CircularLinkedList(const CircularLinkedList &other){
        Node* p = other.m_pRoot;
        if(!p) return;
        do {
            push_back(p->getData(), p->getRef());
            p = p->getNext();
        } while(p != other.m_pRoot);
    }

    CircularLinkedList(CircularLinkedList &&other) noexcept : Base(std::move(other)) {}

    // Destructor: rompe el ciclo y deja que el destructor de la base libere
    // los nodos recorriendo desde m_pRoot hasta nullptr.
    virtual ~CircularLinkedList() override {
        open_circle();
    }

    // Adaptación de la inserción: insertamos como en LinkedList y luego
    // restablecemos la invariante circular.
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

    // Adaptación del recorrido: los iteradores reportan end cuando dan una
    // vuelta completa.
    forward_iterator begin() { return forward_iterator(this, this->m_pRoot, this->m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr,        this->m_pRoot); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        ::ForEach(begin(), end(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, CircularLinkedList<Traits>& list){
    return os << static_cast<LinkedList<Traits>&>(list);
}

#endif // __CIRCULAR_LINKED_LIST_H__
