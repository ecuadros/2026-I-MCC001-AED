#include <iostream>
#include "types.h"
#include "containers/binarytree.h"

using namespace std;

void DemoBinaryTree(){
cout << "=== DEMO: ARBOL BINARIO ASCENDENTE ===" << endl;
    
    // 1. Creamos un árbol que ordena de menor a mayor
    BinaryTree<AscendingBinaryTreeListTrait<TI>> miArbol;

    // 2. Insertamos datos desordenados
    // Insertaremos: 50, 20, 80, 10, 30, 90
    cout << "Insertando: 50, 20, 80, 10, 30, 90..." << endl;
    miArbol.insert(50, 100);
    miArbol.insert(20, 101);
    miArbol.insert(80, 102);
    miArbol.insert(10, 103);
    miArbol.insert(30, 104);
    miArbol.insert(90, 105);

    // 3. Recorremos hacia ADELANTE (de menor a mayor)
    cout << "\nRecorrido Inorder Forward (Menor a Mayor):" << endl;
    for (auto it = miArbol.begin(); it != miArbol.end(); ++it) {
        // Usamos el operador * sobrecargado en tu general_iterator
        cout << (*it).getData() << " "; 
    }
    cout << endl;

    // 4. Recorremos hacia ATRÁS (de mayor a menor)
    cout << "\nRecorrido Inorder Backward (Mayor a Menor):" << endl;
    for (auto it = miArbol.rbegin(); it != miArbol.rend(); ++it) {
        cout << (*it).getData() << " "; 
    }
    cout << "\n======================================" << endl;
}