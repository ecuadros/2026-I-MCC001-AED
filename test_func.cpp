
#include <iostream>
#include <functional>

using namespace std;

void hola(){
    cout << "Hola mundo" << endl;
}

int main() {
    using returnType = decltype(hola());
    if (is_void_v<returnType>) {
        cout << "La función hola() devuelve void." << endl;
        hola();
    } else {
        cout << "La función hola() no devuelve void." << endl;
    }
    auto a = std::invoke(hola); // Llamada a la función hola() usando std::invoke
}