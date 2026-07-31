// ============================================================================
//  Pila.h
//  Pila LIFO construida sobre nuestro Arreglo dinamico.
//
//  Se usa para reconstruir el camino de amistad: el BFS descubre el camino
//  desde el destino hacia el origen siguiendo los "padres", de modo que hay
//  que invertirlo. Apilar y luego desapilar hace esa inversion de forma
//  natural y sin codigo adicional.
//
//  Complejidades: apilar() y desapilar() son O(1) amortizado.
// ============================================================================
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

}  // namespace aed
