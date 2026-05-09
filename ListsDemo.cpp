#include "containers/linkedlist.h"
#include "containers/doublelinkedlist.h"
#include "containers/circularlinkedlist.h"
#include "containers/circulardoublelinkedlist.h"
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
    cout << "Lista descendente 1: " << list1 << endl;

    cout << "Prueba PopFront: " << endl;
    list1.pop_front();
    cout << "Lista descendente 1: " << list1 << endl;

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

    cout << "Prueba del operador >>: " << endl;
    ofstream ofs;
    ofs.open("lista1.txt");
    ofs << list1 << endl;
    ofs.close();

    ifstream file("lista1.txt");
    LinkedList<AscendingLinkedListTrait<TI>> list6;
    file >> list6;
    cout << "Lista 6 creada con datos de la lista 1: " << list6 << endl;

    cout << "Prueba operador []: " << endl;
    cout << "Lista5 [2]: " << list5[2] << endl;
}

void DoubleLinkedListDemo(){
    cout << "\n=========== DoubleLinkedList ===========" << endl;
    using DLL  = DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>>;
    using DNode = DLL::Node;

    DLL dll;
    dll.insert(6, 15);
    dll.insert(2, 25);
    dll.insert(9, 35);
    dll.insert(1, 45);
    dll.insert(7, 55);
    cout << "DLL ascendente            : " << dll << endl;

    cout << "Recorrido forward         : ";
    dll.ForEach(Print<DNode>, cout);
    cout << "Recorrido backward        : ";
    dll.ReverseForEach(Print<DNode>, cout);

    dll.push_front(0, 5);
    dll.push_back(99, 999);
    cout << "Tras push_front(0) y push_back(99): " << dll << endl;
    cout << "Backward verificado       : ";
    dll.ReverseForEach(Print<DNode>, cout);

    cout << "Prueba Copy Constructor   : " << endl;
    DLL dll_copy(dll);
    cout << "DLL copia                 : " << dll_copy << endl;
    cout << "DLL copia backward        : ";
    dll_copy.ReverseForEach(Print<DNode>, cout);
}

void CircularLinkedListDemo(){
    cout << "\n=========== CircularLinkedList ===========" << endl;
    using CLL = CircularLinkedList<AscendingLinkedListTrait<TI>>;
    using CNode = CLL::Node;

    CLL cll;
    cll.push_back(10, 1);
    cll.push_back(20, 2);
    cll.push_back(30, 3);
    cll.push_back(40, 4);
    cout << "CLL                       : " << cll << endl;

    cout << "ForEach (1 vuelta)        : ";
    cll.ForEach(Print<CNode>, cout);

    cll.insert(25, 25);
    cout << "Tras insert(25)           : " << cll << endl;
    cout << "ForEach                   : ";
    cll.ForEach(Print<CNode>, cout);
}

void CircularDoubleLinkedListDemo(){
    cout << "\n=========== CircularDoubleLinkedList ===========" << endl;
    using CDLL = CircularDoubleLinkedList<AscendingDoubleLinkedListTrait<TI>>;
    using CDNode = CDLL::Node;

    CDLL cdll;
    cdll.push_back(100, 1);
    cdll.push_back(200, 2);
    cdll.push_back(300, 3);
    cout << "CDLL                      : " << cdll << endl;

    cout << "Forward (1 vuelta)        : ";
    cdll.ForEach(Print<CDNode>, cout);
    cout << "Backward (1 vuelta)       : ";
    cdll.ReverseForEach(Print<CDNode>, cout);

    cdll.insert(150, 150);
    cout << "Tras insert(150)          : " << cdll << endl;
    cout << "Forward                   : ";
    cdll.ForEach(Print<CDNode>, cout);
    cout << "Backward                  : ";
    cdll.ReverseForEach(Print<CDNode>, cout);
}

void ListsDemo(){
    LinkedListDemo();
    DoubleLinkedListDemo();
    CircularLinkedListDemo();
    CircularDoubleLinkedListDemo();
}
