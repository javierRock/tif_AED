#include "Fecha.h"

#include <cstdio>

namespace aed {

String Fecha::aTexto() const {
    char temporal[16];
    std::snprintf(temporal, sizeof(temporal), "%04d-%02d-%02d",
                  static_cast<int>(anio), static_cast<int>(mes), static_cast<int>(dia));
    return String(temporal);
}

Fecha Fecha::desdeTexto(const String& texto) {

    if (texto.longitud() < 10) return Fecha();

    int valores[3] = {0, 0, 0};
    int campo = 0;
    int acumulado = 0;

    for (int i = 0; i < texto.longitud() && campo < 3; ++i) {
        char c = texto[i];
        if (c >= '0' && c <= '9') {
            acumulado = acumulado * 10 + (c - '0');
        } else if (c == '-') {
            valores[campo++] = acumulado;
            acumulado = 0;
        }
    }
    if (campo < 3) valores[campo] = acumulado;

    return Fecha(valores[0], valores[1], valores[2]);
}

}
