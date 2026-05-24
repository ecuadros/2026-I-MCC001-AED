#ifndef __HEAP_H__
#define __HEAP_H__
#include <vector>
#include <mutex>
#include "vector.h"

template <typename T>
class HeapNode{
    private:
        using value_type = T;
        value_type m_data;
        Ref m_ref;
    public:
        HeapNode(value_type data, Ref ref) : m_data(data), m_ref(ref) {}
        
        value_type GetData() const { return m_data; }
        Ref GetRef()  const { return m_ref;  }

        bool operator<(const HeapNode& other) const {
            return m_data < other.m_data;
        }

        bool operator>(const HeapNode& other) const {
            return m_data > other.m_data;
        }
};

template <typename T>
struct AscendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public AscendingTrait<HeapNode<T>>
{
};

template <typename T>
struct DescendingHeapTrait : public BaseContainerTrait<T, HeapNode<T> >,
                            public DescendingTrait<HeapNode<T>>
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
    mutable mutex m_mtx;
public:

    Heap() = default;

    // 1. Copy constructor
    // Heap(const Heap&) = default;
    Heap(const Heap& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
    }

    // 2. Move constructor
    // Heap(Heap&&) = default;
    Heap(Heap&& other) {
        std::scoped_lock lock(other.m_mtx);
        m_heap = std::exchange(other.m_heap, {});
        m_comp = other.m_comp;
    }

    // Mejora: Copy assignment operator
    Heap& operator=(const Heap& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mtx, other.m_mtx);
            m_heap = other.m_heap;
            m_comp = other.m_comp;
        }
        return *this;
    }

    // Mejora: Move assignment operator
    Heap& operator=(Heap&& other) {
        if (this != &other) {
            std::scoped_lock lock(m_mtx, other.m_mtx);
            m_heap = std::move(other.m_heap);
            m_comp = std::move(other.m_comp);
        }

        return *this;
    }

    // Mejora: Emplace, evitar la construcción de un nodo temporal
    template <typename... Args>
    void emplace(Args&&... args) {
        scoped_lock lock(m_mtx);
        m_heap.emplace_back(std::forward<Args>(args)...);
        heapify_up(m_heap.size() - 1);
    }

    void insert(const value_type &value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        m_heap.push_back( Node(value, ref) );
        heapify_up(m_heap.size() - 1);
    }

    // Revisar completamente
    void extract() {
        scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return;
        }
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
    }

    Node peek() const {
        scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        return m_heap[0];
    }

    bool empty() const {
        scoped_lock<mutex> lock(m_mtx);
        return m_heap.empty();
    }

    size_t size() const {
        scoped_lock<mutex> lock(m_mtx);
        return m_heap.size();
    }

private:
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if ( m_comp(m_heap[index], m_heap[parent]) ) {
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

        if (left < m_heap.size() && m_comp(m_heap[left], m_heap[smallest]) ) {
            smallest = left;
        }
        if (right < m_heap.size() && m_comp(m_heap[right], m_heap[smallest]) ) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(m_heap[index], m_heap[smallest]);
            heapify_down(smallest);
        }
    }
};

// 3. Operator<<
template <typename Traits>
ostream& operator<<(ostream& os, const Heap<Traits>& heap) {
    Heap<Traits> temp(heap);
    os << "[ ";

    while (!temp.empty()) {
        auto node = temp.peek();
        os << node.GetData();
        temp.extract();
        if (!temp.empty()) {
            os << ", ";
        }
    }

    os << " ]";
    return os;
}

// 4. Operator>>
template <typename Traits>
istream& operator>>(istream& is, Heap<Traits>& heap) {
    char ch;
    // Leer '['
    is >> ch;
    if (ch != '[') {
        is.setstate(std::ios::failbit);
        return is;
    }

    typename Traits::value_type value;
    while (true) {
        is >> value;
        heap.insert(value, 0);
        is >> ch;
        if (ch == ']') {
            break;
        }
        if (ch != ',') {
            is.setstate(std::ios::failbit);
            return is;
        }
    }
    return is;
}

#endif // __HEAP_H__