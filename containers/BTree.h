// btree.h

/**
 * @file btree.h
 * @brief Implementación de una estructura de datos Árbol-B (B-Tree) concurrente con iteradores.
 */

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include "BTreePage.h"
#include "../types.h"
#include "../foreach.h"
#include "general_iterator.h"

/**
 * @def DEFAULT_BTREE_ORDER
 * @brief Orden por defecto del Árbol-B si no se especifica uno en el constructor.
 */
#define DEFAULT_BTREE_ORDER 3

/**
 * @class BTreeForwardIterator
 * @brief Iterador hacia adelante (forward) para recorrer los elementos del B-Tree en orden ascendente.
 * @tparam Container Tipo del contenedor B-Tree sobre el cual se itera.
 */
template <typename Container>
class BTreeForwardIterator : public general_iterator<Container,
                             BTreeForwardIterator<Container>>{
    using MySelf = BTreeForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
public:
    /**
     * @brief Constructor del iterador hacia adelante.
     * @param pContainer Puntero al árbol B-Tree asociado.
     * @param pNode Puntero al nodo/elemento inicial.
     */
    BTreeForwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode) {}
        
    /**
     * @brief Operador de pre-incremento para avanzar al siguiente elemento (in-order).
     * @return Referencia al propio iterador modificado.
     */
    MySelf& operator++(){
        if (!this->m_pNode || !this->m_pContainer) {
            this->m_pNode = nullptr;
            return *this;
        }

        this->m_pNode = this->m_pContainer->GetNextNode(this->m_pNode->key);
        
        return *this;
    }
};

/**
 * @class BTreeBackwardIterator
 * @brief Iterador hacia atrás (backward) para recorrer los elementos del B-Tree en orden descendente.
 * @tparam Container Tipo del contenedor B-Tree sobre el cual se itera.
 */
template <typename Container>
class BTreeBackwardIterator : public general_iterator<Container, BTreeBackwardIterator<Container>> {
    using MySelf = BTreeBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
public:
    /**
     * @brief Constructor del iterador hacia atrás.
     * @param pContainer Puntero al árbol B-Tree asociado.
     * @param pNode Puntero al nodo/elemento inicial del recorrido reverso.
     */
    BTreeBackwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode) {} 
        
    /**
     * @brief Operador de pre-incremento para retroceder al elemento anterior (in-order).
     * @return Referencia al propio iterador modificado.
     */
    MySelf& operator++(){
        if (!this->m_pNode || !this->m_pContainer) {
            this->m_pNode = nullptr;
            return *this;
        }

        this->m_pNode = this->m_pContainer->GetPrevNode(this->m_pNode->key);
        
        return *this;
    }
};

/**
 * @struct BTreeTrait
 * @brief Estructura de rasgos (Traits) para definir los tipos de datos clave y referencia del B-Tree.
 * @tparam T Tipo de la clave (Key).
 * @tparam U Tipo del identificador u objeto referenciado (Object ID).
 */
template <typename T, typename U = TL>
struct BTreeTrait{
    using key_type = T;
    using ref_type   = U;
};

/**
 * @class BTree
 * @brief Implementación completa de un contenedor Árbol-B genérico y seguro para hilos (thread-safe).
 * @tparam Traits Estructura de rasgos que define 'key_type' y 'ref_type'.
 */
template <typename Traits>
class BTree 
// this is the full version of the BTree
{
public:
       using keyType = typename Traits::key_type;               ///< Tipo de dato de la clave.
       using ObjIDType = typename Traits::ref_type;             ///< Tipo de dato de la referencia u objeto asociado.
       using forward_iterator = BTreeForwardIterator<BTree<Traits>>; ///< Tipo del iterador forward estándar.

private:
       typedef CBTreePage <Traits> BTNode;// useful shorthand
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
       typedef typename BTNode::Node      Node;                 ///< Tipo de nodo interno expuesto.

public:
       /**
        * @brief Constructor del Árbol-B.
        * @param order Orden o factor de ramificación del árbol.
        * @param unique Define si el árbol permite claves duplicadas (false) o solo únicas (true).
        */
       BTree(TreeOrderT order = DEFAULT_BTREE_ORDER, TB unique = true);
       
       /**
        * @brief Destructor de la clase BTree.
        */
       ~BTree();
       
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       
       /**
        * @brief Inserta una clave con su respectivo objeto/ID en el árbol de forma segura.
        * @param key Clave a insertar.
        * @param ObjID Identificador u objeto asociado.
        * @return StatusFlag True si se insertó con éxito, False en caso de duplicado (si unique=true).
        */
       StatusFlag      Insert (const keyType key, const ObjIDType ObjID);
       
       /**
        * @brief Elimina una clave y su ID del árbol de forma segura.
        * @param key Clave a eliminar.
        * @param ObjID Identificador u objeto asociado a remover.
        * @return StatusFlag True si se eliminó con éxito, False si no fue encontrado.
        */
       StatusFlag      Remove (const keyType key, const ObjIDType ObjID);
       
       /**
        * @brief Busca un objeto/ID en el árbol por medio de su clave.
        * @param key Clave a buscar.
        * @return ObjIDType ID del objeto encontrado, o -1 si no existe.
        */
       ObjIDType        Search (const keyType key);
       
       /**
        * @brief Retorna la cantidad total de claves almacenadas en el árbol.
        * @return SizeT Número total de llaves.
        */
       SizeT            size()  { return m_NumKeys; }
       
       /**
        * @brief Retorna la altura actual del Árbol-B.
        * @return TreeOrderT Altura del árbol.
        */
       TreeOrderT            height() { return m_Height;      }
       
       /**
        * @brief Retorna el orden configurado en el árbol.
        * @return TreeOrderT Orden del árbol.
        */
       TreeOrderT            GetOrder() { return m_Order;     }

       /**
        * @brief Imprime la estructura y elementos del árbol en el flujo especificado.
        * @param os Flujo de salida (ej. std::cout).
        */
       void            Print (ostream &os);

       /**
        * @brief Aplica una función a cada elemento del árbol de manera ordenada (In-order).
        * @tparam Func Tipo del callable (función, lambda, functor).
        * @tparam Args Tipos de los argumentos variables pasados a la función.
        * @param lpfn Función o predicado a ejecutar por cada elemento.
        * @param args Argumentos variables pasados por referencia adaptativa.
        */
       template <typename Func, typename... Args>
       void            ForEach(Func lpfn, Args&&... args);

       /**
        * @brief Busca el primer elemento que cumpla con una condición dada.
        * @tparam Func Tipo del predicado/condición.
        * @tparam Args Tipos de los argumentos del predicado.
        * @param lpfn Condición a evaluar por elemento.
        * @param args Argumentos adicionales para el predicado.
        * @return Node* Puntero al primer nodo que cumple el criterio, o nullptr si ninguno coincide.
        */
       template <typename Func, typename... Args>
       Node* FirstThat(Func lpfn, Args&&... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;     ///< Nodo raíz o página principal del árbol B.
       TreeOrderT      m_Height;   // height of tree
       TreeOrderT      m_Order;    // order of tree
       SizeT            m_NumKeys; // number of keys
       TB              m_Unique;   // Accept the elements only once ?
       mutex           m_mutex;    ///< Mutex para garantizar la exclusión mutua/concurrencia segura.

public:
    /**
     * @brief Retorna un iterador al primer elemento lógico (el menor) del árbol.
     * @return BTreeForwardIterator Iterador apuntando al inicio.
     */
    BTreeForwardIterator<BTree<Traits>> begin();
    
    /**
     * @brief Retorna un iterador que marca el final lógico del árbol (nullptr).
     * @return BTreeForwardIterator Iterador apuntando al fin.
     */
    BTreeForwardIterator<BTree<Traits>> end();
    
    /**
     * @brief Método de soporte para calcular internamente el nodo sucesor de una clave dada.
     * @param currentKey Clave base.
     * @return Node* Puntero al nodo sucesor inmediato, o nullptr si es el máximo.
     */
    Node* GetNextNode(const keyType& currentKey);

    /**
     * @brief Retorna un iterador reverso apuntando al último elemento lógico (el mayor) del árbol.
     * @return BTreeBackwardIterator Iterador reverso apuntando al inicio del recorrido inverso.
     */
    BTreeBackwardIterator<BTree<Traits>> rbegin();
    
    /**
     * @brief Retorna un iterador reverso que marca el final lógico del recorrido hacia atrás (nullptr).
     * @return BTreeBackwardIterator Iterador apuntando al fin del recorrido inverso.
     */
    BTreeBackwardIterator<BTree<Traits>> rend();
    
    /**
     * @brief Método de soporte para calcular internamente el nodo antecesor de una clave dada.
     * @param currentKey Clave base.
     * @return Node* Puntero al nodo antecesor inmediato, o nullptr si es el mínimo.
     */
    Node* GetPrevNode(const keyType& currentKey);
};

/**
 * @brief Límite estático de la altura del árbol.
 */
const TreeOrderT MaxHeight = 5;

template <typename Traits>
BTree<Traits>::BTree(TreeOrderT order, TB unique)
                               : m_Unique(unique),
                                 m_Order(order),
                                 m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
StatusFlag BTree<Traits>::Insert(const typename Traits::key_type key, const typename Traits::ref_type ObjID)
{
        scoped_lock<mutex> lock(m_mutex);
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
StatusFlag BTree<Traits>::Remove (const typename Traits::key_type key, const typename Traits::ref_type ObjID)
{
        scoped_lock<mutex> lock(m_mutex);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename Traits::ref_type BTree<Traits>::Search (const typename Traits::key_type key)
{
        scoped_lock<mutex> lock(m_mutex);
       typename Traits::ref_type ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args&&... args)
{
    scoped_lock<mutex> lock(m_mutex);
    ::for_each(begin(), end(), lpfn, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func lpfn, Args&&... args)
{
    auto it = ::FirstThat(begin(), end(), lpfn, std::forward<Args>(args)...);
    if(it != end())
        return &(*it);
    return nullptr;
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
         //scoped_lock<mutex> lock(m_mutex);
        ForEach([&os](Node &info){
              os << info.key << "->" << info.ObjID << "\n";
        });
}

template <typename Traits>
// Métodos de inicialización de iteradores
BTreeForwardIterator<BTree<Traits>> BTree<Traits>::begin() {
    BTNode* pCurr = &m_Root;
    // El recorrido in-order comienza en la página hoja más a la izquierda
    while (pCurr && pCurr->m_SubPages[0]) {
        pCurr = pCurr->m_SubPages[0];
    }
    // Si el árbol está vacío o no tiene llaves
    if (!pCurr || pCurr->GetNumberOfKeys() == 0) {
        return end();
    }
    // Apuntamos al primer elemento físico (índice 0) de esa página
    return BTreeForwardIterator<BTree<Traits>>(this, &(pCurr->m_Keys[0]));
}

template <typename Traits>
BTreeForwardIterator<BTree<Traits>> BTree<Traits>::end() {
    return BTreeForwardIterator<BTree<Traits>>(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::GetNextNode(const keyType& currentKey)
{
    BTNode* pCurr = &m_Root;
    Node* pBestAncestor = nullptr; // Guarda el sucesor potencial "hacia arriba" en el árbol

    while (pCurr != nullptr) 
    {
        TI i = 0;
        TI numKeys = pCurr->GetNumberOfKeys();

        // Avanzamos en el vector de llaves de la página actual
        while (i < numKeys && currentKey >= pCurr->m_Keys[i].key) {
            i++;
        }

        // Si la posición 'i' es válida en la página, m_Keys[i] es estrictamente mayor.
        // Lo guardamos como candidato por si el nodo actual resulta ser una hoja.
        if (i < numKeys) {
            pBestAncestor = &(pCurr->m_Keys[i]);
        }

        // Comprobamos si la llave actual coincide con el elemento previo procesado
        if (i > 0 && pCurr->m_Keys[i - 1].key == currentKey) 
        {
            // CASO 1: Tiene un hijo derecho disponible en la subpágina 'i'
            if (pCurr->m_SubPages[i] != nullptr) {
                // El sucesor inmediato es el elemento más a la izquierda de ese subárbol derecho
                BTNode* pSub = pCurr->m_SubPages[i];
                while (pSub->m_SubPages[0] != nullptr) {
                    pSub = pSub->m_SubPages[0];
                }
                return &(pSub->m_Keys[0]);
            }
            // CASO 2: Es una página hoja (no tiene hijo derecho)
            else {
                // El sucesor es el ancestro mayor más cercano guardado durante la bajada
                return pBestAncestor;
            }
        }

        // Si no se encuentra en esta página, descendemos por la subpágina correspondiente
        pCurr = pCurr->m_SubPages[i];
    }

    return pBestAncestor; 
}

template <typename Traits>
BTreeBackwardIterator<BTree<Traits>> BTree<Traits>::rbegin() {
    BTNode* pCurr = &m_Root;
    
    // Bajamos siempre por la subpágina del extremo derecho (valores máximos)
    while (pCurr && pCurr->m_SubPages[pCurr->GetNumberOfKeys()]) {
        pCurr = pCurr->m_SubPages[pCurr->GetNumberOfKeys()];
    }
    if (!pCurr || pCurr->GetNumberOfKeys() == 0) return rend();
    
    // El elemento más a la derecha es la última llave de esa página hoja
    TI lastIdx = pCurr->GetNumberOfKeys() - 1;
    return BTreeBackwardIterator<BTree<Traits>>(this, &(pCurr->m_Keys[lastIdx]));
}

template <typename Traits>
BTreeBackwardIterator<BTree<Traits>> BTree<Traits>::rend() {
    return BTreeBackwardIterator<BTree<Traits>>(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::GetPrevNode(const keyType& currentKey)
{
    BTNode* pCurr = &m_Root;
    Node* pBestAncestor = nullptr; // Guarda el antecesor potencial "hacia arriba" (el último menor)

    while (pCurr != nullptr) 
    {
        TI i = 0;
        TI numKeys = pCurr->GetNumberOfKeys();

        // Buscamos la posición idónea en la página actual
        while (i < numKeys && currentKey > pCurr->m_Keys[i].key) {
            i++;
        }

        // Si i > 0, significa que m_Keys[i-1] es estrictamente MENOR que currentKey.
        // Lo guardamos temporalmente como el mejor ancestro menor disponible.
        if (i > 0) {
            pBestAncestor = &(pCurr->m_Keys[i - 1]);
        }

        // Comprobamos si encontramos la llave exacta
        if (i < numKeys && pCurr->m_Keys[i].key == currentKey) 
        {
            // CASO 1: Tiene un hijo izquierdo disponible en la subpágina 'i'
            if (pCurr->m_SubPages[i] != nullptr) {
                // El antecesor es el elemento más a la DERECHA de ese subárbol izquierdo
                BTNode* pSub = pCurr->m_SubPages[i];
                while (pSub->m_SubPages[pSub->GetNumberOfKeys()] != nullptr) {
                    pSub = pSub->m_SubPages[pSub->GetNumberOfKeys()];
                }
                TI lastIdx = pSub->GetNumberOfKeys() - 1;
                return &(pSub->m_Keys[lastIdx]);
            }
            // CASO 2: Es una página hoja (no tiene hijo izquierdo)
            else {
                // Regresamos el ancestro menor más cercano que vimos al bajar
                return pBestAncestor;
            }
        }

        // Descendemos por la subpágina correspondiente
        pCurr = pCurr->m_SubPages[i];
    }

    return pBestAncestor; 
}

#endif