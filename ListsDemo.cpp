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
    cout << "El destructor se prueba automaticamente al salir del scope." << endl;

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

    //DOUBLE LINKED LIST DEMO
    
    cout << endl << "2. DoubleLinkedList PushBack: " << endl;
    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> dlist;
    using DLL = DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>>::Node;

    dlist.push_back(1, 10);
    dlist.push_back(2, 20);
    dlist.push_back(3, 30);

    cout << "Recorrido forward: ";
    dlist.ForEach(Print<DLL>, cout);

    cout << "Recorrido backward: ";
    dlist.ReverseForEach(Print<DLL>, cout);
    cout << endl;

    // Prueba del Copy Constructor
    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> dlist2(dlist);
    cout << "DoubleLinkedList Copy Constructor: " << dlist2 << endl;

    dlist.insert(0, 5);
    dlist.insert(4, 40);
    dlist.insert(5, 50);
    cout << "DoubleLinkedList Insert: " << dlist << endl;

    // Prueba del operador >>
    istringstream ss2("[(0, 5), (1, 10), (2, 20)]");
    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> dlist3;
    ss2 >> dlist3;
    cout << "DoubleLinkedList operator>>: " << dlist3 << endl;

    cout<< "Lectura forward: " << dlist3 << endl;

    cout << "Lectura backward: ";
    dlist3.ReverseForEach(Print<DLL>, cout);
    cout << endl;

    // CIRCULAR LINKED LIST
    cout << endl << "3. CircularLinkedList: " << endl;
    CircularLinkedList<AscendingLinkedListTrait<TI>> clist;
    clist.insert(1, 10);
    clist.insert(2, 20);
    clist.insert(3, 30);

    cout << "Recorrido circular: " << clist << endl;

    // CIRCULAR DOUBLE LINKED LIST
    cout << endl << "4. CircularDoubleLinkedList: " << endl;
    CircularDoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> cdlist;
    using CDLL = CircularDoubleLinkedList<AscendingDoubleLinkedListTrait<TI>>::Node;
    cdlist.insert(1, 10);
    cdlist.insert(2, 20);
    cdlist.insert(3, 30);

    cout << "Recorrido circular hacia adelante: " << cdlist << endl;
    cout << "Recorrido circular hacia atrás: ";
    cdlist.ReverseForEach(Print<CDLL>, cout);
    cout << endl;

}

void ListsDemo(){
    LinkedListDemo();
}
