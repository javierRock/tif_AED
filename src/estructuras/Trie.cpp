#include "Trie.h"

namespace aed {

Trie::Nodo* Trie::Nodo::buscarHijo(char letra) const {
    for (int i = 0; i < letras.tamanio(); ++i) {
        if (letras[i] == letra) return hijos[i];
    }
    return nullptr;
}

Trie::Trie() : _raiz(new Nodo()), _cantidadNodos(1) {}

Trie::~Trie() {
    liberar(_raiz);
}

void Trie::liberar(Nodo* nodo) {
    if (nodo == nullptr) return;
    for (int i = 0; i < nodo->hijos.tamanio(); ++i) {
        liberar(nodo->hijos[i]);
    }
    delete nodo;
}

void Trie::limpiar() {
    liberar(_raiz);
    _raiz = new Nodo();
    _cantidadNodos = 1;
}

// ---------------------------------------------------------------------------
// Insercion: se baja letra por letra creando los nodos que falten.
// ---------------------------------------------------------------------------
void Trie::insertar(const String& texto, int id) {
    String normalizado = texto.aMinusculas();
    Nodo* actual = _raiz;

    for (int i = 0; i < normalizado.longitud(); ++i) {
        char letra = normalizado[i];
        Nodo* hijo = actual->buscarHijo(letra);
        if (hijo == nullptr) {
            hijo = new Nodo();
            actual->letras.agregar(letra);
            actual->hijos.agregar(hijo);
            ++_cantidadNodos;
        }
        actual = hijo;
    }
    actual->idsAqui.agregar(id);
}

// ---------------------------------------------------------------------------
// Eliminacion: se localiza el nodo terminal y se quita el id de su lista.
// ---------------------------------------------------------------------------
bool Trie::eliminar(const String& texto, int id) {
    String normalizado = texto.aMinusculas();
    Nodo* actual = _raiz;

    for (int i = 0; i < normalizado.longitud(); ++i) {
        actual = actual->buscarHijo(normalizado[i]);
        if (actual == nullptr) return false;
    }

    int posicion = actual->idsAqui.buscar(id);
    if (posicion < 0) return false;
    actual->idsAqui.eliminarRapido(posicion);
    return true;
}

// ---------------------------------------------------------------------------
// Busqueda por prefijo: primero se desciende hasta el nodo del prefijo y
// desde ahi se recolectan en profundidad los ids del subarbol.
// ---------------------------------------------------------------------------
void Trie::buscarPorPrefijo(const String& prefijo, int limite, Arreglo<int>& resultado) const {
    resultado.limpiar();
    if (prefijo.vacia() || limite <= 0) return;

    String normalizado = prefijo.aMinusculas();
    const Nodo* actual = _raiz;

    for (int i = 0; i < normalizado.longitud(); ++i) {
        actual = actual->buscarHijo(normalizado[i]);
        if (actual == nullptr) return;  // ningun texto empieza con ese prefijo
    }
    recolectar(actual, limite, resultado);
}

void Trie::recolectar(const Nodo* nodo, int limite, Arreglo<int>& resultado) {
    if (resultado.tamanio() >= limite) return;

    for (int i = 0; i < nodo->idsAqui.tamanio() && resultado.tamanio() < limite; ++i) {
        resultado.agregar(nodo->idsAqui[i]);
    }
    for (int i = 0; i < nodo->hijos.tamanio() && resultado.tamanio() < limite; ++i) {
        recolectar(nodo->hijos[i], limite, resultado);
    }
}

}  // namespace aed
