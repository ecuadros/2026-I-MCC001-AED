#include <iostream>
#include "types.h"
#include "containers/binarytree.h"

using namespace std;

void DemoBinaryTree(){
    cout << "\n=== DEMO: RECORRIDOS DEL ARBOL BINARIO ===" << endl;
    
    BinaryTree<AscendingBinaryTreeListTrait<TI>> miArbol;

    miArbol.insert(50, 100);
    miArbol.insert(30, 101);
    miArbol.insert(70, 102);
    miArbol.insert(20, 103);
    miArbol.insert(40, 104);
    miArbol.insert(60, 105);
    miArbol.insert(80, 106);

    // ¡Mira la elegancia de estas 3 líneas!
    cout << "1. Recorrido InOrder: \n"
         << miArbol << "\n\n";

    cout << "2. Recorrido PreOrder: \n"
         << miArbol.print(TraversalMode::PreOrder) << "\n\n";

    cout << "3. Recorrido PostOrder: \n"
         << miArbol.print(TraversalMode::PostOrder) << "\n\n";

    cout << "=========================================\n" << endl;
}