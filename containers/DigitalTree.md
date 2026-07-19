# Digital Trie

## Definición
Un Digital Trie es una estructura de datos en forma de árbol que almacena claves dividiéndolas por partes digitales, como caracteres o bits. Cada nivel del árbol representa una posición de la clave, permitiendo búsquedas eficientes por prefijo.

Si la clave es texto:
- cada nivel puede representar un carácter
Si la clave es numérica o binaria:
- cada nivel puede representar un bit o un grupo de bits

## Idea principal
La idea principal es que no comparas la clave completa, sino que vas bajando por el árbol usando cada símbolo de la clave.

## Ventajas
- Búsqueda rápida por prefijo
- Inserción y búsqueda simples
- Útil para diccionarios y autocompletado

## Desventajas
- Puede usar bastante memoria
- El borrado es más complejo
- Depende del tamaño del alfabeto

## Ejemplo
Si se insertan las palabras `cat`, `car` y `dog`, las palabras `cat` y `car` comparten el prefijo `ca`, por lo que reutilizan parte del camino en el árbol.

## Diagrama del Digital Trie

```mermaid
flowchart TD
    A["Raiz"] --> C["c"]
    A --> D["d"]

    C --> CA["a"]
    CA --> CAT["t (cat)"]
    CA --> CAR["r (car)"]

    D --> DO["o"]
    DO --> DOG["g (dog)"]
    DO --> DOL["l"]
    DOL --> DOLL["l (doll)"]
    DO --> DOO["o"]
    DOO --> DOOR["r (door)"]

    