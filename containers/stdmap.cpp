#include <iostream>
#include <map>
#include <string_view>
#include "../types.h"
using namespace std;

// Imprimir el contenido del map
static ostream& PrintMap(string_view title, const map<LSS, LSI>& data, ostream& os)
{
    os << title << '\n';
    for(const auto& [key,value] : data)
        os << "  [" << key << "] = " << value << '\n';
    os << '\n';
    return os;
}

// Inserción de elementos
static void DemoInsert(map<LSS,LSI>& alumnos, ostream& os)
{
    os << "\nInsertar\n";
    alumnos.insert({"Andrea",18});
    alumnos.insert({"Bruno",15});
    alumnos.insert({"Carlos",20});
    alumnos.emplace("Diana",17);
    PrintMap("Despues de insertar nuevos alumnos:", alumnos, os);
}


// Orden automático del árbol
static void DemoFind(const map<LSS,LSI>& alumnos, ostream& os)
{
    os << "\nEncontrar\n";
    auto it = alumnos.find("Carlos");
    if(it != alumnos.end())
    {
        os << "Alumno encontrado: " << it->first << " -> " << it->second << '\n';
    }
    else
        os << "Alumno no encontrado\n";
}

// Acceso y modificación
static void DemoAccess(map<LSS,LSI>& alumnos, ostream& os)
{
    os << "\nAcceso\n";
    os << "Nota de Ana: " << alumnos["Ana"] << '\n';
    alumnos["Ana"] = 19; // Modificacion de un valor existente
    os << "Nueva nota de Ana: " << alumnos.at("Ana") << '\n';
    os << "Nota de Felipe: " << alumnos["Felipe"] << '\n'; // [] crea una nueva clave si no existe
    os << "Cantidad de alumnos: " << alumnos.size() << '\n';
}


// Consultas por rango
static void DemoRange(ostream& os)
{
    os << "\nRango\n";
    map<LSI,LSS> rendimiento{{10,"Deficiente"}, {14,"Regular"}, {16,"Bueno"}, {18,"Excelente"}, {20,"Sobresaliente"}};
    auto it = rendimiento.lower_bound(17);
    if(it != rendimiento.end())
    {
        os << "Primer rendimiento >= 17: " << it->first << " -> " << it->second << '\n';
    }
}

// Eliminacion
static void DemoErase(map<LSS,LSI>& alumnos, ostream& os)
{
    os << "\nBorrar\n";
    PrintMap("Antes:", alumnos, os);
    alumnos.erase("Bruno");
    PrintMap("Despues:", alumnos, os);
}

// Prueba
void DemoMap(ostream& os)
{
    os << "\nPrueba\n";
    // Creacion inicial del contenedor
    map<LSS,LSI> alumnos{{"Ana",16}, {"Luis",18}, {"Pedro",14}, {"Maria",20}, {"Jose",15},
        {"Sofia",19}, {"Miguel",17}, {"Laura",13}, {"Carlos",18}, {"Elena",16}};
    PrintMap("Estado inicial del map:", alumnos, os);
    DemoInsert(alumnos, os);
    DemoFind(alumnos, os);
    DemoAccess(alumnos, os);
    DemoRange(os);
    DemoErase(alumnos, os);
    os << "\nComplejidad teorica:\n";
    os << "Insert      : O(log(n))\n";
    os << "Find        : O(log(n))\n";
    os << "Erase       : O(log(n))\n";
    os << "lower_bound : O(log(n))\n";
    os << "\nstd::map utiliza internamente un Red-Black Tree.\n";
}


int main()
{
    DemoMap(cout);
    return 0;
}
