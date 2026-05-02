#include "linkedlist.h"
#include <sstream>
#include <thread>

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
    // INSERT
    // list2.insert(6, 15);
    // list2.insert(2, 25);
    // list2.insert(9, 35);
    // list2.insert(1, 45);
    // list2.insert(7, 55);

    // PUSH FRONT
    // list2.push_front(6, 15);
    // list2.push_front(2, 25);
    // list2.push_front(9, 35);

    // PUSH BACK
    // list2.push_back(6, 15);
    // list2.push_back(2, 25);
    // list2.push_back(9, 35);
    // list2.push_back(7, 7);

    // POP BACK
    // Cuando la lista esta vacia
    // list2.pop_back();

    // Cuando hay un solo nodo
    // list2.push_back(2, 25);
    // list2.pop_back();

    // Cuando hay varios nodos - 2 a mas
    list2.push_back(2, 25);
    list2.push_back(6, 15);
    list2.push_back(9, 35);
    // list2.pop_back();

    // COPY CONSTRUCTOR
    LinkedList<AscendingLinkedListTrait<TI>> list3 = list2;

    cout << "Lista ascendente : " << list2 << endl;
    cout << "Lista copiada (list3): " << list3 << endl;

    // MOVE CONSTRUCTOR
    // LinkedList<AscendingLinkedListTrait<TI>> list4 = std::move(list2);
    // cout << "Lista movida (list4): " << list4 << endl;
    // cout << "Lista original después del movimiento (list2): " << list2 << endl;

    // FIRST THAT
    auto it = list2.FirstThat([](const auto& node, int value){ // El lambda recibe un nodo y un valor, y devuelve true si el valor del nodo es igual al valor buscado
        return node.getData() == value;
    }, 1);

    if(it != list2.end()){ // Si el iterador no es igual al iterador de fin, significa que se encontró un nodo con el valor buscado
        cout << "Nodo encontrado: " << *it << endl;
    }else{
        cout << "Nodo no encontrado" << endl;
    }

    // Operator>>
    LinkedList<AscendingLinkedListTrait<TI>> list5;
    istringstream ss("[(3,30),(1,10),(4,40),(2,20)]");

    ss >> list5;
    cout << "Lista leída desde el flujo de entrada (list5): " << list5 << endl;

    //CONCURRENCY TEST
    LinkedList<AscendingLinkedListTrait<TI>> list6;
    auto pushFunc = [&list6](int value, Ref ref){
        list6.push_back(value, ref);
    };
    thread t1(pushFunc, 1, 10);
    thread t2(pushFunc, 2, 20);
    thread t3(pushFunc, 3, 30);
    t1.join();
    t2.join();
    t3.join();
    cout << "Lista después de inserciones concurrentes (list6): " << list6
            << " Tamaño: " << list6.size() << endl;

    //Operator[]
    // cout << "list2[0] : " << list2[0] << endl;
    // cout << "list2[1] : " << list2[1] << endl;
    // cout << "list2[2] : " << list2[2] << endl;
}

void ListsDemo(){
    LinkedListDemo();
    
}
