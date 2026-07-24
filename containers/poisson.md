## Tablas Hash y la distribución de Poisson
Las tablas Hash son similares a un Array donde se aplica una función Hash a un dato entrante para conocer la posición
del arreglo en la que debería ir.

Idealmente, si tendriamos 100 espacios, esperariamos que, al ingresar 100 datos,
todos los espacios estén ocupados por un solo dato.

En la realidad, al tener 100 espacios, y lanzar 100 datos, en algunos espacios habrán más de dos datos --> Colisión

"Es estadísticamene imposible que cada espacio reciba únicamente un dato"

### Distribución de Poisson
Utilizada para estimar cuántas veces ocurre un evento en un intervalo determinado. Caracaterizadas por el parámetro $\lambda$ : promedio esperado de eventos.  

Ej. Para 100 datos y 100 espacios, $\lambda = 1$

La forma de la distribución de Poisson es:
$$P(k) = \frac{\lambda^k e^{-\lambda}}{k!}$$

Donde: P(k) es la probabilidad de que $k$ elementos caigan en el mismo espacio.

Ej. Considerando un $\lambda = 1$

si $k=0$ (Espacios vacios)............... entonces $P(0) = \frac{1^0 e^{-1}}{0!} \simeq 36.8\%$

si $k=1$ (1 elemento en cada espacio).... entonces $P(1) = \frac{1^1 e^{-1}}{1!} \simeq 36.8\%$... Solo el $36.8\%$ de los espacios tendrá un elemento.

si $k=3$ (3 elementos en cada espacio)... entonces $P(3) = \frac{1^3 e^{-1}}{3!} \simeq 6.1\%$... Solo el $6.1\%$ de los espacios tendrá 3 elementos (colisiones).

Cuando $\lambda = 3$, ej. 300 datos en 100 espacios la probabilidad de tener colisiones se dispara, rompiendo así la complejidad O(1) para la búsqueda.

Volviendo al ejemplo de $\lambda=1$, la probabilidad de que hayan colisiones en el espacio es de:
$$P(k \ge 2) = 1 - P(k=0) - P(k=1) = 1 - \frac{1^0 e^{-1}}{0!} - \frac{1^1 e^{-1}}{1!} \approx 0.26$$
Es decir, aproximadamente un 26%.

Normalmente, se busca mantener el valor de $\lambda$ entre $0.75$ y $1.0$. Cuando $\lambda$ supera ese límite se hace un proceso llamado Rehashing, creando una arreglo más grande, volviendo a mapear los datos usando la función Hash. Todo esto para cotrolar el parámetro $\lambda$.

