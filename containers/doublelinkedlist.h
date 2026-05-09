#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include "linkedlist.h"

// ----------------------------------------------------------------------------
// DLLNode: extiende LLNode<T> agregando puntero al nodo previo.
// Re-expone getNext / getNextRef devolviendo el tipo derivado para que el
// iterador y el insert reutilizado de LinkedList funcionen sin downcasts.
// ----------------------------------------------------------------------------
template <typename T>
class DLLNode : public LLNode<T>{
public:
    using value_type = T;
    using Node       = DLLNode<T>;
    using Base       = LLNode<T>;
private:
    Node *m_pPrev;
public:
    DLLNode(T data, Ref ref, Node *pNext = nullptr, Node *pPrev = nullptr)
        : Base(data, ref, pNext), m_pPrev(pPrev) {}

    Node*  getNext()    const { return static_cast<Node*>(Base::getNext()); }
    Node*& getNextRef()       { return reinterpret_cast<Node*&>(Base::getNextRef()); }

    Node*  getPrev()    const { return m_pPrev; }
    Node*& getPrevRef()       { return m_pPrev; }
    void   setPrev(Node *pPrev) { m_pPrev = pPrev; }
};

// ----------------------------------------------------------------------------
// Traits para DoubleLinkedList
// ----------------------------------------------------------------------------
template <typename T>
struct BaseDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>>{};

template <typename T>
struct AscendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingDoubleLinkedListTrait : public BaseDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

// ----------------------------------------------------------------------------
// Forward iterator: se reutiliza el LinkedListForwardIterator.
// (Ver linkedlist.h)  Solo añadimos un alias por claridad.
// ----------------------------------------------------------------------------

// Backward iterator: avanza usando getPrev().
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

// ----------------------------------------------------------------------------
// DoubleLinkedList: hereda de LinkedList<Traits>.
//   - Reutiliza:  destructor, internal_insert, insert, operator[], toString,
//                 operator<<, operator>>, ForEach, FirstThat, push_front
//                 (ver más abajo el override mínimo para enlazar prev).
//   - Sobreescribe push_front, push_back y on_node_inserted para mantener
//     el puntero m_pPrev consistente.
//   - Agrega backward_iterator, rbegin/rend, ReverseForEach y
//     ReverseFirstThat.
// ----------------------------------------------------------------------------
template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using Base       = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = DoubleLinkedList<Traits>;

    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

public:
    DoubleLinkedList() = default;

    // Constructor copia: recorre la otra lista y replica con push_back, que
    // ya enlaza prev correctamente.
    DoubleLinkedList(const DoubleLinkedList &other) : Base() {
        for(Node* p = other.m_pRoot; p != nullptr; p = p->getNext())
            push_back(p->getData(), p->getRef());
    }

    // Constructor move: el de la base ya transfiere m_pRoot/m_pTail/m_size.
    DoubleLinkedList(DoubleLinkedList &&other) noexcept : Base(std::move(other)) {}

    // Destructor: la cadena de destructores virtuales llega al de LinkedList
    // que libera todos los nodos. No hace falta nada extra aquí.
    virtual ~DoubleLinkedList() override = default;

    // Hook que LinkedList::internal_insert llama tras crear un nodo. Aquí
    // enlazamos m_pPrev del nodo nuevo y, si tiene siguiente, su prev.
    void on_node_inserted(typename Base::Node *pNew, typename Base::Node *pPrev) override {
        Node* pN = static_cast<Node*>(pNew);
        Node* pP = static_cast<Node*>(pPrev);
        pN->setPrev(pP);
        if(pN->getNext() != nullptr)
            pN->getNext()->setPrev(pN);
    }

    // push_front / push_back de LinkedList no actualizan m_pPrev, así que
    // los re-implementamos delegando al de la base + arreglo de prev.
    void push_front(value_type value, Ref ref) override {
        Node* pOldRoot = static_cast<Node*>(this->m_pRoot);
        Base::push_front(value, ref);
        Node* pNewRoot = static_cast<Node*>(this->m_pRoot);
        pNewRoot->setPrev(nullptr);
        if(pOldRoot)
            pOldRoot->setPrev(pNewRoot);
    }

    void push_back(value_type value, Ref ref) override {
        Node* pOldTail = static_cast<Node*>(this->m_pTail);
        Base::push_back(value, ref);
        Node* pNewTail = static_cast<Node*>(this->m_pTail);
        pNewTail->setPrev(pOldTail);
    }

    // Iteradores
    forward_iterator  begin()  { return forward_iterator (this, static_cast<Node*>(this->m_pRoot)); }
    forward_iterator  end()    { return forward_iterator (this, nullptr); }
    backward_iterator rbegin() { return backward_iterator(this, static_cast<Node*>(this->m_pTail)); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        ::ForEach(rbegin(), rend(), func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        return ::FirstThat(rbegin(), rend(), func, std::forward<Args>(args)...);
    }
};

// operator<< y operator>>: delegan al de LinkedList<Traits>. La deducción de
// plantillas no atraviesa la herencia, así que necesitamos estos wrappers
// de una línea — pero la lógica vive en LinkedList (no se duplica).
template <typename Traits>
ostream& operator<<(ostream& os, DoubleLinkedList<Traits>& list){
    return os << static_cast<LinkedList<Traits>&>(list);
}

template <typename Traits>
istream& operator>>(istream& is, DoubleLinkedList<Traits>& list){
    return is >> static_cast<LinkedList<Traits>&>(list);
}

#endif // __DOUBLE_LINKED_LIST_H__
