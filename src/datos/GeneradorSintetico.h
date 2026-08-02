

#pragma once

#include "../sistema/RedSocial.h"
#include "GeneradorAleatorio.h"

namespace aed
{

    struct ParametrosGeneracion
    {
        int cantidadUsuarios = 100000;
        int amistadesPorUsuario = 8;
        int cantidadPublicaciones = 200000;
        int maximoLikesPorPublicacion = 5000;
        int maximoComentariosPorPublicacion = 4;
        unsigned long long semilla = 20260730ULL;
        bool mostrarProgreso = true;
    };

    class GeneradorSintetico
    {
    public:
        static void generar(RedSocial &red, const ParametrosGeneracion &parametros);

    private:
        static void generarUsuarios(RedSocial &red, const ParametrosGeneracion &parametros,
                                    GeneradorAleatorio &azar);
        static void generarAmistades(RedSocial &red, const ParametrosGeneracion &parametros,
                                     GeneradorAleatorio &azar);
        static void generarPublicaciones(RedSocial &red, const ParametrosGeneracion &parametros,
                                         GeneradorAleatorio &azar);
        static void generarInteracciones(RedSocial &red, const ParametrosGeneracion &parametros,
                                         GeneradorAleatorio &azar);
    };

}
