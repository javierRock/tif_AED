#include "RedSocial.h"

#include "../estructuras/MonticuloMinimo.h"

namespace aed {

namespace {

/// Funtor usado en el recorrido en orden del AVL para quedarse con los
/// primeros 'limite' usuarios en orden alfabetico y cortar el recorrido ahi.
struct RecolectorAlfabetico {
    Arreglo<int>* destino;
    int limite;

    bool operator()(const String& /*nombre*/, const Arreglo<int>& indices) {
        for (int i = 0; i < indices.tamanio(); ++i) {
            if (destino->tamanio() >= limite) return false;
            destino->agregar(indices[i]);
        }
        return destino->tamanio() < limite;
    }
};

}  // namespace

RedSocial::RedSocial()
    : _indicePorId(1024),
      _indicePorCorreo(1024),
      _indicePublicacionPorId(1024),
      _siguienteIdUsuario(1),
      _siguienteIdPublicacion(1),
      _siguienteIdComentario(1),
      _usuariosActivos(0),
      _publicacionesActivas(0),
      _indicesDeTextoActivos(true) {}

int RedSocial::indiceDeUsuario(int id) const {
    const int* indice = _indicePorId.buscar(id);
    return (indice == nullptr) ? -1 : *indice;
}

// ===========================================================================
//  1. GESTION DE USUARIOS
// ===========================================================================
int RedSocial::registrarUsuario(const String& nombre, const String& correo,
                                const Fecha& fechaRegistro) {
    // El correo es la clave natural del sistema: no puede repetirse.
    if (correo.vacia() || _indicePorCorreo.contiene(correo)) return -1;

    int id = _siguienteIdUsuario++;
    int indice;

    // Se reutiliza el hueco de un usuario eliminado si lo hay; su nodo del
    // grafo ya existe y esta aislado, listo para usarse de nuevo.
    if (!_posicionesLibres.vacio()) {
        indice = _posicionesLibres.ultimo();
        _posicionesLibres.quitarUltimo();
        _usuarios[indice] = Usuario(id, nombre, correo, fechaRegistro);
    } else {
        indice = _usuarios.tamanio();
        _usuarios.agregar(Usuario(id, nombre, correo, fechaRegistro));
    }
    while (_grafo.cantidadNodos() <= indice) _grafo.agregarNodo();

    _indicePorId.insertar(id, indice);
    _indicePorCorreo.insertar(correo, indice);
    if (_indicesDeTextoActivos) indexarTextos(indice);

    ++_usuariosActivos;
    return id;
}

bool RedSocial::eliminarUsuario(int id) {
    int indice = indiceDeUsuario(id);
    if (indice < 0) return false;
    Usuario& usuario = _usuarios[indice];
    if (!usuario.activo) return false;

    // --- 1. Sus publicaciones dejan de estar activas -----------------------
    for (int i = 0; i < usuario.indicesPublicaciones.tamanio(); ++i) {
        Publicacion& publicacion = _publicaciones[usuario.indicesPublicaciones[i]];
        if (!publicacion.activa) continue;
        publicacion.activa = false;
        publicacion.comentarios.limpiar();
        publicacion.texto.limpiar();
        _indicePublicacionPorId.eliminar(publicacion.id);
        --_publicacionesActivas;
    }
    usuario.indicesPublicaciones.liberar();

    // --- 2. Se desconecta del grafo ----------------------------------------
    // Primero se corrige el contador de cada amigo (leyendo la lista antes de
    // vaciarla) y despues se borran las aristas en ambos sentidos.
    const Arreglo<int>& amigos = _grafo.vecinos(indice);
    for (int i = 0; i < amigos.tamanio(); ++i) {
        Usuario& amigo = _usuarios[amigos[i]];
        if (amigo.cantidadAmigos > 0) --amigo.cantidadAmigos;
        if (amigo.cantidadSeguidores > 0) --amigo.cantidadSeguidores;
    }
    _grafo.aislarNodo(indice);

    // --- 3. Se retira de todos los indices ---------------------------------
    if (_indicesDeTextoActivos) desindexarTextos(indice);
    _indicePorId.eliminar(id);
    _indicePorCorreo.eliminar(usuario.correo);

    // --- 4. Borrado logico y liberacion del hueco --------------------------
    usuario.activo = false;
    usuario.cantidadAmigos = 0;
    usuario.nombre.limpiar();
    usuario.correo.limpiar();
    _posicionesLibres.agregar(indice);
    --_usuariosActivos;
    return true;
}

Usuario* RedSocial::buscarPorId(int id) {
    int indice = indiceDeUsuario(id);
    if (indice < 0) return nullptr;
    return _usuarios[indice].activo ? &_usuarios[indice] : nullptr;
}

Usuario* RedSocial::buscarPorCorreo(const String& correo) {
    const int* indice = _indicePorCorreo.buscar(correo);
    if (indice == nullptr) return nullptr;
    return _usuarios[*indice].activo ? &_usuarios[*indice] : nullptr;
}

void RedSocial::buscarPorNombre(const String& nombre, Arreglo<int>& indices) {
    indices.limpiar();
    const Arreglo<int>* encontrados = _indicePorNombre.buscar(nombre);
    if (encontrados == nullptr) return;
    for (int i = 0; i < encontrados->tamanio(); ++i) {
        int indice = (*encontrados)[i];
        if (_usuarios[indice].activo) indices.agregar(indice);
    }
}

void RedSocial::buscarPorPrefijo(const String& prefijo, int limite, Arreglo<int>& indices) {
    // Se piden mas resultados de los necesarios porque algunos pueden
    // corresponder a usuarios ya eliminados o repetirse (nombre + apellido).
    _indicePrefijos.buscarPorPrefijo(prefijo, limite * 4 + 16, indices);

    int escritura = 0;
    for (int i = 0; i < indices.tamanio() && escritura < limite; ++i) {
        int indice = indices[i];
        if (!_usuarios[indice].activo) continue;

        bool repetido = false;
        for (int k = 0; k < escritura; ++k) {
            if (indices[k] == indice) {
                repetido = true;
                break;
            }
        }
        if (!repetido) indices[escritura++] = indice;
    }
    indices.redimensionar(escritura);
}

void RedSocial::listarEnOrdenAlfabetico(int limite, Arreglo<int>& indices) {
    indices.limpiar();
    if (limite <= 0) return;
    RecolectorAlfabetico recolector{&indices, limite};
    _indicePorNombre.recorrerEnOrden(recolector);
}

void RedSocial::indexarTextos(int indice) {
    const String& nombre = _usuarios[indice].nombre;
    if (nombre.vacia()) return;

    // AVL: un mismo nombre puede corresponder a varios usuarios, por eso el
    // valor asociado a la clave es una lista de indices.
    Arreglo<int>& homonimos = _indicePorNombre.obtenerOInsertar(nombre, Arreglo<int>());
    homonimos.agregar(indice);

    // Trie: se indexa el nombre completo y ademas cada palabra suelta, para
    // poder encontrar a "Ana Torres" escribiendo "ana" o escribiendo "torres".
    _indicePrefijos.insertar(nombre, indice);
    porCadaPalabra(nombre, [&](const String& palabra) {
        if (!(palabra == nombre)) _indicePrefijos.insertar(palabra, indice);
    });
}

void RedSocial::desindexarTextos(int indice) {
    const String& nombre = _usuarios[indice].nombre;
    if (nombre.vacia()) return;

    Arreglo<int>* homonimos = _indicePorNombre.buscar(nombre);
    if (homonimos != nullptr) {
        int posicion = homonimos->buscar(indice);
        if (posicion >= 0) homonimos->eliminarRapido(posicion);
        if (homonimos->vacio()) _indicePorNombre.eliminar(nombre);
    }

    _indicePrefijos.eliminar(nombre, indice);
    porCadaPalabra(nombre, [&](const String& palabra) {
        if (!(palabra == nombre)) _indicePrefijos.eliminar(palabra, indice);
    });
}

// ===========================================================================
//  2. AMISTADES
// ===========================================================================
bool RedSocial::agregarAmigo(int idA, int idB) {
    int a = indiceDeUsuario(idA);
    int b = indiceDeUsuario(idB);
    if (a < 0 || b < 0 || a == b) return false;
    if (!_usuarios[a].activo || !_usuarios[b].activo) return false;
    if (!_grafo.agregarArista(a, b)) return false;

    // En este modelo la amistad es reciproca, asi que cada uno gana un amigo
    // y, al mismo tiempo, un seguidor.
    ++_usuarios[a].cantidadAmigos;
    ++_usuarios[b].cantidadAmigos;
    ++_usuarios[a].cantidadSeguidores;
    ++_usuarios[b].cantidadSeguidores;
    return true;
}

bool RedSocial::eliminarAmigo(int idA, int idB) {
    int a = indiceDeUsuario(idA);
    int b = indiceDeUsuario(idB);
    if (a < 0 || b < 0) return false;
    if (!_grafo.eliminarArista(a, b)) return false;

    if (_usuarios[a].cantidadAmigos > 0) --_usuarios[a].cantidadAmigos;
    if (_usuarios[b].cantidadAmigos > 0) --_usuarios[b].cantidadAmigos;
    if (_usuarios[a].cantidadSeguidores > 0) --_usuarios[a].cantidadSeguidores;
    if (_usuarios[b].cantidadSeguidores > 0) --_usuarios[b].cantidadSeguidores;
    return true;
}

bool RedSocial::sonAmigos(int idA, int idB) {
    int a = indiceDeUsuario(idA);
    int b = indiceDeUsuario(idB);
    if (a < 0 || b < 0) return false;
    return _grafo.sonAdyacentes(a, b);
}

bool RedSocial::caminoDeAmistad(int idA, int idB, Arreglo<int>& indicesDelCamino) {
    int a = indiceDeUsuario(idA);
    int b = indiceDeUsuario(idB);
    if (a < 0 || b < 0) {
        indicesDelCamino.limpiar();
        return false;
    }
    return _grafo.caminoMasCorto(a, b, indicesDelCamino);
}

void RedSocial::amigosEnComun(int idA, int idB, Arreglo<int>& indicesComunes) {
    int a = indiceDeUsuario(idA);
    int b = indiceDeUsuario(idB);
    if (a < 0 || b < 0) {
        indicesComunes.limpiar();
        return;
    }
    _grafo.amigosEnComun(a, b, indicesComunes);
}

void RedSocial::sugerenciasDeAmistad(int id, int cantidad,
                                     Arreglo<SugerenciaAmistad>& sugerencias) {
    int indice = indiceDeUsuario(id);
    if (indice < 0) {
        sugerencias.limpiar();
        return;
    }
    _grafo.sugerirAmistades(indice, cantidad, sugerencias);
}

void RedSocial::amigosDe(int id, Arreglo<int>& indicesAmigos) {
    indicesAmigos.limpiar();
    int indice = indiceDeUsuario(id);
    if (indice < 0) return;

    const Arreglo<int>& lista = _grafo.vecinos(indice);
    indicesAmigos.reservar(lista.tamanio());
    for (int i = 0; i < lista.tamanio(); ++i) indicesAmigos.agregar(lista[i]);
}

// ===========================================================================
//  3. PUBLICACIONES
// ===========================================================================
int RedSocial::crearPublicacion(int idAutor, const String& texto, const Fecha& fecha) {
    int indiceAutor = indiceDeUsuario(idAutor);
    if (indiceAutor < 0 || !_usuarios[indiceAutor].activo) return -1;

    int id = _siguienteIdPublicacion++;
    int indice = _publicaciones.tamanio();
    _publicaciones.agregar(Publicacion(id, idAutor, fecha, texto));

    _usuarios[indiceAutor].indicesPublicaciones.agregar(indice);
    _indicePublicacionPorId.insertar(id, indice);
    ++_publicacionesActivas;
    return id;
}

bool RedSocial::eliminarPublicacion(int idPublicacion) {
    const int* posicion = _indicePublicacionPorId.buscar(idPublicacion);
    if (posicion == nullptr) return false;

    int indice = *posicion;
    Publicacion& publicacion = _publicaciones[indice];
    if (!publicacion.activa) return false;

    // El autor pierde las reacciones que habia acumulado con esta publicacion.
    int indiceAutor = indiceDeUsuario(publicacion.idPropietario);
    if (indiceAutor >= 0) {
        Usuario& autor = _usuarios[indiceAutor];
        int posicionEnAutor = autor.indicesPublicaciones.buscar(indice);
        if (posicionEnAutor >= 0) autor.indicesPublicaciones.eliminarRapido(posicionEnAutor);

        autor.reaccionesRecibidas -= publicacion.totalReacciones();
        if (autor.reaccionesRecibidas < 0) autor.reaccionesRecibidas = 0;
    }

    publicacion.activa = false;
    publicacion.comentarios.limpiar();
    publicacion.texto.limpiar();
    _indicePublicacionPorId.eliminar(idPublicacion);
    --_publicacionesActivas;
    return true;
}

Publicacion* RedSocial::buscarPublicacion(int idPublicacion) {
    const int* indice = _indicePublicacionPorId.buscar(idPublicacion);
    if (indice == nullptr) return nullptr;
    return _publicaciones[*indice].activa ? &_publicaciones[*indice] : nullptr;
}

bool RedSocial::darLike(int idPublicacion) {
    return agregarLikes(idPublicacion, 1);
}

bool RedSocial::agregarLikes(int idPublicacion, int cantidad) {
    if (cantidad <= 0) return false;
    Publicacion* publicacion = buscarPublicacion(idPublicacion);
    if (publicacion == nullptr) return false;

    publicacion->numeroLikes += cantidad;
    int indiceAutor = indiceDeUsuario(publicacion->idPropietario);
    if (indiceAutor >= 0) _usuarios[indiceAutor].reaccionesRecibidas += cantidad;
    return true;
}

int RedSocial::comentarPublicacion(int idPublicacion, int idAutor, const String& texto,
                                   const Fecha& fecha) {
    Publicacion* publicacion = buscarPublicacion(idPublicacion);
    if (publicacion == nullptr) return -1;

    int indiceAutor = indiceDeUsuario(idAutor);
    if (indiceAutor < 0 || !_usuarios[indiceAutor].activo) return -1;

    int idComentario = _siguienteIdComentario++;
    publicacion->comentarios.agregarAlFinal(Comentario(idComentario, idAutor, fecha, texto));
    ++_usuarios[indiceAutor].comentariosRealizados;

    int indicePropietario = indiceDeUsuario(publicacion->idPropietario);
    if (indicePropietario >= 0) ++_usuarios[indicePropietario].reaccionesRecibidas;
    return idComentario;
}

void RedSocial::publicacionesDe(int idUsuario, Arreglo<int>& indicesPublicaciones) {
    indicesPublicaciones.limpiar();
    int indice = indiceDeUsuario(idUsuario);
    if (indice < 0) return;

    const Arreglo<int>& lista = _usuarios[indice].indicesPublicaciones;
    for (int i = 0; i < lista.tamanio(); ++i) {
        if (_publicaciones[lista[i]].activa) indicesPublicaciones.agregar(lista[i]);
    }
}

// ===========================================================================
//  4. RANKINGS
//
//  Ambos usan la misma tecnica: un monticulo de MINIMOS de tamano K.
//  Se recorre una sola vez toda la coleccion y solo se conserva lo mejor
//  visto hasta el momento -> O(n log K) en tiempo y O(K) en memoria, en lugar
//  del O(n log n) y O(n) que costaria ordenar la coleccion entera.
// ===========================================================================
void RedSocial::usuariosMasActivos(int cantidad, Arreglo<ElementoRanking>& ranking) {
    ranking.limpiar();
    if (cantidad <= 0) return;

    MonticuloMinimo<ElementoRanking> mejores(cantidad);
    for (int i = 0; i < _usuarios.tamanio(); ++i) {
        if (!_usuarios[i].activo) continue;
        ElementoRanking candidato(i, _usuarios[i].puntajeActividad());

        if (mejores.tamanio() < cantidad) {
            mejores.insertar(candidato);
        } else if (mejores.minimo() < candidato) {
            // El candidato supera al peor de los K guardados: lo reemplaza.
            mejores.reemplazarMinimo(candidato);
        }
    }
    mejores.volcarDeMayorAMenor(ranking);
}

void RedSocial::publicacionesConMasReacciones(int cantidad, Arreglo<ElementoRanking>& ranking) {
    ranking.limpiar();
    if (cantidad <= 0) return;

    MonticuloMinimo<ElementoRanking> mejores(cantidad);
    for (int i = 0; i < _publicaciones.tamanio(); ++i) {
        if (!_publicaciones[i].activa) continue;
        ElementoRanking candidato(i, _publicaciones[i].totalReacciones());

        if (mejores.tamanio() < cantidad) {
            mejores.insertar(candidato);
        } else if (mejores.minimo() < candidato) {
            mejores.reemplazarMinimo(candidato);
        }
    }
    mejores.volcarDeMayorAMenor(ranking);
}

// ===========================================================================
//  5. ESTADISTICAS Y CARGA MASIVA
// ===========================================================================
long long RedSocial::memoriaAproximadaBytes() const {
    long long total = 0;

    total += static_cast<long long>(_usuarios.capacidad()) * sizeof(Usuario);
    for (int i = 0; i < _usuarios.tamanio(); ++i) {
        total += _usuarios[i].nombre.capacidad() + 1;
        total += _usuarios[i].correo.capacidad() + 1;
        total += static_cast<long long>(_usuarios[i].indicesPublicaciones.capacidad()) * sizeof(int);
    }

    total += static_cast<long long>(_publicaciones.capacidad()) * sizeof(Publicacion);
    for (int i = 0; i < _publicaciones.tamanio(); ++i) {
        total += _publicaciones[i].texto.capacidad() + 1;
        total += static_cast<long long>(_publicaciones[i].numeroComentarios()) *
                 (sizeof(Comentario) + 8);
    }

    total += _grafo.memoriaAproximadaBytes();
    total += static_cast<long long>(_indicePorId.capacidad()) * (sizeof(int) * 2 + 1);
    total += static_cast<long long>(_indicePorCorreo.capacidad()) * (sizeof(String) + sizeof(int) + 1);
    total += static_cast<long long>(_indicePublicacionPorId.capacidad()) * (sizeof(int) * 2 + 1);
    total += static_cast<long long>(_indicePrefijos.cantidadNodos()) * 96;
    return total;
}

void RedSocial::reservarCapacidad(int usuariosEsperados, int publicacionesEsperadas) {
    if (usuariosEsperados > 0) {
        _usuarios.reservar(usuariosEsperados);
        _indicePorId.reservarPara(usuariosEsperados);
        _indicePorCorreo.reservarPara(usuariosEsperados);
    }
    if (publicacionesEsperadas > 0) {
        _publicaciones.reservar(publicacionesEsperadas);
        _indicePublicacionPorId.reservarPara(publicacionesEsperadas);
    }
}

void RedSocial::sincronizarContadoresDeAmigos() {
    for (int i = 0; i < _usuarios.tamanio(); ++i) {
        if (!_usuarios[i].activo) continue;
        int grado = _grafo.grado(i);
        _usuarios[i].cantidadAmigos = grado;
        _usuarios[i].cantidadSeguidores += grado;
    }
}

}  // namespace aed
