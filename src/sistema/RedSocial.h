// ============================================================================
//  RedSocial.h
//  Nucleo del sistema: coordina usuarios, publicaciones y el grafo de amistades.
//
//  ALMACENAMIENTO PRINCIPAL
//  ------------------------
//      _usuarios      -> Arreglo<Usuario>      (posicion = "indice interno")
//      _publicaciones -> Arreglo<Publicacion>
//  Todo lo demas son INDICES que apuntan a esas dos tablas:
//
//      _indicePorId       TablaHash   id de usuario  -> indice     O(1)
//      _indicePorCorreo   TablaHash   correo         -> indice     O(1)
//      _indicePorNombre   ArbolAVL    nombre         -> indices    O(log n) + orden alfabetico
//      _indicePrefijos    Trie        prefijo        -> indices    O(long. prefijo)
//      _grafo             Grafo       indice         -> amigos
//
//  Cada estructura resuelve una consulta distinta: la tabla hash es imbatible
//  para acceso exacto, pero no sabe ordenar; el AVL ordena pero es O(log n);
//  el trie es el unico capaz de responder "todos los que empiezan por MAR".
//
//  POR QUE INDICES Y NO PUNTEROS
//  -----------------------------
//  Cuando el Arreglo de usuarios crece reserva un bloque nuevo y traslada los
//  datos: cualquier puntero guardado quedaria invalido. Un indice entero
//  sobrevive a esa reubicacion y ademas ocupa 4 bytes en lugar de 8.
//
//  ELIMINACION: es un BORRADO LOGICO. El usuario se marca inactivo y su
//  posicion pasa a una lista de huecos libres para ser reutilizada por el
//  proximo registro. Compactar el arreglo obligaria a renumerar millones de
//  aristas del grafo.
//
//  CONVENIO DE ENTRADAS Y SALIDAS
//  ------------------------------
//  Los metodos reciben IDS PUBLICOS (lo que escribe el usuario en el menu) y
//  devuelven INDICES INTERNOS (lo que necesita el programa para acceder a los
//  datos en O(1)). La traduccion de indice a datos se hace con
//  usuarioEnIndice() / publicacionEnIndice().
// ============================================================================
#pragma once

#include "../estructuras/ArbolAVL.h"
#include "../estructuras/Arreglo.h"
#include "../estructuras/Cadena.h"
#include "../estructuras/TablaHash.h"
#include "../estructuras/Trie.h"
#include "../modelo/Publicacion.h"
#include "../modelo/Usuario.h"
#include "GrafoAmistades.h"

namespace aed {

/// Elemento generico de un ranking: una posicion y el puntaje que la ordena.
struct ElementoRanking {
    int indice = -1;
    long long puntaje = 0;

    ElementoRanking() = default;
    ElementoRanking(int i, long long p) : indice(i), puntaje(p) {}

    /// "Menor" = peor puesto en el ranking, para poder filtrar los K mejores
    /// con un monticulo de minimos.
    bool operator<(const ElementoRanking& otro) const {
        if (puntaje != otro.puntaje) return puntaje < otro.puntaje;
        return indice > otro.indice;
    }
};

class RedSocial {
public:
    RedSocial();

    RedSocial(const RedSocial&) = delete;
    RedSocial& operator=(const RedSocial&) = delete;

    // =======================================================================
    //  1. GESTION DE USUARIOS
    // =======================================================================

    /// Registra un usuario nuevo. Devuelve su id, o -1 si el correo ya existe.
    /// O(1) promedio en las tablas hash + O(log n) en el AVL de nombres.
    int registrarUsuario(const Cadena& nombre, const Cadena& correo, const Fecha& fechaRegistro);

    /// Borrado logico: desconecta al usuario del grafo, elimina sus
    /// publicaciones y lo quita de todos los indices.
    /// O(grado + publicaciones del usuario).
    bool eliminarUsuario(int id);

    // ---- Busquedas (funcionalidad "Buscar usuarios") ----------------------
    Usuario* buscarPorId(int id);
    Usuario* buscarPorCorreo(const Cadena& correo);

    /// Nombre exacto usando el AVL. Puede haber homonimos, por eso devuelve
    /// una lista de indices. O(log n).
    void buscarPorNombre(const Cadena& nombre, Arreglo<int>& indices);

    /// Autocompletado por prefijo usando el trie. O(longitud del prefijo).
    void buscarPorPrefijo(const Cadena& prefijo, int limite, Arreglo<int>& indices);

    /// Recorrido en orden del AVL: primeros 'limite' nombres alfabeticamente.
    void listarEnOrdenAlfabetico(int limite, Arreglo<int>& indices);

    // =======================================================================
    //  2. AMISTADES
    // =======================================================================
    bool agregarAmigo(int idA, int idB);
    bool eliminarAmigo(int idA, int idB);
    bool sonAmigos(int idA, int idB);

    /// Camino de amistad mas corto (BFS bidireccional).
    bool caminoDeAmistad(int idA, int idB, Arreglo<int>& indicesDelCamino);

    /// Amigos en comun entre dos usuarios.
    void amigosEnComun(int idA, int idB, Arreglo<int>& indicesComunes);

    /// Sugerencias de amistad ordenadas por cantidad de amigos en comun.
    void sugerenciasDeAmistad(int id, int cantidad, Arreglo<SugerenciaAmistad>& sugerencias);

    /// Lista de amigos de un usuario.
    void amigosDe(int id, Arreglo<int>& indicesAmigos);

    // =======================================================================
    //  3. PUBLICACIONES
    // =======================================================================
    int crearPublicacion(int idAutor, const Cadena& texto, const Fecha& fecha);
    bool eliminarPublicacion(int idPublicacion);
    bool darLike(int idPublicacion);

    /// Suma varias reacciones de una sola vez. Existe para la carga masiva:
    /// llamar a darLike() cien millones de veces desperdiciaria una busqueda
    /// en la tabla hash por cada like.
    bool agregarLikes(int idPublicacion, int cantidad);
    int comentarPublicacion(int idPublicacion, int idAutor, const Cadena& texto,
                            const Fecha& fecha);

    Publicacion* buscarPublicacion(int idPublicacion);

    /// Indices (no ids) de las publicaciones activas de un usuario.
    void publicacionesDe(int idUsuario, Arreglo<int>& indicesPublicaciones);

    // =======================================================================
    //  4. RANKINGS
    // =======================================================================
    /// Los K usuarios con mayor puntaje de actividad. O(n log K).
    void usuariosMasActivos(int cantidad, Arreglo<ElementoRanking>& ranking);

    /// Las K publicaciones con mas reacciones (likes + comentarios). O(p log K).
    void publicacionesConMasReacciones(int cantidad, Arreglo<ElementoRanking>& ranking);

    // =======================================================================
    //  5. ACCESO INTERNO Y ESTADISTICAS
    // =======================================================================
    int cantidadPosicionesUsuario() const { return _usuarios.tamanio(); }
    int cantidadUsuariosActivos() const { return _usuariosActivos; }
    int cantidadPosicionesPublicacion() const { return _publicaciones.tamanio(); }
    int cantidadPublicacionesActivas() const { return _publicacionesActivas; }

    Usuario& usuarioEnIndice(int indice) { return _usuarios[indice]; }
    const Usuario& usuarioEnIndice(int indice) const { return _usuarios[indice]; }
    Publicacion& publicacionEnIndice(int indice) { return _publicaciones[indice]; }
    const Publicacion& publicacionEnIndice(int indice) const { return _publicaciones[indice]; }

    /// Traduce un id publico al indice interno. Devuelve -1 si no existe.
    int indiceDeUsuario(int id) const;

    GrafoAmistades& grafo() { return _grafo; }
    const GrafoAmistades& grafo() const { return _grafo; }

    int alturaDelArbolDeNombres() const { return _indicePorNombre.altura(); }
    int nombresDistintos() const { return _indicePorNombre.tamanio(); }
    int nodosDelTrie() const { return _indicePrefijos.cantidadNodos(); }

    long long memoriaAproximadaBytes() const;

    // =======================================================================
    //  6. APOYO A LA CARGA MASIVA
    // =======================================================================
    /// Reserva de antemano el espacio para evitar redimensiones sucesivas.
    void reservarCapacidad(int usuariosEsperados, int publicacionesEsperadas);

    /// Permite desactivar el AVL y el trie durante las mediciones que solo
    /// analizan el grafo, para separar el costo de cada indice.
    void construirIndicesDeTexto(bool activo) { _indicesDeTextoActivos = activo; }

    /// Tras cargar aristas en bloque con el grafo, recalcula el campo
    /// 'cantidadAmigos' de cada usuario. O(n).
    void sincronizarContadoresDeAmigos();

private:
    void indexarTextos(int indice);
    void desindexarTextos(int indice);

    /// Separa un nombre completo en palabras y ejecuta 'accion' con cada una.
    template <typename Accion>
    static void porCadaPalabra(const Cadena& texto, Accion accion) {
        int inicio = 0;
        for (int i = 0; i <= texto.longitud(); ++i) {
            bool finDePalabra = (i == texto.longitud()) || (texto[i] == ' ');
            if (finDePalabra) {
                if (i > inicio) accion(texto.subcadena(inicio, i - inicio));
                inicio = i + 1;
            }
        }
    }

    // ---- Almacenamiento ---------------------------------------------------
    Arreglo<Usuario> _usuarios;
    Arreglo<Publicacion> _publicaciones;
    GrafoAmistades _grafo;

    // ---- Indices ----------------------------------------------------------
    TablaHash<int, int> _indicePorId;
    TablaHash<Cadena, int> _indicePorCorreo;
    TablaHash<int, int> _indicePublicacionPorId;
    ArbolAVL<Cadena, Arreglo<int>> _indicePorNombre;
    Trie _indicePrefijos;

    // ---- Contadores y huecos reutilizables --------------------------------
    Arreglo<int> _posicionesLibres;  // huecos dejados por usuarios eliminados
    int _siguienteIdUsuario;
    int _siguienteIdPublicacion;
    int _siguienteIdComentario;
    int _usuariosActivos;
    int _publicacionesActivas;
    bool _indicesDeTextoActivos;
};

}  // namespace aed
