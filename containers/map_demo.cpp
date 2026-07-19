#include <iostream>
#include <map>
#include "../types.h"

using namespace std;

void DemoMap() {
    // Clave: código del alumno
    // Valor: nombre del alumno
    map<TI, TS> alumnos;

    cout << "===== DEMO std::map =====\n\n";

    // 1. Insertar elementos usando []
    alumnos[1003] = "Ana";
    alumnos[1001] = "Susana";
    alumnos[1004] = "Luis";
    alumnos[1002] = "Carlos";

    // 2. Insertar usando insert()
    alumnos.insert({1005, "Maria"});

    // 3. Mostrar todos los elementos
    cout << "1. Lista de alumnos:\n";

    for (const auto& alumno : alumnos) {
        cout << "Codigo: " << alumno.first
             << " | Nombre: " << alumno.second << '\n';
    }

    /*
        Aunque los elementos fueron insertados en otro orden,
        std::map los muestra ordenados por la clave.
    */

    // 4. Buscar un elemento
    TI codigoBuscado = 1003;

    cout << "\n2. Buscar alumno con codigo "
         << codigoBuscado << ":\n";

    auto it = alumnos.find(codigoBuscado);

    // si encuentra la clave, devuelve un iterador valido
    if (it != alumnos.end()) { 
        cout << "Alumno encontrado: "
             << it->second << '\n';
    } else {
        cout << "Alumno no encontrado.\n";
    }

    // 5. Verificar si una clave existe
    // Otra forma de demostrar existencia
    TI codigoVerificar = 1001;

    cout << "\n3. Verificar si existe el codigo "
         << codigoVerificar << ":\n";

    if (alumnos.count(codigoVerificar) > 0) {
        cout << "El codigo existe.\n";
    } else {
        cout << "El codigo no existe.\n";
    }

    // 6. Modificar un valor
    cout << "\n4. Modificar alumno con codigo 1002:\n";

    alumnos[1002] = "Carlos Alberto";

    cout << "Nuevo nombre: "
         << alumnos[1002] << '\n';

    // 7. Eliminar un elemento
    cout << "\n5. Eliminar alumno con codigo 1004:\n";

    size_t eliminados = alumnos.erase(1004);

    if (eliminados > 0) {
        cout << "Alumno eliminado correctamente.\n";
    } else {
        cout << "No se encontro el alumno.\n";
    }

    // 8. Recorrer usando iteradores
    cout << "\n6. Recorrido con iteradores:\n";

    for (auto iterador = alumnos.begin();
         iterador != alumnos.end();
         ++iterador) {

        cout << iterador->first
             << " -> "
             << iterador->second
             << '\n';
    }

    // 9. Mostrar tamaño
    cout << "\n7. Cantidad de elementos: "
         << alumnos.size() << '\n';

    // 10. Verificar si está vacío
    cout << "\n8. Verificar si el map esta vacio:\n";

    if (alumnos.empty()) {
        cout << "El map esta vacio.\n";
    } else {
        cout << "El map contiene elementos.\n";
    }

    // 11. Mostrar primero y último
    if (!alumnos.empty()) {
        cout << "\n9. Primer elemento:\n";
        cout << alumnos.begin()->first
             << " -> "
             << alumnos.begin()->second << '\n';

        cout << "\n10. Ultimo elemento:\n";

        auto ultimo = alumnos.end();
        --ultimo;

        cout << ultimo->first
             << " -> "
             << ultimo->second << '\n';
    }

    // 12. Limpiar todo el map
    cout << "\n11. Limpiar el map:\n";

    alumnos.clear();

    cout << "Cantidad despues de clear(): "
         << alumnos.size() << '\n';

    cout << "\n===== FIN DEL DEMO =====\n";
}

TI main() {
    DemoMap();
    return 0;
}