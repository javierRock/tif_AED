

#pragma once

#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"
#include "Fecha.h"

namespace aed
{

    struct Usuario
    {
        int id = -1;
        String nombre;
        String correo;
        Fecha fechaRegistro;

        Arreglo<int> indicesPublicaciones;

        int cantidadAmigos = 0;
        int cantidadSeguidores = 0;
        long long reaccionesRecibidas = 0;
        int comentariosRealizados = 0;
        bool activo = true;

        Usuario() = default;

        Usuario(int idUsuario, const String &nombreUsuario, const String &correoUsuario,
                const Fecha &registro)
            : id(idUsuario), nombre(nombreUsuario), correo(correoUsuario), fechaRegistro(registro) {}

        int cantidadPublicaciones() const { return indicesPublicaciones.tamanio(); }

        long long puntajeActividad() const
        {
            return static_cast<long long>(cantidadPublicaciones()) * 3 +
                   static_cast<long long>(comentariosRealizados) * 2 + reaccionesRecibidas;
        }
    };

}
