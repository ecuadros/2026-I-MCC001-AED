#ifndef __HEAP_H__
#define __HEAP_H__
#include <vector>
#include <mutex>
#include <sstream>
#include "vector.h"

template <typename Traits>
class HeapNode{
    private:
        typename Traits::value_type m_data;
        Ref m_ref;
    public:
        HeapNode(typename Traits::value_type data, Ref ref) : m_data(data), m_ref(ref) {}
        
        typename Traits::value_type GetData() const { return m_data; }
        Ref GetRef()  const { return m_ref;  }
};

template <typename T>
struct AscendingHeapTrait : public BaseContainerTrait<T, HeapNode<AscendingHeapTrait<T>> >,
                            public AscendingTrait<T>
{
};

template <typename T>
struct DescendingHeapTrait : public BaseContainerTrait<T, HeapNode<DescendingHeapTrait<T>> >,
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
    mutable mutex m_mtx;
public:

    virtual string  toString() const;

    //CONSTRUCTOR
    Heap(){}

    //CONSTRUCTOR COPIA
    Heap(const Heap &other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
    }

    //MOVE CONSTRUCTOR
    Heap(Heap &&other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_heap = exchange(other.m_heap, vector<Node>());
        m_comp = exchange(other.m_comp, Comp());
    }

    void insert(const value_type &value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        m_heap.push_back( Node(value, ref) );
        heapify_up(m_heap.size() - 1);
    }

    // Revisar completamente
    void extract() { // elimina la raiz, es decir el minimo, y mueve el ultimo elemento a la raiz
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

    Node peek_min() const {
        scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty()) {
            throw std::out_of_range("Heap is empty");
        }
        return m_heap[0];
    }

    TB empty() const {
        scoped_lock<mutex> lock(m_mtx);
        return m_heap.empty();
    }

    size_t size() const {
        scoped_lock<mutex> lock(m_mtx);
        return m_heap.size();
    }

    //MEJORA 1
    void clear() {
        scoped_lock<mutex> lock(m_mtx);
        m_heap.clear();
    }

    //MEJORA 2 - Este método verifica si el heap cumple con la propiedad de heap, es decir, si cada nodo es menor o igual a sus hijos
    TB isValidHeap() const {
        scoped_lock<mutex> lock(m_mtx);

        for (size_t i = 0; i < m_heap.size(); ++i) {
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;

            if (left < m_heap.size() && m_comp(m_heap[left].GetData(), m_heap[i].GetData())) {
                return false;
            }
            if (right < m_heap.size() && m_comp(m_heap[right].GetData(), m_heap[i].GetData())) {
                return false;
            }
        }
        return true;
    }

    //MEJORA 3 - Imprimir el heap en forma de árbol
    TS toLevelString(){
        scoped_lock<mutex> lock(m_mtx);
        
        stringstream ss;
        size_t nextLevelStart = 1;

        for (size_t i = 0; i < m_heap.size(); ++i) {
            if (i == nextLevelStart) {
                ss << "\n"; // Nueva línea para cada nivel
                nextLevelStart = nextLevelStart * 2 + 1; // Siguiente primera posicion del nivel
            }
            ss << nodeToString(m_heap[i]) << " "; // Imprime el valor y la referencia del nodo
        }
        return ss.str();
    }


private:
    TS nodeToString(const Node &node) const {
        stringstream ss;
        ss << "(" << node.GetData() << ", " << node.GetRef() << ")";
        return ss.str();
    }

    void heapify_up(size_t index) { // mueve el nuevo elemento hacia arriba para mantener la propiedad del heap
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if ( m_comp(m_heap[index].GetData(), m_heap[parent].GetData()) ) { // Si el nuevo elemento es menor que su padre, intercambiarlos
                std::swap(m_heap[index], m_heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(size_t index) { // mueve el elemento de la raiz hacia abajo para mantener la propiedad del heap   
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left < m_heap.size() && m_comp(m_heap[left].GetData(), m_heap[smallest].GetData()) ) { // Si el hijo izquierdo es menor que el elemento actual, actualizar el índice del más pequeño
            smallest = left;
        }
        if (right < m_heap.size() && m_comp(m_heap[right].GetData(), m_heap[smallest].GetData()) ) { // Si el hijo derecho es menor que el elemento actual, actualizar el índice del más pequeño
            smallest = right;
        }

        if (smallest != index) {
            std::swap(m_heap[index], m_heap[smallest]);
            heapify_down(smallest);
        }
    }
};

//TOSTRING
template <typename Traits>
string Heap<Traits>::toString() const {
    scoped_lock<mutex> lock(m_mtx);
        
    stringstream ss;
    ss << "[";
    for (size_t i = 0 ; i < m_heap.size(); ++i){
        if(i > 0)
            ss << ",";
        ss << nodeToString(m_heap[i]);
    }

    ss << "]";
    return ss.str();
}

//OPERATOR<<
template <typename Traits>
ostream& operator<<(ostream& os, Heap<Traits>& heap){
    return os << heap.toString();
}

//OPERATOR>>
template <typename Traits>
istream& operator>>(istream& is, Heap<Traits>& heap){
    using value_type = typename Traits::value_type;
    
    string input;
    getline(is, input);

    for(char& c : input){
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',')
            c = ' ';
    }

    value_type value;
    Ref ref;
    stringstream ss(input);

    while (ss >> value >> ref) {
        heap.insert(value, ref);
    }
    return is;
}

#endif // __HEAP_H__
