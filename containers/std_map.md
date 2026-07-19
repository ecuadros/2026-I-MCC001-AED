# std::map en C++

## ¿Qué es std::map?

`std::map` es un contenedor asociativo de la **STL (Standard Template Library)** que almacena elementos en forma de pares **clave-valor (key-value)**.

Cada clave es **única** y los elementos se mantienen **ordenados automáticamente** por la clave.

```
Clave          Valor
-------------------------
1001   ------> Susana
1002   ------> Carlos
1003   ------> Ana
1004   ------> Luis
```

---

## ¿Cómo funciona internamente?

Internamente, `std::map` utiliza un **árbol Red-Black (Red-Black Tree)**, un árbol binario de búsqueda balanceado muy similar a un árbol AVL.

```
                 50
               /    \
             30      70
            /  \    /  \
          20   40  60   80
```

Gracias a esta estructura, las operaciones de búsqueda, inserción y eliminación tienen una complejidad de **O(log n)**.

---

## Biblioteca necesaria

```cpp
#include <map>
```