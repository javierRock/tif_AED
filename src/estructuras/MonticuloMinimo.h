

#pragma once

#include "Arreglo.h"
#include "Ordenamiento.h"

namespace aed
{

    template <typename T>
    class MonticuloMinimo
    {
    public:
        MonticuloMinimo() {}
        explicit MonticuloMinimo(int capacidadInicial) : _datos(capacidadInicial) {}

        int tamanio() const { return _datos.tamanio(); }
        bool vacio() const { return _datos.vacio(); }

        const T &minimo() const { return _datos[0]; }

        void insertar(const T &valor)
        {
            _datos.agregar(valor);
            subir(_datos.tamanio() - 1);
        }

        T extraerMinimo()
        {
            T resultado = mover(_datos[0]);
            _datos[0] = mover(_datos[_datos.tamanio() - 1]);
            _datos.quitarUltimo();
            if (!_datos.vacio())
                bajar(0);
            return resultado;
        }

        void reemplazarMinimo(const T &valor)
        {
            _datos[0] = valor;
            bajar(0);
        }

        void limpiar() { _datos.limpiar(); }

        void volcarDeMayorAMenor(Arreglo<T> &salida)
        {
            salida.limpiar();
            salida.reservar(_datos.tamanio());
            while (!vacio())
            {
                salida.agregar(extraerMinimo());
            }

            for (int i = 0, j = salida.tamanio() - 1; i < j; ++i, --j)
            {
                intercambiar(salida[i], salida[j]);
            }
        }

    private:
        void subir(int posicion)
        {
            while (posicion > 0)
            {
                int padre = (posicion - 1) / 2;
                if (!(_datos[posicion] < _datos[padre]))
                    break;
                intercambiar(_datos[posicion], _datos[padre]);
                posicion = padre;
            }
        }

        void bajar(int posicion)
        {
            int cantidad = _datos.tamanio();
            while (true)
            {
                int izquierdo = 2 * posicion + 1;
                int derecho = 2 * posicion + 2;
                int menor = posicion;

                if (izquierdo < cantidad && _datos[izquierdo] < _datos[menor])
                    menor = izquierdo;
                if (derecho < cantidad && _datos[derecho] < _datos[menor])
                    menor = derecho;
                if (menor == posicion)
                    break;

                intercambiar(_datos[posicion], _datos[menor]);
                posicion = menor;
            }
        }

        Arreglo<T> _datos;
    };

}
