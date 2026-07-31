#include "GrafoAmistades.h"

#include "../estructuras/Cola.h"
#include "../estructuras/MonticuloMinimo.h"
#include "../estructuras/Ordenamiento.h"
#include "../estructuras/Pila.h"

namespace aed {

GrafoAmistades::GrafoAmistades()
    : _cantidadAristas(0), _selloActual(0), _selloSugerencias(0) {}

// ---------------------------------------------------------------------------
// Nodos
// ---------------------------------------------------------------------------
void GrafoAmistades::establecerCantidadNodos(int cantidad) {
    _adyacencia.redimensionar(cantidad);
}

int GrafoAmistades::agregarNodo() {
    _adyacencia.agregar(Arreglo<int>());
    return _adyacencia.tamanio() - 1;
}

int GrafoAmistades::grado(int nodo) const {
    if (nodo < 0 || nodo >= _adyacencia.tamanio()) return 0;
    return _adyacencia[nodo].tamanio();
}

const Arreglo<int>& GrafoAmistades::vecinos(int nodo) const {
    if (nodo < 0 || nodo >= _adyacencia.tamanio()) return _listaVacia;
    return _adyacencia[nodo];
}

// ---------------------------------------------------------------------------
// Aristas
// ---------------------------------------------------------------------------
bool GrafoAmistades::agregarArista(int a, int b) {
    int total = _adyacencia.tamanio();
    if (a < 0 || b < 0 || a >= total || b >= total || a == b) return false;
    if (_adyacencia[a].buscarBinario(b) >= 0) return false;  // ya eran amigos

    // Al ser una relacion simetrica, la arista se guarda en ambas listas.
    _adyacencia[a].insertarOrdenado(b);
    _adyacencia[b].insertarOrdenado(a);
    ++_cantidadAristas;
    return true;
}

bool GrafoAmistades::eliminarArista(int a, int b) {
    int total = _adyacencia.tamanio();
    if (a < 0 || b < 0 || a >= total || b >= total) return false;

    int posicionEnA = _adyacencia[a].buscarBinario(b);
    if (posicionEnA < 0) return false;
    int posicionEnB = _adyacencia[b].buscarBinario(a);

    // eliminarEn() conserva el orden, requisito para seguir usando busqueda
    // binaria en las siguientes consultas.
    _adyacencia[a].eliminarEn(posicionEnA);
    if (posicionEnB >= 0) _adyacencia[b].eliminarEn(posicionEnB);
    --_cantidadAristas;
    return true;
}

bool GrafoAmistades::sonAdyacentes(int a, int b) const {
    int total = _adyacencia.tamanio();
    if (a < 0 || b < 0 || a >= total || b >= total) return false;
    // Se busca en la lista mas corta: menos comparaciones.
    if (_adyacencia[a].tamanio() <= _adyacencia[b].tamanio()) {
        return _adyacencia[a].buscarBinario(b) >= 0;
    }
    return _adyacencia[b].buscarBinario(a) >= 0;
}

void GrafoAmistades::aislarNodo(int nodo) {
    if (nodo < 0 || nodo >= _adyacencia.tamanio()) return;

    // Para cada amigo hay que borrar la referencia de vuelta, si no quedarian
    // aristas "colgando" apuntando a un usuario inexistente.
    Arreglo<int>& lista = _adyacencia[nodo];
    for (int i = 0; i < lista.tamanio(); ++i) {
        int amigo = lista[i];
        int posicion = _adyacencia[amigo].buscarBinario(nodo);
        if (posicion >= 0) _adyacencia[amigo].eliminarEn(posicion);
        --_cantidadAristas;
    }
    lista.liberar();
}

// ---------------------------------------------------------------------------
// Carga masiva
// ---------------------------------------------------------------------------
void GrafoAmistades::agregarAristaSinOrdenar(int a, int b) {
    int total = _adyacencia.tamanio();
    if (a < 0 || b < 0 || a >= total || b >= total || a == b) return;
    _adyacencia[a].agregar(b);
    _adyacencia[b].agregar(a);
}

void GrafoAmistades::ordenarYLimpiarListas() {
    long long sumaGrados = 0;
    for (int i = 0; i < _adyacencia.tamanio(); ++i) {
        Arreglo<int>& lista = _adyacencia[i];
        if (lista.tamanio() > 1) {
            ordenarRapido(lista);
            eliminarDuplicadosOrdenados(lista);
        }
        sumaGrados += lista.tamanio();
    }
    // En un grafo no dirigido cada arista aparece en dos listas.
    _cantidadAristas = sumaGrados / 2;
}

// ---------------------------------------------------------------------------
// Espacios de trabajo de los recorridos
// ---------------------------------------------------------------------------
void GrafoAmistades::prepararEspaciosDeTrabajo() {
    int n = _adyacencia.tamanio();
    if (_selloLadoA.tamanio() == n) return;  // ya estaban listos

    _selloLadoA.redimensionar(n);
    _distanciaLadoA.redimensionar(n);
    _padreLadoA.redimensionar(n);
    _selloLadoB.redimensionar(n);
    _distanciaLadoB.redimensionar(n);
    _padreLadoB.redimensionar(n);
    _marcaSugerencia.redimensionar(n);
    _conteoSugerencia.redimensionar(n);

    // Los sellos arrancan en 0 y el contador de consultas en 1, de modo que
    // ninguna posicion aparece como visitada al inicio.
    _selloLadoA.rellenar(0);
    _selloLadoB.rellenar(0);
    _marcaSugerencia.rellenar(0);
    _selloActual = 0;
    _selloSugerencias = 0;
}

// ---------------------------------------------------------------------------
// BFS BIDIRECCIONAL
//
// La idea: en vez de una unica busqueda que crece desde el origen, se lanzan
// dos busquedas simultaneas, una desde cada extremo, y se para cuando se
// encuentran. Si el grado medio es g y la distancia es d, un BFS normal
// visita del orden de g^d nodos; dos busquedas de profundidad d/2 visitan
// 2*g^(d/2). En una red social real (donde casi todos estan a 5 o 6 saltos)
// esa diferencia son varios ordenes de magnitud.
//
// En cada iteracion se expande el frente MAS PEQUENO, que es lo que evita
// quedar atrapado explorando el vecindario de un usuario muy popular.
// ---------------------------------------------------------------------------
bool GrafoAmistades::caminoMasCorto(int origen, int destino, Arreglo<int>& camino) {
    camino.limpiar();
    int total = _adyacencia.tamanio();
    if (origen < 0 || destino < 0 || origen >= total || destino >= total) return false;
    if (origen == destino) {
        camino.agregar(origen);
        return true;
    }

    prepararEspaciosDeTrabajo();
    ++_selloActual;

    _selloLadoA[origen] = _selloActual;
    _distanciaLadoA[origen] = 0;
    _padreLadoA[origen] = -1;

    _selloLadoB[destino] = _selloActual;
    _distanciaLadoB[destino] = 0;
    _padreLadoB[destino] = -1;

    _frenteA.limpiar();
    _frenteA.agregar(origen);
    _frenteB.limpiar();
    _frenteB.agregar(destino);

    int encuentro = -1;
    while (encuentro < 0 && !_frenteA.vacio() && !_frenteB.vacio()) {
        if (_frenteA.tamanio() <= _frenteB.tamanio()) {
            encuentro = expandirNivel(_frenteA, _selloLadoA, _distanciaLadoA, _padreLadoA,
                                      _selloLadoB, _distanciaLadoB);
        } else {
            encuentro = expandirNivel(_frenteB, _selloLadoB, _distanciaLadoB, _padreLadoB,
                                      _selloLadoA, _distanciaLadoA);
        }
    }

    if (encuentro < 0) return false;  // estan en componentes distintas
    reconstruirCamino(encuentro, camino);
    return true;
}

int GrafoAmistades::expandirNivel(Arreglo<int>& frente, Arreglo<int>& selloPropio,
                                  Arreglo<int>& distanciaPropia, Arreglo<int>& padrePropio,
                                  const Arreglo<int>& selloOpuesto,
                                  const Arreglo<int>& distanciaOpuesta) {
    _frenteAuxiliar.limpiar();
    int mejorEncuentro = -1;
    int mejorLongitud = 0;

    for (int i = 0; i < frente.tamanio(); ++i) {
        int actual = frente[i];
        const Arreglo<int>& listaVecinos = _adyacencia[actual];

        for (int k = 0; k < listaVecinos.tamanio(); ++k) {
            int vecino = listaVecinos[k];
            if (selloPropio[vecino] == _selloActual) continue;  // ya visitado por este lado

            selloPropio[vecino] = _selloActual;
            distanciaPropia[vecino] = distanciaPropia[actual] + 1;
            padrePropio[vecino] = actual;

            // Si el otro lado ya habia llegado a este nodo, tenemos un camino.
            if (selloOpuesto[vecino] == _selloActual) {
                int longitud = distanciaPropia[vecino] + distanciaOpuesta[vecino];
                if (mejorEncuentro < 0 || longitud < mejorLongitud) {
                    mejorEncuentro = vecino;
                    mejorLongitud = longitud;
                }
            }
            _frenteAuxiliar.agregar(vecino);
        }
    }

    // Se completa TODO el nivel antes de devolver el encuentro y se elige el
    // de menor longitud total: parar en el primer contacto podria dar un
    // camino con un salto de mas.
    intercambiar(frente, _frenteAuxiliar);
    return mejorEncuentro;
}

void GrafoAmistades::reconstruirCamino(int encuentro, Arreglo<int>& camino) const {
    // Del encuentro hacia el origen los padres van "al reves", asi que se
    // apilan para desapilarlos ya en el orden correcto.
    Pila<int> pila;
    for (int actual = encuentro; actual != -1; actual = _padreLadoA[actual]) {
        pila.apilar(actual);
    }
    while (!pila.vacia()) camino.agregar(pila.desapilar());

    // Del encuentro hacia el destino los padres ya van en el orden correcto.
    for (int actual = _padreLadoB[encuentro]; actual != -1; actual = _padreLadoB[actual]) {
        camino.agregar(actual);
    }
}

// ---------------------------------------------------------------------------
// BFS clasico (una sola direccion), usado como referencia de comparacion.
// ---------------------------------------------------------------------------
bool GrafoAmistades::caminoMasCortoBFSSimple(int origen, int destino, Arreglo<int>& camino) {
    camino.limpiar();
    int total = _adyacencia.tamanio();
    if (origen < 0 || destino < 0 || origen >= total || destino >= total) return false;
    if (origen == destino) {
        camino.agregar(origen);
        return true;
    }

    prepararEspaciosDeTrabajo();
    ++_selloActual;

    Cola<int> porVisitar(1024);
    _selloLadoA[origen] = _selloActual;
    _padreLadoA[origen] = -1;
    porVisitar.encolar(origen);

    bool encontrado = false;
    while (!porVisitar.vacia() && !encontrado) {
        int actual = porVisitar.desencolar();
        const Arreglo<int>& listaVecinos = _adyacencia[actual];

        for (int k = 0; k < listaVecinos.tamanio(); ++k) {
            int vecino = listaVecinos[k];
            if (_selloLadoA[vecino] == _selloActual) continue;

            _selloLadoA[vecino] = _selloActual;
            _padreLadoA[vecino] = actual;
            if (vecino == destino) {
                encontrado = true;
                break;
            }
            porVisitar.encolar(vecino);
        }
    }
    if (!encontrado) return false;

    Pila<int> pila;
    for (int actual = destino; actual != -1; actual = _padreLadoA[actual]) {
        pila.apilar(actual);
    }
    while (!pila.vacia()) camino.agregar(pila.desapilar());
    return true;
}

// ---------------------------------------------------------------------------
// Amigos en comun: interseccion de dos listas ordenadas con dos indices que
// avanzan a la vez. Es el mismo esquema que la fase de mezcla del mergesort.
// Coste O(grado(a) + grado(b)) y cero memoria auxiliar.
// ---------------------------------------------------------------------------
void GrafoAmistades::amigosEnComun(int a, int b, Arreglo<int>& resultado) const {
    resultado.limpiar();
    int total = _adyacencia.tamanio();
    if (a < 0 || b < 0 || a >= total || b >= total) return;

    const Arreglo<int>& listaA = _adyacencia[a];
    const Arreglo<int>& listaB = _adyacencia[b];

    int i = 0;
    int j = 0;
    while (i < listaA.tamanio() && j < listaB.tamanio()) {
        if (listaA[i] == listaB[j]) {
            resultado.agregar(listaA[i]);
            ++i;
            ++j;
        } else if (listaA[i] < listaB[j]) {
            ++i;  // avanza la lista que va "por detras"
        } else {
            ++j;
        }
    }
}

// ---------------------------------------------------------------------------
// Sugerencias de amistad: "amigos de mis amigos".
//
// Se recorren los amigos de mis amigos y se cuenta cuantas veces aparece cada
// candidato: esa cuenta es exactamente el numero de amigos que tenemos en
// comun con el. Los candidatos se filtran quitando al propio usuario y a
// quienes ya son amigos, y al final se seleccionan los K mejores con un
// monticulo de minimos.
//
// Coste O(suma de grados de mis amigos + C log K), con C = numero de
// candidatos distintos.
// ---------------------------------------------------------------------------
void GrafoAmistades::sugerirAmistades(int nodo, int cantidad,
                                      Arreglo<SugerenciaAmistad>& salida) {
    salida.limpiar();
    if (nodo < 0 || nodo >= _adyacencia.tamanio() || cantidad <= 0) return;

    prepararEspaciosDeTrabajo();
    ++_selloSugerencias;
    _candidatos.limpiar();

    // Se marcan con conteo -1 el propio usuario y sus amigos actuales: son los
    // que hay que descartar. Marcarlos de antemano convierte el filtro en una
    // comprobacion O(1) dentro del bucle principal.
    const Arreglo<int>& misAmigos = _adyacencia[nodo];
    _marcaSugerencia[nodo] = _selloSugerencias;
    _conteoSugerencia[nodo] = -1;
    for (int i = 0; i < misAmigos.tamanio(); ++i) {
        _marcaSugerencia[misAmigos[i]] = _selloSugerencias;
        _conteoSugerencia[misAmigos[i]] = -1;
    }

    for (int i = 0; i < misAmigos.tamanio(); ++i) {
        const Arreglo<int>& amigosDelAmigo = _adyacencia[misAmigos[i]];
        for (int k = 0; k < amigosDelAmigo.tamanio(); ++k) {
            int candidato = amigosDelAmigo[k];

            if (_marcaSugerencia[candidato] == _selloSugerencias) {
                if (_conteoSugerencia[candidato] < 0) continue;  // descartado
                ++_conteoSugerencia[candidato];
            } else {
                _marcaSugerencia[candidato] = _selloSugerencias;
                _conteoSugerencia[candidato] = 1;
                _candidatos.agregar(candidato);
            }
        }
    }

    // Seleccion de los K mejores: el monticulo nunca guarda mas de K
    // elementos, asi que la memoria es O(K) aunque haya millones de candidatos.
    MonticuloMinimo<SugerenciaAmistad> mejores(cantidad);
    for (int i = 0; i < _candidatos.tamanio(); ++i) {
        int candidato = _candidatos[i];
        SugerenciaAmistad sugerencia(candidato, _conteoSugerencia[candidato]);

        if (mejores.tamanio() < cantidad) {
            mejores.insertar(sugerencia);
        } else if (mejores.minimo() < sugerencia) {
            mejores.reemplazarMinimo(sugerencia);
        }
    }
    mejores.volcarDeMayorAMenor(salida);
}

// ---------------------------------------------------------------------------
// Estadisticas
// ---------------------------------------------------------------------------
int GrafoAmistades::gradoMaximo() const {
    int maximo = 0;
    for (int i = 0; i < _adyacencia.tamanio(); ++i) {
        if (_adyacencia[i].tamanio() > maximo) maximo = _adyacencia[i].tamanio();
    }
    return maximo;
}

double GrafoAmistades::gradoPromedio() const {
    if (_adyacencia.vacio()) return 0.0;
    return (2.0 * static_cast<double>(_cantidadAristas)) /
           static_cast<double>(_adyacencia.tamanio());
}

long long GrafoAmistades::memoriaAproximadaBytes() const {
    long long total = static_cast<long long>(_adyacencia.capacidad()) * sizeof(Arreglo<int>);
    for (int i = 0; i < _adyacencia.tamanio(); ++i) {
        total += static_cast<long long>(_adyacencia[i].capacidad()) * sizeof(int);
    }
    return total;
}

}  // namespace aed
