// ============================================================================
//  GeneradorAleatorio.h
//  Generador de numeros pseudoaleatorios propio (algoritmo xorshift64*).
//
//  Se implementa desde cero en lugar de usar <random> por dos motivos:
//      1. Es REPRODUCIBLE: con la misma semilla se obtiene exactamente el
//         mismo conjunto de datos en cualquier maquina, lo que hace que las
//         mediciones de rendimiento sean comparables.
//      2. Es extremadamente rapido (tres desplazamientos y una multiplicacion),
//         algo importante cuando hay que generar decenas de millones de valores.
// ============================================================================
#pragma once

namespace aed {

class GeneradorAleatorio {
public:
    explicit GeneradorAleatorio(unsigned long long semilla = 20260730ULL) {
        // El estado nunca puede ser cero: xorshift se quedaria atascado en 0.
        _estado = (semilla == 0) ? 88172645463325252ULL : semilla;
    }

    /// Siguiente valor de 64 bits.
    unsigned long long siguiente() {
        _estado ^= _estado << 13;
        _estado ^= _estado >> 7;
        _estado ^= _estado << 17;
        return _estado * 2685821657736338717ULL;
    }

    /// Entero uniforme en el rango [0, limite).
    int enteroMenorQue(int limite) {
        if (limite <= 0) return 0;
        return static_cast<int>(siguiente() % static_cast<unsigned long long>(limite));
    }

    /// Entero uniforme en el rango [minimo, maximo], ambos incluidos.
    int enteroEntre(int minimo, int maximo) {
        if (maximo <= minimo) return minimo;
        return minimo + enteroMenorQue(maximo - minimo + 1);
    }

    /// Real uniforme en [0, 1).
    double real01() {
        // Se toman los 53 bits altos, que son los de mayor calidad.
        return static_cast<double>(siguiente() >> 11) / 9007199254740992.0;
    }

    /// Valor sesgado hacia los numeros pequenos dentro de [0, maximo].
    ///
    /// Elevar un uniforme al cubo concentra la mayoria de resultados cerca de
    /// cero y deja unos pocos valores muy altos. Se usa para las reacciones:
    /// en una red social real casi todas las publicaciones tienen pocos likes
    /// y unas poquisimas se vuelven virales.
    int sesgadoHaciaCero(int maximo) {
        double u = real01();
        return static_cast<int>(u * u * u * static_cast<double>(maximo));
    }

private:
    unsigned long long _estado;
};

}  // namespace aed
