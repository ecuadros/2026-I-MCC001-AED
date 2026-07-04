// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include "../types.h"

#define DEFAULT_BTREE_ORDER 3

template <typename KeyT, typename ObjT>
struct BTreeTraits
{
       using keyType = KeyT;
       using ObjIDType = ObjT;
};

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


public:
       BTree(OrderInt order = DEFAULT_BTREE_ORDER, TB unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       TB            Insert (const keyType key, const ObjIDType ObjID);
       TB            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType     Search (const keyType key);
       numKeysLong            size()  { return m_NumKeys; }
       HeightInt            height() { return m_Height;      }
       OrderInt            GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       template<typename Func, typename... Args>
       void      ForEach(Func lpfn, Args... args);
       template<typename Func, typename... Args>
       Node*     FirstThat(Func lpfn, Args... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       HeightInt             m_Height;  // height of tree
       OrderInt             m_Order;   // order of tree
       numKeysLong           m_NumKeys; // number of keys
       TB            m_Unique;  // Accept the elements only once ?
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
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
TB BTree<Traits>::Insert(const keyType key, const ObjIDType ObjID)
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
TB BTree<Traits>::Remove (const keyType key, const ObjIDType ObjID)
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
typename BTree<Traits>::ObjIDType BTree<Traits>::Search (const keyType key)
{
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template<typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args... args)
{
       m_Root.ForEach(lpfn, 0, std::forward<Args>(args)...);
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
BTree<Traits>::FirstThat(Func lpfn, Args... args)
{
       return m_Root.FirstThat(lpfn, 0, std::forward<Args>(args)...);
}

// template <typename Traits>
// typename BTree<Traits>::Node *
// BTree<Traits>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
// {
//        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
// }

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       m_Root.Print(os);
}






#endif