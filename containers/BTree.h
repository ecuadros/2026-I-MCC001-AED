/**
 * @file BTree.h
 * @brief Implementación principal del contenedor BTree.
 *
 * En este archivo trabajé la clase BTree como contenedor principal.
 * Aquí se centraliza la inserción, eliminación, búsqueda, recorridos
 * por iteradores, uso de concurrencia con mutex y la reutilización
 * de los algoritmos globales ::ForEach y ::FirstThat.
 */

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include <mutex>
#include "../types.h"
#include "general_iterator.h"
#include "../foreach.h"

#define DEFAULT_BTREE_ORDER 3

template <typename Container>
class BTreeForwardIterator: public general_iterator<Container, BTreeForwardIterator<Container> >
{
       using Myself = BTreeForwardIterator<Container>;
       using Parent = general_iterator<Container, Myself>;
       using Node   = typename Container::Node;
public:
       using Parent::Parent;

       /**
        * @brief Avanza al siguiente nodo en el recorrido forward.
        *
        * Este iterador se apoya en el método Next del contenedor
        * para seguir el recorrido inorder hacia adelante.
        *
        * @return Referencia al propio iterador ya avanzado.
        */
       Myself &operator++()
       {
               if( this->m_pNode != nullptr )
                       this->m_pNode = this->m_pContainer->Next(this->m_pNode);
               return *this;
       }
};

template <typename Container>
class BTreeBackwardIterator: public general_iterator<Container, BTreeBackwardIterator<Container>>
{
       using Myself = BTreeBackwardIterator<Container>;
       using Parent = general_iterator<Container, Myself>;
       using Node   = typename Container::Node;
public:
       using Parent::Parent;

       /**
        * @brief Avanza al siguiente nodo del recorrido backward.
        *
        * En este caso avanzar significa ir al nodo anterior en inorder,
        * reutilizando el método Prev del contenedor.
        *
        * @return Referencia al propio iterador ya avanzado.
        */
       Myself &operator++()
       {
               if( this->m_pNode != nullptr )
                       this->m_pNode = this->m_pContainer->Prev(this->m_pNode);
               return *this;
       }
};

template <typename KeyT, typename ObjT>
struct BTreeTraits
{
       /** @brief Tipo de clave que almacena el árbol B. */
       using keyType = KeyT;
       /** @brief Tipo del identificador o referencia asociado a cada clave. */
       using ObjIDType = ObjT;

       /** @brief Alias del iterador forward asociado al contenedor. */
       template <typename Container>
       using ForwardIterator = BTreeForwardIterator<Container>;

       /** @brief Alias del iterador backward asociado al contenedor. */
       template <typename Container>
       using BackwardIterator = BTreeBackwardIterator<Container>;
};

/**
 * @brief Contenedor principal del árbol B.
 *
 * Esta clase representa el árbol B completo y se apoya internamente
 * en CBTreePage para manejar cada página. Aquí también agregué
 * iteradores forward/backward, concurrencia con mutex y la reutilización
 * de los algoritmos globales ::ForEach y ::FirstThat.
 *
 * @tparam Traits Trait que define tipos como keyType, ObjIDType e iteradores.
 */
template <typename Traits>
class BTree 
// this is the full version of the BTree
{
       using keyType = typename Traits::keyType;
       using ObjIDType = typename Traits::ObjIDType;
       using BTNode = CBTreePage<Traits>;// useful shorthand
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/

public:
       //typedef Node iterator;
       // typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       // typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       // typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       // typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       using Node = typename BTNode::Node;
       using Myself = BTree<Traits>;

       using ForwardIterator = typename Traits::template ForwardIterator<Myself>;
       using BackwardIterator = typename Traits::template BackwardIterator<Myself>;


public:
       /**
        * @brief Construye un árbol B vacío.
        * @param order Orden del árbol.
        * @param unique Indica si se permiten o no claves duplicadas.
        */
       BTree(OrderInt order = DEFAULT_BTREE_ORDER, TB unique = true);
       /** @brief Destructor del árbol B. */
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       /**
        * @brief Inserta una clave con su identificador asociado.
        * @param key Clave a insertar.
        * @param ObjID Identificador o referencia asociado a la clave.
        * @return true si la inserción fue exitosa, false si la clave es duplicada.
        */
       TB            Insert (const keyType key, const ObjIDType ObjID);
       /**
        * @brief Elimina una clave del árbol.
        * @param key Clave a eliminar.
        * @param ObjID Identificador asociado.
        * @return true si se eliminó correctamente, false si no se encontró.
        */
       TB            Remove (const keyType key, const ObjIDType ObjID);
       /**
        * @brief Busca una clave dentro del árbol.
        * @param key Clave buscada.
        * @return ObjIDType Identificador asociado o -1 si no se encuentra.
        */
       ObjIDType     Search (const keyType key);
       numKeysLong            size()  { return m_NumKeys; }
       HeightInt            height() { return m_Height;      }
       OrderInt            GetOrder() { return m_Order;     }

       /** @brief Imprime el contenido del árbol usando el recorrido de sus páginas. */
       void            Print (ostream &os);
       /**
        * @brief Recorre el árbol reutilizando el algoritmo global ::ForEach.
        *
        * Esta fue una de las partes nuevas de la tarea: en vez de repetir
        * la lógica del bucle, el contenedor usa begin/end junto al foreach global.
        */
       template<typename Func, typename... Args>
       void      ForEach(Func lpfn, Args &&... args);
       /**
        * @brief Busca el primer nodo que cumpla una condición.
        *
        * Aquí también se reutiliza el algoritmo global ::FirstThat sobre
        * los iteradores del contenedor.
        */
       template<typename Func, typename... Args>
       Node*     FirstThat(Func lpfn, Args &&... args);
       //typedef               Node iterator;


       //ITERATORS
       /** @brief Devuelve el primer nodo del recorrido inorder. */
       Node* First();
       /** @brief Devuelve el último nodo del recorrido inorder. */
       Node* Last();
       /** @brief Devuelve el sucesor inorder de un nodo. */
       Node* Next(Node* pNode);
       /** @brief Devuelve el predecesor inorder de un nodo. */
       Node* Prev(Node* pNode);

       /** @brief Inicio del recorrido forward. */
       ForwardIterator begin();
       /** @brief Fin del recorrido forward. */
       ForwardIterator end();
       /** @brief Inicio del recorrido backward. */
       BackwardIterator rbegin();  
       /** @brief Fin del recorrido backward. */
       BackwardIterator rend();

protected:
       BTNode          m_Root;
       HeightInt             m_Height;  // height of tree
       OrderInt             m_Order;   // order of tree
       numKeysLong           m_NumKeys; // number of keys
       TB            m_Unique;  // Accept the elements only once ?
       mutex m_mtx;
};

const HeightInt MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(OrderInt order, TB unique)
                               : m_Root(2 * order + 1, unique),
                                 m_Height(1),
                                 m_Order(order),
                                 m_NumKeys(0),
                                 m_Unique(unique)
{
       scoped_lock<mutex> lock(m_mtx);
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
       scoped_lock<mutex> lock(m_mtx);
}

template <typename Traits>
TB BTree<Traits>::Insert(const keyType key, const ObjIDType ObjID)
{
       scoped_lock<mutex> lock(m_mtx);
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Traits>
TB BTree<Traits>::Remove (const keyType key, const ObjIDType ObjID)
{
       scoped_lock<mutex> lock(m_mtx);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTree<Traits>::ObjIDType BTree<Traits>::Search (const keyType key)
{
       scoped_lock<mutex> lock(m_mtx);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

template <typename Traits>
template<typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args &&... args)
{
       scoped_lock<mutex> lock(m_mtx);
       ::ForEach(begin(), end(), lpfn, std::forward<Args>(args)...);
}

// template <typename Traits>
// template<typename Func, typename... Args>
// void BTree<Traits>::ForEach(Func lpfn, Args... args)
// {
//        m_Root.ForEach(lpfn, 0, std::forward<Args>(args)...);
// }

template <typename Traits>
template<typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func lpfn, Args &&... args)
{
       scoped_lock<mutex> lock(m_mtx);
       auto iter = ::FirstThat(begin(), end(), lpfn, std::forward<Args>(args)...);
       if( iter == end() )
               return nullptr;
       return iter.getNode();
}

// template <typename Traits>
// typename BTree<Traits>::Node *
// BTree<Traits>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
// {
//        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
// }

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       scoped_lock<mutex> lock(m_mtx);
       m_Root.Print(os);
}


//METODOS DE LOS ITERADORES
template <typename Traits>
typename BTree<Traits>::ForwardIterator BTree<Traits>::begin()
{
       return ForwardIterator(this, First());
}

template <typename Traits>
typename BTree<Traits>::ForwardIterator BTree<Traits>::end()
{
       return ForwardIterator(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::BackwardIterator BTree<Traits>::rbegin()
{
       return BackwardIterator(this, Last());
}

template <typename Traits>
typename BTree<Traits>::BackwardIterator BTree<Traits>::rend()
{
       return BackwardIterator(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::First()
{
       if( m_NumKeys == 0 )
               return nullptr;
       return &m_Root.GetFirstNode();
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::Last()
{
       if( m_NumKeys == 0 )
               return nullptr;
       return &m_Root.GetLastNode();
}


template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::Next(Node* pNode)
{
       if( pNode == nullptr )
               return nullptr;
       return m_Root.Next(pNode);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::Prev(Node* pNode)
{
       if( pNode == nullptr )
               return nullptr;
       return m_Root.Prev(pNode);
}

#endif
