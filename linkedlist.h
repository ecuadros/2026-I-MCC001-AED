#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>     // mutex
#include "general_iterator.h"
#include "util.h"
#include "types.h"
#include "foreach.h"
#include "basetrait.h"

using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, 
                                            LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode{
    using value_type = T;
    using Node       = LLNode<T>;
private:
    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext;
public:
    LLNode(){}
    LLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~LLNode() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
};

template <typename T>
ostream &operator<<(ostream &os, const LLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}

template <typename T>
struct BaseLinkedListTrait : public BaseContainerTrait<T, LLNode<T>>{

};

template <typename T>
struct AscendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Traits>
class LinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = LinkedList<Traits>;

    using forward_iterator = LinkedListForwardIterator<MySelf>; // FT
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutex m_mtx; // Mutex para sincronizar el acceso a la lista en operaciones concurrentes
public:
    LinkedList() {}
    LinkedList(const LinkedList &other); // Copy constructor
    LinkedList(LinkedList &&other); // Move constructor
    LinkedList& operator=(const LinkedList &other); // Copy assignment operator
    LinkedList& operator=(LinkedList &&other); // Move assignment operator
    
    //DESTRUCTOR SEGURO: El destructor de la clase LinkedList se encarga de liberar la memoria 
    // de los nodos de la lista para evitar fugas de memoria. Recorre la lista desde el nodo raíz, 
    // eliminando cada nodo y avanzando al siguiente hasta que se han eliminado todos los nodos. 
    // Finalmente, reinicia los punteros y el tamaño de la lista.
    virtual        ~LinkedList(){ // Destructor que libera la memoria de los nodos de la lista
        Node* current = m_pRoot; // Comenzamos desde el nodo raíz
        while(current != nullptr){
            Node* next = current->getNext(); // Guardamos el siguiente nodo antes de eliminar el actual
            delete current; // Liberamos la memoria del nodo actual
            current = next; // Avanzamos al siguiente nodo
        }
        m_pRoot = nullptr; // Aseguramos que el puntero raíz apunte a nullptr después de liberar la memoria
        m_pTail = nullptr; // Reiniciamos el puntero a la cola de la lista
        m_size = 0; // Reiniciamos el tamaño de la lista a 0
    }

    // PUSH FRONT: El método push_front se encarga de insertar un nuevo nodo al inicio de la lista. 
    // Toma un valor y una referencia como parámetros, crea un nuevo nodo con esos datos y 
    // lo inserta al principio de la lista. Si la lista está vacía, el nuevo nodo se convierte en el nodo raíz y 
    // también en la cola de la lista. Si la lista no está vacía, el nuevo nodo se enlaza al nodo raíz actual y 
    // luego se actualiza el puntero raíz para que apunte al nuevo nodo. Finalmente, se incrementa el tamaño de la lista.
    virtual void    push_front(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx);
        Node* newNode = new Node(value, ref, m_pRoot); // Creamos un nuevo nodo con el valor y la referencia proporcionados, apuntando al nodo raíz actual
        m_pRoot = newNode; // Actualizamos el puntero raíz para que apunte al nuevo nodo
        if(m_size == 0) // Si la lista estaba vacía, el nuevo nodo también es la cola de la lista
            m_pTail = newNode;
        m_size++; // Incrementamos el tamaño de la lista
    }

    //POP FRONT : El método pop_front se encarga de eliminar el primer nodo de la lista y devolver su valor y referencia.
    virtual auto    pop_front() -> std::pair<value_type, Ref>{
        if(m_pRoot == nullptr)
            throw std::out_of_range("pop_front(): empty list");

        Node* pTemp = m_pRoot; // Guardamos el primer nodo a eliminar
        std::pair<value_type, Ref> result(pTemp->getData(), pTemp->getRef()); // Guardamos el valor y la referencia del nodo a eliminar
        m_pRoot = m_pRoot->getNext(); // Actualizamos la raíz al siguiente nodo
        delete pTemp; // Liberamos la memoria del nodo eliminado
        m_size--; // Decrementamos el tamaño de la lista

        if(m_pRoot == nullptr) // Si la lista queda vacía después de eliminar el nodo, reiniciamos el puntero a la cola
            m_pTail = nullptr;

        return result;

        // if( m_pRoot ){
        //     Node* pTemp = m_pRoot; // Guardamos el nodo a eliminar
        //     m_pRoot = m_pRoot->getNext(); // Actualizamos la raíz al siguiente nodo
        //     return std::make_pair(pTemp->getData(), pTemp->getRef());
        // }else
        //     throw std::out_of_range("pop_front(): empty list");
    }

    // PUSH BACK: El método push_back se encarga de insertar un nuevo nodo al final de la lista.
    // Toma un valor y una referencia como parámetros, crea un nuevo nodo con esos datos y lo inserta al final de la lista.
    virtual void    push_back(value_type value, Ref ref){
        scoped_lock<mutex> lock(m_mtx); // Bloqueamos el mutex para asegurar que solo un hilo pueda modificar la lista a la vez
        Node* newNode = new Node(value, ref, nullptr); // Creamos un nuevo nodo con el valor y la referencia proporcionados, apuntando a nullptr ya que será el último nodo

        if(m_size == 0){ // Si la lista está vacía, el nuevo nodo se convierte en el nodo raíz y también en la cola de la lista
            m_pRoot = newNode;
            m_pTail = newNode;
        }else{ // Si la lista no está vacía, enlazamos el nuevo nodo al nodo actual de la cola y luego actualizamos el puntero de la cola para que apunte al nuevo nodo
            m_pTail->setNext(newNode);
            m_pTail = newNode;
        }

        m_size++; // Incrementamos el tamaño de la lista
    }

    // POP BACK : El método pop_back se encarga de eliminar el último nodo de la lista y devolver su valor y referencia.
    virtual auto    pop_back() -> std::pair<value_type, Ref>{
        if(m_pRoot == nullptr) // Si la lista está vacía, lanzamos una excepción
            throw std::out_of_range("pop_back(): empty list");

        if(m_pRoot == m_pTail){ // Si la lista tiene un single nodo, eliminamos ese nodo y reiniciamos los punteros y el tamaño de la lista
            std::pair<value_type, Ref> result(m_pRoot->getData(), m_pRoot->getRef());
            delete m_pRoot; // Liberamos la memoria del nodo único
            m_pRoot = nullptr; // Reiniciamos el puntero raíz a nullptr
            m_pTail = nullptr; // Reiniciamos el puntero a la cola de la lista a nullptr
            m_size--; // Decrementamos el tamaño de la lista
            return result; // Devolvemos el valor y la referencia del nodo eliminado
        }

        Node* current = m_pRoot; // Comenzamos desde el nodo raíz
        while(current->getNext() != m_pTail){ // Recorremos la lista hasta encontrar el nodo anterior a la cola
            current = current->getNext();
        }

        std::pair<value_type, Ref> result(m_pTail->getData(), m_pTail->getRef()); // Guardamos el valor y la referencia del nodo a eliminar (la cola)
        delete m_pTail; // Liberamos la memoria del nodo de la cola
        current->setNext(nullptr); // Actualizamos el nodo anterior a la cola para que apunte a nullptr, ya que ahora será la nueva cola
        m_pTail = current; // Actualizamos el puntero de la cola para que apunte al nuevo último nodo
        m_size--; // Decrementamos el tamaño de la lista
        
        return result;
        // return std::pair<value_type, Ref>();
    }

    // FIRTS THAT: El método first_that de la clase LinkedList se encarga de encontrar el primer nodo de la lista 
    // que cumple con una condición definida por una función proporcionada como parámetro.
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){ // Devuelve un iterador al primer nodo que cumple con la condición definida por la función func
        scoped_lock<mutex> lock(m_mtx);
        for (auto it = begin(); it != end(); ++it){
        if (func(*it, std::forward<Args>(args)...))
            return it;
        }
        return end();
    }
     
private:
            void    internal_insert(Node* &pParent, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual Node& operator[](size_t index);
    virtual size_t  size() const { return m_size; }
    virtual string  toString();

    forward_iterator begin() { return forward_iterator(this, m_pRoot); } // Devuelve un iterador al primer nodo de la lista
    forward_iterator end()   { return forward_iterator(this, nullptr); } // Devuelve un iterador al nodo siguiente al último nodo de la lista (nullptr)

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){ // Aplica la función func a cada nodo de la lista, pasando los argumentos adicionales args a la función func
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename Traits>
void LinkedList<Traits>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){ // Inserta un nuevo nodo en la posición correcta según el criterio de ordenamiento definido por Comp
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev == m_pRoot)
            m_pTail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){ // inserta un nuevo nodo con el valor value y la referencia ref en la lista
    internal_insert(m_pRoot, value, ref);
}

    // COPY CONSTRUCTOR: El constructor de copia de la clase LinkedList se encarga de 
    //crear una nueva instancia de la lista a partir de otra instancia existente. 
    //Recorre la lista original desde el nodo raíz, creando nuevos nodos con los mismos 
    // valores y referencias, y enlazándolos en la nueva lista. Al finalizar, la nueva 
    //lista tendrá la misma estructura y contenido que la lista original.
    template <typename Traits>
    LinkedList<Traits>::LinkedList(const LinkedList &other){ // Copy constructor
        m_pRoot = nullptr; // Inicializamos el puntero raíz de la nueva lista a nullptr
        m_pTail = nullptr; // Inicializamos el puntero a la cola de la nueva lista a nullptr
        m_size = 0;

        Node* current = other.m_pRoot; // Comenzamos desde el nodo raíz de la lista original
        while(current != nullptr){ // Recorremos la lista original hasta el final
            push_back(current->getData(), current->getRef()); // Insertamos un nuevo nodo
            current = current->getNext(); // Avanzamos al siguiente nodo en la lista original
        }
    }

    // MOVE CONSTRUCTOR: El constructor de movimiento de la clase LinkedList se encarga de 
    // transferir la propiedad de los recursos de una instancia a otra sin realizar una copia profunda.
    // Toma una instancia de LinkedList como parámetro y mueve los punteros raíz y
    // cola de la lista original a la nueva instancia, así como el tamaño de la lista.
    // Después de la transferencia, la lista original se deja en un estado válido pero vacío,
    // con sus punteros raíz y cola establecidos en nullptr y su tamaño en 0.
    template <typename Traits>
    LinkedList<Traits>::LinkedList(LinkedList &&other){
        m_pRoot = other.m_pRoot; // Transferimos el puntero raíz de la lista original a la nueva instancia
        m_pTail = other.m_pTail; // Transferimos el puntero a la cola de la lista original a la nueva instancia
        m_size = other.m_size;

        other.m_pRoot = nullptr;
        other.m_pTail = nullptr;
        other.m_size = 0;
    }

template <typename Traits>
string  LinkedList<Traits>::toString() { // Devuelve una representación en forma de cadena de la lista, mostrando los valores y referencias de cada nodo
    scoped_lock<mutex> lock(m_mtx);
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size()-1 ; ++i ){
            ss << *pNode << ",";
            pNode = pNode->getNext();
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, LinkedList<Traits>& list){
    return os << list.toString();
}

template <typename Traits>
LinkedList<Traits>& LinkedList<Traits>::operator=(const LinkedList &other){
    if (this == &other)
        return *this;

    while (m_pRoot != nullptr)
        pop_front();

    Node* current = other.m_pRoot;
    while (current != nullptr){
        push_back(current->getData(), current->getRef());
        current = current->getNext();
    }

    return *this;
}

template <typename Traits>
LinkedList<Traits>& LinkedList<Traits>::operator=(LinkedList &&other){
    if (this == &other)
        return *this;

    while (m_pRoot != nullptr)
        pop_front();

    m_pRoot = other.m_pRoot;
    m_pTail = other.m_pTail;
    m_size = other.m_size;

    other.m_pRoot = nullptr;
    other.m_pTail = nullptr;
    other.m_size = 0;

    return *this;
}


template <typename Traits>
istream& operator>>(istream& is, LinkedList<Traits>& list){
    LinkedList<Traits> temp;
    char ch;
    typename LinkedList<Traits>::value_type value;
    Ref ref;

    if(!(is >> ch) || ch != '['){ // Verificamos que el primer carácter sea '['
        is.setstate(std::ios::failbit); // Si no es así, establecemos el estado de fallo en el flujo de entrada
        return is;
    }
        
    if(!(is >> ch)){ // Verificamos que podamos leer el siguiente carácter
        is.setstate(std::ios::failbit); // Si no podemos leer, establecemos el estado de fallo en el flujo de entrada
        return is;
    }

    if(ch == ']'){ // Si el siguiente carácter es ']', significa que la lista está vacía, por lo que devolvemos el flujo de entrada sin modificar la lista
        return is;
    }

    is.putback(ch); // Si el siguiente carácter no es ']', lo devolvemos al flujo de entrada para procesarlo como parte de los nodos de la lista

    while (true)
    {
        if(!(is >> ch) || ch != '('){ // Verificamos que podamos leer un nodo, el formato esperado es '(value, ref)'
            is.setstate(std::ios::failbit); // Si no podemos leer un nodo o el formato no es correcto, establecemos el estado de fallo en el flujo de entrada
            return is;
        }

        if(!(is >> value)){ // Verificamos que podamos leer el valor del nodo
            is.setstate(std::ios::failbit); // Si no podemos leer el valor, establecemos el estado de fallo en el flujo de entrada
            return is;
        }

        if(!(is >> ch) || ch != ','){ // Verificamos que el siguiente carácter sea una coma ',' después del valor
            is.setstate(std::ios::failbit); // Si no es así, establecemos el estado de fallo en el flujo de entrada
            return is;
        }

        if(!(is >> ref)){ // Verificamos que podamos leer la referencia del nodo
            is.setstate(std::ios::failbit); // Si no podemos leer la referencia, establecemos el estado de fallo en el flujo de entrada
            return is;
        }

        if(!(is >> ch) || ch != ')'){ // Verificamos que el siguiente carácter sea un paréntesis de cierre ')' después de la referencia
            is.setstate(std::ios::failbit); // Si no es así, establecemos el estado de fallo en el flujo de entrada
            return is;
        }

        temp.push_back(value, ref); // Si hemos leído correctamente un nodo, lo agregamos a la lista temporal

        if(!(is >> ch)){
            is.setstate(std::ios::failbit);
            return is;
        }

        if(ch == ']')
            break; // Si el siguiente carácter es ']', significa que hemos terminado de leer la lista, por lo que salimos del bucle
        
        if(ch != ','){ // Si el siguiente carácter no es una coma ',' ni un paréntesis de cierre ']', el formato es incorrecto, por lo que establecemos el estado de fallo en el flujo de entrada
            is.setstate(std::ios::failbit);
            return is;
        }
    }

    list = std::move(temp); // Asignamos la lista temporal a la lista de destino utilizando el operador de movimiento para evitar copias innecesarias
    return is; // Devolvemos el flujo de entrada después de haber leído correctamente la lista
}


template <typename Traits>
typename LinkedList<Traits>::Node& LinkedList<Traits>::operator[](size_t index){ // Sobrecarga del operador [] para acceder a los nodos de la lista por índice
    if(index >= m_size) // Si el índice está fuera de rango, lanzamos una excepción
        throw std::out_of_range("operator[]: index out of range");

    Node* current = m_pRoot; // Comenzamos desde el nodo raíz
    for(size_t i = 0; i < index; ++i){ // Recorremos la lista hasta llegar al nodo en la posición especificada por el índice
         current = current->getNext();
    }

    return *current; // Devolvemos una referencia al nodo encontrado
}

#endif // __LINKEDLIST_H__