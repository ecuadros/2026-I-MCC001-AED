#ifndef __HEAP_H__
#define __HEAP_H__
#include <vector>
#include <stdexcept>
#include <utility>
#include <mutex>
#include <iostream>
#include "vector.h"
using namespace std;
template <typename T>
class HeapNode{
    private:
        T m_data;
        //typename Traits::value_type m_data;
        Ref m_ref;
    public:
        HeapNode(T data, Ref ref) : m_data(data), m_ref(ref) {}
        
        T GetData()   const { return m_data; }
        Ref GetRef()  const { return m_ref;  }
};

template <typename T>
struct AscendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public AscendingTrait<T>
{
};

template <typename T>
struct DescendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public DescendingTrait<T>
{
};

// Revisar: https://www.cs.usfca.edu/~galles/visualization/Heap.html
// Pero en este ejercicio empezamos el la posicion [0]
template <typename Traits>
class Heap {
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = Heap<Traits>;
private:
    vector<Node> m_heap;
    Comp         m_comp;
public:

    void insert(const value_type &value, Ref ref) {
        m_heap.push_back( Node(value, ref) );
        heapify_up(m_heap.size() - 1);
    }

    // Revisar completamente
    pair<value_type, Ref> extract() {
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        
        auto top_element = make_pair(m_heap[0].GetData(), m_heap[0].GetRef());

        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return top_element;
        }
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);

        return top_element;
    }

    pair<value_type, Ref> peek_top() const {
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        return make_pair(m_heap[0].GetData(), m_heap[0].GetRef());
    }

    bool empty() const {
        return m_heap.empty();
    }

    size_t size() const {
        return m_heap.size();
    }

private:
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if ( m_comp(m_heap[index].GetData(), m_heap[parent].GetData()) ) {
                std::swap(m_heap[index], m_heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(size_t index) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left < m_heap.size() && m_comp(m_heap[left].GetData(), m_heap[smallest].GetData()) ) {
            smallest = left;
        }
        if (right < m_heap.size() && m_comp(m_heap[right].GetData(), m_heap[smallest].GetData()) ) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(m_heap[index], m_heap[smallest]);
            heapify_down(smallest);
        }
    }
};

#endif // __HEAP_H__