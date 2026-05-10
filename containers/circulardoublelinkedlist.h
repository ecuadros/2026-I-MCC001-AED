#ifndef _CIRCULARDOUBLELINKEDLIST_H__
#define _CIRCULARDOUBLELINKEDLIST_H__
#include "doublelinkedlist.h"

template <typename Traits>
class CircularDoubleLinkedList : public DoubleLinkedList<Traits>{
public:
    using Parent     = DoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;

    // Constructor
    CircularDoubleLinkedList() = default;
    CircularDoubleLinkedList(const CircularDoubleLinkedList &other) = default;
    CircularDoubleLinkedList(CircularDoubleLinkedList &&other) = default;
    ~CircularDoubleLinkedList() override{
        if (this->m_pRoot != nullptr && this->m_pTail != nullptr){
            this->m_pTail->setNext(nullptr);
            this->m_pRoot->setPrev(nullptr);
        }
    }

    void insert(const value_type &value, Ref ref) override{
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pRoot != nullptr && this->m_pTail != nullptr){
            this->m_pTail->setNext(nullptr);
            this->m_pRoot->setPrev(nullptr);
        }

        auto result = this->internal_insert(this->m_pRoot, nullptr, value, ref);
        Node* pPrevious = result.previous;
        Node* pInserted = result.inserted;

        pInserted->setPrev(pPrevious);

        Node* pNext = pInserted->getNext();
        if (pNext != nullptr)
            pNext->setPrev(pInserted);

        if (this->m_pRoot != nullptr && this->m_pTail != nullptr) {
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
        }
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pRoot == nullptr)
            return;
        
        Node* current = this->m_pRoot;
        do {
            func(*current, std::forward<Args>(args)...);
            current = current->getNext();
        } while (current != this->m_pRoot);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pTail == nullptr)
            return;
        
        Node* current = this->m_pTail;
        do {
            func(*current, std::forward<Args>(args)...);
            current = current->getPrev();
        } while (current != this->m_pTail);
    }

    string toString() override{
        scoped_lock<mutex> lock(this->m_mtx);
        stringstream ss;
        Node* current = this->m_pRoot;

        ss << "[";
        if (this->m_size > 0) {
            for (size_t i = 0; i < this->m_size - 1; ++i){
                ss << *current << ",";
                current = current->getNext();
            }
            ss << *current;
        }
        ss << "]";
        return ss.str();
    }

};

#endif
