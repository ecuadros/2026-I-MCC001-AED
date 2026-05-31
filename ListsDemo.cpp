#include "containers/linkedlist.h"
#include "containers/binarytree.h"
#include <fstream>

template <typename Node>
void Print(Node &node, ostream& os){
    os << node << ",";
}

template <typename Node>
void AddX(Node &node, typename Node::value_type value){
    node.getDataRef() += value;
}

template <typename Node>
void AddY(Node &node, typename Node::value_type value1, typename Node::value_type value2){
    node.getDataRef() += value1 + value2;
}

template <typename Node, typename T>
bool IsGreaterThan(Node &node, T x){
    return node.getDataRef() > x;
}

void LinkedListDemo(){
    // 
    LinkedList<DescendingLinkedListTrait<TI>> list1;
    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);
    cout << "Lista descendente: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2;
    using LI = LinkedList<AscendingLinkedListTrait<TI>>::Node;
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "Lista ascendente : " << list2 << endl;

    list2.ForEach(AddX<LI>, 3);
    cout << "Prueba ForEach + 3: " << endl;
    list2.ForEach(Print<LI>, cout);
    list2.ForEach(AddY<LI>, 10, 11);
    cout << "Prueba ForEach + 10 + 11: " << endl;
    list2.ForEach(Print<LI>, cout);
    
    cout << "Prueba First That: " << endl;
    auto it = list2.FirstThat(IsGreaterThan<LI, TI>, 6);
    if (it != list2.end())
        cout << "Primer mayor a 6   : " << *it << endl;
    cout << "Fin recorrido con iteradores" << endl;

    cout << "Prueba PushFront: " << endl;
    list1.push_front(5, 5);
    cout << "Lista ascendente 1: " << list1 << endl;

    cout << "Prueba PopFront: " << endl;
    list1.pop_front();
    cout << "Lista ascendente 1: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list3;
    cout << "Prueba PushBack: " << endl;
    list3.push_back(1, 10);
    list3.push_back(2, 20);
    list3.push_back(3, 30);
    cout << "Lista ascendente 3: " << list3 << endl;

    cout << "Prueba PopBack: " << endl;
    list3.pop_back();
    cout << "Lista ascendente 3: " << list3 << endl;

    list3.push_back(4, 40);
    cout << "Prueba Copy Constructor: " << endl;
    LinkedList<AscendingLinkedListTrait<TI>> list4(list3);
    cout << "Lista ascendente 4: " << list4 << endl;

    list3.push_back(5, 50);
    cout << "Prueba Move Constructor: " << endl;
    LinkedList<AscendingLinkedListTrait<TI>> list5 = std::move(list3);
    cout << "Lista ascendente 5: " << list5 << endl;

    cout << "Prueba del Destructor: " << endl;
    list4.~LinkedList();
    cout << "Lista ascendente 4: " << list4 << endl;

    cout << "Prueba del operador >>: "<<endl;
    ofstream ofs;
    ofs.open("lista1.txt");
    ofs <<  list1 <<   endl;
    ofs.close();

    ifstream file("lista1.txt");
    LinkedList<AscendingLinkedListTrait<TI>> list6;
    file >> list6;
    cout << "Lista 6 creada con datos de la lista 1: " << list6 << endl;

    cout << "Prueba operador []: " << endl;
    cout << "Lista5 [2]: " << list5[2] << endl;
}

void BinaryTreeDemo(){
    cout << "Binary Tree: " << endl;
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree1;
    using BT = BinaryTree<AscendingBinaryTreeListTrait<TI>>::Node;

    tree1.insert(10, 100);
    tree1.insert(5, 50);
    tree1.insert(7, 70);
    tree1.insert(6, 60);
    cout << "Árbol ascendente: " << tree1.toString() << endl;

    cout << endl << "ForEach Ascendente" << endl;
    tree1.ForEach(Print<BT>, cout);
    cout << endl;

    auto found = tree1.FirstThat(IsGreaterThan<BT, TI>, 6);
    if (found != nullptr)
        cout << "Primer mayor a 6   : " << *found << endl;
    else
        cout << "No se encontró un nodo mayor a 6" << endl;

    cout << endl << "Recorrido InOrder con iterador Forward: " << endl;
    for (auto it = tree1.begin(); it != tree1.end(); ++it) {
        cout << *it << ","; 
    }
    cout << endl;

    cout << endl << "Recorrido InOrder con iterador Backward: " << endl;
    for (auto it = tree1.rbegin(); it != tree1.rend(); ++it) {
        cout << *it << ",";
    }
    cout << endl;

    cout << endl << "Recorrido PreOrder con iterador Forward: " << endl;
    for (auto it = tree1.preorder_begin(); it != tree1.preorder_end(); ++it) {
        cout << *it << ",";
    }
    cout << endl;

    cout << endl << "Recorrido PreOrder con iterador Backward: " << endl;
    for (auto it = tree1.preorder_rbegin(); it != tree1.preorder_rend(); ++it) {
        cout << *it << ",";
    }
    cout << endl;

    cout << endl << "Recorrido PostOrder con iterador Forward: " << endl;
    for (auto it = tree1.postorder_begin(); it != tree1.postorder_end(); ++it) {
        cout << *it << ",";
    }
    cout << endl;

    cout << endl << "Recorrido PostOrder con iterador Backward: " << endl;
    for (auto it = tree1.postorder_rbegin(); it != tree1.postorder_rend(); ++it) {
        cout << *it << ",";
    }
    cout << endl;

    cout << endl << "Lectura desde archivo (operator>>):" << endl;

    ofstream outTree("tree.txt");
    // outTree << "6 15\n";
    // outTree << "2 25\n";
    // outTree << "9 35\n";
    // outTree << "1 45\n";
    // outTree << "7 55\n";
    outTree << tree1;
    outTree.close();

    ifstream inTree("tree.txt");
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree2;
    inTree >> tree2;
    inTree.close();

    cout << "Árbol leído desde archivo: " << tree2.toString() << endl;

    ofstream savedTree("treeOut.txt");
    savedTree << tree1;
    savedTree.close();
}

void ListsDemo(){
    // LinkedListDemo();
    BinaryTreeDemo();
}
