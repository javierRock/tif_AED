

#pragma once

#include "Utilidades.h"

namespace aed
{

    template <typename T>
    class Arreglo
    {
    public:
        Arreglo() : _datos(nullptr), _tamanio(0), _capacidad(0) {}

        explicit Arreglo(int capacidadInicial) : _datos(nullptr), _tamanio(0), _capacidad(0)
        {
            reservar(capacidadInicial);
        }

        Arreglo(const Arreglo &otro) : _datos(nullptr), _tamanio(0), _capacidad(0)
        {
            reservar(otro._tamanio);
            for (int i = 0; i < otro._tamanio; ++i)
                _datos[i] = otro._datos[i];
            _tamanio = otro._tamanio;
        }

        Arreglo(Arreglo &&otro) noexcept
            : _datos(otro._datos), _tamanio(otro._tamanio), _capacidad(otro._capacidad)
        {
            otro._datos = nullptr;
            otro._tamanio = 0;
            otro._capacidad = 0;
        }

        ~Arreglo() { delete[] _datos; }

        Arreglo &operator=(const Arreglo &otro)
        {
            if (this == &otro)
                return *this;
            reservar(otro._tamanio);
            for (int i = 0; i < otro._tamanio; ++i)
                _datos[i] = otro._datos[i];
            for (int i = otro._tamanio; i < _tamanio; ++i)
                _datos[i] = T();
            _tamanio = otro._tamanio;
            return *this;
        }

        Arreglo &operator=(Arreglo &&otro) noexcept
        {
            if (this == &otro)
                return *this;
            delete[] _datos;
            _datos = otro._datos;
            _tamanio = otro._tamanio;
            _capacidad = otro._capacidad;
            otro._datos = nullptr;
            otro._tamanio = 0;
            otro._capacidad = 0;
            return *this;
        }

        int tamanio() const { return _tamanio; }
        int capacidad() const { return _capacidad; }
        bool vacio() const { return _tamanio == 0; }

        T &operator[](int posicion) { return _datos[posicion]; }
        const T &operator[](int posicion) const { return _datos[posicion]; }

        T &primero() { return _datos[0]; }
        const T &primero() const { return _datos[0]; }
        T &ultimo() { return _datos[_tamanio - 1]; }
        const T &ultimo() const { return _datos[_tamanio - 1]; }

        T *datos() { return _datos; }
        const T *datos() const { return _datos; }

        void agregar(const T &valor)
        {
            crecerSiHaceFalta();
            _datos[_tamanio++] = valor;
        }

        void agregar(T &&valor)
        {
            crecerSiHaceFalta();
            _datos[_tamanio++] = mover(valor);
        }

        void insertarEn(int posicion, const T &valor)
        {
            crecerSiHaceFalta();
            for (int i = _tamanio; i > posicion; --i)
            {
                _datos[i] = mover(_datos[i - 1]);
            }
            _datos[posicion] = valor;
            ++_tamanio;
        }

        void eliminarEn(int posicion)
        {
            for (int i = posicion; i < _tamanio - 1; ++i)
            {
                _datos[i] = mover(_datos[i + 1]);
            }
            --_tamanio;
            _datos[_tamanio] = T();
        }

        void eliminarRapido(int posicion)
        {
            _datos[posicion] = mover(_datos[_tamanio - 1]);
            --_tamanio;
            _datos[_tamanio] = T();
        }

        void quitarUltimo()
        {
            --_tamanio;
            _datos[_tamanio] = T();
        }

        void limpiar()
        {
            for (int i = 0; i < _tamanio; ++i)
                _datos[i] = T();
            _tamanio = 0;
        }

        void liberar()
        {
            delete[] _datos;
            _datos = nullptr;
            _tamanio = 0;
            _capacidad = 0;
        }

        void reservar(int nuevaCapacidad)
        {
            if (nuevaCapacidad <= _capacidad)
                return;
            T *nuevos = new T[nuevaCapacidad];
            for (int i = 0; i < _tamanio; ++i)
                nuevos[i] = mover(_datos[i]);
            delete[] _datos;
            _datos = nuevos;
            _capacidad = nuevaCapacidad;
        }

        void redimensionar(int nuevoTamanio)
        {
            reservar(nuevoTamanio);
            for (int i = _tamanio; i < nuevoTamanio; ++i)
                _datos[i] = T();
            for (int i = nuevoTamanio; i < _tamanio; ++i)
                _datos[i] = T();
            _tamanio = nuevoTamanio;
        }

        void redimensionar(int nuevoTamanio, const T &relleno)
        {
            reservar(nuevoTamanio);
            for (int i = _tamanio; i < nuevoTamanio; ++i)
                _datos[i] = relleno;
            for (int i = nuevoTamanio; i < _tamanio; ++i)
                _datos[i] = T();
            _tamanio = nuevoTamanio;
        }

        void rellenar(const T &valor)
        {
            for (int i = 0; i < _tamanio; ++i)
                _datos[i] = valor;
        }

        int buscar(const T &valor) const
        {
            for (int i = 0; i < _tamanio; ++i)
            {
                if (_datos[i] == valor)
                    return i;
            }
            return -1;
        }

        bool contiene(const T &valor) const { return buscar(valor) >= 0; }

        int buscarBinario(const T &valor) const
        {
            int izquierda = 0;
            int derecha = _tamanio - 1;
            while (izquierda <= derecha)
            {
                int medio = izquierda + (derecha - izquierda) / 2;
                if (_datos[medio] == valor)
                    return medio;
                if (_datos[medio] < valor)
                {
                    izquierda = medio + 1;
                }
                else
                {
                    derecha = medio - 1;
                }
            }
            return -1;
        }

        int posicionDeInsercion(const T &valor) const
        {
            int izquierda = 0;
            int derecha = _tamanio;
            while (izquierda < derecha)
            {
                int medio = izquierda + (derecha - izquierda) / 2;
                if (_datos[medio] < valor)
                {
                    izquierda = medio + 1;
                }
                else
                {
                    derecha = medio;
                }
            }
            return izquierda;
        }

        void insertarOrdenado(const T &valor)
        {
            insertarEn(posicionDeInsercion(valor), valor);
        }

    private:
        void crecerSiHaceFalta()
        {
            if (_tamanio == _capacidad)
            {
                reservar(_capacidad == 0 ? 4 : _capacidad * 2);
            }
        }

        T *_datos;
        int _tamanio;
        int _capacidad;
    };

}
