

##id INT INTEGER SERIAL PRIMARY KEY,
CREATE TABLE iiee (
    codinst varchar(20),
    cod_mod varchar(7),
    anexo char(1),
    cen_edu TEXT,
    nlat_ie NUMERIC,
    nlong_ie NUMERIC
);
-- En caso de tener data.csv de colegios
\copy iiee FROM 'C:/dev/maestria/01ciclo/algoritmos/2026-I-MCC001-AED/sqlpueba/data.csv' DELIMITER ',' CSV HEADER;

/*
-- Insertar datos (usando DEFAULT en la columna anexo)
INSERT INTO iiee (codinst, cod_mod, anexo, cen_edu, nlat_ie, nlong_ie)
VALUES
('23548844', '0248765', '0', '21607', -10.726017, -77.726183),
('25255585', '0253906', '0', '20351', -11.1341, -77.1907),
('23064545', '0254904', '0', '002', -13.0579, -76.3553),
('23326582', '0600023', '0', '003 DIVINO JESUS', -12.657652, -76.633586),
('22797921', '0255091', '0', '013', -13.0433, -76.392);
*/


# INDEX HASH
-- Download https://download.osgeo.org/postgis/windows/pg17/
-- 1. Asegúrate de tener la extensión PostGIS activa
CREATE EXTENSION IF NOT EXISTS postgis;

-- 2. Añade la nueva columna geométrica para almacenar el punto (Longitud, Latitud)
ALTER TABLE iiee ADD COLUMN geom geometry(Point, 4326);

-- 3. Convierte las columnas numéricas actuales en el objeto geométrico Punto
-- NOTA: PostGIS requiere el orden (Longitud, Latitud) -> (X, Y)
UPDATE iiee 
SET geom = ST_SetSRID(ST_MakePoint(nlong_ie, nlat_ie), 4326)
WHERE nlong_ie IS NOT NULL AND nlat_ie IS NOT NULL;

-- 4. Crea el índice espacial GiST
CREATE INDEX iiee_geom_gist ON iiee USING gist(geom);

EXPLAIN ANALYZE
SELECT count(*)
FROM iiee
WHERE nlat_ie BETWEEN -12.5 AND -12.0
  AND nlong_ie BETWEEN -77.5 AND -76.5;

/*
QUERY PLAN
---------------------------------------------------------------------------------------------------------------------------------------------------------
 Aggregate  (cost=211.92..211.93 rows=1 width=8) (actual time=1.384..1.384 rows=1 loops=1)
   ->  Seq Scan on iiee  (cost=0.00..207.92 rows=1598 width=0) (actual time=0.104..1.275 rows=1827 loops=1)
         Filter: ((nlat_ie >= '-12.5'::numeric) AND (nlat_ie <= '-12.0'::numeric) AND (nlong_ie >= '-77.5'::numeric) AND (nlong_ie <= '-76.5'::numeric))
         Rows Removed by Filter: 2769
 Planning Time: 0.251 ms
 Execution Time: 1.426 ms

*/


EXPLAIN ANALYZE
SELECT count(*)
FROM iiee
WHERE ST_Within(
    geom,
    ST_MakeEnvelope(
        -77.5,   -- longitud mínima
        -12.5,   -- latitud mínima
        -76.5,   -- longitud máxima
        -12.0,   -- latitud máxima
        4326
    )
);
/*
QUERY PLAN                                                                                                             
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Aggregate  (cost=23180.89..23180.90 rows=1 width=8) (actual time=0.811..0.811 rows=1 loops=1)
   ->  Bitmap Heap Scan on iiee  (cost=54.40..23176.29 rows=1839 width=0) (actual time=0.128..0.740 rows=1827 loops=1)
         Filter: st_within(geom, '0103000020E6100000010000000500000000000000006053C000000000000029C000000000006053C000000000000028C000000000002053C000000000000028C000000000002053C000000000000029C000000000006053C000000000000029C0'::geometry)
         Heap Blocks: exact=51
         ->  Bitmap Index Scan on iiee_geom_gist  (cost=0.00..53.94 rows=1839 width=0) (actual time=0.108..0.108 rows=1827 loops=1)
               Index Cond: (geom @ '0103000020E6100000010000000500000000000000006053C000000000000029C000000000006053C000000000000028C000000000002053C000000000000028C000000000002053C000000000000029C000000000006053C000000000000029C0'::geometry)
 Planning Time: 0.185 ms
 Execution Time: 0.835 ms
*/



EXPLAIN ANALYZE
SELECT count(*)
FROM iiee
WHERE geom && ST_MakeEnvelope(
    -77.5,
    -12.5,
    -76.5,
    -12.0,
    4326
);
/*
                                                                                                                QUERY PLAN
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Aggregate  (cost=178.05..178.06 rows=1 width=8) (actual time=1.671..1.671 rows=1 loops=1)
   ->  Seq Scan on iiee  (cost=0.00..173.45 rows=1839 width=0) (actual time=0.074..1.562 rows=1827 loops=1)
         Filter: (geom && '0103000020E6100000010000000500000000000000006053C000000000000029C000000000006053C000000000000028C000000000002053C000000000000028C000000000002053C000000000000029C000000000006053C000000000000029C0'::geometry)
         Rows Removed by Filter: 2769
 Planning Time: 0.176 ms
 Execution Time: 1.692 ms
*/

btree, hash
#Creando index btree
ALTER TABLE iiee ADD COLUMN cod_mod_idx varchar(7);
UPDATE iiee set cod_mod_idx = cod_mod;

CREATE INDEX idx_cod_mod ON iiee (cod_mod_idx);


#Creando index hash

ALTER TABLE iiee ADD column cod_mod_hash varchar(7);

update iiee set cod_mod_hash = cod_mod;

CREATE INDEX idx_cod_mod_hash ON iiee USING hash (cod_mod_hash);

EXPLAIN ANALYZE
SELECT * from iiee where cod_mod = '0248765';
/* Sin indice
QUERY PLAN
--------------------------------------------------------------------------------------------------
 Seq Scan on iiee  (cost=0.00..203.45 rows=1 width=102) (actual time=0.025..0.251 rows=1 loops=1)
   Filter: ((cod_mod)::text = '0248765'::text)
   Rows Removed by Filter: 4595
 Planning Time: 1.094 ms
 Execution Time: 0.259 ms
*/
EXPLAIN ANALYZE
SELECT * from iiee where cod_mod_idx = '0248765';
/* Btree index
                                                    QUERY PLAN
--------------------------------------------------------------------------------------------------------------------
 Index Scan using idx_cod_mod on iiee  (cost=0.28..8.30 rows=1 width=102) (actual time=0.028..0.028 rows=1 loops=1)
   Index Cond: ((cod_mod_idx)::text = '0248765'::text)
 Planning Time: 0.092 ms
 Execution Time: 0.041 ms
*/

EXPLAIN ANALYZE
SELECT * from iiee where cod_mod_hash = '0248765';
/* Hash index
 QUERY PLAN
-------------------------------------------------------------------------------------------------------------------------
 Index Scan using idx_cod_mod_hash on iiee  (cost=0.00..8.02 rows=1 width=102) (actual time=0.013..0.014 rows=1 loops=1)
   Index Cond: ((cod_mod_hash)::text = '0248765'::text)
 Planning Time: 0.070 ms
 Execution Time: 0.023 ms
*/





