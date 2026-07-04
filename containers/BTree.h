// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include "../types.h"
#include "general_iterator.h"

#define DEFAULT_BTREE_ORDER 3

template <typename Container>
class BTreeForwardIterator : public general_iterator<Container,
                             BTreeForwardIterator<Container>>{
    using MySelf = BTreeForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
public:
    BTreeForwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode) {}
    MySelf& operator++(){
        if (!this->m_pNode || !this->m_pContainer) {
            this->m_pNode = nullptr;
            return *this;
        }

        this->m_pNode = this->m_pContainer->GetNextNode(this->m_pNode->key);
        
        return *this;
    }
};

template <typename Container>
class BTreeBackwardIterator : public general_iterator<Container, BTreeBackwardIterator<Container>> {
    using MySelf = BTreeBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
public:
    BTreeBackwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode) {} 
    MySelf& operator++(){
        if (!this->m_pNode || !this->m_pContainer) {
            this->m_pNode = nullptr;
            return *this;
        }

        this->m_pNode = this->m_pContainer->GetPrevNode(this->m_pNode->key);
        
        return *this;
    }
};

template <typename T, typename U = TL>
struct BTreeTrait{
    using key_type = T;
    using ref_type   = U;
};

template <typename Traits>
class BTree 
// this is the full version of the BTree
{
public:
       using keyType = typename Traits::key_type;
       using ObjIDType = typename Traits::ref_type;
       using forward_iterator = BTreeForwardIterator<BTree<Traits>>;

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
       typedef typename BTNode::Node      Node;

public:
       BTree(TreeOrderT order = DEFAULT_BTREE_ORDER, TB unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       StatusFlag      Insert (const keyType key, const ObjIDType ObjID);
       StatusFlag      Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       SizeT            size()  { return m_NumKeys; }
       TreeOrderT            height() { return m_Height;      }
       TreeOrderT            GetOrder() { return m_Order;     }

       void            Print (ostream &os);

       template <typename Func, typename... Args>
       void            ForEach(Func lpfn, Args&&... args);

       template <typename Func, typename... Args>
       Node*           FirstThat(Func lpfn, Args&&... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       TreeOrderT      m_Height;  // height of tree
       TreeOrderT      m_Order;   // order of tree
       SizeT            m_NumKeys; // number of keys
       TB            m_Unique;  // Accept the elements only once ?

public:
    BTreeForwardIterator<BTree<Traits>> begin();
    BTreeForwardIterator<BTree<Traits>> end();
    Node* GetNextNode(const keyType& currentKey);

    BTreeBackwardIterator<BTree<Traits>> rbegin();
    BTreeBackwardIterator<BTree<Traits>> rend();
    Node* GetPrevNode(const keyType& currentKey);
};

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
       typename Traits::ref_type ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args&&... args)
{
       m_Root.ForEach(lpfn, 0, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func lpfn, Args&&... args)
{
       return m_Root.FirstThat(lpfn, 0, std::forward<Args>(args)...);
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       ForEach([&os](Node &info, TI level){
              for(TI i = 0; i < level; ++i){
                     os << "\t";
              }
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
        int i = 0;
        int numKeys = pCurr->GetNumberOfKeys();

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
    int lastIdx = pCurr->GetNumberOfKeys() - 1;
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
        int i = 0;
        int numKeys = pCurr->GetNumberOfKeys();

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
                int lastIdx = pSub->GetNumberOfKeys() - 1;
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