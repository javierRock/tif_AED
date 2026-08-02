







#pragma once

namespace aed {





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








template <typename T>
typename QuitarReferencia<T>::Tipo&& mover(T&& valor) {
    return static_cast<typename QuitarReferencia<T>::Tipo&&>(valor);
}




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

}
