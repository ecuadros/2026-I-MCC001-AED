#include <fstream>
#include <iostream>

#include "containers/heap.h"

using namespace std;

void HeapDemo() {
	Heap<AscendingHeapTrait<TI>> hmin;
	Heap<DescendingHeapTrait<TI>> hmax;

	ifstream file;

	// Primero intenta list1.txt, luego lista1.txt para compatibilidad.
	file.open("list1.txt");
	if (!file.is_open()) {
		file.clear();
		file.open("lista1.txt");
	}

	if (!file.is_open()) {
		cout << "No se pudo abrir list1.txt ni lista1.txt" << endl;
		return;
	}

	file >> hmin;
	file.close();

	// Para mantener demo simple: cargamos de nuevo el archivo en el heap max.
	file.open("list1.txt");
	if (!file.is_open()) {
		file.clear();
		file.open("lista1.txt");
	}

	if (!file.is_open()) {
		cout << "No se pudo recargar archivo para heap max" << endl;
		return;
	}

	file >> hmax;
	file.close();

	cout << "Heap min cargado con >> : " << hmin << endl;
	cout << "Tope min (peek)        : " << hmin.peek() << endl;

	cout << "Heap max cargado con >> : " << hmax << endl;
	cout << "Tope max (peek)        : " << hmax.peek() << endl;

	auto top = hmin.extract_top();
	cout << "Extract top min        : " << top << endl;
	cout << "Heap min despues       : " << hmin << endl;

	cout << "Valida heap min        : " << (hmin.validate_heap() ? "OK" : "ERROR") << endl;
	cout << "Valida heap max        : " << (hmax.validate_heap() ? "OK" : "ERROR") << endl;

	vector<pair<TI, Ref>> raw = {
		{12, 120}, {4, 40}, {18, 180}, {1, 10}, {9, 90}, {7, 70}
	};

	hmin.build_from_vector(raw);
	cout << "Build from vector min  : " << hmin << endl;
	cout << "Tope min (peek)        : " << hmin.peek() << endl;
	cout << "Valida build min       : " << (hmin.validate_heap() ? "OK" : "ERROR") << endl;

	// Cambio de comparador con recolocacion completa.
	auto hchanged = hmin.change_comp_to<DescendingHeapTrait<TI>>();
	cout << "Change comp -> max     : " << hchanged << endl;
	cout << "Tope max (peek)        : " << hchanged.peek() << endl;
	cout << "Valida changed heap    : " << (hchanged.validate_heap() ? "OK" : "ERROR") << endl;
}

