# Diagrama de Clases - Estructuras de Datos

```mermaid
classDiagram
    direction TB

    class LLNode~T~ {
        <<template>>
        #T m_data
        #Ref m_ref
        #LLNode* m_pNext
        +getData() T
        +getDataRef() T&
        +getRef() Ref
        +setNext(LLNode*)
        +getNext() LLNode*
    }

    class DLLNode~T~ {
        <<template>>
        -DLLNode* m_pPrev
        +getPrev() DLLNode*
        +setPrev(DLLNode*)
        +getNext() DLLNode*
    }
    DLLNode~T~ --|> LLNode~T~

    class VectorNode~Traits~ {
        <<template>>
        +value_type m_data
        +Ref m_ref
        +GetData() value_type
        +GetDataRef() value_type&
        +operator++()
        +operator+=(value_type)
    }

    class BaseContainerTrait {
        <<abstract>>
        +value_type
        +Node
    }

    class BaseLinkedListTrait~T~ {
        <<template>>
        +value_type = T
        +Node = LLNode~T~
    }
    BaseLinkedListTrait~T~ --|> BaseContainerTrait

    class AscendingLinkedListTrait~T~ {
        <<template>>
        +Comp = less~T~
    }
    AscendingLinkedListTrait~T~ --|> BaseLinkedListTrait~T~

    class DescendingLinkedListTrait~T~ {
        <<template>>
        +Comp = greater~T~
    }
    DescendingLinkedListTrait~T~ --|> BaseLinkedListTrait~T~

    class BaseDoubleLinkedListTrait~T~ {
        <<template>>
        +value_type = T
        +Node = DLLNode~T~
    }
    BaseDoubleLinkedListTrait~T~ --|> BaseContainerTrait

    class AscendingDoubleLinkedListTrait~T~ {
        <<template>>
        +Comp = less~T~
    }
    AscendingDoubleLinkedListTrait~T~ --|> BaseDoubleLinkedListTrait~T~

    class DescendingDoubleLinkedListTrait~T~ {
        <<template>>
        +Comp = greater~T~
    }
    DescendingDoubleLinkedListTrait~T~ --|> BaseDoubleLinkedListTrait~T~

    class VectorTraits~T~ {
        <<template>>
        +value_type = T
        +Node = VectorNode~T~
    }
    VectorTraits~T~ --|> BaseContainerTrait

    class general_iterator {
        <<abstract>>
        #Container* m_pContainer
        #Node* m_pNode
        +operator*() Node&
        +operator->() Node*
        +getNode() Node*
        +==() bool
    }

    class LinkedListForwardIterator {
        +operator++()
    }
    LinkedListForwardIterator --|> general_iterator

    class DoubleLinkedListBackwardIterator {
        +operator++() (prev)
    }
    DoubleLinkedListBackwardIterator --|> general_iterator

    class vector_forward_iterator {
        +operator++()
    }
    vector_forward_iterator --|> general_iterator

    class vector_backward_iterator {
        +operator++() (decrement)
    }
    vector_backward_iterator --|> general_iterator

    class LinkedList~Traits~ {
        <<template>>
        #Node* m_pRoot
        #Node* m_pTail
        #size_t m_size
        #Comp m_comp
        #mutable mutex m_mtx
        +push_front(value, ref)
        +pop_front() pair
        +push_back(value, ref)
        +pop_back() pair
        +insert(value, ref)
        +operator[](index) Node&
        +size() size_t
        +toString() string
        +begin() forward_iterator
        +end() forward_iterator
        +ForEach(func, args...)
        +FirstThat(func, args...) forward_iterator
    }

    class DoubleLinkedList~Traits~ {
        <<template>>
        +push_front(value, ref) override
        +pop_front() override
        +push_back(value, ref) override
        +pop_back() override
        +insert(value, ref) override
        +rbegin() backward_iterator
        +rend() backward_iterator
        +ReverseForEach(func, args...)
        +ReverseFirstThat(func, args...) backward_iterator
    }
    DoubleLinkedList~Traits~ --|> LinkedList~Traits~

    class CLinkedList~Traits~ {
        <<template>>
        <<circular singly linked>>
        -breakCircle()
        -restoreCircle()
        +push_front(value, ref) override
        +pop_front() override
        +push_back(value, ref) override
        +pop_back() override
        +insert(value, ref) override
        +toString() string override
        +ForEach(func, args...)
        +FirstThat(func, args...) forward_iterator
    }
    CLinkedList~Traits~ --|> LinkedList~Traits~

    class CDoubleLinkedList~Traits~ {
        <<template>>
        <<circular doubly linked>>
        -breakCircle()
        -restoreCircle()
        +push_front(value, ref) override
        +pop_front() override
        +push_back(value, ref) override
        +pop_back() override
        +insert(value, ref) override
        +toString() string override
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
        +ForEach(func, args...)
        +ReverseForEach(func, args...)
        +FirstThat(func, args...) forward_iterator
        +ReverseFirstThat(func, args...) backward_iterator
    }
    CDoubleLinkedList~Traits~ --|> DoubleLinkedList~Traits~

    class Vector~Traits~ {
        <<template>>
        -Node* m_data
        -size_t m_size
        -size_t m_capacity
        -mutex m_mtx
        +push_back(value, ref)
        +size() size_t
        +ToString() string
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
        +ForEach(func, args...)
        +ReverseForEach(func, args...)
        +FirstThat(func, args...) forward_iterator
        +ReverseFirstThat(func, args...) backward_iterator
    }

    LinkedList~Traits~ *-- Node : uses
    LinkedList~Traits~ ..> LinkedListForwardIterator : creates
    DoubleLinkedList~Traits~ ..> DoubleLinkedListBackwardIterator : creates
    CDoubleLinkedList~Traits~ ..> LinkedListForwardIterator : creates
    CDoubleLinkedList~Traits~ ..> DoubleLinkedListBackwardIterator : creates
    Vector~Traits~ *-- Node : contains array
    Vector~Traits~ ..> vector_forward_iterator : creates
    Vector~Traits~ ..> vector_backward_iterator : creates

    class ForEach {
        <<function>>
        +ForEach(begin, end, func, args...)
    }
    class FirstThat {
        <<function>>
        +FirstThat(begin, end, func, args...) Iterator
    }

    ForEach <.. LinkedList~Traits~ : uses
    FirstThat <.. LinkedList~Traits~ : uses
    ForEach <.. DoubleLinkedList~Traits~ : uses
    FirstThat <.. DoubleLinkedList~Traits~ : uses
    ForEach <.. Vector~Traits~ : uses
    FirstThat <.. Vector~Traits~ : uses

    LinkedList~Traits~ ..> mutex : uses
    Vector~Traits~ ..> mutex : uses

    note for LinkedListDemo "ListsDemo.cpp, main.cpp"
    note for VectorDemo "DemoVector.cpp"