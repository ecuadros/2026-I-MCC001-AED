#ifndef _CIRCULARLINKEDLIST_H__
#define _CIRCULARLINKEDLIST_H__
#include "linkedlist.h"

template <typename Traits>
class CircularLinkedList : public LinkedList<Traits>{
public:
    using Parent     = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;

    // Constructor
    CircularLinkedList() = default;
    CircularLinkedList(const CircularLinkedList &other) = default;
    CircularLinkedList(CircularLinkedList &&other) = default;
    ~CircularLinkedList() override{
        if (this->m_pRoot != nullptr && this->m_pTail != nullptr)
            this->m_pTail->setNext(nullptr);
    }

    void insert(const value_type &value, Ref ref) override{
        scoped_lock<mutex> lock(this->m_mtx);

        if(this->m_pTail != nullptr){
            this->m_pTail->setNext(nullptr);
        }

        this->internal_insert(this->m_pRoot, nullptr, value, ref);

        if (this->m_pTail != nullptr) {
            this->m_pTail->setNext(this->m_pRoot);
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
