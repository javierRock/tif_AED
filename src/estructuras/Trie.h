// ============================================================================
//  Trie.h
//  Arbol de prefijos (trie) para busqueda de usuarios por el comienzo del
//  nombre, como el autocompletado del buscador de una red social.
//
//  Cada arista representa una letra y cada camino desde la raiz representa un
//  prefijo. Buscar "mar" cuesta O(3) mas el costo de recolectar resultados:
//  NO depende de cuantos usuarios haya en el sistema. Con un millon de
//  usuarios eso es una ventaja enorme frente a recorrer todos comparando.
//
//  DETALLE DE IMPLEMENTACION: los hijos de cada nodo se guardan en dos
//  arreglos paralelos (letras[] y hijos[]) en lugar de un arreglo fijo de 27
//  punteros. La mayoria de nodos tiene 1 o 2 hijos, asi que la version
//  dispersa ahorra muchisima memoria a costa de una busqueda lineal
//  entre poquisimos elementos.
//
//  Se indexa el nombre completo y ademas cada palabra por separado, de manera
//  que "Ana Torres" se encuentra tanto escribiendo "ana" como "torres".
// ============================================================================
#pragma once

#include "Arreglo.h"
#include "String.h"

namespace aed {

class Trie {
public:
    Trie();
    ~Trie();

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    /// Indexa 'texto' (se normaliza a minusculas) apuntando al id indicado.
    /// Coste O(longitud del texto).
    void insertar(const String& texto, int id);

    /// Quita el id del nodo terminal correspondiente. Los nodos vacios se
    /// conservan: reaprovecharlos cuesta menos que reconstruir el camino.
    bool eliminar(const String& texto, int id);

    /// Deja en 'resultado' hasta 'limite' ids cuyo texto empieza con el
    /// prefijo dado. Coste O(longitud del prefijo + nodos visitados).
    void buscarPorPrefijo(const String& prefijo, int limite, Arreglo<int>& resultado) const;

    int cantidadNodos() const { return _cantidadNodos; }
    void limpiar();

private:
    struct Nodo {
        Arreglo<char> letras;    // letras de las aristas hacia los hijos
        Arreglo<Nodo*> hijos;    // hijos, en el mismo orden que 'letras'
        Arreglo<int> idsAqui;    // ids cuyo texto termina exactamente aqui

        Nodo* buscarHijo(char letra) const;
    };

    static void liberar(Nodo* nodo);
    static void recolectar(const Nodo* nodo, int limite, Arreglo<int>& resultado);

    Nodo* _raiz;
    int _cantidadNodos;
};

}  // namespace aed
