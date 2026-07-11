-- ==========================================
-- 1. CREACIÓN DE LA TABLA -> es el molde del bloque
-- ==========================================
CREATE TABLE detecciones (
    id SERIAL PRIMARY KEY, -- es el id unico de cada fila
    -- SERIAL : le dice que el id va a crecer tras cada insercion
    -- PRIMARY KEY : evita que dos elementos compartan el mismo id
    energia_depositada FLOAT,      -- B-Tree (búsquedas por rangos o mayor/menor en datos de tipo flotante)
    codigo_identificador VARCHAR,  -- Hash (búsquedas dirigida) aplica una funcion que devuelve la direccion de memoria, para búsquedas rapidas
    region_detectada BOX           -- Ideal para GiST (búsquedas geometricas)
);

-- ==========================================
-- 2. POBLADO DE DATOS (Muestra representativa)
-- Generamos 100,000 registros aleatorios para obligar al motor a usar los índices
-- ==========================================
INSERT INTO detecciones (energia_depositada, codigo_identificador, region_detectada) -- insercion masiva de datos
SELECT 
    random() * 1000, -- para energia_depositada
    md5(random()::text), -- para el codigo identificador, usa funcion de cifrado md5
    box(point(random()*100, random()*100), point(random()*100, random()*100)) -- para la region detectada, creacion de cajas
FROM generate_series(1, 100000); -- genera 100 000 datos aleatorios

-- 
-- Insertamos un dato conocido y controlable para nuestras pruebas de búsqueda
INSERT INTO detecciones (energia_depositada, codigo_identificador, region_detectada)
VALUES (550.5, 'OBJETO-XYZ-123', box(point(10,10), point(20,20)));

-- ==========================================
-- 3. CREACIÓN DE LOS ÍNDICES - INDEXACION DE DATOS
-- ==========================================
-- B-Tree: Es el predeterminado. Estructura el árbol dividiendo los datos de forma ordenada.
CREATE INDEX idx_btree_energia ON detecciones USING btree (energia_depositada);

-- Hash: Solo almacena un valor hash de 32 bits. O(1) pero inútil para rangos.
CREATE INDEX idx_hash_codigo ON detecciones USING hash (codigo_identificador);

-- GiST: Árbol de búsqueda generalizado. Agrupa los datos por proximidad o superposición.
CREATE INDEX idx_gist_region ON detecciones USING gist (region_detectada);

-- ==========================================
-- 4. DEMOSTRACIÓN DE BÚSQUEDA
-- ==========================================

-- Búsqueda con B-Tree (Demostración de rango)
SELECT * FROM detecciones -- el * significa que me devuelva toda la fila que cumple la condicion de energia
WHERE energia_depositada BETWEEN 550.0 AND 551.0;

-- Búsqueda con Hash (Demostración de igualdad exacta)
SELECT * FROM detecciones 
WHERE codigo_identificador = 'OBJETO-XYZ-123';

-- Búsqueda con GiST (Demostración geométrica)
-- El operador && significa "se superpone con". Busca cajas que intercepten con la caja dada.
EXPLAIN ANALYZE 
SELECT * FROM detecciones 
WHERE region_detectada && box(point(15,15), point(25,25));
