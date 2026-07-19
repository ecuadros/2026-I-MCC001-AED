# Hash y Poisson

## Idea general

Hash y Poisson suelen estudiarse juntos cuando se quiere analizar qué tan bien se distribuyen las claves dentro de una tabla hash.

- **Hash** nos dice en qué posición se guarda una clave.
- **Poisson** nos ayuda a estimar cuántas claves caerán en cada cubeta y cuántas colisiones pueden aparecer.

---

## 1. ¿Qué es un Hash?
Una función hash transforma una clave en una posición o índice de una tabla.

### Ejemplo simple
- clave: `"ana"`
- hash: `5`

Entonces el dato asociado a `"ana"` se guarda en la posición `5`.

### Objetivo de una buena función hash
Una buena función hash debe:
- ser rápida
- distribuir bien las claves
- reducir colisiones

### ¿Qué es una colisión?
Una colisión ocurre cuando dos claves distintas producen la misma posición.

### Ejemplo

- `"ana"` → `5`
- `"luz"` → `5`

Ambas claves caen en la misma cubeta, por lo tanto hay colisión.

---

## 2. ¿Qué es la distribución de Poisson?
La distribución de Poisson es un modelo matemático que permite estimar cuántos eventos ocurren en un intervalo, cuando esos eventos se producen de manera aleatoria e independiente.

En tablas hash, esos “eventos” son:

- claves cayendo en una cubeta

Entonces Poisson nos ayuda a estimar:

- cuántas cubetas quedarán vacías
- cuántas tendrán 1 elemento
- cuántas tendrán 2 o más elementos
- cuántas colisiones es razonable esperar

---

## 3. Fórmula de Poisson

La distribución de Poisson se expresa con la siguiente fórmula:

**P(X = k) = (e^-lambda * lambda^k) / k!**

### Donde:
- `k` = cantidad de elementos en una cubeta
- `lambda` = promedio de ocupación
- en hashing, normalmente se cumple que `lambda = n / m`

### Aquí:
- `n` = número total de claves
- `m` = número total de cubetas

---
## 4. Interpretación en una tabla hash
Si tenemos:
- `n = 100` claves
- `m = 200` cubetas

Entonces:
`lambda = 100 / 200 = 0.5`

Eso significa que, en promedio, cada cubeta tendrá `0.5` elementos.

No quiere decir que todas tengan medio elemento, sino que:
- algunas estarán vacías
- otras tendrán `1` elemento
- pocas tendrán `2` o más

---

## 5. Ejemplo numérico
Si `lambda = 1`, entonces:

### Probabilidad de que una cubeta esté vacía

`P(0) = e^-1 ≈ 0.3679`

### Probabilidad de que una cubeta tenga 1 elemento

`P(1) = e^-1 ≈ 0.3679`

### Probabilidad de que una cubeta tenga 2 elementos

`P(2) = e^-1 / 2 ≈ 0.1839`

### Interpretación

- `36.79%` de cubetas vacías
- `36.79%` de cubetas con `1` elemento
- `18.39%` de cubetas con `2` elementos

Esto muestra que, incluso con una buena función hash, pueden existir colisiones, pero en una proporción razonable.

---

## 6. Relación entre Hash y Poisson

La relación entre ambos conceptos es la siguiente:

- La función **hash** decide en qué cubeta cae cada clave
- La distribución de **Poisson** permite analizar si esa distribución está siendo razonable

Si la función hash reparte bien las claves, la ocupación de las cubetas suele aproximarse a una distribución de Poisson.

---

## 7. Ejemplo
Supongamos una tabla hash con `8` cubetas.

### Claves insertadas

- `"ana"` → `2`
- `"luz"` → `5`
- `"sol"` → `2`
- `"leo"` → `7`
- `"eva"` → `5`

### Tabla resultante

| Índice | Contenido   |
|--------|-------------|
| 0      | -           |
| 1      | -           |
| 2      | ana, sol    |
| 3      | -           |
| 4      | -           |
| 5      | luz, eva    |
| 6      | -           |
| 7      | leo         |

### Interpretación

- cubetas vacías: `0, 1, 3, 4, 6`
- cubeta con `1` elemento: `7`
- cubetas con colisión: `2, 5`

Aquí vemos claramente que:

- la función hash distribuyó las claves
- algunas posiciones quedaron vacías
- algunas recibieron una sola clave
- otras tuvieron colisión

Eso es justamente lo que Poisson ayuda a modelar.

---

## 8. Conclusión
Hash y Poisson se complementan:

- **Hash** es la técnica de almacenamiento y búsqueda rápida
- **Poisson** es la herramienta matemática para analizar la calidad de la distribución

Es decir:
- Hash resuelve el problema práctico
- Poisson ayuda a evaluar el rendimiento esperado


```mermaid
flowchart LR
    A["Claves: ana, luz, sol, leo, eva"] --> B["Funcion Hash"]
    B --> C["Tabla Hash"]

    C --> C0["0: -"]
    C --> C1["1: -"]
    C --> C2["2: ana, sol"]
    C --> C3["3: -"]
    C --> C4["4: -"]
    C --> C5["5: luz, eva"]
    C --> C6["6: -"]
    C --> C7["7: leo"]

    C2 --> D["Colision"]
    C5 --> D
    C0 --> E["Cubeta vacia"]
    C1 --> E
    C3 --> E
    C4 --> E
    C6 --> E