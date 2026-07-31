# Algoritmos y Estructuras de Datos Proyecto Final

Percy Maldonado Q. pmaldonado@unsa.edu.pe

30 de julio de 2026

El objetivo del presente proyecto consiste en diseñar e implementar desde cero el núcleo de funcionamiento de una red social similar a Facebook utilizando únicamente estructuras de datos implementadas por el estudiante. El sistema deberá ser capaz de administrar millones de usuarios, relaciones de amistad, publicaciones e interacciones, priorizando eficiencia computacional, escalabilidad y análisis de rendimiento. No se evaluará el aspecto visual del sistema, sino el correcto diseño de las estructuras de datos, la eficiencia de los algoritmos implementados y la capacidad del sistema para procesar grandes volúmenes de información.

## 1. Descripción

Una red social moderna administra millones de usuarios conectados entre sí mediante relaciones de amistad. Cada usuario genera publicaciones, comenta contenido, reacciona a publicaciones y mantiene una red de conexiones que cambia constantemente.

En este proyecto los estudiantes deberán implementar una versión simplificada del backend de una red social, desarrollando todas las estructuras de datos desde cero. El sistema deberá soportar un conjunto de datos compuesto por cientos de miles o millones de usuarios. Dichos datos podrán obtenerse mediante una fuente pública (Kaggle, SNAP Stanford, Network Repository, etc.) o generarse sintéticamente mediante un algoritmo propio.

Todo el procesamiento deberá realizarse utilizando estructuras desarrolladas por el estudiante.

## 2. Restricciones

Durante el desarrollo del proyecto NO está permitido utilizar estructuras de datos de la STL de C++.

Se permite utilizar bibliotecas únicamente para:

- Lectura de archivos CSV.
- Lectura de archivos JSON.
- Interfaz gráfica (opcional)., No realices una interfaz grafica
- Medición de tiempos.
- Generación de gráficos estadísticos.

No está permitido utilizar:
- vector, list, map, unordered_map, set, queue, etc.

Todas las estructuras deberán implementarse completamente desde cero.

## 3. Arquitectura del Sistema

El sistema deberá almacenar como mínimo la siguiente información.

- Cada usuario deberá contener:
  - ID único, Nombre, Correo electrónico, Fecha de registro, Lista de amigos, Publicaciones realizadas, Cantidad de seguidores, Cantidad de amigos, Reacciones recibidas.

- Cada publicación deberá contener:
  - ID, Usuario propietario, Fecha, Texto, Número de likes, Número de comentarios, Lista de comentarios.

## 4. Funcionalidades

Como mínimo el sistema deberá permitir:

- Registrar usuarios.
- Eliminar usuarios.
- Buscar usuarios.
- Crear publicaciones.
- Eliminar publicaciones.
- Agregar amigos.
- Eliminar amigos.
- Buscar el camino de amistad entre dos usuarios.
- Mostrar amigos en común.
- Calcular sugerencias de amistad.
- Mostrar publicaciones de un usuario.
- Mostrar los usuarios más activos.
- Mostrar las publicaciones con mayor cantidad de reacciones.

Todas estas operaciones deberán estar implementadas utilizando las estructuras desarrolladas por el estudiante.

## 5. Consideraciones Finales

- El proyecto deberá desarrollarse íntegramente en C++.
- Todo el código fuente deberá encontrarse documentado y organizado en módulos.
- Durante la sustentación, cualquier integrante podrá ser consultado sobre cualquier parte del código desarrollado.
