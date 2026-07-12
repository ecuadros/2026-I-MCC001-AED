# Colisiones en Hash Map.  
Se asumen $p = \frac{1}{m}$ ya que las claves pueden caer en cualquier bucket/clave.  

### Numero de elementos en un bucket.  
- $p = \frac{1}{m}$  cae en el bucket  
- $1-p = 1-\frac{1}{m}$ no cae en el bucket

entonces la cantidad de elementos que cae en el bucket siguie  una distribucion binomial tipo: 
$$
X \sim \operatorname{Binomial}\left(n, \frac{1}{m}\right)
$$
### Probilidad de colision.
Entonces la probabilidad de colicion es:
$$
P(\text{colisión}) = P(X\ge2)=1-P(X=0)-P(X=1)
$$
Entonces tenemos que la probilidad de colicion es:
$$
\boxed{
P(\text{colisión en un bucket}) = 1-\left(1-\frac{1}{m}\right)^n - n\frac{1}{m}\left(1-\frac{1}{m}\right)^{n-1}
}
$$
## Colision con Poisson.
Cuando $n$ y $m$ son grandes, y el factor de carga es

$$
\lambda = \frac{n}{m},
$$

Sigue una distribuicion de posion.
$$
X\sim\operatorname{Poisson}(\lambda)
$$