// ============================================================================
//  Fecha.h
//  Fecha simple (dia / mes / anio) usada en el registro de usuarios y en la
//  publicacion de contenidos.
//
//  Se guarda en tipos pequenos (short y unsigned char) porque este objeto se
//  repite en cada usuario, cada publicacion y cada comentario: a escala de
//  millones de registros, cada byte ahorrado se multiplica.
// ============================================================================
#pragma once

#include "../estructuras/Cadena.h"

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

    /// Convierte la fecha a un unico numero comparable con forma AAAAMMDD.
    /// Comparar enteros es mas rapido y sencillo que comparar tres campos.
    int comoNumero() const { return anio * 10000 + mes * 100 + dia; }

    /// Devuelve el texto en formato ISO "AAAA-MM-DD".
    Cadena aTexto() const;

    /// Interpreta un texto "AAAA-MM-DD". Si el formato no es valido devuelve
    /// la fecha por defecto.
    static Fecha desdeTexto(const Cadena& texto);
};

inline bool operator<(const Fecha& a, const Fecha& b) {
    return a.comoNumero() < b.comoNumero();
}

inline bool operator==(const Fecha& a, const Fecha& b) {
    return a.comoNumero() == b.comoNumero();
}

}  // namespace aed
