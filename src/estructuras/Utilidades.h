// ============================================================================
//  Utilidades.h
//  Funciones auxiliares genericas escritas desde cero.
//
//  Motivo: el enunciado prohibe usar estructuras de datos de la STL. Para no
//  depender siquiera de <utility> o <algorithm> definimos aqui nuestras
//  propias versiones de mover(), intercambiar(), minimo() y maximo().
// ============================================================================
#pragma once

namespace aed {

// ---------------------------------------------------------------------------
// QuitarReferencia: pequeno rasgo de tipo que elimina '&' o '&&' de un tipo.
// Es la pieza que necesita mover() para funcionar igual que std::move.
// ---------------------------------------------------------------------------
template <typename T>
struct QuitarReferencia {
    using Tipo = T;
};

template <typename T>
struct QuitarReferencia<T&> {
    using Tipo = T;
};

template <typename T>
struct QuitarReferencia<T&&> {
    using Tipo = T;
};

// ---------------------------------------------------------------------------
// mover(): convierte una variable en una "referencia de movimiento".
//
// Sirve para que, al hacer crecer un arreglo, los objetos se TRASLADEN
// (robandoles el puntero interno) en lugar de copiarse byte a byte.
// Con un millon de usuarios la diferencia es enorme.
// ---------------------------------------------------------------------------
template <typename T>
typename QuitarReferencia<T>::Tipo&& mover(T&& valor) {
    return static_cast<typename QuitarReferencia<T>::Tipo&&>(valor);
}

// ---------------------------------------------------------------------------
// intercambiar(): permuta el contenido de dos variables usando movimientos.
// ---------------------------------------------------------------------------
template <typename T>
void intercambiar(T& a, T& b) {
    T temporal = mover(a);
    a = mover(b);
    b = mover(temporal);
}

template <typename T>
const T& minimo(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template <typename T>
const T& maximo(const T& a, const T& b) {
    return (a < b) ? b : a;
}

// ---------------------------------------------------------------------------
// Comparadores por defecto usados por los algoritmos de ordenamiento.
// Encapsular la comparacion en un "functor" permite reutilizar el mismo
// quicksort para ordenar de forma ascendente, descendente o por otro criterio.
// ---------------------------------------------------------------------------
struct MenorQue {
    template <typename T>
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};

struct MayorQue {
    template <typename T>
    bool operator()(const T& a, const T& b) const {
        return b < a;
    }
};

}  // namespace aed
