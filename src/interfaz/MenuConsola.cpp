#include "MenuConsola.h"

#include <cstdio>
#include <iostream>

#include "../analisis/BancoPruebas.h"
#include "../analisis/Cronometro.h"
#include "../datos/ArchivoCSV.h"

namespace aed {

namespace {

void linea() {
    std::printf("  ------------------------------------------------------------\n");
}



Fecha fechaDeHoy() {
    return Fecha(2026, 7, 31);
}

}

MenuConsola::MenuConsola(RedSocial& red) : _red(red), _salir(false), _entradaCerrada(false) {}




void MenuConsola::ejecutar() {
    while (!_salir && !_entradaCerrada) {
        mostrarMenu();
        int opcion = leerEntero("  Opcion: ");
        if (_entradaCerrada) break;
        std::printf("\n");

        switch (opcion) {
            case 1:  registrarUsuario();              break;
            case 2:  eliminarUsuario();               break;
            case 3:  buscarUsuario();                 break;
            case 4:  listarAlfabeticamente();         break;
            case 5:  agregarAmigo();                  break;
            case 6:  eliminarAmigo();                 break;
            case 7:  verAmigos();                     break;
            case 8:  caminoDeAmistad();               break;
            case 9:  amigosEnComun();                 break;
            case 10: sugerenciasDeAmistad();          break;
            case 11: crearPublicacion();              break;
            case 12: eliminarPublicacion();           break;
            case 13: verPublicacionesDeUsuario();     break;
            case 14: interactuarConPublicacion();     break;
            case 15: usuariosMasActivos();            break;
            case 16: publicacionesConMasReacciones(); break;
            case 17: mostrarEstadisticas();           break;
            case 18: ejecutarBancoDePruebas();        break;
            case 19: exportarDatos();                 break;
            case 20: importarDatos();                 break;
            case 0:  _salir = true;                   break;
            default: std::printf("  Opcion no valida.\n");
        }
        std::printf("\n");
    }
    std::printf("  Fin del programa.\n");
}

void MenuConsola::mostrarMenu() const {
    std::printf("============================================================\n");
    std::printf("  RED SOCIAL - Proyecto Final de Algoritmos y Estructuras\n");
    std::printf("  Usuarios activos: %d   |   Publicaciones activas: %d\n",
                _red.cantidadUsuariosActivos(), _red.cantidadPublicacionesActivas());
    std::printf("============================================================\n");
    std::printf("   USUARIOS                       AMISTADES\n");
    std::printf("    1. Registrar usuario            5. Agregar amigo\n");
    std::printf("    2. Eliminar usuario             6. Eliminar amigo\n");
    std::printf("    3. Buscar usuario               7. Ver amigos\n");
    std::printf("    4. Listar alfabeticamente       8. Camino de amistad\n");
    std::printf("                                    9. Amigos en comun\n");
    std::printf("                                   10. Sugerencias de amistad\n");
    std::printf("\n");
    std::printf("   PUBLICACIONES                  RANKINGS Y ANALISIS\n");
    std::printf("   11. Crear publicacion          15. Usuarios mas activos\n");
    std::printf("   12. Eliminar publicacion       16. Publicaciones top\n");
    std::printf("   13. Ver publicaciones          17. Estadisticas del sistema\n");
    std::printf("   14. Reaccionar / comentar      18. Banco de pruebas\n");
    std::printf("                                  19. Exportar datos a CSV\n");
    std::printf("                                  20. Importar datos desde CSV\n");
    std::printf("\n    0. Salir\n");
    std::printf("============================================================\n");
}




void MenuConsola::registrarUsuario() {
    String nombre = leerTexto("  Nombre completo: ");
    String correo = leerTexto("  Correo electronico: ");
    if (nombre.vacia() || correo.vacia()) {
        std::printf("  El nombre y el correo son obligatorios.\n");
        return;
    }

    int id = _red.registrarUsuario(nombre, correo, fechaDeHoy());
    if (id < 0) {
        std::printf("  No se pudo registrar: el correo ya esta en uso.\n");
        return;
    }
    std::printf("  Usuario registrado con ID %d.\n", id);
}

void MenuConsola::eliminarUsuario() {
    int id = leerEntero("  ID del usuario a eliminar: ");
    if (_red.eliminarUsuario(id)) {
        std::printf("  Usuario %d eliminado (junto con sus amistades y publicaciones).\n", id);
    } else {
        std::printf("  No existe un usuario activo con ese ID.\n");
    }
}

void MenuConsola::buscarUsuario() {
    std::printf("  1. Por ID (TablaHash)        2. Por correo (TablaHash)\n");
    std::printf("  3. Por nombre exacto (AVL)   4. Por prefijo del nombre (Trie)\n");
    int modo = leerEntero("  Tipo de busqueda: ");

    Cronometro reloj;

    if (modo == 1) {
        int id = leerEntero("  ID: ");
        reloj.iniciar();
        Usuario* usuario = _red.buscarPorId(id);
        double ms = reloj.milisegundos();
        if (usuario == nullptr) {
            std::printf("  No encontrado.\n");
        } else {
            mostrarFichaDeUsuario(_red.indiceDeUsuario(id));
            std::printf("  (busqueda en %.4f ms)\n", ms);
        }
        return;
    }

    if (modo == 2) {
        String correo = leerTexto("  Correo: ");
        reloj.iniciar();
        Usuario* usuario = _red.buscarPorCorreo(correo);
        double ms = reloj.milisegundos();
        if (usuario == nullptr) {
            std::printf("  No encontrado.\n");
        } else {
            mostrarFichaDeUsuario(_red.indiceDeUsuario(usuario->id));
            std::printf("  (busqueda en %.4f ms)\n", ms);
        }
        return;
    }

    Arreglo<int> indices;
    if (modo == 3) {
        String nombre = leerTexto("  Nombre exacto: ");
        reloj.iniciar();
        _red.buscarPorNombre(nombre, indices);
    } else if (modo == 4) {
        String prefijo = leerTexto("  Prefijo: ");
        reloj.iniciar();
        _red.buscarPorPrefijo(prefijo, 20, indices);
    } else {
        std::printf("  Opcion no valida.\n");
        return;
    }
    double ms = reloj.milisegundos();

    if (indices.vacio()) {
        std::printf("  Sin coincidencias.\n");
        return;
    }
    std::printf("  %d coincidencia(s) en %.4f ms:\n", indices.tamanio(), ms);
    linea();
    int mostrados = indices.tamanio() < 20 ? indices.tamanio() : 20;
    for (int i = 0; i < mostrados; ++i) mostrarLineaDeUsuario(indices[i]);
}

void MenuConsola::listarAlfabeticamente() {
    int limite = leerEntero("  Cuantos usuarios mostrar: ");
    if (limite <= 0) limite = 10;

    Arreglo<int> indices;
    _red.listarEnOrdenAlfabetico(limite, indices);

    std::printf("  Recorrido EN ORDEN del arbol AVL de nombres:\n");
    linea();
    for (int i = 0; i < indices.tamanio(); ++i) mostrarLineaDeUsuario(indices[i]);
}




void MenuConsola::agregarAmigo() {
    int idA = leerEntero("  ID del primer usuario: ");
    int idB = leerEntero("  ID del segundo usuario: ");
    if (_red.agregarAmigo(idA, idB)) {
        std::printf("  Amistad creada entre %d y %d.\n", idA, idB);
    } else {
        std::printf("  No se pudo crear (ya eran amigos, o algun ID no existe).\n");
    }
}

void MenuConsola::eliminarAmigo() {
    int idA = leerEntero("  ID del primer usuario: ");
    int idB = leerEntero("  ID del segundo usuario: ");
    if (_red.eliminarAmigo(idA, idB)) {
        std::printf("  Amistad eliminada.\n");
    } else {
        std::printf("  Esos usuarios no eran amigos.\n");
    }
}

void MenuConsola::verAmigos() {
    int id = leerEntero("  ID del usuario: ");
    int indice = _red.indiceDeUsuario(id);
    if (indice < 0) {
        std::printf("  No existe ese usuario.\n");
        return;
    }

    Arreglo<int> amigos;
    _red.amigosDe(id, amigos);
    std::printf("  %s tiene %d amigo(s).\n", _red.usuarioEnIndice(indice).nombre.texto(),
                amigos.tamanio());

    int mostrados = amigos.tamanio() < 25 ? amigos.tamanio() : 25;
    linea();
    for (int i = 0; i < mostrados; ++i) mostrarLineaDeUsuario(amigos[i]);
    if (amigos.tamanio() > mostrados) {
        std::printf("  ... y %d mas.\n", amigos.tamanio() - mostrados);
    }
}

void MenuConsola::caminoDeAmistad() {
    int idA = leerEntero("  ID de origen: ");
    int idB = leerEntero("  ID de destino: ");

    Arreglo<int> camino;
    Cronometro reloj;
    bool existe = _red.caminoDeAmistad(idA, idB, camino);
    double ms = reloj.milisegundos();

    if (!existe) {
        std::printf("  No hay ningun camino de amistad entre esos usuarios.\n");
        return;
    }

    std::printf("  Camino de %d salto(s), hallado en %.3f ms con BFS bidireccional:\n",
                camino.tamanio() - 1, ms);
    linea();
    for (int i = 0; i < camino.tamanio(); ++i) {
        const Usuario& usuario = _red.usuarioEnIndice(camino[i]);
        std::printf("   %2d. %-28s (ID %d)\n", i + 1, usuario.nombre.texto(), usuario.id);
    }
}

void MenuConsola::amigosEnComun() {
    int idA = leerEntero("  ID del primer usuario: ");
    int idB = leerEntero("  ID del segundo usuario: ");

    Arreglo<int> comunes;
    Cronometro reloj;
    _red.amigosEnComun(idA, idB, comunes);
    double ms = reloj.milisegundos();

    std::printf("  %d amigo(s) en comun (interseccion en %.4f ms):\n", comunes.tamanio(), ms);
    linea();
    int mostrados = comunes.tamanio() < 25 ? comunes.tamanio() : 25;
    for (int i = 0; i < mostrados; ++i) mostrarLineaDeUsuario(comunes[i]);
    if (comunes.tamanio() > mostrados) {
        std::printf("  ... y %d mas.\n", comunes.tamanio() - mostrados);
    }
}

void MenuConsola::sugerenciasDeAmistad() {
    int id = leerEntero("  ID del usuario: ");
    int cantidad = leerEntero("  Cuantas sugerencias: ");
    if (cantidad <= 0) cantidad = 10;

    Arreglo<SugerenciaAmistad> sugerencias;
    Cronometro reloj;
    _red.sugerenciasDeAmistad(id, cantidad, sugerencias);
    double ms = reloj.milisegundos();

    if (sugerencias.vacio()) {
        std::printf("  No hay sugerencias disponibles.\n");
        return;
    }
    std::printf("  %d sugerencia(s) calculada(s) en %.3f ms:\n", sugerencias.tamanio(), ms);
    linea();
    for (int i = 0; i < sugerencias.tamanio(); ++i) {
        const Usuario& usuario = _red.usuarioEnIndice(sugerencias[i].indiceUsuario);
        std::printf("   %2d. %-28s (ID %6d)  amigos en comun: %d\n", i + 1,
                    usuario.nombre.texto(), usuario.id, sugerencias[i].amigosEnComun);
    }
}




void MenuConsola::crearPublicacion() {
    int id = leerEntero("  ID del autor: ");
    String texto = leerTexto("  Texto de la publicacion: ");
    if (texto.vacia()) {
        std::printf("  El texto no puede estar vacio.\n");
        return;
    }

    int idPublicacion = _red.crearPublicacion(id, texto, fechaDeHoy());
    if (idPublicacion < 0) {
        std::printf("  No se pudo crear: el autor no existe.\n");
        return;
    }
    std::printf("  Publicacion creada con ID %d.\n", idPublicacion);
}

void MenuConsola::eliminarPublicacion() {
    int id = leerEntero("  ID de la publicacion: ");
    if (_red.eliminarPublicacion(id)) {
        std::printf("  Publicacion eliminada.\n");
    } else {
        std::printf("  No existe una publicacion activa con ese ID.\n");
    }
}

void MenuConsola::verPublicacionesDeUsuario() {
    int id = leerEntero("  ID del usuario: ");
    int indice = _red.indiceDeUsuario(id);
    if (indice < 0) {
        std::printf("  No existe ese usuario.\n");
        return;
    }

    Arreglo<int> publicaciones;
    _red.publicacionesDe(id, publicaciones);

    std::printf("  %s tiene %d publicacion(es).\n", _red.usuarioEnIndice(indice).nombre.texto(),
                publicaciones.tamanio());
    linea();
    int mostradas = publicaciones.tamanio() < 15 ? publicaciones.tamanio() : 15;
    for (int i = 0; i < mostradas; ++i) mostrarPublicacion(publicaciones[i], false);
    if (publicaciones.tamanio() > mostradas) {
        std::printf("  ... y %d mas.\n", publicaciones.tamanio() - mostradas);
    }
}

void MenuConsola::interactuarConPublicacion() {
    int idPublicacion = leerEntero("  ID de la publicacion: ");
    Publicacion* publicacion = _red.buscarPublicacion(idPublicacion);
    if (publicacion == nullptr) {
        std::printf("  No existe esa publicacion.\n");
        return;
    }

    std::printf("  1. Dar like     2. Comentar\n");
    int accion = leerEntero("  Accion: ");

    if (accion == 1) {
        _red.darLike(idPublicacion);
        std::printf("  Like registrado. La publicacion tiene ahora %d likes.\n",
                    publicacion->numeroLikes);
        return;
    }
    if (accion == 2) {
        int idAutor = leerEntero("  ID del autor del comentario: ");
        String texto = leerTexto("  Comentario: ");
        int idComentario = _red.comentarPublicacion(idPublicacion, idAutor, texto, fechaDeHoy());
        if (idComentario < 0) {
            std::printf("  No se pudo comentar: el autor no existe.\n");
        } else {
            std::printf("  Comentario registrado con ID %d.\n", idComentario);
        }
        return;
    }
    std::printf("  Accion no valida.\n");
}




void MenuConsola::usuariosMasActivos() {
    int cantidad = leerEntero("  Cuantos usuarios en el ranking: ");
    if (cantidad <= 0) cantidad = 10;

    Arreglo<ElementoRanking> ranking;
    Cronometro reloj;
    _red.usuariosMasActivos(cantidad, ranking);
    double ms = reloj.milisegundos();

    std::printf("  Ranking calculado en %.2f ms sobre %d usuarios (monticulo de minimos).\n",
                ms, _red.cantidadUsuariosActivos());
    std::printf("  Puntaje = publicaciones*3 + comentarios*2 + reacciones recibidas.\n");
    linea();
    std::printf("   %-4s %-28s %10s %8s %8s %10s\n", "#", "NOMBRE", "PUNTAJE", "PUBL.",
                "AMIGOS", "REACC.");
    for (int i = 0; i < ranking.tamanio(); ++i) {
        const Usuario& usuario = _red.usuarioEnIndice(ranking[i].indice);
        std::printf("   %-4d %-28s %10lld %8d %8d %10lld\n", i + 1, usuario.nombre.texto(),
                    ranking[i].puntaje, usuario.cantidadPublicaciones(), usuario.cantidadAmigos,
                    usuario.reaccionesRecibidas);
    }
}

void MenuConsola::publicacionesConMasReacciones() {
    int cantidad = leerEntero("  Cuantas publicaciones en el ranking: ");
    if (cantidad <= 0) cantidad = 10;

    Arreglo<ElementoRanking> ranking;
    Cronometro reloj;
    _red.publicacionesConMasReacciones(cantidad, ranking);
    double ms = reloj.milisegundos();

    std::printf("  Ranking calculado en %.2f ms sobre %d publicaciones.\n", ms,
                _red.cantidadPublicacionesActivas());
    linea();
    for (int i = 0; i < ranking.tamanio(); ++i) {
        std::printf("   %2d.", i + 1);
        mostrarPublicacion(ranking[i].indice, false);
    }
}

void MenuConsola::mostrarEstadisticas() {
    const GrafoAmistades& grafo = _red.grafo();
    double megas = static_cast<double>(_red.memoriaAproximadaBytes()) / (1024.0 * 1024.0);

    std::printf("  ESTADO DEL SISTEMA\n");
    linea();
    std::printf("   Usuarios activos ............ %d\n", _red.cantidadUsuariosActivos());
    std::printf("   Posiciones ocupadas ......... %d\n", _red.cantidadPosicionesUsuario());
    std::printf("   Publicaciones activas ....... %d\n", _red.cantidadPublicacionesActivas());
    std::printf("\n  GRAFO DE AMISTADES\n");
    linea();
    std::printf("   Nodos ....................... %d\n", grafo.cantidadNodos());
    std::printf("   Aristas (amistades) ......... %lld\n", grafo.cantidadAristas());
    std::printf("   Grado promedio .............. %.2f\n", grafo.gradoPromedio());
    std::printf("   Grado maximo (usuario hub) .. %d\n", grafo.gradoMaximo());
    std::printf("\n  INDICES\n");
    linea();
    std::printf("   Nombres distintos (AVL) ..... %d\n", _red.nombresDistintos());
    std::printf("   Altura del AVL .............. %d\n", _red.alturaDelArbolDeNombres());
    std::printf("   Nodos del trie .............. %d\n", _red.nodosDelTrie());
    std::printf("\n   Memoria aproximada .......... %.1f MB\n", megas);
}

void MenuConsola::ejecutarBancoDePruebas() {
    std::printf("  1. Medir sobre la red actual\n");
    std::printf("  2. Serie completa con escalas crecientes (construye redes nuevas)\n");
    int modo = leerEntero("  Modo: ");

    Arreglo<ResultadoPrueba> resultados;
    if (modo == 2) {
        int maximo = leerEntero("  Maximo de usuarios por escala (ej. 1000000): ");
        if (maximo <= 0) maximo = 100000;
        BancoPruebas::ejecutarSerieDeEscalas(maximo, "benchmark.csv");
        return;
    }

    std::printf("  Midiendo...\n\n");
    BancoPruebas::medirOperaciones(_red, resultados);
    BancoPruebas::imprimirTabla(resultados);
    if (BancoPruebas::exportarCSV(resultados, "benchmark.csv")) {
        std::printf("\n  Resultados exportados a benchmark.csv\n");
    }
}

void MenuConsola::exportarDatos() {
    Cronometro reloj;
    bool bien = archivo::guardarUsuarios(_red, "usuarios.csv");
    bien = archivo::guardarAmistades(_red, "amistades.csv") && bien;
    bien = archivo::guardarPublicaciones(_red, "publicaciones.csv") && bien;

    if (bien) {
        std::printf("  Exportado en %.2f s: usuarios.csv, amistades.csv, publicaciones.csv\n",
                    reloj.segundos());
    } else {
        std::printf("  Hubo un problema al escribir los archivos.\n");
    }
}

void MenuConsola::importarDatos() {
    if (_red.cantidadUsuariosActivos() > 0) {
        std::printf("  La red ya tiene datos. Vuelve a lanzar el programa con --vacia\n");
        std::printf("  para importar sobre una red limpia.\n");
        return;
    }

    Cronometro reloj;


    if (!archivo::cargarUsuarios(_red, "usuarios.csv")) {
        std::printf("  No se pudo abrir usuarios.csv\n");
        return;
    }
    archivo::cargarAmistades(_red, "amistades.csv");
    archivo::cargarPublicaciones(_red, "publicaciones.csv");

    std::printf("  Importados en %.2f s: %d usuarios, %lld amistades, %d publicaciones.\n",
                reloj.segundos(), _red.cantidadUsuariosActivos(),
                _red.grafo().cantidadAristas(), _red.cantidadPublicacionesActivas());
}




void MenuConsola::mostrarFichaDeUsuario(int indice) const {
    if (indice < 0) return;
    const Usuario& usuario = _red.usuarioEnIndice(indice);

    linea();
    std::printf("   ID ................ %d\n", usuario.id);
    std::printf("   Nombre ............ %s\n", usuario.nombre.texto());
    std::printf("   Correo ............ %s\n", usuario.correo.texto());
    std::printf("   Fecha de registro . %s\n", usuario.fechaRegistro.aTexto().texto());
    std::printf("   Amigos ............ %d\n", usuario.cantidadAmigos);
    std::printf("   Seguidores ........ %d\n", usuario.cantidadSeguidores);
    std::printf("   Publicaciones ..... %d\n", usuario.cantidadPublicaciones());
    std::printf("   Comentarios hechos  %d\n", usuario.comentariosRealizados);
    std::printf("   Reacciones recib. . %lld\n", usuario.reaccionesRecibidas);
    linea();
}

void MenuConsola::mostrarLineaDeUsuario(int indice) const {
    const Usuario& usuario = _red.usuarioEnIndice(indice);
    std::printf("   ID %-8d %-28s amigos: %-6d publicaciones: %d\n", usuario.id,
                usuario.nombre.texto(), usuario.cantidadAmigos, usuario.cantidadPublicaciones());
}

void MenuConsola::mostrarPublicacion(int indice, bool conComentarios) const {
    const Publicacion& publicacion = _red.publicacionEnIndice(indice);
    int indiceAutor = _red.indiceDeUsuario(publicacion.idPropietario);
    const char* autor = (indiceAutor >= 0) ? _red.usuarioEnIndice(indiceAutor).nombre.texto()
                                           : "(usuario eliminado)";

    std::printf("   [ID %d] %s - %s\n", publicacion.id, autor,
                publicacion.fecha.aTexto().texto());
    std::printf("      \"%s\"\n", publicacion.texto.texto());
    std::printf("      %d likes, %d comentarios\n", publicacion.numeroLikes,
                publicacion.numeroComentarios());

    if (!conComentarios) return;
    const ListaEnlazada<Comentario>::Nodo* nodo = publicacion.comentarios.primero();
    while (nodo != nullptr) {
        std::printf("        - %s\n", nodo->valor.texto.texto());
        nodo = nodo->siguiente;
    }
}




String MenuConsola::leerTexto(const char* mensaje) {
    std::printf("%s", mensaje);
    std::fflush(stdout);

    char buffer[512];
    std::cin.getline(buffer, sizeof(buffer));

    if (std::cin.eof()) {
        _entradaCerrada = true;
        return String();
    }
    if (std::cin.fail()) {

        std::cin.clear();
        while (std::cin.get() != '\n' && !std::cin.eof()) {}
    }
    return String(buffer);
}

int MenuConsola::leerEntero(const char* mensaje) {
    return static_cast<int>(leerTexto(mensaje).aEntero());
}

}
