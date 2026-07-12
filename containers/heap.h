#ifndef __HEAP_H__
#define __HEAP_H__

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <mutex>
#include "vector.h"
#include "basetrait.h"
#include "basenode.h"
#include "../types.h"



template <typename T>
class HeapNode : public BaseNode<T> {
public:
    using Parent     = BaseNode<T>;
    using value_type = T;
    using Node       = HeapNode<T>;

public:
    HeapNode() : Parent() {}
    HeapNode(value_type data, Ref ref) : Parent(data, ref) {}
};
//template <typename T>
//std::ostream& operator<<(std::ostream& os, const HeapNode<T>& node) {
//    return os << node.ToString();
//}
//
//template <typename T>
//std::istream& operator>>(std::istream& is, HeapNode<T>& node) {
//    return node.fromIstream(is);
//}


template <typename T>
struct BaseHeapTrait : public BaseContainerTrait<T, HeapNode<T>> {
};

template <typename T>
struct AscendingHeapTrait : public BaseHeapTrait<T>,
                            public AscendingTrait<T>
{
};

template <typename T>
struct DescendingHeapTrait : public BaseHeapTrait<T>,
                             public DescendingTrait<T>
{
};

// Heap generico:
// - AscendingHeapTrait<T>  => min-heap
// - DescendingHeapTrait<T> => max-heap
template <typename Traits>
class Heap {
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = Heap<Traits>;

private:
    std::vector<Node> m_heap;
    Comp m_comp;
    mutable std::mutex m_mtx;

public:
    Heap() = default;

    // Destructor seguro en concurrencia.
    ~Heap() noexcept {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_heap.clear();
    }

    Heap(const Heap& other) {
        std::scoped_lock<std::mutex> lock(other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
    }

    Heap(Heap&& other) noexcept {
        std::scoped_lock<std::mutex> lock(other.m_mtx);
        //m_heap = exchange(other.m_heap, nullptr);
        //m_heap = exchange(other.m_heap, std::vector<Node>());
        //el vector queda valido y no necesita resetearse manualmente
        m_heap = std::move(other.m_heap);
        m_comp = std::move(other.m_comp);
    }

    Heap& operator=(const Heap& other) {
        if (this == &other) {
            return *this;
        }

        std::scoped_lock lock(m_mtx, other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
        return *this;
    }

    Heap& operator=(Heap&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        std::scoped_lock lock(m_mtx, other.m_mtx);
        m_heap = std::move(other.m_heap);
        m_comp = std::move(other.m_comp);
        return *this;
    }

    void insert(const value_type& value, Ref ref) {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_heap.push_back(Node(value, ref));
        heapify_up(m_heap.size() - 1);
    }

    // Mantiene firma original: extrae la raiz y descarta el valor.
    void extract() {
        // (void) se usa para indicar que el valor de retorno de extract_top() se ignora intencionalmente.
        (void)extract_top();
    }

    // Metodo util para obtener la raiz removida.
    Node extract_top() {
        std::scoped_lock<std::mutex> lock(m_mtx);
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }

        Node top = m_heap.front();

        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return top;
        }

        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
        return top;
    }

    // Cambiamos peek_min por peek para evitar confusiones entre min-heap y max-heap.
    value_type peek() const {
        return peek_node().GetData();
    }

    //casos especiales.
    // Version no-excepcion para escenarios donde un heap vacio no es error.
    bool try_peek_node(Node& out) const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        if (m_heap.empty()) {
            return false;
        }
        out = m_heap.front();
        return true;
    }

    // Version no-excepcion de extract_top().
    bool try_extract_top(Node& out) {
        std::scoped_lock<std::mutex> lock(m_mtx);
        if (m_heap.empty()) {
            return false;
        }

        out = m_heap.front();

        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return true;
        }

        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
        return true;
    }

    Node peek_node() const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        // igual a m_heap[0]
        return m_heap.front();
    }

    Ref peek_ref() const {
        return peek_node().GetRef();
    }

    bool empty() const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        return m_heap.empty();
    }

    std::size_t size() const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        return m_heap.size();
    }

    void clear() {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_heap.clear();
    }

    //sobrecarga build_from_vector
    void build_from_vector(const std::vector<Node>& nodes) {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_heap = nodes;
        heapify_all();
    }

    
    void build_from_vector(const std::vector<std::pair<value_type, Ref>>& entries) {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_heap.clear();
        m_heap.reserve(entries.size());
        for (const auto& entry : entries) {
            m_heap.push_back(Node(entry.first, entry.second));
        }
        heapify_all();
    }

    // Valida la propiedad de heap en modo concurrente.
    bool validate_heap() const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        for (std::size_t i = 0; i < m_heap.size(); ++i) {
            std::size_t left_index = left(i);
            std::size_t right_index = right(i);

            if (left_index < m_heap.size() && has_higher_priority(m_heap[left_index], m_heap[i])) {
                return false;
            }

            if (right_index < m_heap.size() && has_higher_priority(m_heap[right_index], m_heap[i])) {
                return false;
            }
        }
        return true;
    }

    // Cambia el comparador actual (mismo tipo Comp) y reordena todo el heap.
    void change_comp(const Comp& new_comp) {
        std::scoped_lock<std::mutex> lock(m_mtx);
        m_comp = new_comp;
        heapify_all();
    }

    // Convierte el heap actual a otro tipo de trait/comparador y reordena.
    template <typename NewTraits>
    Heap<NewTraits> change_comp_to() const {
        static_assert(std::is_same_v<value_type, typename NewTraits::value_type>,
                      "NewTraits::value_type debe coincidir con value_type");

        std::vector<std::pair<value_type, Ref>> entries;
        {
            std::scoped_lock<std::mutex> lock(m_mtx);
            entries.reserve(m_heap.size());
            for (const auto& node : m_heap) {
                entries.emplace_back(node.GetData(), node.GetRef());
            }
        }

        Heap<NewTraits> other;
        other.build_from_vector(entries);
        return other;
    }

    std::string toString() const {
        std::scoped_lock<std::mutex> lock(m_mtx);
        std::ostringstream oss;
        oss << "[";

        if (!m_heap.empty()) {
            for (std::size_t i = 0; i + 1 < m_heap.size(); ++i) {
                oss << m_heap[i].ToString() << ",";
            }
            oss << m_heap.back().ToString();
        }

        oss << "]";
        return oss.str();
    }

    std::istream& fromIstream(std::istream& is) {
        char ch;
        if (!(is >> ch) || ch != '[') {
            is.setstate(std::ios::failbit);
            return is;
        }

        std::vector<std::pair<value_type, Ref>> entries;

        is >> std::ws;
        if (is.peek() == ']') {
            is.get();
            clear();
            return is;
        }

        while (true) {
            Node node;
            if (!(is >> node)) {
                is.setstate(std::ios::failbit);
                return is;
            }

            entries.emplace_back(node.GetData(), node.GetRef());

            if (!(is >> ch)) {
                is.setstate(std::ios::failbit);
                return is;
            }

            if (ch == ']') {
                break;
            }

            if (ch != ',') {
                is.setstate(std::ios::failbit);
                return is;
            }
        }

        build_from_vector(entries);

        return is;
    }

private:
    bool has_higher_priority(const Node& lhs, const Node& rhs) const {
        return m_comp(lhs.GetData(), rhs.GetData());
    }

    static std::size_t parent(std::size_t index) {
        return (index - 1) / 2;
    }

    static std::size_t left(std::size_t index) {
        return (2 * index) + 1;
    }

    static std::size_t right(std::size_t index) {
        return (2 * index) + 2;
    }

    void heapify_up(std::size_t index) {
        while (index > 0) {
            std::size_t parent_index = parent(index);

            if (!has_higher_priority(m_heap[index], m_heap[parent_index])) {
                break;
            }

            std::swap(m_heap[index], m_heap[parent_index]);
            index = parent_index;
        }
    }

    void heapify_down(std::size_t index) {
        while (true) {
            std::size_t left_index = left(index);
            std::size_t right_index = right(index);
            std::size_t best_index = index;

            if (left_index < m_heap.size() && has_higher_priority(m_heap[left_index], m_heap[best_index])) {
                best_index = left_index;
            }

            if (right_index < m_heap.size() && has_higher_priority(m_heap[right_index], m_heap[best_index])) {
                best_index = right_index;
            }

            if (best_index == index) {
                break;
            }

            std::swap(m_heap[index], m_heap[best_index]);
            index = best_index;
        }
    }

    void heapify_all() {
        if (m_heap.empty()) {
            return;
        }

        for (std::size_t i = m_heap.size() / 2; i > 0; --i) {
            heapify_down(i - 1);
        }
    }
};



template <typename Traits>
std::ostream& operator<<(std::ostream& os, const Heap<Traits>& heap) {
    return os << heap.toString();
}

template <typename Traits>
std::istream& operator>>(std::istream& is, Heap<Traits>& heap) {
    return heap.fromIstream(is);
}

#endif // __HEAP_H__