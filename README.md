# Red Social — Proyecto Final de Algoritmos y Estructuras de Datos

Núcleo (backend) de una red social tipo Facebook implementado **íntegramente en C++
sin usar ninguna estructura de datos de la STL**. Todas las estructuras —arreglo
dinámico, cadena de texto, lista enlazada, pila, cola, tabla hash, montículo, árbol
AVL, trie y grafo— están escritas desde cero en este repositorio.

Probado con **1 000 000 de usuarios, 8 000 000 de amistades y 2 000 000 de
publicaciones**, generados en **2.9 segundos** y ocupando **~900 MB** de memoria.

---

## 1. Compilar y ejecutar

```bash
make                # compila ./redsocial y ./pruebas_aed
make probar         # ejecuta las 125 pruebas automáticas
make ejecutar       # lanza el sistema con 100 000 usuarios
make banco          # ejecuta el análisis de rendimiento hasta 1 000 000
```

Alternativa con CMake:

```bash
cmake -B build && cmake --build build -j
./build/redsocial
```

Opciones de línea de comandos:

| Opción              | Significado                                                         |
| ------------------- | ------------------------------------------------------------------- |
| `--usuarios N`      | Usuarios a generar (por defecto 100 000)                            |
| `--amistades M`     | Amistades por usuario nuevo, modelo Barabási-Albert (por defecto 8) |
| `--publicaciones P` | Publicaciones a generar (por defecto `2*N`)                         |
| `--semilla S`       | Semilla del generador: los datos son reproducibles                  |
| `--vacia`           | Arranca sin datos (para registrar usuarios a mano o importar CSV)   |
| `--banco`           | Ejecuta solo el banco de pruebas y termina                          |
| `--maximo N`        | Escala máxima del banco de pruebas                                  |

Ejemplo de la demostración a escala real:

```bash
./redsocial --usuarios 1000000
```

---

## 2. Estructuras de datos implementadas

Todas están en `src/estructuras/`. Ninguna usa la STL.

| Estructura               | Archivo                         | Dónde se usa                     | Por qué esa y no otra                                                                                         |
| ------------------------ | ------------------------------- | -------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| **Cadena**               | `Cadena.h/.cpp`                 | Nombres, correos, textos         | Reemplaza a `std::string`. Con semántica de movimiento para no copiar millones de textos                      |
| **Arreglo\<T\>**         | `Arreglo.h`                     | Base de todo                     | Reemplaza a `std::vector`. Crecimiento geométrico, O(1) amortizado                                            |
| **ListaEnlazada\<T\>**   | `ListaEnlazada.h`               | Comentarios de cada publicación  | Pocos elementos por publicación y siempre se agregan al final: no desperdicia capacidad reservada × 2 000 000 |
| **Pila\<T\>**            | `Pila.h`                        | Reconstruir el camino de amistad | El BFS descubre el camino al revés; apilar y desapilar lo invierte sin código extra                           |
| **Cola\<T\>**            | `Cola.h`                        | Recorrido BFS                    | Arreglo **circular**: contiguo en memoria, sin un `new` por elemento                                          |
| **TablaHash\<K,V\>**     | `TablaHash.h`                   | id→índice, correo→índice         | Direccionamiento abierto con sondeo lineal y **tumbas**. Ver §4                                               |
| **MonticuloMinimo\<T\>** | `MonticuloMinimo.h`             | Rankings top-K                   | Filtro de K elementos: O(n log K) en vez de O(n log n)                                                        |
| **ArbolAVL\<K,V\>**      | `ArbolAVL.h`                    | Índice de nombres                | Único que mantiene **orden**: permite listar alfabéticamente                                                  |
| **Trie**                 | `Trie.h/.cpp`                   | Búsqueda por prefijo             | Único que responde "todos los que empiezan por MAR" en O(longitud del prefijo)                                |
| **GrafoAmistades**       | `sistema/GrafoAmistades.h/.cpp` | Amistades                        | Listas de adyacencia **ordenadas**. Ver §4                                                                    |
| **Ordenamiento**         | `Ordenamiento.h`                | Listas de adyacencia, rankings   | Quicksort (mediana de tres) y mergesort propios                                                               |

Utilidades genéricas propias (`Utilidades.h`): `mover()` (equivalente a `std::move`),
`intercambiar()`, `minimo()`, `maximo()` y los comparadores. El proyecto no incluye
`<utility>` ni `<algorithm>`.

**Bibliotecas del sistema usadas:** solo `<cstdio>`, `<cstdlib>`, `<cstring>`,
`<iostream>`/`<ostream>`/`<iosfwd>` (entrada y salida) y `<chrono>`.

---

## 3. Dónde está cada funcionalidad exigida

Todas viven en la fachada `RedSocial` (`src/sistema/RedSocial.h/.cpp`), que es el
único punto por el que pasa la lógica del sistema.

| #   | Funcionalidad del enunciado      | Método                                                                  | Complejidad                          |
| --- | -------------------------------- | ----------------------------------------------------------------------- | ------------------------------------ |
| 1   | Registrar usuarios               | `registrarUsuario`                                                      | O(1) promedio + O(log n) del AVL     |
| 2   | Eliminar usuarios                | `eliminarUsuario`                                                       | O(grado + publicaciones)             |
| 3   | Buscar usuarios                  | `buscarPorId`, `buscarPorCorreo`, `buscarPorNombre`, `buscarPorPrefijo` | O(1) / O(1) / O(log n) / O(longitud) |
| 4   | Crear publicaciones              | `crearPublicacion`                                                      | O(1) amortizado                      |
| 5   | Eliminar publicaciones           | `eliminarPublicacion`                                                   | O(1) promedio                        |
| 6   | Agregar amigos                   | `agregarAmigo`                                                          | O(grado)                             |
| 7   | Eliminar amigos                  | `eliminarAmigo`                                                         | O(grado)                             |
| 8   | Camino de amistad                | `caminoDeAmistad` → `GrafoAmistades::caminoMasCorto`                    | BFS bidireccional                    |
| 9   | Amigos en común                  | `amigosEnComun`                                                         | O(gA + gB) por intersección          |
| 10  | Sugerencias de amistad           | `sugerenciasDeAmistad`                                                  | O(Σ grados de mis amigos + C log K)  |
| 11  | Publicaciones de un usuario      | `publicacionesDe`                                                       | O(publicaciones del usuario)         |
| 12  | Usuarios más activos             | `usuariosMasActivos`                                                    | O(n log K)                           |
| 13  | Publicaciones con más reacciones | `publicacionesConMasReacciones`                                         | O(p log K)                           |

Extras: `listarEnOrdenAlfabetico` (recorrido en orden del AVL), exportación e
importación CSV, estadísticas del sistema y banco de pruebas.

---

## 4. Las cuatro decisiones de diseño que hay que saber defender

### 4.1 Tabla hash con direccionamiento abierto y tumbas

Con encadenamiento, cada usuario necesitaría un nodo con puntero: un millón de
llamadas a `new` y un salto de memoria por consulta. Con direccionamiento abierto
todo vive en un único arreglo contiguo y las colisiones se resuelven mirando la
casilla contigua, que ya está en la caché del procesador.

El punto delicado: **al borrar no se puede dejar la casilla vacía**, porque eso
cortaría la cadena de sondeo y volvería invisibles a las claves que colisionaron
después. Por eso se marca como `BORRADA` (una "tumba"): la búsqueda la atraviesa,
la inserción la reutiliza y la redimensión las elimina todas. Hay una prueba
automática dedicada exactamente a este caso.

### 4.2 Listas de adyacencia ordenadas

Una matriz de adyacencia para un millón de usuarios necesitaría 125 GB. Las listas
de adyacencia ocupan O(n + m): unos 80 MB para 8 millones de amistades.

Mantenerlas **ordenadas** cuesta O(grado) al insertar, pero a cambio:

- `sonAmigos()` → búsqueda binaria, O(log grado)
- `amigosEnComun()` → intersección con dos punteros, O(gA + gB) y **cero memoria auxiliar**
- `eliminarAmigo()` → localización en O(log grado)

Durante la carga masiva se usa el camino rápido: agregar todo al final (O(1) cada
arista) y ordenar una sola vez al terminar.

### 4.3 BFS bidireccional

En vez de una búsqueda desde el origen, se lanzan dos a la vez —una desde cada
extremo— y se para cuando se encuentran. Si el grado medio es _g_ y la distancia
_d_, un BFS normal visita ~_g^d_ nodos; dos búsquedas de profundidad _d/2_ visitan
~2·_g^(d/2)_.

En cada paso se expande el frente **más pequeño**, lo que evita quedar atrapado
explorando el vecindario de un usuario con miles de amigos. Se expande el nivel
**completo** antes de devolver el encuentro, y entre todos los encuentros de ese
nivel se elige el de menor longitud total: parar en el primer contacto podría dar
un camino con un salto de más.

Medido a 1 000 000 de usuarios: **95 µs frente a 7 854 µs**, unas **82 veces más
rápido**. Hay una prueba automática que verifica, sobre 300 consultas en un grafo
de 20 000 nodos, que ambos BFS devuelven caminos de la **misma longitud** y que
cada paso del camino es una amistad real.

### 4.4 Borrado lógico e índices en vez de punteros

El grafo referencia a los usuarios por su **índice** en el arreglo. Si al eliminar
un usuario se compactara el arreglo, habría que renumerar millones de aristas. Por
eso el borrado es **lógico**: se marca `activo = false`, se desconecta del grafo, se
retira de todos los índices y su posición pasa a una lista de huecos libres que
reutilizará el próximo registro.

Por el mismo motivo se guardan índices enteros y no punteros: cuando un arreglo
crece traslada sus datos a un bloque nuevo e invalidaría cualquier puntero
guardado. Además un índice ocupa 4 bytes en lugar de 8.

---

## 5. Datos: modelo Barabási-Albert

`src/datos/GeneradorSintetico.cpp`. Si las amistades se sortearan uniformemente,
todos tendrían aproximadamente el mismo número de amigos. Las redes reales siguen
una **ley de potencias**: casi todos tienen pocos amigos y unos pocos "hubs" tienen
miles.

Barabási-Albert reproduce eso con una regla simple: cada usuario nuevo elige a sus
_m_ amigos con probabilidad proporcional a los amigos que ya tiene cada candidato.
Para hacerlo en O(1) por sorteo se usa una **bolsa de repeticiones**: un arreglo
donde cada nodo aparece tantas veces como amigos tiene, del que basta sacar un
elemento al azar.

Resultado real a 1 000 000 de usuarios: grado promedio **16**, grado máximo
**4 263**. Ese hub es precisamente el peor caso para los algoritmos, lo que hace
que la demostración sea honesta.

El generador es **reproducible**: con la misma `--semilla` se obtienen exactamente
los mismos datos en cualquier máquina.

---

## 6. Resultados del análisis de rendimiento

`./redsocial --banco --maximo 1000000` (los resultados se exportan a
`benchmark.csv` para graficarlos). Tiempo **por operación**, en microsegundos:

| Operación                       | 10 000   | 100 000   | 1 000 000   | Comportamiento                           |
| ------------------------------- | -------- | --------- | ----------- | ---------------------------------------- |
| Consultar TablaHash             | 0.0039   | 0.0055    | 0.0157      | **O(1)** (el leve aumento es caché)      |
| Consultar ArbolAVL              | 0.0535   | 0.0828    | 0.1597      | **O(log n)**                             |
| Construir TablaHash (por clave) | 0.0081   | 0.0058    | 0.0115      | O(1) amortizado                          |
| Construir ArbolAVL (por clave)  | 0.1352   | 0.1719    | 0.3437      | O(log n)                                 |
| Buscar por ID                   | 0.0119   | 0.0106    | 0.0433      | O(1)                                     |
| Comprobar amistad (binaria)     | 0.0289   | 0.0546    | 0.0655      | O(log grado)                             |
| Amigos en común                 | 0.1381   | 0.1911    | 0.2107      | O(gA + gB)                               |
| **Camino BFS bidireccional**    | **1.92** | **7.42**  | **95.3**    |                                          |
| **Camino BFS clásico**          | **34.9** | **425.9** | **7 854.0** | **82× más lento**                        |
| Sugerencias de amistad          | 3.69     | 4.82      | 14.8        |                                          |
| Quicksort propio                | 0.0261   | 0.0326    | 0.0394      | O(n log n)                               |
| Mergesort propio                | 0.0385   | 0.0460    | 0.0579      | O(n log n), ~1.5× más lento pero estable |

Construcción completa de la red de 1 000 000 de usuarios + 8 000 000 de amistades +
2 000 000 de publicaciones: **2.7 segundos**.

La comparación TablaHash/ArbolAVL se hace llenando **ambas estructuras con
exactamente las mismas claves** y consultándolas con la misma secuencia, de modo
que lo único que se mide es la estructura en sí.

---

## 7. Pruebas automáticas

```bash
make probar
```

125 comprobaciones sin ningún framework externo (`pruebas/pruebas.cpp`), agrupadas
por estructura. Las más relevantes:

- **Tabla hash:** tras borrar la mitad de 50 000 claves, el resto sigue siendo
  accesible (validación de las tumbas).
- **AVL:** insertar 10 000 claves consecutivas —el peor caso de un ABB sin
  balancear— mantiene la altura ≤ 20, y sigue balanceado tras eliminar la mitad.
- **BFS:** en 300 consultas sobre un grafo de 20 000 nodos, el bidireccional y el
  clásico coinciden en la longitud del camino mínimo, y cada paso del camino
  devuelto es una amistad real.
- **RedSocial:** al eliminar un usuario desaparece de los cuatro índices, sus
  amigos pierden esa amistad, se borran sus publicaciones y su posición se
  reutiliza sin heredar las amistades del anterior.
- **Generador:** el grafo es simétrico, las listas están ordenadas y sin
  duplicados, los contadores coinciden con el grado y existen hubs con grado muy
  superior al promedio.

---

## 8. Organización del código

```
src/
├── estructuras/     Estructuras de datos genéricas (sin dependencias del dominio)
│   ├── Utilidades.h      mover, intercambiar, comparadores
│   ├── Cadena.h/.cpp     cadena de texto propia
│   ├── Arreglo.h         arreglo dinámico
│   ├── ListaEnlazada.h   lista simplemente enlazada
│   ├── Pila.h            pila LIFO
│   ├── Cola.h            cola FIFO circular
│   ├── TablaHash.h       hash con direccionamiento abierto
│   ├── MonticuloMinimo.h montículo binario / cola de prioridad
│   ├── ArbolAVL.h        árbol de búsqueda autobalanceado
│   ├── Trie.h/.cpp       árbol de prefijos
│   └── Ordenamiento.h    quicksort, mergesort, inserción
├── modelo/          Entidades del dominio
│   ├── Fecha.h/.cpp, Usuario.h, Publicacion.h, Comentario.h
├── sistema/         Lógica del sistema
│   ├── GrafoAmistades.h/.cpp   grafo, BFS, intersección, sugerencias
│   └── RedSocial.h/.cpp        fachada con las 13 funcionalidades
├── datos/           Generación y persistencia
│   ├── GeneradorAleatorio.h    xorshift64* propio
│   ├── GeneradorSintetico.*    modelo Barabási-Albert
│   └── ArchivoCSV.h/.cpp       lectura y escritura CSV propias
├── analisis/        Medición
│   ├── Cronometro.h            <chrono>
│   └── BancoPruebas.h/.cpp     batería de mediciones + export CSV
├── interfaz/
│   └── MenuConsola.h/.cpp      menú de texto (única parte que hace E/S)
└── main.cpp                    argumentos y arranque
pruebas/pruebas.cpp             suite de pruebas propia
```

La interfaz está aislada a propósito: el banco de pruebas y las pruebas automáticas
ejecutan exactamente el mismo código que el menú, sin ninguna dependencia de la
entrada y salida.
