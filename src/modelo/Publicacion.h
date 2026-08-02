// ============================================================================
//  Publicacion.h
//  Publicacion realizada por un usuario, con sus reacciones y comentarios.
//
//  Campos exigidos por el enunciado: ID, usuario propietario, fecha, texto,
//  numero de likes, numero de comentarios y lista de comentarios.
//
//  NOTA DE DISENO: el "numero de comentarios" NO se guarda en un campo aparte,
//  sino que se pregunta a la lista (que ya lleva su propio contador en O(1)).
//  Duplicar el dato obligaria a mantener dos fuentes de verdad sincronizadas,
//  que es una fuente clasica de errores.
//
//  BORRADO LOGICO: al eliminar una publicacion se marca 'activa = false' en
//  lugar de sacarla del arreglo. Asi los indices de las demas publicaciones
//  no se desplazan y todas las referencias guardadas siguen siendo validas.
// ============================================================================
#pragma once

#include "../estructuras/String.h"
#include "../estructuras/ListaEnlazada.h"
#include "Comentario.h"
#include "Fecha.h"

namespace aed {

struct Publicacion {
    int id = -1;               // identificador unico de la publicacion
    int idPropietario = -1;    // id del usuario que la creo
    Fecha fecha;
    String texto;
    int numeroLikes = 0;
    ListaEnlazada<Comentario> comentarios;
    bool activa = true;

    Publicacion() = default;

    Publicacion(int idPublicacion, int propietario, const Fecha& cuando, const String& contenido)
        : id(idPublicacion), idPropietario(propietario), fecha(cuando), texto(contenido) {}

    int numeroComentarios() const { return comentarios.tamanio(); }

    /// Total de reacciones que recibio la publicacion. Se usa en el ranking
    /// de "publicaciones con mayor cantidad de reacciones".
    long long totalReacciones() const {
        return static_cast<long long>(numeroLikes) + comentarios.tamanio();
    }
};

}  // namespace aed
