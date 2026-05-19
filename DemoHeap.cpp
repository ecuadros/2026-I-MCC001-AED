#include <iostream>
#include "containers/heap.h"
#include "types.h" // Aquí está definido TI (int) y Ref (long)

using namespace std;

void DemoMinHeap() {
    cout << "--- CREANDO MIN HEAP (Ascending Trait) ---" << endl;
    
    // Inyectamos la configuración usando Traits
    Heap<AscendingHeapTrait<TI>> minHeap;

    minHeap.insert(50, 1001);
    minHeap.insert(15, 1002);
    minHeap.insert(80, 1003);
    minHeap.insert(5,  1004); // Este debería convertirse en la nueva raíz

    cout << "El valor en la cima es: " << minHeap.peek_top().first << endl;

    cout << "Extrayendo valores en orden de prioridad:" << endl;
    while (!minHeap.empty()) {
        auto elemento = minHeap.extract();
        cout << "Valor: " << elemento.first << " | Ref: " << elemento.second << endl;
    }
}

int main() {
    DemoMinHeap();
    return 0;
}