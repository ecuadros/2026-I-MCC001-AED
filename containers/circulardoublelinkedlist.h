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
    // Destructor
    ~CircularDoubleLinkedList() override{
        if(this->m_pRoot != nullptr && this->m_pTail != nullptr)
        this->m_pTail->setNext(nullptr); // Desconectar el último nodo del nodo raíz para evitar ciclos infinitos durante la destrucción
        this->m_pRoot->setPrev(nullptr); // Desconectar el nodo raíz del último nodo para evitar ciclos infinitos durante la destrucción
    }

    //Insert circular double linked list
    void insert(const value_type &value, Ref ref) override{       
        // scoped_lock<mutex> lock(this->m_mtx);
        if(this->m_pRoot != nullptr && this->m_pTail != nullptr){
            this->m_pTail->setNext(nullptr); // Conectar el último nodo con el nodo raíz para mantener la circularidad
            this->m_pRoot->setPrev(nullptr); // Conectar el nuevo nodo raíz con el último nodo para mantener la circularidad
        }

        Parent::insert(value, ref); 
        
        Node* current = this->m_pRoot;
        Node* prev= nullptr;
        while(current != nullptr){ 
            current->setPrev(prev);
            prev = current;

            if(current->getNext() == nullptr)
                this->m_pTail = current;
            current = current->getNext();
        }

        if (this->m_pRoot != nullptr && this->m_pTail != nullptr) {
            this->m_pTail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_pTail);
            }
    }

    // Recorrer la lista circular hacia adelante
    template <typename Func, typename... Args>
    void CircularDoubleForEach(Func func, Args &&... args){
        // scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pRoot == nullptr) return; // Si la lista está vacía, no hacer nada
        
        Node* current = this->m_pRoot;
        do {
            func(*current, std::forward<Args>(args)...); // Aplicar la función al nodo actual
            current = current->getNext(); // Avanzar al siguiente nodo
        } while (current != this->m_pRoot); // Continuar hasta volver al nodo raíz
    }

    // Recorrido circular hacia atrás
    template <typename Func, typename... Args>
    void ReverseCircularDoubleForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pTail == nullptr) return;
        
        Node* current = this->m_pTail;
        do {
            func(*current, std::forward<Args>(args)...);
            current = current->getPrev(); // Avanzar al nodo anterior
        } while (current != this->m_pTail); // Continuar hasta volver al nodo final
    }

    // Agregar método para obtener la representación en cadena de la lista circular
    string toStringCircularDoubleNext() {
        scoped_lock<mutex> lock(this->m_mtx);
        stringstream ss;
        Node* current = this->m_pRoot;

        ss << "[";
        if (this->m_size > 0) {
            for (size_t i = 0; i < this->m_size - 1; ++i)
            {
                ss << *current << ",";
                current = current->getNext(); // Avanzar al siguiente nodo
            }
            ss << *current;
        }
        ss << "]";
        return ss.str();
    }

    // Agregar método para obtener la representación en cadena de la lista circular hacia atrás
    string toStringCircularReverse() {
        scoped_lock<mutex> lock(this->m_mtx);
        stringstream ss;
        Node* current = this->m_pTail;

        ss << "[";
        if (this->m_size > 0) {
            for (size_t i = 0; i < this->m_size - 1; ++i)
            {
                ss << *current << ",";
                current = current->getPrev(); // Avanzar al nodo anterior
            }
            ss << *current;
        }
        ss << "]";
        return ss.str();
    }

};

#endif