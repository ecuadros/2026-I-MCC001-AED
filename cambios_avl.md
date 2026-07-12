# Cambios AVL respecto al commit anterior

## Lista de tareas realizadas

- [x] Se creo la implementacion de AVL en [containers/avl_tree.h](containers/avl_tree.h) heredando de BinaryTree.
- [x] Se creo el nodo especializado AVLTreeNode heredando de BinaryTreeNode, agregando manejo de altura.
- [x] Se implementaron utilitarios de balanceo: calculo de altura, factor de balance y actualizacion de altura.
- [x] Se implementaron rotaciones simples a la izquierda y derecha para rebalanceo.
- [x] Se incorporo rebalanceo automatico en insercion usando casos LL, RR, LR y RL.
- [x] Se agregaron aliases de traits para AVL: AscendingAVLTreeTrait y DescendingAVLTreeTrait.
- [x] Se agregaron operaciones de copia y movimiento en AVL preservando estructura y altura del arbol.
- [x] Se refactorizo BinaryTree para reutilizar insercion desde clases derivadas:
  - [x] Hook create_node.
  - [x] Hook after_insert.
  - [x] internal_insert pasa a protected y retorna la nueva raiz del subarbol.
- [x] Se adapto AVL para usar la insercion heredada de BinaryTree mediante overrides de hooks.
- [x] Se creo un demo dedicado en [AVLTreeDemo.cpp](AVLTreeDemo.cpp) con inserciones que fuerzan rotaciones.
- [x] Se actualizo el build en [Makefile](Makefile) para compilar AVLTreeDemo.cpp.
- [x] Se declaro AVLTreeDemo en [lists.h](lists.h) y se integro su ejecucion desde [main.cpp](main.cpp).
- [x] Se actualizo el diagrama de herencia en [inheritance-diagram.md](inheritance-diagram.md) incluyendo AVLTree, AVLTreeNode y traits AVL.

## Resultado

- [x] AVL integrado al proyecto con herencia real desde BinaryTree.
- [x] Insercion balanceada funcionando y validada por compilacion y ejecucion de demos.
