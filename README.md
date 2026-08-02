# Red Social — Proyecto Final de Algoritmos y Estructuras de Datos

Implementamos el núcleo (backend) de una red social tipo Facebook **íntegramente en
C++ y sin usar ninguna estructura de datos de la STL**. Escribimos desde cero las
once estructuras que sostienen el sistema: cadena de texto, arreglo dinámico, lista
enlazada, pila, cola, tabla hash, montículo mínimo, árbol AVL, trie, grafo y los
algoritmos de ordenamiento.

Lo probamos con **1 000 000 de usuarios, 8 000 000 de amistades y 2 000 000 de
publicaciones**: el sistema los construye en **4,7 segundos** y ocupa **1,03 GB**.
Responde una búsqueda por ID en 0,02 µs y encuentra el camino de amistad entre dos
usuarios cualesquiera en 134 µs.

**Universidad Nacional de San Agustín** · Javier · Lizzy · Albert

---

## 1. Compilar y ejecutar

### Linux, macOS o WSL

```bash
make                # compila redsocial y pruebas_aed
make probar         # ejecuta las 131 pruebas automáticas
make ejecutar       # lanza el sistema con 100 000 usuarios
make banco          # mide el rendimiento hasta 1 000 000
make limpiar        # borra los binarios y los objetos
```

### Windows 11

El Makefile detecta el sistema y se adapta solo. Con **MSYS2, MinGW64 o Git Bash**
las órdenes son idénticas a las de Linux. Desde **cmd.exe o PowerShell** cambia el
nombre del programa:

```bat
mingw32-make
mingw32-make probar
```

Enlazamos estáticamente en Windows (`-static-libgcc -static-libstdc++`), así que el
`.exe` funciona en cualquier máquina sin necesidad de instalar las DLL de MinGW.
Ejecuta `make entorno` si quieres ver qué detectó.

### Con CMake, en cualquier sistema

```bash
cmake -B build && cmake --build build -j
```

### Opciones de línea de comandos

| Opción              | Qué hace                                                            |
| ------------------- | ------------------------------------------------------------------- |
| `--usuarios N`      | Genera N usuarios (por defecto 100 000)                             |
| `--amistades M`     | Amistades por usuario nuevo, modelo Barabási-Albert (por defecto 8) |
| `--publicaciones P` | Genera P publicaciones (por defecto `2*N`)                          |
| `--semilla S`       | Fija la semilla: los datos salen idénticos en cualquier máquina      |
| `--vacia`           | Arranca sin datos, para registrar usuarios a mano o importar CSV    |
| `--banco`           | Ejecuta solo el análisis de rendimiento y termina                   |
| `--maximo N`        | Limita la escala máxima del banco de pruebas                        |
| `--ayuda`           | Muestra esta lista                                                  |

La demostración a escala real:

```bash
./redsocial --usuarios 1000000
```

---

## 2. Las estructuras que escribimos

Todas viven en `src/estructuras/` y ninguna depende del dominio: no saben qué es un
usuario ni una publicación.

| Estructura               | Archivo                         | Dónde la usamos                  | Por qué elegimos esa                                                                                          |
| ------------------------ | ------------------------------- | -------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| **String**               | `String.h/.cpp`                 | Nombres, correos, textos         | Reemplaza a `std::string`. La dotamos de semántica de movimiento para no copiar millones de textos            |
| **Arreglo\<T\>**         | `Arreglo.h`                     | Base de todo lo demás            | Reemplaza a `std::vector`. Crece al doble, lo que da O(1) amortizado                                          |
| **ListaEnlazada\<T\>**   | `ListaEnlazada.h`               | Comentarios de cada publicación  | Cada publicación tiene pocos comentarios y siempre se agregan al final: no desperdicia capacidad × 2 000 000  |
| **Pila\<T\>**            | `Pila.h`                        | Reconstruir el camino de amistad | El BFS descubre el camino al revés; apilar y desapilar lo invierte sin código extra                           |
| **Cola\<T\>**            | `Cola.h`                        | Recorrido BFS                    | La hicimos **circular** sobre un arreglo: contigua en memoria, sin un `new` por elemento                      |
| **TablaHash\<K,V\>**     | `TablaHash.h`                   | id→índice, correo→índice         | Direccionamiento abierto con sondeo lineal y **tumbas**. Ver §4.1                                             |
| **MonticuloMinimo\<T\>** | `MonticuloMinimo.h`             | Rankings top-K                   | Filtra K elementos en O(n log K) en vez de ordenar todo en O(n log n)                                         |
| **ArbolAVL\<K,V\>**      | `ArbolAVL.h`                    | Índice de nombres                | Es el único que mantiene el **orden**, y por eso sostiene el listado alfabético                               |
| **Trie**                 | `Trie.h/.cpp`                   | Búsqueda por prefijo             | Es el único que responde «todos los que empiezan por MAR» en O(longitud del prefijo)                          |
| **GrafoAmistades**       | `sistema/GrafoAmistades.h/.cpp` | Amistades                        | Listas de adyacencia **ordenadas**. Ver §4.2                                                                  |
| **Ordenamiento**         | `Ordenamiento.h`                | Listas de adyacencia, rankings   | Quicksort con mediana de tres y mergesort estable, ambos propios                                              |

En `Utilidades.h` escribimos también `mover()` (nuestro `std::move`),
`intercambiar()`, `minimo()`, `maximo()` y los comparadores, de modo que el proyecto
no incluye `<utility>` ni `<algorithm>`.

**Del sistema usamos únicamente** `<cstdio>`, `<cstdlib>`, `<cstring>`,
`<iostream>`/`<ostream>`/`<iosfwd>` para la consola y `<chrono>` para medir tiempos.

---

## 3. Dónde resolvemos cada funcionalidad exigida

Las trece viven en la fachada `RedSocial` (`src/sistema/RedSocial.h/.cpp`), el único
punto por el que pasa la lógica del sistema.

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

Añadimos además el listado alfabético (recorrido en orden del AVL), exportación e
importación CSV, estadísticas del sistema y el banco de pruebas.

---

## 4. Las cuatro decisiones que sabemos defender

### 4.1 Tabla hash con direccionamiento abierto y tumbas

Con encadenamiento, cada usuario necesitaría un nodo con puntero: un millón de
llamadas a `new` y un salto de memoria por consulta. Con direccionamiento abierto
guardamos todo en un único arreglo contiguo y resolvemos las colisiones mirando la
casilla siguiente, que el procesador ya trajo a la caché.

Aquí está el punto delicado: **al borrar no podemos dejar la casilla vacía**, porque
cortaríamos la cadena de sondeo y volveríamos invisibles las claves que colisionaron
después. Por eso la marcamos como `BORRADA` —una «tumba»—: la búsqueda la atraviesa,
la inserción la reutiliza y la redimensión las elimina todas. Escribimos una prueba
automática dedicada exactamente a este caso.

Usamos capacidad potencia de dos, así que reducimos el hash al rango de la tabla con
una máscara de bits en lugar de una división, que es mucho más costosa.

### 4.2 Listas de adyacencia ordenadas

Una matriz de adyacencia para un millón de usuarios necesitaría 125 GB. Las listas
de adyacencia ocupan O(n + m): unos 80 MB para ocho millones de amistades.

Mantenerlas **ordenadas** nos cuesta O(grado) al insertar, pero a cambio ganamos:

- `sonAmigos()` → búsqueda binaria sobre la lista más corta, O(log grado)
- `amigosEnComun()` → intersección con dos punteros, O(gA + gB) y **cero memoria auxiliar**
- `eliminarAmigo()` → localiza la posición en O(log grado)

Durante la carga masiva tomamos el camino rápido: agregamos todas las aristas al
final en O(1) y ordenamos una sola vez al terminar.

### 4.3 BFS bidireccional

En vez de una búsqueda desde el origen, lanzamos dos a la vez —una desde cada
extremo— y paramos cuando se encuentran. Si el grado medio es *g* y la distancia
*d*, un BFS normal visita ~*g^d* nodos; dos búsquedas de profundidad *d/2* visitan
~2·*g^(d/2)*.

Dos detalles hacen que funcione:

1. **Expandimos siempre el frente más pequeño**, lo que nos evita quedar atrapados
   explorando el vecindario de un usuario con miles de amigos.
2. **Terminamos el nivel completo antes de devolver** y, entre todos los encuentros
   de ese nivel, elegimos el de menor longitud total. Parar en el primer contacto
   devolvería a veces un camino con un salto de más: es el error habitual de esta
   técnica.

Medido con 1 000 000 de usuarios: **134 µs frente a 9 560 µs**, unas **71 veces más
rápido**. Y lo verificamos: una prueba automática comprueba, sobre 300 consultas en
un grafo de 20 000 nodos, que ambos BFS devuelven caminos de la **misma longitud** y
que cada paso del camino es una amistad real.

### 4.4 Borrado lógico e índices en vez de punteros

El grafo referencia a los usuarios por su **índice** en el arreglo. Si al eliminar
un usuario compactáramos el arreglo, tendríamos que renumerar millones de aristas.
Por eso el borrado es **lógico**: marcamos `activo = false`, desconectamos el nodo
del grafo, lo retiramos de los cuatro índices y su posición pasa a una lista de
huecos que reutilizará el próximo registro.

Por el mismo motivo guardamos índices enteros y no punteros: cuando un arreglo crece
traslada sus datos a un bloque nuevo e invalidaría cualquier puntero guardado.
Además, un índice ocupa 4 bytes en lugar de 8.

---

## 5. Cómo generamos los datos

`src/datos/GeneradorSintetico.cpp`

### Amistades: modelo Barabási-Albert

Si sorteáramos las amistades uniformemente, todos tendrían aproximadamente el mismo
número de amigos. Las redes reales siguen una **ley de potencias**: casi todos
tienen pocos amigos y unos pocos *hubs* tienen miles.

Barabási-Albert lo reproduce con una regla simple: cada usuario nuevo elige a sus
*m* amigos con probabilidad proporcional a los amigos que ya tiene cada candidato.
Para sortear en O(1) usamos una **bolsa de repeticiones**: un arreglo donde cada
nodo aparece tantas veces como amigos tiene, del que basta sacar un elemento al
azar.

Con 1 000 000 de usuarios obtenemos un grado promedio de **16** y un grado máximo de
**4 852**. Ese hub es justamente el peor caso para nuestros algoritmos, y por eso la
demostración es honesta.

### Nombres: todos distintos, sin comprobar nada

Cada usuario recibe un nombre completo **único** (nombre de pila más apellido
paterno y materno). Con catálogos de 128 nombres y 128 apellidos disponemos de
2 097 152 combinaciones.

No llevamos un registro de las combinaciones usadas —eso costaría memoria y
reintentos cada vez más lentos—. En su lugar recorremos el espacio completo con un
**paso coprimo** con el total: como `mcd(paso, total) = 1`, la función
`i → (desplazamiento + i·paso) mod total` es una biyección, de modo que dos usuarios
distintos jamás caen en la misma combinación. Cuesta O(1) por usuario y no gasta
memoria adicional.

Si pides más usuarios que combinaciones disponibles, el programa te lo dice y no
genera nada, en lugar de repetir nombres en silencio.

El generador es **reproducible**: con la misma `--semilla` obtienes exactamente los
mismos datos en cualquier máquina.

---

## 6. Resultados del análisis de rendimiento

```bash
./redsocial --banco --maximo 1000000     # exporta benchmark.csv
```

Tiempo **por operación**, en microsegundos:

| Operación                       | 10 000    | 100 000   | 1 000 000   | Comportamiento                            |
| ------------------------------- | --------- | --------- | ----------- | ----------------------------------------- |
| Consultar TablaHash             | 0.0044    | 0.0058    | 0.0201      | **O(1)** (el leve aumento es la caché)    |
| Consultar ArbolAVL              | 0.0660    | 0.0830    | 0.1941      | **O(log n)**                              |
| Buscar por prefijo (Trie)       | 0.1745    | 0.1523    | 0.1682      | **No depende de n**                       |
| Construir TablaHash (por clave) | 0.0100    | 0.0066    | 0.0181      | O(1) amortizado                           |
| Construir ArbolAVL (por clave)  | 0.1517    | 0.1879    | 0.4301      | O(log n)                                  |
| Buscar por ID                   | 0.0132    | 0.0156    | 0.0617      | O(1)                                      |
| Comprobar amistad (binaria)     | 0.0324    | 0.0474    | 0.0906      | O(log grado)                              |
| Amigos en común                 | 0.1445    | 0.1431    | 0.2641      | O(gA + gB)                                |
| **Camino BFS bidireccional**    | **2.80**  | **6.86**  | **134.0**   |                                           |
| **Camino BFS clásico**          | **47.53** | **394.5** | **9 559.7** | **71× más lento**                         |
| Sugerencias de amistad          | 3.79      | 3.35      | 13.15       |                                           |
| Quicksort propio                | 0.0389    | 0.0335    | 0.0405      | O(n log n)                                |
| Mergesort propio                | 0.0564    | 0.0468    | 0.0600      | O(n log n), ~1,5× más lento pero estable  |

Tres lecturas que conviene destacar:

- **El trie es la única estructura cuyo tiempo no depende del tamaño del sistema.**
  Pasó de 0,174 a 0,168 µs mientras multiplicábamos n por cien.
- **La ventaja del BFS bidireccional crece con la escala**: de 17× con 10 000
  usuarios a 71× con un millón. Ambos algoritmos comparten la misma cota O(n + m);
  lo que cambia es cuántos nodos exploran de verdad.
- **El crecimiento de la tabla hash no contradice su O(1).** El número de sondeos
  por consulta no cambia; lo que cambia es que con un millón de entradas la tabla
  deja de caber en la caché del procesador.

Comparamos TablaHash y ArbolAVL llenando **ambas con exactamente las mismas claves**
y consultándolas con la misma secuencia, de modo que lo único que medimos es la
estructura en sí.

---

## 7. Pruebas automáticas

```bash
make probar
```

Escribimos 131 comprobaciones sin ningún framework externo (`pruebas/pruebas.cpp`),
agrupadas por estructura. Las más relevantes:

- **Tabla hash:** tras borrar la mitad de 50 000 claves, el resto sigue accesible.
  Es la validación de las tumbas.
- **AVL:** insertamos 10 000 claves consecutivas —el peor caso de un ABB sin
  balancear— y la altura se mantiene ≤ 20; sigue balanceado tras eliminar la mitad.
- **BFS:** en 300 consultas sobre un grafo de 20 000 nodos, el bidireccional y el
  clásico coinciden en la longitud del camino mínimo, y cada paso del camino
  devuelto es una amistad real.
- **RedSocial:** al eliminar un usuario desaparece de los cuatro índices, sus amigos
  pierden esa amistad, se borran sus publicaciones y su posición se reutiliza sin
  heredar las amistades del anterior.
- **Generador:** los 30 000 nombres generados son todos distintos, el grafo es
  simétrico, las listas están ordenadas y sin duplicados, los contadores coinciden
  con el grado real y existen hubs muy por encima del promedio.

También ejecutamos la batería bajo *AddressSanitizer* y *UndefinedBehaviorSanitizer*
sin ningún diagnóstico.

---

## 8. Organización del código

```
src/
├── estructuras/     Estructuras genéricas, sin dependencias del dominio
│   ├── Utilidades.h      mover, intercambiar, comparadores
│   ├── String.h/.cpp     cadena de texto propia
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
│   └── Fecha.h/.cpp, Usuario.h, Publicacion.h, Comentario.h
├── sistema/         Lógica del sistema
│   ├── GrafoAmistades.h/.cpp   grafo, BFS, intersección, sugerencias
│   └── RedSocial.h/.cpp        fachada con las 13 funcionalidades
├── datos/           Generación y persistencia
│   ├── GeneradorAleatorio.h    xorshift64* propio
│   ├── GeneradorSintetico.*    modelo Barabási-Albert
│   └── ArchivoCSV.h/.cpp       lectura y escritura CSV propias
├── analisis/        Medición
│   ├── Cronometro.h            <chrono>
│   └── BancoPruebas.h/.cpp     batería de mediciones + exportación CSV
├── interfaz/
│   └── MenuConsola.h/.cpp      menú de texto, única parte que hace E/S
└── main.cpp                    argumentos y arranque

pruebas/pruebas.cpp             nuestra suite de pruebas
```

Aislamos la interfaz a propósito: el banco de pruebas y las pruebas automáticas
ejecutan exactamente el mismo código que el menú, sin ninguna dependencia de la
entrada y salida.
