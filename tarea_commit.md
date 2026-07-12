# Tareas del Commit

## Objetivo
Implementar soporte de Double Linked List, Circular Linked List y Circular Double Linked List, con demo y soporte de documentación.

## Tareas Implementadas

- [x] Refactorizar la clase base LinkedList para soportar extension por herencia.
- [x] Agregar hooks polimorficos de insercion y eliminacion (`on_node_inserted` y `on_node_removed`).
- [x] Mejorar insercion interna con `internal_insert` reutilizable para clases derivadas.
- [x] Agregar serializacion y deserializacion de nodos/listas (`ToString`, `fromIstream`, operadores `<<` y `>>`).

- [x] Implementar `DLLNode` con puntero al nodo previo.
- [x] Crear traits de DoubleLinkedList (ascendente y descendente).
- [x] Implementar `DoubleLinkedListBackwardIterator`.
- [x] Implementar clase `DoubleLinkedList` heredando de `LinkedList`.
- [x] Agregar constructores copy/move en DoubleLinkedList.

- [x] Crear `CircularLinkedList` con traits e iterador circular forward.
- [x] Implementar operaciones principales en CircularLinkedList (`insert`, `push_back`, `pop_back`).
- [x] Asegurar manejo correcto del ciclo en constructor/destructor.

- [x] Crear `CircularDoubleLinkedList` con traits e iteradores circular forward/backward.
- [x] Implementar operaciones principales en CircularDoubleLinkedList (`insert`, `push_front`, `pop_front`, `push_back`, `pop_back`).
- [x] Mantener consistencia del anillo doble (`tail->next` y `root->prev`).

- [x] Crear demo `DDListDemo` en `DLLDemo.cpp`.
- [x] Registrar `DDListDemo` en `lists.h` y `main.cpp`.
- [x] Actualizar `Makefile` para compilar la demo.

- [x] Generar documentacion de apoyo (`Doxyfile`, `changes.txt`).
- [x] Agregar diagrama Mermaid de herencia (`inheritance-diagram.md`).

## Resultado
Se incorporo una jerarquia completa de listas enlazadas (simple, doble, circular y circular doble) con enfoque en polimorfismo, reutilizacion de codigo y demo funcional.
