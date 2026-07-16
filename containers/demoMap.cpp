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
    m[2] = "jose";
    m[3] = "juan";
    m[4] = "ana";

    // Manera 2 de crear valores
    m.emplace(5,"luis"); // te deja insertar datos, si solo no existen
    m.emplace(4,"mabe"); // si compila, pero no inserta el valor, m[4] sigue siendo "ana"

    m.insert(std::make_pair(6, "Carla"));
    m.insert(std::make_pair(7, "Aaron"));
    m.insert({8, "pablo"});
    



    std::string b = m.at(8); // es una forma de llamar a la m[3]
    std::cout << b << std::endl;
	
    // Accediendo al valor... maneras
    std::cout << "En la posición 3 está: " << m[3] << "\n";
    std::cout << "En la posición 3 está: " << m.at(3) << std::endl;
    //std::cout << "En la posición 1 está: " << m.at(1) << std::endl;
    
    m[3] = "karla";
    std::cout << "En la posición 3 está: " << m[3] << "\n";
    std::cout << "En la posición 3 está: " << m.at(3) << std::endl;
    //std::cout << "En la posición 1 está: " << m.at(1) << std::endl;
    
    std::cout << "==============================" << std::endl;
    
    
    return 0;
}
