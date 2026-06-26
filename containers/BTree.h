// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

template <typename Traits>
class BTree 
// this is the full version of the BTree
{
       using keyType   = typename Traits::value_type;
       using ObjIDType = typename Traits::ref;

       typedef CBTreePage <Traits> BTNode;// useful shorthand
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/

public:
       //typedef Node iterator;
       //typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       //typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       //typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       //typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       
       template <typename... Args>
       using lpfnForEach = typename BTNode::template lpfnForEach<Args...>;

       template <typename... Args>
       using lpfnFirstThat = typename BTNode::template lpfnFirstThat<Args...>;
       
       using Node = typename BTNode::Node;

public:
       BTree(size_t order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType& key, const ObjIDType ObjID);
       bool            Remove (const keyType& key, const ObjIDType ObjID);
       bool            Search (const keyType& key, ObjIDType& ObjID);
       size_t          size()     { return m_NumKeys; }
       size_t          height()   { return m_Height;  }
       size_t          GetOrder() { return m_Order;   }

       void            Print (ostream &os);
       //void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       //void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       //Node*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       //Node*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef               Node iterator;
       template <typename... Args>
       void ForEach(lpfnForEach<Args...> lpfn, Args... args);

       template <typename... Args>
       Node* FirstThat(lpfnFirstThat<Args...> lpfn, Args... args);



protected:
       BTNode          m_Root;
       size_t          m_Height;  // height of tree
       size_t          m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
};

const size_t MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(size_t order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_Height(1),
                                 m_Order(order),
                                 m_NumKeys(0),
                                 m_Unique(unique)
{
       m_Root.SetMaxKeysForChilds(order);
       //m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
bool BTree<Traits>::Insert(const typename Traits::value_type& key, const typename Traits::ref ObjID)
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
bool BTree<Traits>::Remove (const typename Traits::value_type& key, const typename Traits::ref ObjID)
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
bool BTree<Traits>::Search (const typename Traits::value_type& key, typename Traits::ref& ObjID)
{
       //ObjIDType ObjID = -1;
       return m_Root.Search(key, ObjID);
       //return ObjID;
}

/*
template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::Node *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::Node *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}
*/
template <typename Traits>
template <typename... Args>
void BTree<Traits>::ForEach(lpfnForEach<Args...> lpfn, Args... args)
{
       m_Root.ForEach(lpfn, 0, args...);
}

template <typename Traits>
template <typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(lpfnFirstThat<Args...> lpfn, Args... args)
{
       return m_Root.FirstThat(lpfn, 0, args...);
}


template <typename Traits>
void BTree<Traits>::Print(std::ostream &os){
       m_Root.Print(os);
}






#endif