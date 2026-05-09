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
    // Destructor
    ~CircularLinkedList() override{
        if(this->m_pRoot != nullptr)
        this->m_pTail->setNext(nullptr); // Desconectar el último nodo del nodo raíz para evitar ciclos infinitos durante la destrucción
    }

    //Insert circular linked list
    void insert(const value_type &value, Ref ref) override{
        // Insertar el nuevo nodo utilizando la función de inserción de LinkedList
        // Luego, conectar el último nodo con el nuevo nodo para mantener la circularidad de la lista         
        scoped_lock<mutex> lock(this->m_mtx);

        if(this->m_pTail != nullptr){
            this->m_pTail->setNext(nullptr); // Conectar el último nodo con el nodo raíz para mantener la circularidad
        }

        Parent::insert(value, ref);

        Node* current = this->m_pRoot;
        while (current != nullptr && current->getNext() != nullptr) {
            current = current->getNext();
        }
        this->m_pTail = current; // Actualizar el puntero al último nodo

        if (this->m_pTail!= nullptr) {
            this->m_pTail->setNext(this->m_pRoot); // Conectar el último nodo con el nuevo nodo raíz para mantener la circularidad
        }
    }

    // Recorrer la lista circular hacia adelante
    template <typename Func, typename... Args>
    void CircularForEach(Func func, Args &&... args){
        scoped_lock<mutex> lock(this->m_mtx);

        if (this->m_pRoot == nullptr) return; // Si la lista está vacía, no hacer nada
        
        Node* current = this->m_pRoot;
        do {
            func(*current, std::forward<Args>(args)...); // Aplicar la función al nodo actual
            current = current->getNext(); // Avanzar al siguiente nodo
        } while (current != this->m_pRoot); // Continuar hasta volver al nodo raíz
    }

    // Agregar método para obtener la representación en cadena de la lista circular
    string toStringCircular() {
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


};

#endif