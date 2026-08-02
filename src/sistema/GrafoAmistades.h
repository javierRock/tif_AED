

#pragma once

#include "../estructuras/Arreglo.h"

namespace aed
{

    struct SugerenciaAmistad
    {
        int indiceUsuario = -1;
        int amigosEnComun = 0;

        SugerenciaAmistad() = default;
        SugerenciaAmistad(int indice, int comunes)
            : indiceUsuario(indice), amigosEnComun(comunes) {}

        bool operator<(const SugerenciaAmistad &otra) const
        {
            if (amigosEnComun != otra.amigosEnComun)
                return amigosEnComun < otra.amigosEnComun;
            return indiceUsuario > otra.indiceUsuario;
        }
    };

    class GrafoAmistades
    {
    public:
        GrafoAmistades();

        void establecerCantidadNodos(int cantidad);
        int agregarNodo();
        int cantidadNodos() const { return _adyacencia.tamanio(); }
        long long cantidadAristas() const { return _cantidadAristas; }

        bool agregarArista(int a, int b);

        bool eliminarArista(int a, int b);

        bool sonAdyacentes(int a, int b) const;

        int grado(int nodo) const;
        const Arreglo<int> &vecinos(int nodo) const;

        void aislarNodo(int nodo);

        void agregarAristaSinOrdenar(int a, int b);
        void ordenarYLimpiarListas();

        bool caminoMasCorto(int origen, int destino, Arreglo<int> &camino);

        bool caminoMasCortoBFSSimple(int origen, int destino, Arreglo<int> &camino);

        void amigosEnComun(int a, int b, Arreglo<int> &resultado) const;

        void sugerirAmistades(int nodo, int cantidad, Arreglo<SugerenciaAmistad> &salida);

        int gradoMaximo() const;
        double gradoPromedio() const;
        long long memoriaAproximadaBytes() const;

    private:
        void prepararEspaciosDeTrabajo();

        int expandirNivel(Arreglo<int> &frente, Arreglo<int> &selloPropio,
                          Arreglo<int> &distanciaPropia, Arreglo<int> &padrePropio,
                          const Arreglo<int> &selloOpuesto, const Arreglo<int> &distanciaOpuesta);

        void reconstruirCamino(int encuentro, Arreglo<int> &camino) const;

        Arreglo<Arreglo<int>> _adyacencia;
        Arreglo<int> _listaVacia;
        long long _cantidadAristas;

        int _selloActual;
        Arreglo<int> _selloLadoA, _distanciaLadoA, _padreLadoA;
        Arreglo<int> _selloLadoB, _distanciaLadoB, _padreLadoB;
        Arreglo<int> _frenteA, _frenteB, _frenteAuxiliar;

        int _selloSugerencias;
        Arreglo<int> _marcaSugerencia;
        Arreglo<int> _conteoSugerencia;
        Arreglo<int> _candidatos;
    };

}
