// ============================================================================
//  ArbolAVL.h
//  Arbol binario de busqueda AUTOBALANCEADO (AVL).
//
//  Invariante AVL: para todo nodo, la diferencia de altura entre su subarbol
//  izquierdo y el derecho nunca supera 1. Eso garantiza altura O(log n) y por
//  lo tanto busquedas, inserciones y borrados en O(log n) en el PEOR caso
//  (un arbol de busqueda sin balancear degenera en una lista de O(n)).
//
//  PARA QUE LO USAMOS: indice de usuarios por nombre.
//  A diferencia de la tabla hash, el AVL mantiene las claves ORDENADAS, lo que
//  permite dos cosas que una tabla hash no puede hacer:
//      1. Listar usuarios en orden alfabetico (recorrido en orden).
//      2. Consultar rangos ("todos los nombres entre X e Y").
//  En el banco de pruebas se compara su tiempo de busqueda contra el de la
//  tabla hash para mostrar la diferencia entre O(log n) y O(1).
//
//  Cada clave guarda un VALOR asociado; como varios usuarios pueden llamarse
//  igual, en la practica el valor sera un Arreglo<int> con sus indices.
// ============================================================================
#pragma once

#include "Utilidades.h"

namespace aed {

template <typename Clave, typename Valor>
class ArbolAVL {
public:
    struct Nodo {
        Clave clave;
        Valor valor;
        Nodo* izquierda;
        Nodo* derecha;
        int altura;

        Nodo(const Clave& c, const Valor& v)
            : clave(c), valor(v), izquierda(nullptr), derecha(nullptr), altura(1) {}
    };

    ArbolAVL() : _raiz(nullptr), _tamanio(0) {}
    ~ArbolAVL() { liberar(_raiz); }

    ArbolAVL(const ArbolAVL&) = delete;
    ArbolAVL& operator=(const ArbolAVL&) = delete;

    int tamanio() const { return _tamanio; }
    bool vacio() const { return _tamanio == 0; }
    int altura() const { return alturaDe(_raiz); }

    // ---- Busqueda ---------------------------------------------------------
    /// O(log n). Devuelve puntero al valor o nullptr.
    Valor* buscar(const Clave& clave) {
        Nodo* actual = _raiz;
        while (actual != nullptr) {
            if (clave < actual->clave) {
                actual = actual->izquierda;
            } else if (actual->clave < clave) {
                actual = actual->derecha;
            } else {
                return &actual->valor;
            }
        }
        return nullptr;
    }

    const Valor* buscar(const Clave& clave) const {
        return const_cast<ArbolAVL*>(this)->buscar(clave);
    }

    bool contiene(const Clave& clave) const { return buscar(clave) != nullptr; }

    // ---- Insercion --------------------------------------------------------
    /// Inserta la clave (o sustituye su valor si ya existia). O(log n).
    void insertar(const Clave& clave, const Valor& valor) {
        _raiz = insertarEn(_raiz, clave, valor);
    }

    /// Devuelve una referencia al valor de la clave, creandolo con
    /// 'valorInicial' si la clave todavia no estaba. O(log n).
    Valor& obtenerOInsertar(const Clave& clave, const Valor& valorInicial) {
        Valor* existente = buscar(clave);
        if (existente != nullptr) return *existente;
        _raiz = insertarEn(_raiz, clave, valorInicial);
        return *buscar(clave);
    }

    // ---- Eliminacion ------------------------------------------------------
    /// O(log n). Devuelve true si la clave existia.
    bool eliminar(const Clave& clave) {
        bool seElimino = false;
        _raiz = eliminarEn(_raiz, clave, seElimino);
        return seElimino;
    }

    // ---- Recorrido --------------------------------------------------------
    /// Recorrido EN ORDEN (izquierda -> nodo -> derecha): visita las claves
    /// de menor a mayor, es decir en orden alfabetico.
    ///
    /// 'accion' es cualquier objeto con operator()(clave, valor) que devuelve
    /// bool: si devuelve false el recorrido se detiene (util para "los
    /// primeros 20 usuarios en orden alfabetico" sin recorrer el millon).
    template <typename Accion>
    void recorrerEnOrden(Accion& accion) const {
        recorrer(_raiz, accion);
    }

private:
    static int alturaDe(Nodo* nodo) { return nodo == nullptr ? 0 : nodo->altura; }

    static int factorDeBalance(Nodo* nodo) {
        return nodo == nullptr ? 0 : alturaDe(nodo->izquierda) - alturaDe(nodo->derecha);
    }

    static void actualizarAltura(Nodo* nodo) {
        int izq = alturaDe(nodo->izquierda);
        int der = alturaDe(nodo->derecha);
        nodo->altura = 1 + (izq > der ? izq : der);
    }

    // -- Rotaciones ---------------------------------------------------------
    /*
     *        y                                  x
     *       / \       rotacionDerecha(y)       / \
     *      x   C      ------------------>     A   y
     *     / \                                    / \
     *    A   B                                  B   C
     *
     * El subarbol B "cambia de padre": era el hijo derecho de x y pasa a ser
     * el hijo izquierdo de y. El recorrido en orden (A, x, B, y, C) no varia,
     * que es justo lo que garantiza que el arbol sigue siendo de busqueda.
     */
    static Nodo* rotacionDerecha(Nodo* y) {
        Nodo* x = y->izquierda;
        Nodo* B = x->derecha;
        x->derecha = y;
        y->izquierda = B;
        actualizarAltura(y);
        actualizarAltura(x);
        return x;
    }

    /*
     *      x                                      y
     *     / \       rotacionIzquierda(x)         / \
     *    A   y      ------------------->        x   C
     *       / \                                / \
     *      B   C                              A   B
     *
     * Es la operacion simetrica de la anterior.
     */
    static Nodo* rotacionIzquierda(Nodo* x) {
        Nodo* y = x->derecha;
        Nodo* B = y->izquierda;
        y->izquierda = x;
        x->derecha = B;
        actualizarAltura(x);
        actualizarAltura(y);
        return y;
    }

    /// Restaura el invariante AVL en un nodo aplicando la rotacion que toque.
    static Nodo* rebalancear(Nodo* nodo) {
        actualizarAltura(nodo);
        int balance = factorDeBalance(nodo);

        // Caso Izquierda-Izquierda
        if (balance > 1 && factorDeBalance(nodo->izquierda) >= 0) {
            return rotacionDerecha(nodo);
        }
        // Caso Izquierda-Derecha
        if (balance > 1) {
            nodo->izquierda = rotacionIzquierda(nodo->izquierda);
            return rotacionDerecha(nodo);
        }
        // Caso Derecha-Derecha
        if (balance < -1 && factorDeBalance(nodo->derecha) <= 0) {
            return rotacionIzquierda(nodo);
        }
        // Caso Derecha-Izquierda
        if (balance < -1) {
            nodo->derecha = rotacionDerecha(nodo->derecha);
            return rotacionIzquierda(nodo);
        }
        return nodo;
    }

    Nodo* insertarEn(Nodo* nodo, const Clave& clave, const Valor& valor) {
        if (nodo == nullptr) {
            ++_tamanio;
            return new Nodo(clave, valor);
        }
        if (clave < nodo->clave) {
            nodo->izquierda = insertarEn(nodo->izquierda, clave, valor);
        } else if (nodo->clave < clave) {
            nodo->derecha = insertarEn(nodo->derecha, clave, valor);
        } else {
            nodo->valor = valor;  // la clave ya existia
            return nodo;
        }
        return rebalancear(nodo);
    }

    static Nodo* minimoDe(Nodo* nodo) {
        while (nodo->izquierda != nullptr) nodo = nodo->izquierda;
        return nodo;
    }

    Nodo* eliminarEn(Nodo* nodo, const Clave& clave, bool& seElimino) {
        if (nodo == nullptr) return nullptr;

        if (clave < nodo->clave) {
            nodo->izquierda = eliminarEn(nodo->izquierda, clave, seElimino);
        } else if (nodo->clave < clave) {
            nodo->derecha = eliminarEn(nodo->derecha, clave, seElimino);
        } else {
            seElimino = true;
            // Caso 1 y 2: el nodo tiene como mucho un hijo -> ocupa su lugar.
            if (nodo->izquierda == nullptr || nodo->derecha == nullptr) {
                Nodo* hijo = (nodo->izquierda != nullptr) ? nodo->izquierda : nodo->derecha;
                delete nodo;
                --_tamanio;
                return hijo;
            }
            // Caso 3: dos hijos -> se sustituye por su SUCESOR EN ORDEN (el
            // menor del subarbol derecho) y luego se borra ese sucesor.
            Nodo* sucesor = minimoDe(nodo->derecha);
            nodo->clave = sucesor->clave;
            nodo->valor = sucesor->valor;
            bool auxiliar = false;
            nodo->derecha = eliminarEn(nodo->derecha, sucesor->clave, auxiliar);
        }
        return rebalancear(nodo);
    }

    template <typename Accion>
    static bool recorrer(Nodo* nodo, Accion& accion) {
        if (nodo == nullptr) return true;
        if (!recorrer(nodo->izquierda, accion)) return false;
        if (!accion(nodo->clave, nodo->valor)) return false;
        return recorrer(nodo->derecha, accion);
    }

    static void liberar(Nodo* nodo) {
        if (nodo == nullptr) return;
        liberar(nodo->izquierda);
        liberar(nodo->derecha);
        delete nodo;
    }

    Nodo* _raiz;
    int _tamanio;
};

}  // namespace aed
