










#pragma once

#include "Arreglo.h"

namespace aed {

template <typename T>
class Pila {
public:
    Pila() {}

    void apilar(const T& valor) { _datos.agregar(valor); }

    T desapilar() {
        T valor = mover(_datos.ultimo());
        _datos.quitarUltimo();
        return valor;
    }

    const T& cima() const { return _datos.ultimo(); }

    bool vacia() const { return _datos.vacio(); }
    int tamanio() const { return _datos.tamanio(); }
    void limpiar() { _datos.limpiar(); }

private:
    Arreglo<T> _datos;
};

}
