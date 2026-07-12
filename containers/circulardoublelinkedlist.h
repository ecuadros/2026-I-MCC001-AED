#ifndef __CIRCULARDOUBLELINKEDLIST_H__
#define __CIRCULARDOUBLELINKEDLIST_H__

#include "doublelinkedlist.h"

template <typename T>
struct BaseCircularDoubleLinkedListTrait : public BaseContainerTrait<T, DLLNode<T>>{};

template <typename T>
struct AscendingCircularDoubleLinkedListTrait : public BaseCircularDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingCircularDoubleLinkedListTrait : public BaseCircularDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Container>
class CircularDoubleLinkedListForwardIterator
    : public general_iterator<Container, CircularDoubleLinkedListForwardIterator<Container>>
{
    using MySelf = CircularDoubleLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Parent::Parent;

    MySelf& operator++(){
        this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getNext());
        if (this->m_pNode == this->m_pContainer->getRoot())
            this->m_pNode = nullptr;
        return *this;
    }
};

template <typename Container>
class CircularDoubleLinkedListBackwardIterator
    : public general_iterator<Container, CircularDoubleLinkedListBackwardIterator<Container>>
{
    using MySelf = CircularDoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Parent::Parent;

    MySelf& operator++(){
        this->m_pNode = static_cast<typename Container::Node*>(this->m_pNode->getPrev());
        if (this->m_pNode == this->m_pContainer->getTail())
            this->m_pNode = nullptr;
        return *this;
    }
};

template <typename Traits>
class CircularDoubleLinkedList : public DoubleLinkedList<Traits>
{
public:
    using value_type        = typename Traits::value_type;
    using Node              = typename Traits::Node;
    using Comp              = typename Traits::Comp;
    using MySelf            = CircularDoubleLinkedList<Traits>;
    using forward_iterator  = CircularDoubleLinkedListForwardIterator<MySelf>;
    using backward_iterator = CircularDoubleLinkedListBackwardIterator<MySelf>;

    Node* getRoot() const { return this->m_pRoot; }
    Node* getTail() const { return this->m_pTail; }

protected:
    
    void on_node_inserted(Node* pNew, Node* pParent, Node* pNext) override {
        if (pNew && pParent)
            pNew->setPrev(pParent);

        if (pNext){
            pNext->setPrev(pNew);
        }

        // Cierra el anillo: tail → root, root.prev → tail
        if (this->m_pTail && this->m_pRoot){
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
        }
    }

    void on_node_removed(Node* /*pRemoved*/, Node* pParent, Node* pNext) override {
        if (pParent){
            pParent->setNext(pNext);
        }
        if (pNext){
            pNext->setPrev(pParent);
        }

        // Re-cierra el anillo tras eliminación
        if (this->m_pTail && this->m_pRoot){
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
        } else if (this->m_pTail){
            this->m_pTail->setNext(nullptr);
            this->m_pTail->setPrev(nullptr);
        }
    }

public:
    
    CircularDoubleLinkedList() : DoubleLinkedList<Traits>() {}

    CircularDoubleLinkedList(const CircularDoubleLinkedList& other) {
        if (!other.m_pRoot) return;
        Node* pTemp = other.m_pRoot;
        do {
            push_back(pTemp->getData(), pTemp->getRef());
            pTemp = static_cast<Node*>(pTemp->getNext());
        } while (pTemp != other.m_pRoot);
    }

    CircularDoubleLinkedList(CircularDoubleLinkedList&& other) 
        : DoubleLinkedList<Traits>(std::move(other)) {}

    
    ~CircularDoubleLinkedList() override {
        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_pTail){
            this->m_pTail->setNext(nullptr);
        }
        if (this->m_pRoot){
            this->m_pRoot->setPrev(nullptr);
        }
    }

    void push_back(value_type value, Ref ref) override {
        Node* pNew = new Node(value, ref, this->m_pRoot, nullptr);

        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_size == 0){
            pNew->setNext(pNew);
            pNew->setPrev(pNew);        // auto-referencia bidireccional
            this->m_pRoot = pNew;
            this->m_pTail = pNew;
        } else {
            this->m_pTail->setNext(pNew);
            pNew->setPrev(this->m_pTail);
            this->m_pTail = pNew;
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
        }
        ++this->m_size;
    }


    auto pop_back() -> pair<value_type, Ref> override {
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_pRoot)
            throw out_of_range("pop_back(): empty circular double list");

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
        this->m_pRoot->setPrev(this->m_pTail);
        --this->m_size;
        return result;
    }

    
    void push_front(value_type value, Ref ref) override {
        Node* pNew = new Node(value, ref, this->m_pRoot, nullptr);

        scoped_lock<mutex> lock(this->m_mtx);
        if (this->m_size == 0){
            pNew->setNext(pNew);
            pNew->setPrev(pNew);
            this->m_pRoot = pNew;
            this->m_pTail = pNew;
        } else {
            pNew->setNext(this->m_pRoot);
            pNew->setPrev(this->m_pTail);
            this->m_pRoot->setPrev(pNew);
            this->m_pTail->setNext(pNew);
            this->m_pRoot = pNew;
        }
        ++this->m_size;
    }

    auto pop_front() -> pair<value_type, Ref> override {
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_pRoot)
            throw out_of_range("pop_front(): empty circular double list");

        auto result = make_pair(this->m_pRoot->getData(), this->m_pRoot->getRef());

        if (this->m_size == 1){
            delete this->m_pRoot;
            this->m_pRoot = this->m_pTail = nullptr;
            --this->m_size;
            return result;
        }

        Node* pNext = static_cast<Node*>(this->m_pRoot->getNext());
        delete this->m_pRoot;
        this->m_pRoot = pNext;
        this->m_pRoot->setPrev(this->m_pTail);
        this->m_pTail->setNext(this->m_pRoot);
        --this->m_size;
        return result;
    }

    void insert(const value_type& value, Ref ref) override {
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_size == 0){
            Node* pNew = new Node(value, ref, nullptr, nullptr);
            pNew->setNext(pNew);
            pNew->setPrev(pNew);
            this->m_pRoot = this->m_pTail = pNew;
            ++this->m_size;
            return;
        }

        // Abre el anillo para que internal_insert termine en nullptr
        this->m_pTail->setNext(nullptr);
        this->m_pRoot->setPrev(nullptr);
        this->internal_insert(nullptr, this->m_pRoot, value, ref);
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

    //forward_iterator begin()   { return forward_iterator(this, this->m_pRoot); }
    //forward_iterator end()     { return forward_iterator(this, nullptr); }
    //backward_iterator rbegin() { return backward_iterator(this, this->m_pTail); }
    //backward_iterator rend()   { return backward_iterator(this, nullptr); }

    //template <typename Func, typename... Args>
    //void ForEach(Func func, Args&&... args){
    //    scoped_lock<mutex> lock(this->m_mtx);
    //    ::ForEach(begin(), end(), func, forward<Args>(args)...);
    //}

    //template <typename Func, typename... Args>
    //void ReverseForEach(Func func, Args&&... args){
    //    scoped_lock<mutex> lock(this->m_mtx);
    //    ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    //}

    //template <typename Func, typename... Args>
    //forward_iterator FirstThat(Func func, Args&&... args){
    //    return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    //}

    //template <typename Func, typename... Args>
    //backward_iterator ReverseFirstThat(Func func, Args&&... args){
    //    return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    //}
};


template <typename Traits>
ostream& operator<<(ostream& os, CircularDoubleLinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
istream& operator>>(istream& is, CircularDoubleLinkedList<Traits>& list){
    return list.fromIstream(is);
}

#endif // __CIRCULARDOUBLELINKEDLIST_H__
