


















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



    void insertar(const String& texto, int id);



    bool eliminar(const String& texto, int id);



    void buscarPorPrefijo(const String& prefijo, int limite, Arreglo<int>& resultado) const;

    int cantidadNodos() const { return _cantidadNodos; }
    void limpiar();

private:
    struct Nodo {
        Arreglo<char> letras;
        Arreglo<Nodo*> hijos;
        Arreglo<int> idsAqui;

        Nodo* buscarHijo(char letra) const;
    };

    static void liberar(Nodo* nodo);
    static void recolectar(const Nodo* nodo, int limite, Arreglo<int>& resultado);

    Nodo* _raiz;
    int _cantidadNodos;
};

}
