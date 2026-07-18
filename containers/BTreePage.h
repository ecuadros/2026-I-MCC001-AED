/**
 * @file BTreePage.h
 * @brief Implementación de la página interna del árbol B.
 *
 * En este archivo se maneja la lógica interna de cada página del árbol B:
 * inserción, eliminación, redistribución, merge, split y acceso a nodos
 * extremos para los iteradores. Esta clase no es el contenedor final,
 * sino la estructura interna sobre la que se apoya BTree.
 */

//CBTreePage.h

/*************************
#ifndef BTPage_H
#define BTPage_H
***************************/
#ifndef CBTreePage_H
#define CBTreePage_H
#include <vector>
#include <iostream>
#include <assert.h>
#include <mutex>
#include "../types.h"

// Si no lo encuentra, deberia decirme:
// cual es la posicion donde deberia estar
/**
 * @brief Búsqueda binaria sobre un contenedor indexable.
 *
 * Esta función se usa para hallar la posición donde está o debería estar
 * una clave dentro del vector de keys de una página.
 */
template <typename Container, typename ObjType>
TI binary_search(Container& container, TI first, TI last, ObjType &object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               TI mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}

/**
 * @brief Inserta un elemento en una posición desplazando hacia la derecha.
 */
template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, TI pos)
{
       TI size = container.size();
       for(TI i = size-2 ; i >= pos ; i--)
               container[i+1] = container[i];
       container[pos] =  object;
}

/**
 * @brief Elimina lógicamente una posición desplazando elementos hacia la izquierda.
 */
template <typename Container>
void remove(Container& container, TI pos)
{
       TI size = container.size();
       for(TI i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename Traits>
class BTree;

template <typename Container>
class BTreeForwardIterator;

template <typename Container>
class BTreeBackwardIterator;

using namespace std;
enum bt_ErrorCode {bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged};

/*template <typename keyType>
bool operator>=(const _Node<keyType>& object1, const _Node<keyType>& object2)
{ return object1.key >= object2.key;    }

template <typename keyType>
bool operator<=(const _Node<keyType>& object1, const _Node<keyType>& object2)
{ return object1.key <= object2.key;    }*/

/**
 * @brief Nodo lógico que almacena la clave y su identificador.
 *
 * Este nodo no tiene hijos directos; los hijos se manejan a nivel de página.
 * Aquí solo se guarda la información de cada entrada del árbol B.
 */
template <typename Traits>
struct tagNode
{
       using keyType = typename Traits::keyType;
       using ObjIDType = typename Traits::ObjIDType;

       keyType                 key;
       ObjIDType               ObjID;
       Ref                     UseCounter;

       tagNode(const keyType     &_key, ObjIDType _ObjID)
               : key(_key), ObjID(_ObjID), UseCounter(0) {}
       tagNode()                          {}
       operator keyType                   ()     { return key; }
       Ref                    GetUseCounter() { return UseCounter;    }
};


/**
 * @brief Página interna del árbol B.
 *
 * Una página agrupa varias claves y varios punteros a subpáginas.
 * En esta clase se resuelve la lógica estructural del árbol B:
 * insertar, partir páginas, fusionar páginas y redistribuir elementos.
 */
template <typename Traits>
class CBTreePage 
// this is the in-memory version of the CBTreePage
{
       friend class BTree<Traits>;
       using keyType = typename Traits::keyType;
       using ObjIDType = typename Traits::ObjIDType;

       using BTPage = CBTreePage<Traits>;         // useful shorthand
       using Node = tagNode<Traits>;

       //typedef void (*lpfnForEach2)(Node &info, TI level, void *pExtra1);
       //typedef void (*lpfnForEach3)(Node &info, TI level, void *pExtra1, void *pExtra2);

       //typedef Node *(*lpfnFirstThat2)(Node &info, TI level, void *pExtra1);
       //typedef Node *(*lpfnFirstThat3)(Node &info, TI level, void *pExtra1, void *pExtra2);
 public:
       /** @brief Construye una página con una capacidad máxima de claves. */
       CBTreePage(TI maxKeys, TB unique = true);
       /** @brief Destructor de la página. */
       virtual ~CBTreePage();

       /** @brief Inserta una clave dentro de la página o sus subpáginas. */
       bt_ErrorCode    Insert (const keyType &key, const ObjIDType ObjID);
       /** @brief Elimina una clave de la página o sus subpáginas. */
       bt_ErrorCode    Remove (const keyType &key, const ObjIDType ObjID);
       /** @brief Busca una clave y devuelve su ObjID si existe. */
       TB            Search (const keyType &key, ObjIDType &ObjID);
       /** @brief Imprime la página siguiendo su recorrido interno. */
       void            Print  (ostream &os);
       template <typename Func, typename... Args>
       void      ForEach(Func func, TI level, Args&&... args);
//        template <typename Func, typename... Args>
//        Node*     FirstThat(Func func, TI level, Args&&... args);

protected:
       TI  m_MinKeys; // minimum number of keys in a node
       TI  m_MaxKeys, // maximum number of keys in a node
                m_MaxKeysForChilds; // just to distinguish the root
       TB m_Unique;
       TB m_isRoot;
       vector<Node>       m_Keys;
       vector<BTPage *>   m_SubPages;
       TI  m_KeyCount;
       mutex m_mtx;

       void  Create();
       void  Reset ();
       void  Destroy () {   Reset(); delete this;}
       void  clear ();

       TB  Redistribute1   (TI &pos);
       TB  Redistribute2   (TI pos);
       void  RedistributeR2L (TI pos);
       void  RedistributeL2R (TI pos);

       TB    TreatUnderflow  (TI &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       bt_ErrorCode    Merge  (TI pos);
       bt_ErrorCode    MergeRoot ();
       void  SplitChild (TI pos);

       Node &GetFirstNode();
       Node &GetLastNode();
       /** @brief Obtiene el sucesor inorder de un nodo dentro del árbol B. */
       Node *Next(Node *pNode);
       /** @brief Obtiene el predecesor inorder de un nodo dentro del árbol B. */
       Node *Prev(Node *pNode);

       TB Overflow()  { return m_KeyCount > m_MaxKeys; }
       TB Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       TB IsFull()    { return m_KeyCount >= m_MaxKeys; }
       TI  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
       TI  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       TI& NumberOfKeys()  { return m_KeyCount; }
       TI  GetNumberOfKeys()  { return m_KeyCount; }
       TB IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       void SetMaxKeysForChilds(TI orderforchilds)
       {
               m_MaxKeysForChilds = orderforchilds;
       }

       TI GetFreeCellsOnLeft(TI pos);
       TI GetFreeCellsOnRight(TI pos);

private:
       TB SplitRoot();
       void SplitPageInto3(vector<Node>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               Node        & oi1,
                                               Node        & oi2);
       void MovePage(BTPage *  pChildPage,vector<Node> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Traits>
CBTreePage<Traits>::CBTreePage(TI maxKeys, TB unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
        scoped_lock<mutex> lock(m_mtx);
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename Traits>
CBTreePage<Traits>::~CBTreePage()
{
        scoped_lock<mutex> lock(m_mtx);
       Reset();
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Insert(const keyType& key, const ObjIDType ObjID)
{
       scoped_lock<mutex> lock(m_mtx);
       TI pos = binary_search(m_Keys, 0, m_KeyCount, key);
       bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; // this key is duplicate

       if( !m_SubPages[pos] ) // this is a leave
       {
               ::insert_at(m_Keys, Node(key, ObjID), pos);
               NumberOfKeys()++;
               if( Overflow() )
                       return bt_overflow;
               return bt_ok;
       }
        // recursive insertion
        error = m_SubPages[pos]->Insert(key, ObjID);
        if( error == bt_overflow )
        {
                if( !Redistribute1(pos) )
                        SplitChild(pos);
                if( Overflow() )          // Propagate overflow
                        return bt_overflow;
                return bt_ok;
        }
       return bt_ok;
}

template <typename Traits>
TB CBTreePage<Traits>::Redistribute1(TI &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {       // nkol = Number of keys on left brother, nkor = Number of keys on right brother
               TI nkol = 0,
                  nkor = 0;
               // is this the first element or there are more elements on right brother
               if( pos > 0 )
                       nkol = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       nkor = m_SubPages[pos+1]->NumberOfKeys();

               if( nkol > nkor ){
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); // bring elements from left brother
                       else{
                               if( pos == NumberOfKeys() )
                                        --pos;
                                return false;
                       }
                }
               else //nkol < nkor )
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); // bring elements from right brother
                       else{
                               if( pos == 0 )
                                       ++pos;
                               return false;
                       }
       }
       else // it is due to overflow
       {
               TI fcol = GetFreeCellsOnLeft(pos),   // Free Cells On Left
                   fcor = GetFreeCellsOnRight(pos);  // Free Cells On Right

               if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( fcol > fcor ) // There is more space on left
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);

       }
       return true;
}

// Redistribute2 function
// it considers two brothers m_SubPages[pos-1] && m_SubPages[pos+1]
// if it fails the only way is merge !
template <typename Traits>
TB CBTreePage<Traits>::Redistribute2(TI pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       // Rotate R2L
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else // The problem is exactly at pos !
       {
               // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(TI pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
             pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-left page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               // Move the pointer leftest pointer to the rightest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               // Move the leftest element to the root
               m_Keys[pos-1] = pSource->m_Keys[0];

               // Remove the leftest element from rigth page
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeL2R(TI pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-RIGHT page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               // Move the pointer rightest pointer to the leftest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               // Move the rightest element to the root
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               // Remove the leftest element from rigth page
               // it is not necessary erase because m_KeyCount controls
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::SplitChild(TI pos)
{
       // FIRST: deciding the second page to split
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   // is left page full ?
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   // is right page full ?
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

       //TI nKeys = pChild1->GetNumberOfKeys() + pChild2->GetNumberOfKeys() + 1;

       // SECOND: copy both pages to a temporal one
       // Create two tmp vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;
       //tmpKeys.resize(nKeys+1);

       // Prepara el vectpor unificado de las 2 paginas a ser divididas en 3
       // copy from left child
       MovePage(pChild1, tmpKeys, tmpSubPages);
       // copy a key from parent
       tmpKeys    .push_back(m_Keys[pos]);

       // copy from right child
       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       Node oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       // copy the first element to the root
       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       // copy the second element to the root
       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename Traits>
void CBTreePage<Traits>::SplitPageInto3(vector<Node>& tmpKeys,
                                    vector<BTPage *>  & tmpSubPages,
                                    BTPage*                   &     pChild1,
                                    BTPage*                   &     pChild2,
                                    BTPage*                   &     pChild3,
                                    Node                & oi1,
                                    Node                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       // Split tmpKeys page into 3 pages
       // copy 1/3 elements to the first child
       pChild1->clear();
       TI nKeys = (tmpKeys.size()-2)/3;
       TI i = 0;
       for( ; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       // first element to go up !
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       // copy 1/3 to the second child
       nKeys += (tmpKeys.size()-2)/3 + 1;
       TI j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       // copy the second element to the root
       oi2 = tmpKeys[i++];

       // copy 1/3 to the third child
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename Traits>
TB CBTreePage<Traits>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       Node oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       // copy the first element to the root
       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       // copy the second element to the root
       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

template <typename Traits>
TB CBTreePage<Traits>::Search(const keyType &key, ObjIDType &ObjID)
{
       scoped_lock<mutex> lock(m_mtx);
       TI pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos >= m_KeyCount ){
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
               else
                       return false;
       }
       if( key == m_Keys[pos].key )
       {
               ObjID = m_Keys[pos].ObjID;
               m_Keys[pos].UseCounter++;
               return true;
       }
       if( key < m_Keys[pos].key )
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
       return false;
}

/*template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEachReverse(lpfnForEach2 lpfn, int level, void *pExtra1)
{
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1);
       for( int i = m_KeyCount-1 ; i >= 0  ; i--)
       {
               lpfn(m_Keys[i], level, pExtra1);
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
       }
}*/

template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::ForEach(Func lpfn, TI level, Args&&... args)
{
       scoped_lock<mutex> lock(m_mtx);
       for( TI i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, std::forward<Args>(args)...);
               lpfn(m_Keys[i], level, std::forward<Args>(args)...);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, std::forward<Args>(args)...);
}

// template <typename Traits>
// void CBTreePage<Traits>::ForEach(lpfnForEach3 lpfn, TI level, void *pExtra1, void *pExtra2)
// {
//        for( TI i = 0 ; i < m_KeyCount ; i++)
//        {
//                if( m_SubPages[i] )
//                        m_SubPages[i]->ForEach(lpfn, level+1, pExtra1, pExtra2);
//                lpfn(m_Keys[i], level, pExtra1, pExtra2);
//        }
//        if( m_SubPages[m_KeyCount] )
//                m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1, pExtra2);
// }

// template <typename Traits>
// template <typename Func, typename... Args>
// typename CBTreePage<Traits>::Node *
// CBTreePage<Traits>::FirstThat(Func func,
//                                         TI level, Args&&... args)
// {
//        scoped_lock<mutex> lock(m_mtx);
//        Node *pTmp;
//        for( TI i = 0 ; i < m_KeyCount ; i++)
//        {
//                if( m_SubPages[i] ){
//                         pTmp = m_SubPages[i]->FirstThat(func, level+1, std::forward<Args>(args)...);
//                        if( pTmp )
//                                return pTmp;
//                }
//                if( func(m_Keys[i], level, std::forward<Args>(args)...) )
//                        return &m_Keys[i];
//        }
//        if( m_SubPages[m_KeyCount] ){
//                 pTmp = m_SubPages[m_KeyCount]->FirstThat(func, level+1, std::forward<Args>(args)...);
//                if( pTmp )
//                        return pTmp;
//        }
//        return 0;
// }

// template <typename Traits>
// typename CBTreePage<Traits>::Node *
// CBTreePage<Traits>::FirstThat(lpfnFirstThat3 lpfn,TI level, void *pExtra1, void *pExtra2)
// {
//        Node *pTmp;
//        for( TI i = 0 ; i < m_KeyCount ; i++){
//                if( m_SubPages[i] ){
//                        pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
//                        if( pTmp )
//                            return pTmp;
//                }
//                if( lpfn(m_Keys[i], level, pExtra1, pExtra2) )
//                        return &m_Keys[i];
//        }
//         if( m_SubPages[m_KeyCount] )
//         {       pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
//                 if( pTmp ) 
//                 return pTmp;
//         }
//         return 0;
// }

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Remove(const keyType &key, const ObjIDType ObjID)
{
       scoped_lock<mutex> lock(m_mtx);
       bt_ErrorCode error = bt_ok;
       TI pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos < NumberOfKeys() && key == m_Keys[pos].key /*&& m_Keys[pos].m_ObjID == ObjID*/) // We found it !
       {
               // This is a leave: First
               if( !m_SubPages[pos+1] )  // This is a leave ? FIRST CASE !
               {
                       ::remove(m_Keys, pos);
                       NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                       return bt_ok;
               }

               // We FOUND IT BUT it is NOT a leave ? SECOND CASE !
               {
                       // Get the first element from right branch
                       Node &rFirstFromRight = m_SubPages[pos+1]->GetFirstNode();
                       // change with a leave
                       swap(m_Keys[pos], rFirstFromRight);
                       // Remove it from this leave

                       //Print(cout);
                       error = m_SubPages[++pos]->Remove(key, ObjID);
               }
       }
       else if( pos == NumberOfKeys() ) // it is not here, go by the last branch
               error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ){ // = is because identical keys are inserted on left (see Insert)
               if( m_SubPages[pos] )
                       error = m_SubPages[pos]->Remove(key, ObjID);
               else
                       return bt_nofound;
       }
       if( error == bt_underflow ){
               // THIRD CASE: After removing the element we have an underflow
               //Print(cout);
               if( TreatUnderflow(pos) )
                       return bt_ok;
               // FOURTH CASE: it was not possible to redistribute -> Merge
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
               return Merge(pos);
       }
       if( error == bt_nofound )
               return bt_nofound;
       return bt_ok;
}


template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Merge(TI pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                m_SubPages[ pos ]->NumberOfKeys() +
                m_SubPages[pos+1]->NumberOfKeys() ==
                3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       // FIRST: Put all the elements into a vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       // Move 1/2 elements to pChild1
       TI nKeys = pChild1->GetFreeCells();
       TI i = 0;
       for( ; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();
       TI j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::MergeRoot()
{
       TI pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       TI nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       // FIRST: Put all the elements into a vector
       vector<Node> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       TI i = 0;
       for( ; i < nKeys ; i++ ){
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       //Print(cout);
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename Traits>
typename CBTreePage<Traits>::Node &
CBTreePage<Traits>::GetFirstNode()
{
       scoped_lock<mutex> lock(m_mtx);
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstNode();
       return m_Keys[0];
}

// Deben eliminarlo e imprimir con un ForEach
template <typename Traits>
void Print(tagNode<Traits> &info, TI level, void *pExtra)
{
        ostream &os = *(ostream *)pExtra;
        for( TI i = 0; i < level ; i++)
                os << "\t";
        os << info.key << "->" << info.ObjID << "\n";
}

template <typename Traits>
void CBTreePage<Traits>::Print(ostream & os)
{
       ForEach(&::Print<Traits>, 0, &os);
}

template <typename Traits>
void CBTreePage<Traits>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = 2 * m_MaxKeys/3;
}

template <typename Traits>
void CBTreePage<Traits>::Reset()
{
       for( TI i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename Traits>
void CBTreePage<Traits>::clear()
{
       //m_Keys.clear();
       //m_SubPages.clear();
       m_KeyCount = 0;
}

template <typename Traits>
CBTreePage<Traits> * CreateBTreeNode (TI maxKeys, TB unique)
{
       return new CBTreePage<Traits> (maxKeys, unique);
}

template <typename Traits>
void CBTreePage<Traits>::MovePage(BTPage *pChildPage, vector<Node> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
       TI nKeys = pChildPage->GetNumberOfKeys();
       TI i = 0;
       for( ; i < nKeys; i++ )
       {
               tmpKeys    .push_back(pChildPage->m_Keys[i]);
               tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

template <typename Traits>
TI CBTreePage<Traits>::GetFreeCellsOnLeft(TI pos)
{
       if( pos > 0 )                                   // there is some page on left ?
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename Traits>
TI CBTreePage<Traits>::GetFreeCellsOnRight(TI pos)
{
       if( pos < GetNumberOfKeys() )   // there is some page on right ?
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

// IMPLEMENTACION TAREA
/**
 * @brief Devuelve el último nodo del subárbol actual.
 *
 * Se usa como apoyo para los iteradores y para poder iniciar
 * el recorrido backward desde el nodo más a la derecha.
 */
template <typename Traits>
typename CBTreePage<Traits>::Node &CBTreePage<Traits>::GetLastNode()
{
       scoped_lock<mutex> lock(m_mtx);
       if( m_SubPages[m_KeyCount] )
               return m_SubPages[m_KeyCount]->GetLastNode();
       return m_Keys[m_KeyCount-1];
}

/**
 * @brief Busca el sucesor inorder de un nodo.
 *
 */
template <typename Traits>
typename CBTreePage<Traits>::Node *CBTreePage<Traits>::Next(Node *pNode)
{
       scoped_lock<mutex> lock(m_mtx);

       // pNode es un puntero a un nodo, y se está accediendo a su miembro key. Esto es útil para buscar la posición de la clave en la página actual del árbol B.
       keyType key = pNode->key;
       // Buscar la posición de la clave en la página actual
       TI pos = binary_search(m_Keys, 0, m_KeyCount, key);

       // Caso 1: La clave esta en esta misma pagina
       if( pos < m_KeyCount && &m_Keys[pos] == pNode )
       {
               // Si tiene subpagina derecha, el sucesor es el menor de ese subarbol
               if( m_SubPages[pos + 1] ) return &m_SubPages[pos + 1]->GetFirstNode();

               // Si no tiene subpagina derecha, el sucesor puede estar en la siguiente key de la pagina
               if( pos + 1 < m_KeyCount )return &m_Keys[pos + 1];

               // Si no hay nada a la derecha en esta pagina, el sucesor lo resolvera el ancestro
               return nullptr;
       }

       // Caso 2: la clave no esta en esta pagina, bajar solo por la rama donde deberia estar
       if( pos<= m_KeyCount && m_SubPages[pos] )
       {
               Node *pRes = m_SubPages[pos]->Next(pNode);
               if( pRes ) return pRes;

               // Si el hijo no encontro sucesor dentro de su subarbol, entonces el sucesor puede ser la key actual del padre
               if( pos < m_KeyCount ) return &m_Keys[pos];
       }

       return nullptr;
}

/**
 * @brief Busca el predecesor inorder de un nodo.
 *
 */
template <typename Traits>
typename CBTreePage<Traits>::Node *CBTreePage<Traits>::Prev(Node *pNode)
{
    scoped_lock<mutex> lock(m_mtx);

    keyType key = pNode->key;
    TI pos = binary_search(m_Keys, 0, m_KeyCount, key);

    // Caso 1: el nodo esta en esta misma pagina
    if( pos < m_KeyCount && &m_Keys[pos] == pNode )
    {
        // El anterior esta en la subpagina izquierda inmediata
        if( m_SubPages[pos] ) return &m_SubPages[pos]->GetLastNode();

        // O puede ser la key anterior de esta misma pagina
        if( pos > 0 ) return &m_Keys[pos - 1];

        // No hay predecesor en esta pagina
        return nullptr;
    }

    // Caso 2: el nodo debe estar dentro de una subpagina
    if( pos <= m_KeyCount && m_SubPages[pos] )
    {
        Node *pRes = m_SubPages[pos]->Prev(pNode);
        if( pRes ) return pRes;

        // Si ya no encontro mas dentro de esa subpagina, el anterior es la key separadora de la izquierda
        if( pos > 0 ) return &m_Keys[pos - 1];
    }

    return nullptr;
}



#endif
