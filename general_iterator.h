#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <algorithm>
#include <utility>

template <typename Container, class IteratorBase> // 
class general_iterator
{public:
    using Node   = typename Container::Node; // Definimos el tipo Node a partir del Container
    using Myself = general_iterator<Container, IteratorBase>; 

protected:
    Container *m_pContainer;
    Node      *m_pNode;
public:
    general_iterator(Container *pContainer, Node *pNode)
        : m_pContainer(pContainer), m_pNode(pNode) {}
    general_iterator(Myself &other) // Copy constructor
          : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode){}
    general_iterator(Myself &&other) // Move constructor
          {   m_pContainer = std::move(other.m_pContainer);
              m_pNode      = std::move(other.m_pNode);
          }
    IteratorBase operator=(IteratorBase &iter)
          {   m_pContainer = std::move(iter.m_pContainer);
              m_pNode      = std::move(iter.m_pNode);
              return *(IteratorBase *)this; // Pending static_cast?
          }
    Node *getNode() const { return m_pNode; }
    friend bool operator==(const IteratorBase &a, const IteratorBase &b) { return a.getNode() == b.getNode(); } // Comparación de igualdad basada en la posición del nodo
    friend bool operator!=(const IteratorBase &a, const IteratorBase &b) { return a.getNode() != b.getNode(); } // Comparación de desigualdad basada en la posición del nodo
    Node &operator*(){ return *m_pNode; } // Desreferenciación para acceder al nodo actual
    Node* operator->() const { return m_pNode; } // Operador -> para acceder a los miembros del nodo actual
};

#endif
