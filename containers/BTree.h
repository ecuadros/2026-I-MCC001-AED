// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include <utility>

#define DEFAULT_BTREE_ORDER 3

template <typename Traits>
class BTree
// this is the full version of the BTree
{
public:
       using keyType     = typename Traits::key_type;
       using ObjIDType = typename Traits::ObjIDType;
       typedef CBTreePage<keyType, Traits> BTNode;// useful shorthand
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/
       //typedef Node iterator;
       typedef typename BTNode::Node      Node;

public:
       BTree(LSI order = DEFAULT_BTREE_ORDER, LSB unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       LSB             Insert (const keyType key, const ObjIDType ObjID);
       LSB             Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       LSL             size()  { return m_NumKeys; }
       LSL             height() { return m_Height;      }
       LSL             GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       template <typename Func, typename... Args>
	   void ForEach(Func lpfn, Args&&... args);
	   template <typename Func, typename... Args>
	   Node* FirstThat(Func lpfn, Args&&... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       LSI             m_Height;  // height of tree
       LSI             m_Order;   // order of tree
       LSL             m_NumKeys; // number of keys
       LSB             m_Unique;  // Accept the elements only once ?
};

const LSI MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(LSI order, LSB unique) : m_Root(2 * order + 1, unique), m_Height(1), m_Order(order), m_NumKeys(0), m_Unique(unique)
{
       m_Root.SetMaxKeysForChilds(order);
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
LSB BTree<Traits>::Insert(const typename BTree<Traits>::keyType key, const typename BTree<Traits>::ObjIDType ObjID)
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
LSB BTree<Traits>::Remove(const keyType key, const ObjIDType ObjID)
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
typename BTree<Traits>::ObjIDType
BTree<Traits>::Search(const keyType key)
{
       ObjIDType ObjID = -1;
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
typename BTree<Traits>::Node*
BTree<Traits>::FirstThat(Func lpfn, Args&&... args)
{
    return m_Root.FirstThat(lpfn, 0, std::forward<Args>(args)...);
}

template <typename Traits>
void BTree<Traits>::Print(std::ostream &os){
       m_Root.Print(os);
}

#endif
