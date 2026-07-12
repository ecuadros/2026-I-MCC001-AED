# Cambios realizados en heap.h

## Enfoque aplicado
Se implementó una versión funcional y concurrente de Heap min/max respetando la estructura del proyecto (traits + node + estilo de contenedores), y se mantuvo el código anterior **sin borrarlo**: se dejó comentado mediante un bloque `#if 0 ... #endif` para conservar historial y referencia.

## Cambios principales

1. Corrección de modelado del nodo
- Se añadió una implementación nueva de `HeapNode<T>` (genérica por valor, no por traits).
- Se incluyeron métodos consistentes con el proyecto:
  - `GetData`, `GetDataRef`, `SetData`
  - `GetRef`, `GetRefRef`, `SetRef`
  - `ToString`

2. Traits para min/max heap
- Se agregó `BaseHeapTrait<T>`.
- `AscendingHeapTrait<T>` hereda de `AscendingTrait<T>` -> prioridad al menor (min-heap).
- `DescendingHeapTrait<T>` hereda de `DescendingTrait<T>` -> prioridad al mayor (max-heap).

3. Estructura Heap concurrente
- Se implementó una nueva clase `Heap<Traits>` con:
  - `std::vector<Node> m_heap`
  - `Comp m_comp`
  - `mutable std::mutex m_mtx`
- Todas las operaciones públicas usan `std::scoped_lock`:
  - `insert`
  - `extract`
  - `extract_top`
  - `peek_min`
  - `peek_node`
  - `peek_ref`
  - `empty`
  - `size`
  - `clear`
  - `toString`

4. Métodos necesarios implementados
- Inserción y rebalanceo: `insert` + `heapify_up`.
- Extracción de raíz:
  - `extract()` mantiene firma original (void, compatibilidad).
  - `extract_top()` devuelve el nodo removido.
- Consulta de raíz:
  - `peek_min()` devuelve el valor del tope.
  - `peek_node()` y `peek_ref()` para acceso completo.
- Utilitarios: `empty`, `size`, `clear`, `toString`.

5. Lógica correcta de comparación
- Se creó `has_higher_priority(lhs, rhs)` para comparar por `GetData()` usando `Comp`.
- Esto permite que la misma implementación funcione para min-heap o max-heap según trait.

6. Constructores y asignaciones
- Se añadieron:
  - constructor por copia
  - constructor por movimiento
  - asignación por copia
  - asignación por movimiento
- Todos con bloqueo para seguridad en concurrencia.

7. Compatibilidad y salida
- Se añadió `operator<<` para `HeapNode` y `Heap`.
- Se mantuvo el nombre `peek_min()` por compatibilidad; en `DescendingHeapTrait` representa el tope (máximo lógico del heap).

## Justificación
- El código original tenía problemas de tipado (`HeapNode`), comparaciones incompatibles entre `Node` y `value_type`, y ausencia de sincronización entre hilos.
- La nueva implementación resuelve esos problemas sin romper el estilo de la base existente y preserva líneas originales comentadas, como fue solicitado.
