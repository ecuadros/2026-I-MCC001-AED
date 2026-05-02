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
}

void ListsDemo(){
    LinkedListDemo();
    
}
