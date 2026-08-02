



















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



    void insertar(const Clave& clave, const Valor& valor) {
        _raiz = insertarEn(_raiz, clave, valor);
    }



    Valor& obtenerOInsertar(const Clave& clave, const Valor& valorInicial) {
        Valor* existente = buscar(clave);
        if (existente != nullptr) return *existente;
        _raiz = insertarEn(_raiz, clave, valorInicial);
        return *buscar(clave);
    }



    bool eliminar(const Clave& clave) {
        bool seElimino = false;
        _raiz = eliminarEn(_raiz, clave, seElimino);
        return seElimino;
    }








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













    static Nodo* rotacionDerecha(Nodo* y) {
        Nodo* x = y->izquierda;
        Nodo* B = x->derecha;
        x->derecha = y;
        y->izquierda = B;
        actualizarAltura(y);
        actualizarAltura(x);
        return x;
    }










    static Nodo* rotacionIzquierda(Nodo* x) {
        Nodo* y = x->derecha;
        Nodo* B = y->izquierda;
        y->izquierda = x;
        x->derecha = B;
        actualizarAltura(x);
        actualizarAltura(y);
        return y;
    }


    static Nodo* rebalancear(Nodo* nodo) {
        actualizarAltura(nodo);
        int balance = factorDeBalance(nodo);


        if (balance > 1 && factorDeBalance(nodo->izquierda) >= 0) {
            return rotacionDerecha(nodo);
        }

        if (balance > 1) {
            nodo->izquierda = rotacionIzquierda(nodo->izquierda);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && factorDeBalance(nodo->derecha) <= 0) {
            return rotacionIzquierda(nodo);
        }

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
            nodo->valor = valor;
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

            if (nodo->izquierda == nullptr || nodo->derecha == nullptr) {
                Nodo* hijo = (nodo->izquierda != nullptr) ? nodo->izquierda : nodo->derecha;
                delete nodo;
                --_tamanio;
                return hijo;
            }


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

}
