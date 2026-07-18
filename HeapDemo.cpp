#include "containers/heap.h"
#include <fstream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <random>
using namespace std;

void HeapDemo(){
    Heap<DescendingHeapTrait<TI>> heap1;
    heap1.insert(6, 15);
    heap1.insert(2, 25);
    heap1.insert(9, 35);
    heap1.insert(1, 45);
    heap1.insert(7, 55);
    cout << "Heap descendente: " << endl << endl;
    heap1.printTree();
    cout << endl << endl;
    Heap<AscendingHeapTrait<TI>> heap2;
    heap2.insert(6, 15);
    heap2.insert(2, 25);
    heap2.insert(9, 35);
    heap2.insert(1, 45);
    heap2.insert(7, 55);
    cout << "Heap ascendente : " << endl;
    heap2.printTree();
    cout << endl << endl;
    cout << "Prueba peek_min(): " << "(retorna la raiz): " << endl;
    cout << "Raiz: " << heap2.peek_min() << endl << endl;
    cout << "Prueba peek_smallest(): " << endl;
    cout << "Smallest: " << heap1.peek_smallest() << endl << endl;
    cout << "Prueba peek_max(): " << endl;
    cout << "Biggest: " << heap1.peek_max() << endl << endl;
    cout << "Prueba extract(): " << endl;
    auto value = heap2.extract();
    cout << "Extraido: (" << value.first << ", " << value.second << ")" << endl;
    cout << "Heap despues extract: " << endl;
    heap2.printTree();
    cout << endl;
    cout << "Prueba remove_smallest(): " << endl;
    auto smallest = heap1.remove_smallest();
    cout << "Smallest removido: (" << smallest.first << ", " << smallest.second << ")" << endl;
    cout << "Heap despues remove_smallest(): " << endl;
    heap1.printTree();
    cout << endl << endl;
    cout << "Prueba remove_biggest(): " << endl;
    auto biggest = heap1.remove_biggest();
    cout << "Biggest removido: (" << biggest.first << ", " << biggest.second << ")" << endl;
    cout << "Heap despues remove_biggest(): " << endl;
    heap1.printTree();
    cout << endl << endl;
    cout << "Prueba Copy Constructor: " << endl;
    Heap<AscendingHeapTrait<TI>> heap3(heap2);
    cout << "Heap 3: " << endl;
    heap3.printTree();
    cout << endl << endl;
    cout << "Prueba Move Constructor: " << endl;
    Heap<AscendingHeapTrait<TI>> heap4 = move(heap3);
    cout << "Heap 4: " << endl;
    heap4.printTree();
    cout << endl << endl;
    cout << "Prueba operator >> : " << endl;
    ofstream ofs("heap1.txt");
    ofs << heap1 << endl;
    ofs.close();
    ifstream ifs("heap1.txt");
    Heap<AscendingHeapTrait<TI>> heap5;
    ifs >> heap5;
    cout << "Heap 5 desde archivo: " << endl;
    heap5.printTree();
    cout << endl << endl;
    cout << "Prueba empty() y size(): " << endl;
    cout << "Empty: " << heap5.empty() << endl;
    cout << "Size : " << heap5.size()  << endl;
}

void DemoConcurrentHeap(){
    cout << "Heap con Concurrencia" << endl;
    Heap<AscendingHeapTrait<TI>> heap;
    auto worker = [&heap](int thread_id){
    	random_device rd;
        mt19937 gen(rd() + thread_id);
        uniform_int_distribution<> dist(1, 999);
        for(int i = 0 ; i < 13 ; i++){
            int value = dist(gen);
			heap.insert(value, thread_id);
            if(heap.size() > 62){
                heap.extract();
            }
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
    cout << "Heap final: " << endl;
    heap.printTree();
    cout << endl << endl;
    cout << "Size final: " << heap.size() << endl;
    cout << endl;
}

int main(){
	srand(time(nullptr));
    HeapDemo();
    cout << endl;
    DemoConcurrentHeap();
    return 0;
}
