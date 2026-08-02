

#pragma once

#include "Utilidades.h"

namespace aed
{

    template <typename T>
    class ListaEnlazada
    {
    public:
        struct Nodo
        {
            T valor;
            Nodo *siguiente;

            Nodo() : valor(), siguiente(nullptr) {}
            explicit Nodo(const T &v) : valor(v), siguiente(nullptr) {}
        };

        ListaEnlazada() : _cabeza(nullptr), _cola(nullptr), _tamanio(0) {}

        ListaEnlazada(const ListaEnlazada &otra) : _cabeza(nullptr), _cola(nullptr), _tamanio(0)
        {
            for (Nodo *actual = otra._cabeza; actual != nullptr; actual = actual->siguiente)
            {
                agregarAlFinal(actual->valor);
            }
        }

        ListaEnlazada(ListaEnlazada &&otra) noexcept
            : _cabeza(otra._cabeza), _cola(otra._cola), _tamanio(otra._tamanio)
        {
            otra._cabeza = nullptr;
            otra._cola = nullptr;
            otra._tamanio = 0;
        }

        ~ListaEnlazada() { limpiar(); }

        ListaEnlazada &operator=(const ListaEnlazada &otra)
        {
            if (this == &otra)
                return *this;
            limpiar();
            for (Nodo *actual = otra._cabeza; actual != nullptr; actual = actual->siguiente)
            {
                agregarAlFinal(actual->valor);
            }
            return *this;
        }

        ListaEnlazada &operator=(ListaEnlazada &&otra) noexcept
        {
            if (this == &otra)
                return *this;
            limpiar();
            _cabeza = otra._cabeza;
            _cola = otra._cola;
            _tamanio = otra._tamanio;
            otra._cabeza = nullptr;
            otra._cola = nullptr;
            otra._tamanio = 0;
            return *this;
        }

        int tamanio() const { return _tamanio; }
        bool vacia() const { return _tamanio == 0; }
        Nodo *primero() { return _cabeza; }
        const Nodo *primero() const { return _cabeza; }

        void agregarAlFinal(const T &valor)
        {
            Nodo *nuevo = new Nodo(valor);
            if (_cola == nullptr)
            {
                _cabeza = nuevo;
                _cola = nuevo;
            }
            else
            {
                _cola->siguiente = nuevo;
                _cola = nuevo;
            }
            ++_tamanio;
        }

        void agregarAlInicio(const T &valor)
        {
            Nodo *nuevo = new Nodo(valor);
            nuevo->siguiente = _cabeza;
            _cabeza = nuevo;
            if (_cola == nullptr)
                _cola = nuevo;
            ++_tamanio;
        }

        template <typename Predicado>
        bool eliminarPrimeroQue(Predicado cumple)
        {
            Nodo *anterior = nullptr;
            Nodo *actual = _cabeza;
            while (actual != nullptr)
            {
                if (cumple(actual->valor))
                {
                    if (anterior == nullptr)
                    {
                        _cabeza = actual->siguiente;
                    }
                    else
                    {
                        anterior->siguiente = actual->siguiente;
                    }
                    if (actual == _cola)
                        _cola = anterior;
                    delete actual;
                    --_tamanio;
                    return true;
                }
                anterior = actual;
                actual = actual->siguiente;
            }
            return false;
        }

        void limpiar()
        {
            Nodo *actual = _cabeza;
            while (actual != nullptr)
            {
                Nodo *siguiente = actual->siguiente;
                delete actual;
                actual = siguiente;
            }
            _cabeza = nullptr;
            _cola = nullptr;
            _tamanio = 0;
        }

    private:
        Nodo *_cabeza;
        Nodo *_cola;
        int _tamanio;
    };

}
