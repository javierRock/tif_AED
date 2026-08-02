

#pragma once

#include "../datos/GeneradorSintetico.h"
#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"
#include "../sistema/RedSocial.h"

namespace aed
{

    struct ResultadoPrueba
    {
        String operacion;
        long long escala = 0;
        long long repeticiones = 0;
        double milisegundosTotales = 0.0;

        double microsegundosPorOperacion() const
        {
            if (repeticiones <= 0)
                return 0.0;
            return (milisegundosTotales * 1000.0) / static_cast<double>(repeticiones);
        }
    };

    class BancoPruebas
    {
    public:
        static void medirOperaciones(RedSocial &red, Arreglo<ResultadoPrueba> &resultados);

        static void ejecutarEscala(const ParametrosGeneracion &parametros,
                                   Arreglo<ResultadoPrueba> &resultados);

        static void ejecutarSerieDeEscalas(int usuariosMaximos, const char *rutaCsv);

        static void imprimirTabla(const Arreglo<ResultadoPrueba> &resultados);
        static bool exportarCSV(const Arreglo<ResultadoPrueba> &resultados, const char *ruta);
    };

}
