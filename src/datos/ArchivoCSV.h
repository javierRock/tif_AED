

#pragma once

#include <cstdio>

#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"

namespace aed
{

    class RedSocial;

    class LectorCSV
    {
    public:
        LectorCSV();
        ~LectorCSV();

        LectorCSV(const LectorCSV &) = delete;
        LectorCSV &operator=(const LectorCSV &) = delete;

        bool abrir(const char *ruta);
        void cerrar();
        bool estaAbierto() const { return _archivo != nullptr; }

        bool leerFila(Arreglo<String> &campos);

    private:
        int siguienteCaracter();

        std::FILE *_archivo;
        char *_buffer;
        int _bytesEnBuffer;
        int _posicion;
    };

    class EscritorCSV
    {
    public:
        EscritorCSV();
        ~EscritorCSV();

        EscritorCSV(const EscritorCSV &) = delete;
        EscritorCSV &operator=(const EscritorCSV &) = delete;

        bool abrir(const char *ruta);
        void cerrar();
        bool estaAbierto() const { return _archivo != nullptr; }

        void campo(const String &texto);
        void campo(const char *texto);
        void campo(long long numero);
        void finDeFila();

    private:
        void escribirSeparador();

        std::FILE *_archivo;
        bool _primerCampoDeLaFila;
    };

    namespace archivo
    {

        bool guardarUsuarios(const RedSocial &red, const char *ruta);
        bool guardarAmistades(const RedSocial &red, const char *ruta);
        bool guardarPublicaciones(const RedSocial &red, const char *ruta);

        bool cargarUsuarios(RedSocial &red, const char *ruta);
        bool cargarAmistades(RedSocial &red, const char *ruta);
        bool cargarPublicaciones(RedSocial &red, const char *ruta);

    }

}
