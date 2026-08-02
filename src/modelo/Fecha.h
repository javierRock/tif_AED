








#pragma once

#include "../estructuras/String.h"

namespace aed {

struct Fecha {
    short anio = 2026;
    unsigned char mes = 1;
    unsigned char dia = 1;

    Fecha() = default;
    Fecha(int a, int m, int d)
        : anio(static_cast<short>(a)),
          mes(static_cast<unsigned char>(m)),
          dia(static_cast<unsigned char>(d)) {}



    int comoNumero() const { return anio * 10000 + mes * 100 + dia; }


    String aTexto() const;



    static Fecha desdeTexto(const String& texto);
};

inline bool operator<(const Fecha& a, const Fecha& b) {
    return a.comoNumero() < b.comoNumero();
}

inline bool operator==(const Fecha& a, const Fecha& b) {
    return a.comoNumero() == b.comoNumero();
}

}
