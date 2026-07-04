// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include <utility>
#include <mutex>
#include "general_iterator.h"

#define DEFAULT_BTREE_ORDER 3

template <typename Container>
class BTreeForwardIterator;
template <typename Container>
class BTreeBackwardIterator;

template <typename Container>
class BTreeForwardIterator : public general_iterator<Container, BTreeForwardIterator<Container>>
{
       using MySelf = BTreeForwardIterator<Container>;
       using Parent = general_iterator<Container, MySelf>;

public:
       using Node = typename Container::Node;
       using Parent::Parent;
       MySelf& operator++()
       {
               if( this->m_pNode ) {this->m_pNode = this->m_pContainer->Next(this->m_pNode);}
               return *this;
       }
};

template <typename Container>
class BTreeBackwardIterator : public general_iterator<Container, BTreeBackwardIterator<Container>>
{
       using MySelf = BTreeBackwardIterator<Container>;
       using Parent = general_iterator<Container, MySelf>;
public:
       using Node = typename Container::Node;
       using Parent::Parent;
       MySelf& operator++()
       {
               if( this->m_pNode ) {this->m_pNode = this->m_pContainer->Prev(this->m_pNode);}
               return *this;
       }
};
       
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
       using MySelf = BTree<Traits>;
       using forward_iterator  = BTreeForwardIterator<MySelf>;
       using backward_iterator = BTreeBackwardIterator<MySelf>;

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
       mutable std::mutex m_Mutex;

       void            Print (ostream &os);
       template <typename Func, typename... Args>
	   void ForEach(Func lpfn, Args&&... args);
	   template <typename Func, typename... Args>
	   Node* FirstThat(Func lpfn, Args&&... args);
	   forward_iterator begin();
	   forward_iterator end();
	   backward_iterator rbegin();
	   backward_iterator rend();
	   Node*           Next(Node *pNode);
       Node*           Prev(Node *pNode);
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
       std::lock_guard<std::mutex> lock(m_Mutex);
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
       std::lock_guard<std::mutex> lock(m_Mutex);
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
       std::lock_guard<std::mutex> lock(m_Mutex);
	   ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args&&... args)
{
       std::lock_guard<std::mutex> lock(m_Mutex);
       m_Root.ForEach(lpfn, 0, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node*
BTree<Traits>::FirstThat(Func lpfn, Args&&... args)
{
       std::lock_guard<std::mutex> lock(m_Mutex);
       return m_Root.FirstThat(lpfn, 0, std::forward<Args>(args)...);
}

template <typename Traits>
void BTree<Traits>::Print(std::ostream &os){
       std::lock_guard<std::mutex> lock(m_Mutex);
       m_Root.Print(os);
}

template <typename Traits>
typename BTree<Traits>::forward_iterator
BTree<Traits>::begin()
{
       return forward_iterator(this, &m_Root.GetFirstNode());
}

template <typename Traits>
typename BTree<Traits>::forward_iterator BTree<Traits>::end()
{
       return forward_iterator(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::backward_iterator BTree<Traits>::rbegin()
{
       return backward_iterator(this, &m_Root.GetLastNode());
}

template <typename Traits>
typename BTree<Traits>::backward_iterator BTree<Traits>::rend()
{
       return backward_iterator(this, nullptr);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::Next(Node *pNode)
{
       std::lock_guard<std::mutex> lock(m_Mutex);
	   if( pNode == nullptr ) {return nullptr;}
       return m_Root.Next(pNode);
}

template <typename Traits>
typename BTree<Traits>::Node* BTree<Traits>::Prev(Node *pNode)
{
       std::lock_guard<std::mutex> lock(m_Mutex);
	   if( pNode == nullptr ) {return nullptr;}
       return m_Root.Prev(pNode);
}
#endif
