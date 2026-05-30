#include "containers/binarytree.h"
#include <fstream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <random>
using namespace std;

void BinaryDemo(){
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree1;
    tree1.insert(6, 155);
    tree1.insert(2, 255);
    tree1.insert(9, 355);
    tree1.insert(8, 285);
    tree1.insert(4, 245);
    tree1.insert(1, 245);
    tree1.insert(12, 115);
    cout << "Binary tree ascendente: " << endl;
    tree1.printTree();
    cout << endl << endl;
    BinaryTree<DescendingBinaryTreeListTrait<TI>> tree2;
    tree2.insert(6, 155);
    tree2.insert(2, 255);
    tree2.insert(9, 355);
    tree2.insert(21, 365);
    tree2.insert(4, 245);
    tree2.insert(1, 115);
    tree2.insert(7, 555);
    cout << "Binary Tree Descendente:" << endl << endl;
	tree2.printTree();
    cout << endl << endl;
    cout << "Prueba Copy Constructor:" << endl;
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree3(tree1);
    tree3.printTree();
    cout << endl << endl;
    cout << "Prueba Move Constructor:" << endl;
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree4 = move(tree3);
    tree4.printTree();
    cout << endl << endl;
    cout << "Tree3 luego del move:" << endl;
	tree3.printTree();
	cout << endl << endl;
    cout << "Recorrido Inorder Forward:" << endl;
    for(auto it = tree1.begin(); it != tree1.end(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Recorrido Inorder Backward:" << endl;
    for(auto it = tree1.rbegin(); it != tree1.rend(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Recorrido Preorder Forward:" << endl;
    for(auto it = tree1.pbegin(); it != tree1.pend(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Recorrido Preorder Backward:" << endl;
    for(auto it = tree1.prbegin(); it != tree1.prend(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Recorrido Postorder Forward:" << endl;
    for(auto it = tree1.postbegin(); it != tree1.postend(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Recorrido Postorder Backward:" << endl;
    for(auto it = tree1.postrbegin(); it != tree1.postrend(); ++it)
        cout << it->getData() << " ";
    cout << endl << endl;
    cout << "Prueba ForEach:" << endl;
    tree1.ForEach(
        [](auto& node){
            cout << node.getData() << " ";
        }
    );
    cout << endl << endl;
    cout << "Prueba FirstThat (buscar valor 7):" << endl;
    auto found = tree1.FirstThat(
        [](auto& node){
            return node.getData() == 7;
        }
    );
    if(found != tree1.end())
        cout << "Encontrado: " << found->getData() << " Ref: " << found->getRef() << endl;
    else
        cout << "No encontrado" << endl;
    cout << endl;
}

void DemoConcurrentBinaryTree(){
    cout << "Binary Tree con Concurrencia" << endl;
    BinaryTree<AscendingBinaryTreeListTrait<TI>> tree;
    auto worker = [&tree](int thread_id){
        random_device rd;
        mt19937 gen(rd() + thread_id);
        uniform_int_distribution<> dist(1, 999);
        for(int i = 0 ; i < 3 ; i++){
            int value = dist(gen);
            tree.insert(value, thread_id);
        }
        cout << "Thread " << thread_id << " terminado" << endl;
    };
    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    cout << endl;
    cout << "Binary Tree final:" << endl << endl;
    tree.printTree();
    cout << endl;
}

int main(){
	srand(time(nullptr));
    BinaryDemo();
    cout << endl;
    DemoConcurrentBinaryTree();
    return 0;
}
