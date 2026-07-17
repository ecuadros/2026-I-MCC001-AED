#include <iostream>
#include <map> // cargando libreria map
// organiza los indices en un arbol binario - red black tree
#include <string>

using TS = std::string;
using TI = int;


int main() {
    std::map<TI, TS> m;
    
    // Manera 1 de crear valores
    m[0] = "denis"; 
    m[1] = "pepe";

    // Manera 2 de crear valores
    m.emplace(2,"luis"); // te deja insertar datos, si solo no existen
    m.emplace(3,"mabe"); // si compila, pero no inserta el valor, m[4] sigue siendo "ana"

    m.insert(std::make_pair(4, "Carla"));
    m.insert(std::make_pair(5, "Aaron"));
    m.insert({6, "pablo"});
    m.insert({10,"juan"});

    std::cout << "==============================" << std::endl;

    
    std::cout << "Imprimiendo posiciones" << std::endl;
    std::cout << "Posicion 5: " << m.at(6) << std::endl;
    
    // Accediendo al valor... maneras
    std::cout << "En la posición 3 está: " << m[3] << "\n";
    std::cout << "En la posición 3 está: " << m.at(3) << std::endl;
    //std::cout << "En la posición 1 está: " << m.at(1) << std::endl;
    
    m[3] = "karla"; // Modificando la posicion 3
    std::cout << "En la posición 3 está: " << m[3] << "\n";
    std::cout << "En la posición 3 está: " << m.at(3) << std::endl;

    std::cout << "==============================" << std::endl;
    std::cout << "Buscando valores" << std::endl;
    // Iterador a las posiciones de los datos
    auto it = m.find(3);
    if (it != m.end())
        std::cout << "Valor encontrado en la posicion 3: " << it->second << std::endl;
    else
        std::cout << "Valor no existente" << std::endl;

    std::cout << "==============================" << std::endl;
    
    std::cout << "Tamaño de arreglo (solo cuenta los existentes)" << std::endl;
    std::cout << "Tamaño: " << m.size() << std::endl;
    
    std::cout << "==============================" << std::endl;
    std::cout << "Recorrido:" << std::endl;
    std::cout << "Manera 1:" << std::endl;
    for (auto it = m.begin(); it != m.end(); ++it)
        std::cout << "Posición: " << it->first << ", Dato: " << it->second << std::endl;

    std::cout << "Manera 2:" << std::endl;
    for (const auto& [posicion, data] : m)
        std::cout << "Posición: " << posicion << ", Dato: " << data << std::endl;

    std::cout << "==============================" << std::endl;

    
    
    return 0;
}
