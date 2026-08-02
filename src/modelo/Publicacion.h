















#pragma once

#include "../estructuras/String.h"
#include "../estructuras/ListaEnlazada.h"
#include "Comentario.h"
#include "Fecha.h"

namespace aed {

struct Publicacion {
    int id = -1;
    int idPropietario = -1;
    Fecha fecha;
    String texto;
    int numeroLikes = 0;
    ListaEnlazada<Comentario> comentarios;
    bool activa = true;

    Publicacion() = default;

    Publicacion(int idPublicacion, int propietario, const Fecha& cuando, const String& contenido)
        : id(idPublicacion), idPropietario(propietario), fecha(cuando), texto(contenido) {}

    int numeroComentarios() const { return comentarios.tamanio(); }



    long long totalReacciones() const {
        return static_cast<long long>(numeroLikes) + comentarios.tamanio();
    }
};

}
