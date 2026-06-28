// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include "../types.h"

#define DEFAULT_BTREE_ORDER 3

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
       m_Root.Print(os);
}






#endif