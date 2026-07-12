// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <iterator>
#include <cstddef>
#include <utility>


#include "BTreePage.h"
#include "types.h"
#include "basetrait.h"
#include "general_iterator.h"
#include <mutex>
#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long, typename Compare = std::less<keyType>>
struct BTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>
{
       using Comp = Compare;
};

template <typename keyType, typename ObjIDType = long>
struct AscendingBTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>, public AscendingTrait<keyType>
{
};

template <typename keyType, typename ObjIDType = long>
struct DescendingBTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>, public DescendingTrait<keyType>
{
};

template <typename keyType, typename ObjIDType = long, typename Compare = std::less<keyType>>
using BTree = BTreeT<BTreeTraits<keyType, ObjIDType, Compare>>;

template<typename Container>
class BTreForwardIterator : public general_iterator<Container, BTreForwardIterator<Container>>{
       using MySelf = BTreForwardIterator<Container>;
       using Parent = general_iterator<Container, MySelf>;
       using BTPage = typename Container::BTPage;
       using Node = typename Container::Node;
       using Parent::Parent;
       std::vector<std::pair<BTPage *, TINDEX>> m_Stack;

       void PushLeftPath(BTPage *pPage)
       {
              while( pPage )
              {
                     m_Stack.push_back(std::make_pair(pPage, 0));
                     pPage = pPage->m_SubPages[0];
              }
       }

       Node* AdvanceInternal()
       {
              while( !m_Stack.empty() )
              {
                     BTPage *pPage = m_Stack.back().first;
                     TINDEX &nextIndex = m_Stack.back().second;

                     if( nextIndex < pPage->m_KeyCount )
                     {
                            TINDEX emittedIndex = nextIndex;
                            Node *pNode = &pPage->m_Keys[emittedIndex];
                            ++nextIndex;
                            PushLeftPath(pPage->m_SubPages[emittedIndex + 1]);
                            return pNode;
                     }
                     m_Stack.pop_back();
              }
              return nullptr;
       }

       Node* BeginInternal()
       {
              if( !this->m_pContainer )
                     return nullptr;

              m_Stack.clear();
              m_Stack.reserve(this->m_pContainer->m_Height + 1);
              PushLeftPath(&this->m_pContainer->m_Root);
              return AdvanceInternal();
       }

public:
       BTreForwardIterator(Container *pContainer, bool atEnd = false)
              : Parent(pContainer, nullptr)
       {
              if( !pContainer || atEnd )
                     return;

              this->m_pNode = BeginInternal();
       }

       MySelf& operator++(){
              if( !this->m_pNode || !this->m_pContainer )
                     return *this; // end()

              this->m_pNode = AdvanceInternal();
              return *this;
       }

};

template<typename Container>
class BTreBackwardIterator : public general_iterator<Container, BTreBackwardIterator<Container>>{
       using MySelf = BTreBackwardIterator<Container>;
       using Parent = general_iterator<Container, MySelf>;
       using BTPage = typename Container::BTPage;
       using Node = typename Container::Node;
       using Parent::Parent;
       std::vector<std::pair<BTPage *, TINDEX>> m_Stack;

       void PushRightPath(BTPage *pPage)
       {
              while( pPage )
              {
                     m_Stack.push_back(std::make_pair(pPage, pPage->m_KeyCount - 1));
                     pPage = pPage->m_SubPages[pPage->m_KeyCount];
              }
       }

       Node* AdvanceInternal()
       {
              while( !m_Stack.empty() )
              {
                     BTPage *pPage = m_Stack.back().first;
                     TINDEX &nextIndex = m_Stack.back().second;

                     if( nextIndex >= 0 )
                     {
                            TINDEX emittedIndex = nextIndex;
                            Node *pNode = &pPage->m_Keys[emittedIndex];
                            --nextIndex;
                            PushRightPath(pPage->m_SubPages[emittedIndex]);
                            return pNode;
                     }
                     m_Stack.pop_back();
              }
              return nullptr;
       }

       Node* BeginInternal()
       {
              if( !this->m_pContainer )
                     return nullptr;

              m_Stack.clear();
              m_Stack.reserve(this->m_pContainer->m_Height + 1);
              PushRightPath(&this->m_pContainer->m_Root);
              return AdvanceInternal();
       }

public:
       BTreBackwardIterator(Container *pContainer, bool atEnd = false)
              : Parent(pContainer, nullptr)
       {
              if( !pContainer || atEnd )
                     return;

              this->m_pNode = BeginInternal();
       }

       MySelf& operator++(){
              if( !this->m_pNode || !this->m_pContainer )
                     return *this; // rend()

              this->m_pNode = AdvanceInternal();
              return *this;
       }
};



template <typename Traits>

class BTreeT 
// this is the full version of the BTree
{
       template <typename>
       friend class BTreForwardIterator;
       template <typename>
       friend class BTreBackwardIterator;

public:
       using value_type = typename Traits::value_type;
       using objid_type = typename Traits::objid_type;
       using Node       = typename Traits::Node;
       using Comp       = typename Traits::Comp;
       using BTPage     = CBTreePage<Traits>;
       using MySelf     = BTreeT<Traits>;
       using forward_iterator  = BTreForwardIterator<MySelf>;
       using backward_iterator = BTreBackwardIterator<MySelf>;
       using iterator          = forward_iterator;
       using reverse_iterator  = backward_iterator;

private:
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/
       
public:
       BTreeT(int order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTreeT();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       TOBT            Insert (const value_type key, const objid_type ObjID);
       TOBT            Remove (const value_type key, const objid_type ObjID);
       objid_type      Search (const value_type key);
       TOBT            size()  { return m_NumKeys; }
       TOBT            height() { return m_Height;      }
       TOBT            GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       template <typename Func, typename... Args>
       void            ForEach(Func func, Args &&... args);
       template <typename Func, typename... Args>
       void            ReverseForEach(Func func, Args &&... args);
       iterator        begin();
       iterator        end();
       reverse_iterator rbegin();
       reverse_iterator rend();

       
       template <typename Func, typename... Args>
       Node*           FirstThat(Func func, Args&&... args);
       template <typename Func, typename... Args>
       Node*           ReverseFirstThat(Func func, Args&&... args);
       //typedef               Node iterator;

protected:
       BTPage        m_Root;
       TOBT             m_Height;  // height of tree
       TOBT             m_Order;   // order of tree
       TOBT            m_NumKeys; // number of keys
       TOBT            m_Unique;  // Accept the elements only once ?
       std::mutex mtx;

private:
       // Iteration state is handled by iterator classes.
};



const TOBT MaxHeight = 5; //??
template <typename Traits>
BTreeT<Traits>::BTreeT(int order, bool unique)
                                                                                                          : m_Root(2 * order  + 1, unique),
                                                                                                                 m_Height(1),
                                                                                                                 m_Order(order),
                                                                                                                 m_NumKeys(0),
                                                                                                                 m_Unique(unique)
{
       m_Root.SetMaxKeysForChilds(order);
}

template <typename Traits>
BTreeT<Traits>::~BTreeT()
{
}

template <typename Traits>
TOBT BTreeT<Traits>::Insert(const value_type key, const objid_type ObjID)
{
       std::scoped_lock<std::mutex> lock(mtx);
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
TOBT BTreeT<Traits>::Remove (const value_type key, const objid_type ObjID)
{
       std::scoped_lock<std::mutex> lock(mtx);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTreeT<Traits>::objid_type BTreeT<Traits>::Search (const value_type key)
{
       std::scoped_lock<std::mutex> lock(mtx);
       objid_type ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template <typename Func, typename... Args>
void BTreeT<Traits>::ForEach(Func func, Args&&... args)
{
       std::scoped_lock<std::mutex> lock(mtx);
       ::ForEach(begin(), end(), func, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
void BTreeT<Traits>::ReverseForEach(Func func, Args&&... args)
{
       std::scoped_lock<std::mutex> lock(mtx);
       ::ForEach(rbegin(), rend(), func, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTreeT<Traits>::Node *
BTreeT<Traits>::FirstThat(Func func, Args&&... args)
{      
       std::scoped_lock<std::mutex> lock(mtx);
       auto it = ::FirstThat(begin(), end(), func, std::forward<Args>(args)...);
       if( it == end() ) return nullptr;
       return it.getNode();
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTreeT<Traits>::Node *
BTreeT<Traits>::ReverseFirstThat(Func func, Args&&... args)
{
       std::scoped_lock<std::mutex> lock(mtx);
       auto it = ::FirstThat(rbegin(), rend(), func, std::forward<Args>(args)...);
       if( it == rend() ) return nullptr;
       return it.getNode();
}

template <typename Traits>
void BTreeT<Traits>::Print(ostream &os){
       std::scoped_lock<std::mutex> lock(mtx);
       m_Root.Print(os);
}

template <typename Traits>
typename BTreeT<Traits>::iterator BTreeT<Traits>::begin()
{
       return iterator(this, false);
}

template <typename Traits>
typename BTreeT<Traits>::iterator BTreeT<Traits>::end()
{
       return iterator(this, true);
}

template <typename Traits>
typename BTreeT<Traits>::reverse_iterator BTreeT<Traits>::rbegin()
{
       return reverse_iterator(this, false);
}

template <typename Traits>
typename BTreeT<Traits>::reverse_iterator BTreeT<Traits>::rend()
{
       return reverse_iterator(this, true);
}






#endif