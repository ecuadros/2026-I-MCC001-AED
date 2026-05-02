#include "linkedlist.h"

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
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "Lista ascendente : " << list2 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2_copy(list2); // Copy constructor
    cout << "Copia de lista ascendente : " << list2_copy << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2_moved(std::move(list2)); // Move constructor
    cout << "Lista movida : " << list2_moved << endl;
    cout << "Lista original después de mover : " << list2 << endl;

    list2.push_front(0, 5); //example of push front
    cout << "Lista después de push_front: " << list2 << endl;

    // example of push back
    list2.push_back(10, 65);
    cout << "Lista después de push_back: " << list2 << endl;

    // example of pop back
    auto [value, ref] = list2.pop_back();
    cout << "Elemento pop_back: " << value << ", Ref: " << ref << endl;
    cout << "Lista después de pop_back: " << list2 << endl;

    // example of [] operator
    cout << "Elemento en índice 0: " << list2[0] << endl;

    // example of FirstThat
    // Encontrar el primer nodo donde el valor sea mayor que 4
    list2.insert(7, 55);
    list2.insert(30, 40);
    auto it = list2.FirstThat([](const LLNode<TI>& node) {
        return node.getData() > 4;
    });
    if (it != list2.end()) {
    cout << "Encontrado: " << *it << endl;  // Imprime (5,30)
    } else {
    cout << "No encontrado" << endl;
    }

    // example  of >> operator
    istringstream iss("8 75");
    iss >> list2;
    cout << "Lista después de >> operator: " << list2 << endl;
}

void ListsDemo(){
    LinkedListDemo();
    
}
