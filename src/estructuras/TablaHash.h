// ============================================================================
//  TablaHash.h
//  Tabla de dispersion con DIRECCIONAMIENTO ABIERTO y SONDEO LINEAL.
//  (equivalente propio de std::unordered_map)
//
//  Es el indice principal del sistema: traduce "id de usuario" o "correo"
//  a la posicion real dentro del arreglo de usuarios, en tiempo O(1) promedio.
//
//  POR QUE DIRECCIONAMIENTO ABIERTO Y NO ENCADENAMIENTO
//  ----------------------------------------------------
//  Con encadenamiento cada elemento necesita un nodo con un puntero: para un
//  millon de usuarios eso son un millon de llamadas a new y un salto de
//  memoria por consulta. Con direccionamiento abierto todo vive en un unico
//  arreglo contiguo, las colisiones se resuelven mirando la casilla de al
//  lado (que ya esta en la cache del procesador) y no hay ni un solo puntero.
//
//  EL PROBLEMA DE BORRAR Y LA SOLUCION "TUMBA"
//  -------------------------------------------
//  Si al borrar dejaramos la casilla VACIA, cortariamos la cadena de sondeo y
//  las claves que colisionaron despues se volverian invisibles. Por eso la
//  casilla se marca como BORRADA ("tumba"): la busqueda la atraviesa, pero la
//  insercion puede reutilizarla.
//
//  Complejidades (con factor de carga <= 0.70):
//      insertar / buscar / eliminar -> O(1) promedio
//      redimensionar                -> O(n), amortizado a O(1) por insercion
// ============================================================================
#pragma once

#include "Cadena.h"
#include "Utilidades.h"

namespace aed {

// ---------------------------------------------------------------------------
// Funciones de dispersion para los tipos de clave que usa el proyecto.
// ---------------------------------------------------------------------------

/// Mezclador de bits de splitmix64. Los ids de usuario son consecutivos
/// (0, 1, 2, ...) y sin mezclar se agruparian en zonas contiguas de la tabla;
/// esparcirlos reduce muchisimo las colisiones.
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

inline unsigned long long calcularHash(const Cadena& clave) {
    return clave.codigoHash();
}

/// Redondea hacia arriba a la siguiente potencia de dos.
/// Trabajar con potencias de dos permite reemplazar el costoso operador '%'
/// por una mascara de bits: posicion & (capacidad - 1).
inline int siguientePotenciaDeDos(int valor) {
    int resultado = 8;
    while (resultado < valor) resultado *= 2;
    return resultado;
}

// ---------------------------------------------------------------------------
// TablaHash
// ---------------------------------------------------------------------------
template <typename Clave, typename Valor>
class TablaHash {
public:
    enum EstadoCasilla : unsigned char {
        VACIA = 0,   // nunca se uso
        OCUPADA = 1, // contiene un par clave/valor valido
        BORRADA = 2  // "tumba": se uso y se elimino
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

    // Copiar un indice de un millon de entradas casi nunca es lo que se
    // quiere, asi que lo prohibimos explicitamente para evitar errores.
    TablaHash(const TablaHash&) = delete;
    TablaHash& operator=(const TablaHash&) = delete;

    // ---- Consulta ---------------------------------------------------------
    int tamanio() const { return _tamanio; }
    int capacidad() const { return _capacidad; }
    bool vacia() const { return _tamanio == 0; }

    double factorCarga() const {
        return static_cast<double>(_tamanio + _borradas) / static_cast<double>(_capacidad);
    }

    /// Devuelve un puntero al valor asociado a la clave, o nullptr si no existe.
    Valor* buscar(const Clave& clave) {
        int posicion = posicionDe(clave);
        return (posicion < 0) ? nullptr : &_casillas[posicion].valor;
    }

    const Valor* buscar(const Clave& clave) const {
        int posicion = posicionDe(clave);
        return (posicion < 0) ? nullptr : &_casillas[posicion].valor;
    }

    bool contiene(const Clave& clave) const { return posicionDe(clave) >= 0; }

    // ---- Modificacion -----------------------------------------------------
    /// Inserta o actualiza. Devuelve true si la clave era nueva.
    bool insertar(const Clave& clave, const Valor& valor) {
        // El umbral 0.70 es el punto de equilibrio clasico del sondeo lineal:
        // por encima, los grupos de casillas ocupadas crecen y las busquedas
        // dejan de ser practicamente O(1).
        if (factorCarga() >= 0.70) {
            redimensionar(_capacidad * 2);
        }

        unsigned long long codigo = calcularHash(clave);
        int posicion = static_cast<int>(codigo & static_cast<unsigned long long>(_capacidad - 1));
        int primeraTumba = -1;

        while (_casillas[posicion].estado != VACIA) {
            if (_casillas[posicion].estado == OCUPADA) {
                if (_casillas[posicion].clave == clave) {
                    _casillas[posicion].valor = valor;  // ya existia: se actualiza
                    return false;
                }
            } else if (primeraTumba < 0) {
                primeraTumba = posicion;  // recordamos la tumba para reutilizarla
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

    /// Marca la casilla como tumba. Devuelve true si la clave existia.
    bool eliminar(const Clave& clave) {
        int posicion = posicionDe(clave);
        if (posicion < 0) return false;
        _casillas[posicion].estado = BORRADA;
        _casillas[posicion].clave = Clave();  // libera memoria si la clave es Cadena
        _casillas[posicion].valor = Valor();
        --_tamanio;
        ++_borradas;
        return true;
    }

    /// Devuelve una referencia al valor de la clave, creandola con
    /// 'valorInicial' si aun no existia. Util para contar ocurrencias.
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

    /// Reserva de antemano espacio para 'cantidadEsperada' elementos, evitando
    /// las redimensiones sucesivas durante una carga masiva de datos.
    void reservarPara(int cantidadEsperada) {
        int necesaria = siguientePotenciaDeDos(static_cast<int>(cantidadEsperada / 0.70) + 1);
        if (necesaria > _capacidad) redimensionar(necesaria);
    }

    // ---- Recorrido --------------------------------------------------------
    // Se expone el arreglo interno por posiciones para poder recorrer todas
    // las entradas sin necesidad de construir iteradores.
    bool casillaOcupada(int posicion) const { return _casillas[posicion].estado == OCUPADA; }
    const Clave& claveEn(int posicion) const { return _casillas[posicion].clave; }
    const Valor& valorEn(int posicion) const { return _casillas[posicion].valor; }
    Valor& valorEn(int posicion) { return _casillas[posicion].valor; }

private:
    /// Sondeo lineal: devuelve la posicion de la clave o -1 si no esta.
    int posicionDe(const Clave& clave) const {
        unsigned long long codigo = calcularHash(clave);
        int posicion = static_cast<int>(codigo & static_cast<unsigned long long>(_capacidad - 1));
        int recorridas = 0;

        while (_casillas[posicion].estado != VACIA) {
            if (_casillas[posicion].estado == OCUPADA && _casillas[posicion].clave == clave) {
                return posicion;
            }
            posicion = (posicion + 1) & (_capacidad - 1);
            if (++recorridas >= _capacidad) break;  // proteccion ante tabla llena
        }
        return -1;
    }

    /// Reconstruye la tabla en un arreglo mas grande. Ademas de dar espacio,
    /// esto ELIMINA todas las tumbas acumuladas.
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

    /// Insercion usada solo durante la redimension: se sabe que la clave no
    /// esta repetida y que hay espacio, asi que se omiten esas comprobaciones.
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
    int _capacidad;  // siempre potencia de dos
    int _tamanio;    // casillas OCUPADAS
    int _borradas;   // casillas BORRADAS (tumbas)
};

}  // namespace aed
