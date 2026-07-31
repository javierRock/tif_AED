// ============================================================================
//  Cola.h
//  Cola FIFO implementada sobre un arreglo circular.
//
//  Es la estructura que hace posible el recorrido BFS del grafo de amistades
//  (busqueda del camino mas corto entre dos personas).
//
//  Por que circular y no una lista enlazada: en un BFS sobre un millon de
//  nodos se encolan y desencolan millones de enteros. Un arreglo contiguo
//  aprovecha la cache del procesador y evita un new/delete por elemento.
//
//  Complejidades: encolar() y desencolar() son O(1) amortizado.
// ============================================================================
#pragma once

#include "Utilidades.h"

namespace aed {

template <typename T>
class Cola {
public:
    Cola() : _datos(nullptr), _capacidad(0), _inicio(0), _cantidad(0) {}

    explicit Cola(int capacidadInicial)
        : _datos(nullptr), _capacidad(0), _inicio(0), _cantidad(0) {
        reservar(capacidadInicial);
    }

    Cola(const Cola& otra) : _datos(nullptr), _capacidad(0), _inicio(0), _cantidad(0) {
        reservar(otra._cantidad);
        for (int i = 0; i < otra._cantidad; ++i) {
            _datos[i] = otra._datos[(otra._inicio + i) % otra._capacidad];
        }
        _cantidad = otra._cantidad;
    }

    Cola& operator=(const Cola& otra) {
        if (this == &otra) return *this;
        delete[] _datos;
        _datos = nullptr;
        _capacidad = 0;
        _inicio = 0;
        _cantidad = 0;
        reservar(otra._cantidad);
        for (int i = 0; i < otra._cantidad; ++i) {
            _datos[i] = otra._datos[(otra._inicio + i) % otra._capacidad];
        }
        _cantidad = otra._cantidad;
        return *this;
    }

    ~Cola() { delete[] _datos; }

    // ---- Consulta ---------------------------------------------------------
    bool vacia() const { return _cantidad == 0; }
    int tamanio() const { return _cantidad; }
    int capacidad() const { return _capacidad; }

    const T& frente() const { return _datos[_inicio]; }

    // ---- Modificacion -----------------------------------------------------
    void encolar(const T& valor) {
        if (_cantidad == _capacidad) {
            reservar(_capacidad == 0 ? 16 : _capacidad * 2);
        }
        // El modulo hace que al llegar al final del arreglo se vuelva a
        // escribir al principio: de ahi el nombre "circular".
        int posicion = (_inicio + _cantidad) % _capacidad;
        _datos[posicion] = valor;
        ++_cantidad;
    }

    T desencolar() {
        T valor = mover(_datos[_inicio]);
        _inicio = (_inicio + 1) % _capacidad;
        --_cantidad;
        return valor;
    }

    /// Vacia la cola SIN liberar la memoria reservada. Asi el mismo objeto
    /// se reutiliza en miles de recorridos BFS sin volver a pedir memoria.
    void limpiar() {
        _inicio = 0;
        _cantidad = 0;
    }

    void reservar(int nuevaCapacidad) {
        if (nuevaCapacidad <= _capacidad) return;
        T* nuevos = new T[nuevaCapacidad];
        for (int i = 0; i < _cantidad; ++i) {
            nuevos[i] = mover(_datos[(_inicio + i) % _capacidad]);
        }
        delete[] _datos;
        _datos = nuevos;
        _capacidad = nuevaCapacidad;
        _inicio = 0;
    }

private:
    T* _datos;
    int _capacidad;
    int _inicio;    // posicion del primer elemento
    int _cantidad;  // cuantos elementos hay guardados
};

}  // namespace aed
