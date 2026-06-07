#include "containers/linkedlist.h"
#include "containers/binarytree.h"
#include "containers/avltree.h"
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
    LinkedList<AscendingLinkedListTrait<TI>> list5 = move(list3);
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
    BinaryTree<AscendingTrait<TI>> tree;
    tree.insert(4, 0);
    tree.insert(2, 0);
    tree.insert(6, 0);
    tree.insert(1, 0);
    tree.insert(3, 0);
    tree.insert(5, 0);
    tree.insert(7, 0);

    cout << "inorder tree walk: llenado con tree.insert" << endl;
    cout << tree << endl;

    cout << "inorder tree walk: llenado con tree.operator>>" << endl;
    stringstream ss("4 0 2 0 6 0 1 0 3 0 5 0 7 0");
    BinaryTree<AscendingTrait<int>> tree2;
    ss >> tree2;
    cout << tree2 << endl;

    cout << "reverse inorder tree walk: " << endl;
    for (auto it = tree.reverse_inorder_begin(); it != tree.reverse_inorder_end(); ++it)
        cout << *it << endl; 

    cout << "preorder tree walk: " << endl;
    for (auto it = tree.preorder_begin(); it != tree.preorder_end(); ++it)
        cout << *it << endl;

    cout << "reverse preorder tree walk: " << endl;
    for (auto it = tree.reverse_preorder_begin(); it != tree.reverse_preorder_end(); ++it)
        cout << *it << endl;

    cout << "postorder tree walk: " << endl;
    for (auto it = tree.postorder_begin(); it != tree.postorder_end(); ++it)
        cout << *it << endl;

    cout << "reverse postorder tree walk: " << endl;
    for (auto it = tree.reverse_postorder_begin(); it != tree.reverse_postorder_end(); ++it)
        cout << *it << endl;
}

void AVLTreeDemo(){
    AVLTree<AscendingTrait<TI>> avlTree;
    avlTree.insert(1, 0);
    avlTree.insert(2, 0);
    avlTree.insert(3, 0);
    avlTree.insert(4, 0);
    avlTree.insert(5, 0);
    avlTree.insert(6, 0);
    avlTree.insert(7, 0);

    cout << "inorder tree walk: llenado con avlTree.insert" << endl;
    //cout << avlTree << endl;
    avlTree.ForEach([](auto &node){
        cout << node << " - Height: " << static_cast<AVLTree<AscendingTrait<TI>>::AVLTreeNode&>(node).getHeight() << endl;
    });
}

void ListsDemo(){
    BinaryTreeDemo();
    AVLTreeDemo();
}
