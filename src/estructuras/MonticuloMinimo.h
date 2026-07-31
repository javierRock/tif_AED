// ============================================================================
//  MonticuloMinimo.h
//  Monticulo binario de minimos (cola de prioridad propia, reemplaza a
//  std::priority_queue).
//
//  Se guarda en un arreglo: para el nodo de la posicion i,
//      padre        -> (i - 1) / 2
//      hijo izq.    -> 2i + 1
//      hijo der.    -> 2i + 2
//  Asi no hacen falta punteros y el recorrido aprovecha la cache.
//
//  PARA QUE LO USAMOS: rankings de "top K".
//  Para hallar los 10 usuarios mas activos entre un millon NO hace falta
//  ordenar el millon (O(n log n)). Basta mantener un monticulo de MINIMOS con
//  solo K elementos: si el candidato supera al peor de los K guardados, entra
//  y expulsa al peor. Coste O(n log K), y memoria O(K) en lugar de O(n).
//
//  Complejidades:
//      insertar()      -> O(log n)
//      extraerMinimo() -> O(log n)
//      minimo()        -> O(1)
// ============================================================================
#pragma once

#include "Arreglo.h"
#include "Ordenamiento.h"

namespace aed {

template <typename T>
class MonticuloMinimo {
public:
    MonticuloMinimo() {}
    explicit MonticuloMinimo(int capacidadInicial) : _datos(capacidadInicial) {}

    int tamanio() const { return _datos.tamanio(); }
    bool vacio() const { return _datos.vacio(); }

    /// El minimo siempre esta en la raiz, es decir en la posicion 0.
    const T& minimo() const { return _datos[0]; }

    void insertar(const T& valor) {
        _datos.agregar(valor);
        subir(_datos.tamanio() - 1);
    }

    T extraerMinimo() {
        T resultado = mover(_datos[0]);
        _datos[0] = mover(_datos[_datos.tamanio() - 1]);
        _datos.quitarUltimo();
        if (!_datos.vacio()) bajar(0);
        return resultado;
    }

    /// Operacion clave del "top K": si el candidato es mejor que el peor
    /// elemento guardado, lo reemplaza. Es un extraer + insertar en una sola
    /// bajada, por lo que cuesta O(log K) en vez de 2 * O(log K).
    void reemplazarMinimo(const T& valor) {
        _datos[0] = valor;
        bajar(0);
    }

    void limpiar() { _datos.limpiar(); }

    /// Vacia el monticulo dejando sus elementos ordenados de mayor a menor
    /// dentro de 'salida'. Se usa para presentar los rankings ya listos.
    void volcarDeMayorAMenor(Arreglo<T>& salida) {
        salida.limpiar();
        salida.reservar(_datos.tamanio());
        while (!vacio()) {
            salida.agregar(extraerMinimo());  // sale de menor a mayor...
        }
        // ...por eso lo invertimos al final.
        for (int i = 0, j = salida.tamanio() - 1; i < j; ++i, --j) {
            intercambiar(salida[i], salida[j]);
        }
    }

private:
    /// Sube el elemento mientras sea menor que su padre.
    void subir(int posicion) {
        while (posicion > 0) {
            int padre = (posicion - 1) / 2;
            if (!(_datos[posicion] < _datos[padre])) break;
            intercambiar(_datos[posicion], _datos[padre]);
            posicion = padre;
        }
    }

    /// Hunde el elemento intercambiandolo con el menor de sus hijos.
    void bajar(int posicion) {
        int cantidad = _datos.tamanio();
        while (true) {
            int izquierdo = 2 * posicion + 1;
            int derecho = 2 * posicion + 2;
            int menor = posicion;

            if (izquierdo < cantidad && _datos[izquierdo] < _datos[menor]) menor = izquierdo;
            if (derecho < cantidad && _datos[derecho] < _datos[menor]) menor = derecho;
            if (menor == posicion) break;

            intercambiar(_datos[posicion], _datos[menor]);
            posicion = menor;
        }
    }

    Arreglo<T> _datos;
};

}  // namespace aed
