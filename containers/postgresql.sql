DROP TABLE IF EXISTS libros; --Borrado en caso libros ya exista
---- Creacion
CREATE TABLE libros (
    id SERIAL PRIMARY KEY,          -- Identificador único del libro
    numero_paginas INT,             -- Para B-Tree consultas por rango
    codigo_isbn VARCHAR(50),        -- Para Hash busquedas exactas
    ubicacion BOX                   -- Para GiST ubicación física en la biblioteca
);

-- Insercion de datos, Genera 100 000 registros 
INSERT INTO libros (numero_paginas, codigo_isbn, ubicacion)
SELECT
    (random()*900 + 100)::INT,             -- Aleatoriamente entre 100 y 1000 páginas
    md5(random()::text), box(point(random()*50, random()*50), point(random()*50, random()*50))
FROM generate_series(1,100000);

-- Registro para las pruebas
INSERT INTO libros (numero_paginas, codigo_isbn, ubicacion)
VALUES (350, 'ISBN-978-UNI-2026', box(point(12,8), point(18,14)));

---- Creacion de indices

-- B-Tree
-- Se organiza los valores en un árbol balanceado
CREATE INDEX idx_btree_paginas ON libros USING btree(numero_paginas);

-- Hash
-- Se utiliza una funcion hash para acelerar búsquedas por igualdad
CREATE INDEX idx_hash_isbn ON libros USING hash(codigo_isbn);

-- GiST
-- Indice generalizado para datos espaciales
CREATE INDEX idx_gist_ubicacion ON libros USING gist(ubicacion);

ANALYZE libros; --Actualizar

---- Consulta de prueba

-- B-Tree, buscar libros entre 300 y 400 páginas
EXPLAIN ANALYZE SELECT * FROM libros WHERE numero_paginas BETWEEN 300 AND 400;

-- Hash, buscar un ISBN específico
EXPLAIN ANALYZE SELECT * FROM libros WHERE codigo_isbn = 'ISBN-978-UNI-2026';

-- GiST, busca estantes cuya ubicación se superpone con la región indicada.
EXPLAIN ANALYZE SELECT * FROM libros WHERE ubicacion && box(point(10,5), point(20,15));