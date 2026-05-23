#include <iostream>
#include <vector>
#include "containers/heap.h"
#include "types.h"

using namespace std;

void DemoHeap() {
    cout << "--- CREANDO HEAP ---" << endl;
    
    Heap<AscendingHeapTrait<TI>> myHeap;

    myHeap.insert(50, 10);
    myHeap.insert(15, 5);
    myHeap.insert(80, 9);
    myHeap.insert(5,  7);

    Heap<AscendingHeapTrait<TI>> myHeap2(myHeap);
    cout << "Constructor copia" << endl;
    cout << myHeap2 <<endl;

    Heap<AscendingHeapTrait<TI>> myHeap3 = move(myHeap2);
    cout << "Move constructor" << endl;
    cout << myHeap3 <<endl;
    cout << myHeap2 <<endl;

}