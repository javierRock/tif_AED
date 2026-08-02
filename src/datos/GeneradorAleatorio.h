










#pragma once

namespace aed {

class GeneradorAleatorio {
public:
    explicit GeneradorAleatorio(unsigned long long semilla = 20260730ULL) {

        _estado = (semilla == 0) ? 88172645463325252ULL : semilla;
    }


    unsigned long long siguiente() {
        _estado ^= _estado << 13;
        _estado ^= _estado >> 7;
        _estado ^= _estado << 17;
        return _estado * 2685821657736338717ULL;
    }


    int enteroMenorQue(int limite) {
        if (limite <= 0) return 0;
        return static_cast<int>(siguiente() % static_cast<unsigned long long>(limite));
    }


    int enteroEntre(int minimo, int maximo) {
        if (maximo <= minimo) return minimo;
        return minimo + enteroMenorQue(maximo - minimo + 1);
    }


    double real01() {

        return static_cast<double>(siguiente() >> 11) / 9007199254740992.0;
    }







    int sesgadoHaciaCero(int maximo) {
        double u = real01();
        return static_cast<int>(u * u * u * static_cast<double>(maximo));
    }

private:
    unsigned long long _estado;
};

}
