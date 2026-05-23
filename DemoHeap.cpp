#include <fstream>
#include <thread>
#include <vector>
#include "containers/heap.h"

void HeapDemo(){
    cout << "-- Heap Minimo-- " << endl;
    Heap<AscendingHeapTrait<TI>> minHeap;
    minHeap.insert(6, 15);
    minHeap.insert(2, 25);
    minHeap.insert(9, 35);
    minHeap.insert(1, 45);
    minHeap.insert(7, 55);
    cout << "Heap ascendente : " << minHeap << endl;
    cout << "Heap por Niveles: " << endl << minHeap.toLevelString() << endl;
    cout << "Heap Valido     : " << minHeap.isValidHeap() << endl;

    cout << "-- Peek Min --" << endl;
    auto minNode = minHeap.peek_min();
    cout << "Raiz: (" << minNode.GetData() << ", " << minNode.GetRef() << ")" << endl;

    cout << "-- Extract --" << endl;
    minHeap.extract();
    cout << "Heap Ascendente : " << minHeap << endl;
    cout << "Heap Valido     : " << minHeap.isValidHeap() << endl;

    cout << "-- Constructor Copia --" << endl;
    Heap<AscendingHeapTrait<TI>> copyHeap(minHeap);
    cout << "Heap copia      : " << copyHeap << endl;

    cout << "-- Move Constructor --" << endl;
    Heap<AscendingHeapTrait<TI>> movedHeap(std::move(copyHeap));
    cout << "Heap movido     : " << movedHeap << endl;

    cout << "-- Operador >> --" << endl;
    ofstream ofs;
    ofs.open("heap1.txt");
    ofs << minHeap << endl;
    ofs.close();

    ifstream file("heap1.txt");
    Heap<AscendingHeapTrait<TI>> inputHeap;
    file >> inputHeap;
    cout << "Heap desde archivo: " << inputHeap << endl;
    cout << "Heap valido       : " << inputHeap.isValidHeap() << endl;

    cout << "-- Prueba Clear --" << endl;
    inputHeap.clear();
    cout << "Heap limpio       : " << inputHeap << endl;

    cout << "-- Heap Maximo --" << endl;
    Heap<DescendingHeapTrait<TI>> maxHeap;
    maxHeap.insert(6, 15);
    maxHeap.insert(2, 25);
    maxHeap.insert(9, 35);
    maxHeap.insert(1, 45);
    maxHeap.insert(7, 55);
    cout << "Heap Descendente: " << maxHeap << endl;

    cout << "-- Prueba Concurrency --" << endl;
    Heap<AscendingHeapTrait<TI>> concurrentHeap;
    std::vector<std::thread> threads;
    for (TI i = 0; i < 5; ++i){
        threads.push_back(std::thread([&concurrentHeap, i](){
            concurrentHeap.insert(i + 1, (i + 1) * 10);
        }));
    }
    for (auto &thread : threads)
        thread.join();
    cout << "Heap concurrente: " << concurrentHeap << endl;
    cout << "Heap valido     : " << concurrentHeap.isValidHeap() << endl;
}
