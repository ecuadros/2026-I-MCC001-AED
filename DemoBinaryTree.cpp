#include <iostream>
#include "types.h"
#include "containers/binarytree.h"

using namespace std;

void DemoBinaryTree(){
    cout << "Demo Arbol Binario" << endl;
    
    BinaryTree<AscendingBinaryTreeListTrait<TI>> miArbol;

    miArbol.insert(50, 100);
    miArbol.insert(30, 101);
    miArbol.insert(70, 102);
    miArbol.insert(20, 103);
    miArbol.insert(40, 104);
    miArbol.insert(60, 105);
    miArbol.insert(80, 106);

    cout << "Recorrido InOrder: " << endl;
    cout << miArbol << endl;;

    cout << "Recorrido PreOrder: " << endl;
    cout << miArbol.print(TraversalMode::PreOrder) << endl;

    cout << "Recorrido PostOrder: " << endl;
    cout << miArbol.print(TraversalMode::PostOrder) << endl;

}
