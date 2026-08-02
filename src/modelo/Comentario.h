// ============================================================================
//  Comentario.h
//  Comentario que un usuario deja en una publicacion.
//
//  Los comentarios viven dentro de una ListaEnlazada dentro de su publicacion:
//  siempre se consultan junto con ella y nunca se buscan por su cuenta.
// ============================================================================
#pragma once

#include "../estructuras/String.h"
#include "Fecha.h"

namespace aed {

struct Comentario {
    int id = -1;        // identificador global del comentario
    int idAutor = -1;   // id del usuario que lo escribio
    Fecha fecha;
    String texto;

    Comentario() = default;

    Comentario(int idComentario, int autor, const Fecha& cuando, const String& contenido)
        : id(idComentario), idAutor(autor), fecha(cuando), texto(contenido) {}
};

}  // namespace aed
