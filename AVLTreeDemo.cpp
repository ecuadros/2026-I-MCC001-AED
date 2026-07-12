#include <iostream>
#include <queue>

#include "containers/avl_tree.h"
#include<fstream>
using namespace std;

template <typename Node>
void PrintLevelOrderAVL(const Node* root) {
    if (!root) {
        return;
    }

    queue<const Node*> q;
    q.push(root);

    while (!q.empty()) {
        const Node* current = q.front();
        q.pop();

        cout << *current << " ";

        if (current->GetChild(0)) {
            q.push(current->GetChild(0));
        }
        if (current->GetChild(1)) {
            q.push(current->GetChild(1));
        }
    }
}

void AVLTreeDemo() {
    AVLTree<AscendingAVLTreeTrait<TI>> avl;

    cout << "AVLTreeDemo: inserciones que fuerzan rotaciones" << endl;

    // LL
    avl.insert(30, 1);
    avl.insert(20, 2);
    avl.insert(10, 3);

    // RR
    avl.insert(40, 4);
    avl.insert(50, 5);

    // LR
    avl.insert(25, 6);

    // RL
    avl.insert(45, 7);

    cout << "In-order   : ";
    for (auto& node : avl.inorder()) {
        cout << node << " ";
    }
    cout << endl;

    cout << "Pre-order  : ";
    for (auto& node : avl.preorder()) {
        cout << node << " ";
    }
    cout << endl;

    cout << "Level-order: ";
    PrintLevelOrderAVL(avl.root());
    cout << endl;

    if (avl.root()) {
        cout << "Root actual: " << *avl.root() << endl;
    }

    TI sum = 0;
    avl.ForEach([](auto& node, TI& acc) {
        acc += node.GetData();
    }, sum);
    cout << "Suma de nodos (in-order): " << sum << endl;

    ofstream ofs("avl_tree_dump.txt");
    ofs << avl;
    ofs.close();
    AVLTree<AscendingAVLTreeTrait<TI>> avl2;
    // 100 90 80 70 60 50 40 30 20 10
    avl2.insert(100, 1);
    avl2.insert(90, 2);
    avl2.insert(80, 3);
    avl2.insert(70, 4);
    avl2.insert(60, 5);
    avl2.insert(50, 6);
    avl2.insert(40, 7);
    avl2.insert(30, 8);
    avl2.insert(20, 9);
    avl2.insert(10, 10);
    ofstream ofs2("avl_tree_dump2.txt");
    ofs2 << avl2;
    ofs2.close();
    cout<< "AVL2" << endl;
    cout<< avl2 << endl;
    PrintLevelOrderAVL(avl2.root());
}
