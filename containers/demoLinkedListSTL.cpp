// La herramienta STL ya tiene los estándares en estructura de datos bien construidos
#include <iostream>
#include <list> // para listas doblemente enlazadas std::list
#include <iterator> // para iteradores std::advance


template <typename T>
void printList(const std::list<T>& lista, const std::string& mensaje){
    std::cout << "-------" << mensaje << "-------" << std::endl;
    for (const T& valor : lista)
        std::cout << valor << " -> ";
    std::cout << "NULL\n" << std::endl;
}

int main() {
    std::list<int> lista1;
    lista1.push_back(1); // usando el metodo pushback ya construido
    lista1.push_back(2);
    lista1.push_back(3);
    lista1.push_back(4);
    lista1.push_back(5);
    lista1.push_back(6);
    printList(lista1, "lista1");

    // Insertando usando iteradores y metodo insert
    auto it = lista1.begin(); // apuntamos al primer elemento
    std::advance(it, 2); // hacemos que el iterador avance dos espacios

    lista1.insert(it, 15); // insertamos el dato en la posicion

    printList(lista1, "lista1 tras la insercion");



}

