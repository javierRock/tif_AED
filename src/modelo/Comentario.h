






#pragma once

#include "../estructuras/String.h"
#include "Fecha.h"

namespace aed {

struct Comentario {
    int id = -1;
    int idAutor = -1;
    Fecha fecha;
    String texto;

    Comentario() = default;

    Comentario(int idComentario, int autor, const Fecha& cuando, const String& contenido)
        : id(idComentario), idAutor(autor), fecha(cuando), texto(contenido) {}
};

}
