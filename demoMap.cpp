#include <iostream>
#include <map>
#include <string>
// unordered_map: implementado con hash table, operaciones promedio O(1), pero no mantiene orden
using TI = int;
using TV = std::string;
int main() {
	// std::map se implementa normalmente con un arbol rojo-negro (balanceado).
	// Mantiene las claves ordenadas y operaciones como buscar/insertar en O(log n).
	std::map<TI, TV> alumnos;

	// 1) Insertar elementos
	alumnos.insert({103, "Luis"});
	alumnos.emplace(101, "Ana"); // recibe argumentos y construye directamente en el contenedor
	alumnos[102] = "Carlos"; // Inserta si no existe, o actualiza si ya existe

	std::cout << "Contenido inicial (ordenado por clave):\n";
	for (const auto& par : alumnos) {
		std::cout << "Codigo: " << par.first << " -> " << par.second << '\n';
	}

	// 2) Acceso y actualizacion
	std::cout << "\nAcceso con at(101): " << alumnos.at(101) << '\n';
	alumnos[102] = "Carlos M.";
	std::cout << "Actualizacion con operator[] en clave 102: " << alumnos[102] << '\n';

	// 3) Buscar elementos
	int claveBuscada = 103;
	auto it = alumnos.find(claveBuscada);
	if (it != alumnos.end()) {
		std::cout << "\nfind(" << claveBuscada << ") encontro: " << it->second << '\n';
	}

	std::cout << "count(200): " << alumnos.count(200) << " (0 significa que no existe)\n";

	// 4) lower_bound: primer elemento con clave >= 102
	auto lb = alumnos.lower_bound(102);
	if (lb != alumnos.end()) {
		std::cout << "lower_bound(102): " << lb->first << " -> " << lb->second << '\n';
	}

	// 5) Eliminar
	std::cout << "\nEliminando clave 101...\n";
	alumnos.erase(101);

	std::cout << "Contenido despues de erase:\n";
	for (const auto& par : alumnos) {
		std::cout << "Codigo: " << par.first << " -> " << par.second << '\n';
	}

	// 6) Utilidades
	std::cout << "\nsize(): " << alumnos.size() << '\n';
	std::cout << "empty(): " << (alumnos.empty() ? "true" : "false") << '\n';

	// 7) Limpiar contenedor
	alumnos.clear();
	std::cout << "Despues de clear(), size(): " << alumnos.size() << '\n';

	return 0;
}
