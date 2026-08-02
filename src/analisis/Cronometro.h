








#pragma once

#include <chrono>

namespace aed {

class Cronometro {
public:
    Cronometro() { iniciar(); }

    void iniciar() { _inicio = std::chrono::steady_clock::now(); }

    double milisegundos() const {
        std::chrono::duration<double, std::milli> transcurrido =
            std::chrono::steady_clock::now() - _inicio;
        return transcurrido.count();
    }

    double segundos() const { return milisegundos() / 1000.0; }

private:
    std::chrono::steady_clock::time_point _inicio;
};

}
