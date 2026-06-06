#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include <mutex>
#include <sstream>
#include <iostream>
#include <utility>
#include "vector.h"
#include "../types.h"
#include "basetrait.h"

using namespace std;

template <typename T>
class HeapNode{
public:
    using value_type = T;
private:
    value_type m_data;
    Ref        m_ref;
public:
    HeapNode(){}
    HeapNode(value_type data, Ref ref)
        : m_data(data), m_ref(ref) {}
    value_type  GetData() const { return m_data; }
    value_type& GetDataRef()    { return m_data; }
    Ref         GetRef() const  { return m_ref; }
    Ref&        GetRefRef()     { return m_ref; }
};

// HeapNode operator <<
template <typename T>
ostream& operator<<(ostream& os, const HeapNode<T>& node)
{
    return os << "("
              << node.GetData()
              << ", "
              << node.GetRef()
              << ")";
}

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
    mutable mutex m_mtx;
public:

    // Constructor
    Heap() {}

    // Copy constructor
    Heap(const Heap &other) {
        scoped_lock lock(m_mtx, other.m_mtx);
        m_heap = other.m_heap;
        m_comp = other.m_comp;
    }

    // Move constructor
    Heap(Heap &&other) {
        scoped_lock lock(m_mtx, other.m_mtx);
        m_heap = std::move(other.m_heap);
        m_comp = std::move(other.m_comp);
    }

    // insert	
    void insert(const value_type &value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
		m_heap.push_back( Node(value, ref) );
        heapify_up(m_heap.size() - 1);
    }

    // Revisar completamente
    pair<value_type, Ref> extract() {
        scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty())
            throw out_of_range("Heap is empty");
        auto result = make_pair(
            m_heap[0].GetData(), m_heap[0].GetRef());
        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return result;
        }
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
        return result;
    }
    
    // Mejora 1 = remove smallest and remove biggest
    pair<value_type, Ref> remove_smallest() {
    	scoped_lock<mutex> lock(m_mtx);
    	if (m_heap.empty())
        	throw out_of_range("Heap is empty");
    	size_t minIndex = 0;
    	for (size_t i = 1; i < m_heap.size(); ++i) {
        	if (m_heap[i].GetData() < m_heap[minIndex].GetData()) {
            	minIndex = i;
        	}
    	}
    	auto result = make_pair(
        	m_heap[minIndex].GetData(), m_heap[minIndex].GetRef()
    	);
    	m_heap[minIndex] = m_heap.back();
    	m_heap.pop_back();
    	if (minIndex < m_heap.size()) {
        	heapify_down(minIndex);
        	heapify_up(minIndex);
    	}
    	return result;
	}
	pair<value_type, Ref> remove_biggest() {
    	scoped_lock<mutex> lock(m_mtx);
    	if (m_heap.empty())
        	throw out_of_range("Heap is empty");
    	size_t maxIndex = 0;
    	for (size_t i = 1; i < m_heap.size(); ++i) {
        	if (m_heap[i].GetData() > m_heap[maxIndex].GetData()) {
            	maxIndex = i;
        	}
    	}
    	auto result = make_pair(
        	m_heap[maxIndex].GetData(), m_heap[maxIndex].GetRef()
    	);
    	m_heap[maxIndex] = m_heap.back();
    	m_heap.pop_back();
    	if (maxIndex < m_heap.size()) {
        	heapify_down(maxIndex);
        	heapify_up(maxIndex);
    	}
    	return result;
	}
	
	// Mejora 2 = peek_max y peel_smalest
	value_type peek_max() const {
    	scoped_lock<mutex> lock(m_mtx);
    	if (m_heap.empty())
        	throw out_of_range("Heap is empty");
    	size_t maxIndex = 0;
    	for (size_t i = 1; i < m_heap.size(); ++i) {
        	if (m_heap[i].GetData() > m_heap[maxIndex].GetData()) {
            	maxIndex = i;
        	}
    	}
    	return m_heap[maxIndex].GetData();
	}
	value_type peek_smallest() const {
    	scoped_lock<mutex> lock(m_mtx);
    	if (m_heap.empty())
        	throw out_of_range("Heap is empty");
    	size_t minIndex = 0;
    	for (size_t i = 1; i < m_heap.size(); ++i) {
        	if (m_heap[i].GetData() < m_heap[minIndex].GetData()) {
            	minIndex = i;
        	}
    	}
    return m_heap[minIndex].GetData();
}
	
	// peek_min Devuelve la razil no el minimo
	value_type peek_min() const {
    	scoped_lock<mutex> lock(m_mtx);
        if (m_heap.empty())
            throw out_of_range("Heap is empty");
        return m_heap[0].GetData();
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
            if (m_comp(m_heap[index].GetData(), m_heap[parent].GetData())) {
                std::swap(m_heap[index], m_heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(size_t index) {
        while (true) {
			size_t left = 2 * index + 1;
        	size_t right = 2 * index + 2;
        	size_t smallest = index;
        	if (left < m_heap.size() && m_comp(m_heap[left].GetData(), m_heap[smallest].GetData())) {
            	smallest = left;
        	}
        	if (right < m_heap.size() && m_comp(m_heap[right].GetData(), m_heap[smallest].GetData())) {
            	smallest = right;
        	}
        	if (smallest == index)
                	break;
        	swap(m_heap[index], m_heap[smallest]);
        	index = smallest;
        }
    }
    
// Mejora 3 (en caso alguna de la anteriores no cuente) = Imprimir como un arbol
public:
    void printTree(size_t index = 0, int depth = 0) const {
        if(index >= m_heap.size())
            return;
        printTree(2 * index + 2, depth + 1);
        for(int i = 0; i < depth; i++)
            cout << "        ";
        cout << m_heap[index].GetData() << "(" << m_heap[index].GetRef() << ")" << endl;
        printTree(2 * index + 1, depth + 1);
    }
    string toString() {
        scoped_lock<mutex> lock(m_mtx);
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < m_heap.size(); i++) {
            ss << "(" << m_heap[i].GetData() << ", " << m_heap[i].GetRef() << ")";
            if (i + 1 < m_heap.size())
                ss << ",";
        }
        ss << "]";
        return ss.str();
	}
    template <typename T>
    friend ostream& operator<<(ostream& os, Heap<T>& heap);

    template <typename T>
    friend istream& operator>>(istream& is, Heap<T>& heap);
    };

// operator <<
template <typename Traits>
ostream& operator<<(ostream& os, Heap<Traits>& heap)
{
    return os << heap.toString();
}

// operator >>
template <typename Traits>
istream& operator>>(istream& is, Heap<Traits>& heap)
{
    string line;
    getline(is, line);
    for (char &c : line){
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',') {c = ' ';}
    }
    stringstream ss(line);
    typename Heap<Traits>::value_type value;
    Ref ref;
    while (ss >> value >> ref){
        heap.insert(value, ref);
    }
    return is;
}
#endif // __HEAP_H__
