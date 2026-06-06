#include "containers/avl.h"
#include <fstream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <random>
#include <iostream>
using namespace std;

void AVLDemo(){
    AVLTree<AVLTreeTrait<TI>> tree;
    cout << "Insertando 30, 20, 10 (Rotacion LL)" << endl;
    tree.insert(30, 1);
    tree.insert(20, 2);
    tree.insert(10, 3);
    tree.printTree();
    cout << endl << endl;
    AVLTree<AVLTreeTrait<TI>> tree2;
    cout << "Insertando 10, 20, 30 (Rotacion RR)" << endl;
    tree2.insert(10, 1);
    tree2.insert(20, 2);
    tree2.insert(30, 3);
    tree2.printTree();
    cout << endl << endl;
    AVLTree<AVLTreeTrait<TI>> tree3;
    cout << "Insertando 30, 10, 20 (Rotacion LR)" << endl;
    tree3.insert(30, 1);
    tree3.insert(10, 2);
    tree3.insert(20, 3);
    tree3.printTree();
    cout << endl << endl;
    AVLTree<AVLTreeTrait<TI>> tree4;
    cout << "Insertando 10, 30, 20 (Rotacion RL)" << endl;
    tree4.insert(10, 1);
    tree4.insert(30, 2);
    tree4.insert(20, 3);
    tree4.printTree();
    cout << endl << endl;
    AVLTree<AVLTreeTrait<TI>> tree5;
    int values[] = {
        50,25,75,10,30,60,90,
        5,15,27,35,55,65,85,95
    };
    for(auto v : values)
        tree5.insert(v, v);
    cout << "AVL grande:" << endl;
    tree5.printTree();
    cout << endl << endl;
    cout << "Recorrido Inorder:" << endl;
    for(auto it = tree5.begin(); it != tree5.end(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Prueba Copy Constructor:" << endl;
    AVLTree<AVLTreeTrait<TI>> copy(tree5);
    copy.printTree();
    cout << endl << endl;
    cout << "Prueba Move Constructor:" << endl;
    AVLTree<AVLTreeTrait<TI>> moved(move(copy));
    moved.printTree();
    cout << endl << endl;
    cout << "Copy luego del move:" << endl;
    copy.printTree();
    cout << endl;
}

int main(){
    AVLDemo();
    return 0;
}
