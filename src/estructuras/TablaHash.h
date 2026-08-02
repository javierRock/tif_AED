


























#pragma once

#include "String.h"
#include "Utilidades.h"

namespace aed {








inline unsigned long long mezclarBits(unsigned long long valor) {
    valor += 0x9E3779B97F4A7C15ULL;
    valor = (valor ^ (valor >> 30)) * 0xBF58476D1CE4E5B9ULL;
    valor = (valor ^ (valor >> 27)) * 0x94D049BB133111EBULL;
    return valor ^ (valor >> 31);
}

inline unsigned long long calcularHash(int clave) {
    return mezclarBits(static_cast<unsigned long long>(clave));
}

inline unsigned long long calcularHash(long long clave) {
    return mezclarBits(static_cast<unsigned long long>(clave));
}

inline unsigned long long calcularHash(const String& clave) {
    return clave.codigoHash();
}




inline int siguientePotenciaDeDos(int valor) {
    int resultado = 8;
    while (resultado < valor) resultado *= 2;
    return resultado;
}




template <typename Clave, typename Valor>
class TablaHash {
public:
    enum EstadoCasilla : unsigned char {
        VACIA = 0,
        OCUPADA = 1,
        BORRADA = 2
    };

    struct Casilla {
        Clave clave;
        Valor valor;
        unsigned char estado = VACIA;
    };

    explicit TablaHash(int capacidadInicial = 16)
        : _casillas(nullptr), _capacidad(0), _tamanio(0), _borradas(0) {
        _capacidad = siguientePotenciaDeDos(capacidadInicial);
        _casillas = new Casilla[_capacidad];
    }

    ~TablaHash() { delete[] _casillas; }



    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;


    int tamanio() const { return _tamanio; }
    int capacidad() const { return _capacidad; }
    bool vacia() const { return _tamanio == 0; }

    double factorCarga() const {
        return static_cast<double>(_tamanio + _borradas) / static_cast<double>(_capacidad);
    }


    Valor* buscar(const Clave& clave) {
        int posicion = posicionDe(clave);
        return (posicion < 0) ? nullptr : &_casillas[posicion].valor;
    }

    const Valor* buscar(const Clave& clave) const {
        int posicion = posicionDe(clave);
        return (posicion < 0) ? nullptr : &_casillas[posicion].valor;
    }

    bool contiene(const Clave& clave) const { return posicionDe(clave) >= 0; }



    bool insertar(const Clave& clave, const Valor& valor) {



        if (factorCarga() >= 0.70) {
            redimensionar(_capacidad * 2);
        }

        unsigned long long codigo = calcularHash(clave);
        int posicion = static_cast<int>(codigo & static_cast<unsigned long long>(_capacidad - 1));
        int primeraTumba = -1;

        while (_casillas[posicion].estado != VACIA) {
            if (_casillas[posicion].estado == OCUPADA) {
                if (_casillas[posicion].clave == clave) {
                    _casillas[posicion].valor = valor;
                    return false;
                }
            } else if (primeraTumba < 0) {
                primeraTumba = posicion;
            }
            posicion = (posicion + 1) & (_capacidad - 1);
        }

        if (primeraTumba >= 0) {
            posicion = primeraTumba;
            --_borradas;
        }
        _casillas[posicion].clave = clave;
        _casillas[posicion].valor = valor;
        _casillas[posicion].estado = OCUPADA;
        ++_tamanio;
        return true;
    }


    bool eliminar(const Clave& clave) {
        int posicion = posicionDe(clave);
        if (posicion < 0) return false;
        _casillas[posicion].estado = BORRADA;
        _casillas[posicion].clave = Clave();
        _casillas[posicion].valor = Valor();
        --_tamanio;
        ++_borradas;
        return true;
    }



    Valor& obtenerOCrear(const Clave& clave, const Valor& valorInicial) {
        Valor* existente = buscar(clave);
        if (existente != nullptr) return *existente;
        insertar(clave, valorInicial);
        return *buscar(clave);
    }

    void limpiar() {
        delete[] _casillas;
        _casillas = new Casilla[_capacidad];
        _tamanio = 0;
        _borradas = 0;
    }



    void reservarPara(int cantidadEsperada) {
        int necesaria = siguientePotenciaDeDos(static_cast<int>(cantidadEsperada / 0.70) + 1);
        if (necesaria > _capacidad) redimensionar(necesaria);
    }




    bool casillaOcupada(int posicion) const { return _casillas[posicion].estado == OCUPADA; }
    const Clave& claveEn(int posicion) const { return _casillas[posicion].clave; }
    const Valor& valorEn(int posicion) const { return _casillas[posicion].valor; }
    Valor& valorEn(int posicion) { return _casillas[posicion].valor; }

private:

    int posicionDe(const Clave& clave) const {
        unsigned long long codigo = calcularHash(clave);
        int posicion = static_cast<int>(codigo & static_cast<unsigned long long>(_capacidad - 1));
        int recorridas = 0;

        while (_casillas[posicion].estado != VACIA) {
            if (_casillas[posicion].estado == OCUPADA && _casillas[posicion].clave == clave) {
                return posicion;
            }
            posicion = (posicion + 1) & (_capacidad - 1);
            if (++recorridas >= _capacidad) break;
        }
        return -1;
    }



    void redimensionar(int nuevaCapacidad) {
        Casilla* antiguas = _casillas;
        int capacidadAntigua = _capacidad;

        _capacidad = siguientePotenciaDeDos(nuevaCapacidad);
        _casillas = new Casilla[_capacidad];
        _tamanio = 0;
        _borradas = 0;

        for (int i = 0; i < capacidadAntigua; ++i) {
            if (antiguas[i].estado == OCUPADA) {
                insertarSinControl(antiguas[i].clave, antiguas[i].valor);
            }
        }
        delete[] antiguas;
    }



    void insertarSinControl(const Clave& clave, const Valor& valor) {
        unsigned long long codigo = calcularHash(clave);
        int posicion = static_cast<int>(codigo & static_cast<unsigned long long>(_capacidad - 1));
        while (_casillas[posicion].estado == OCUPADA) {
            posicion = (posicion + 1) & (_capacidad - 1);
        }
        _casillas[posicion].clave = clave;
        _casillas[posicion].valor = valor;
        _casillas[posicion].estado = OCUPADA;
        ++_tamanio;
    }

    Casilla* _casillas;
    int _capacidad;
    int _tamanio;
    int _borradas;
};

}
