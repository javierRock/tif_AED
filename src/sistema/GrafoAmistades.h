// ============================================================================
//  GrafoAmistades.h
//  Grafo NO DIRIGIDO que modela las relaciones de amistad.
//
//  REPRESENTACION: listas de adyacencia ORDENADAS.
//  ------------------------------------------------
//  Una matriz de adyacencia necesitaria n^2 bits: para un millon de usuarios
//  son 125 GB, imposible. Las listas de adyacencia ocupan O(n + m), es decir
//  unos 80 MB para 10 millones de amistades.
//
//  Ademas cada lista se mantiene ORDENADA de menor a mayor. Eso da tres
//  ventajas que se aprovechan en todo el proyecto:
//      1. sonAmigos()      -> busqueda binaria en O(log grado) en vez de O(grado).
//      2. amigosEnComun()  -> interseccion con dos punteros en O(gA + gB),
//                             sin estructuras auxiliares.
//      3. eliminarArista() -> se localiza en O(log grado).
//  El precio es que insertar cuesta O(grado) por el desplazamiento, algo
//  irrelevante porque el grado medio de una red social es de unas decenas.
//
//  IDENTIFICACION POR INDICE: los nodos son indices enteros (0..n-1) que
//  coinciden con la posicion del usuario en el arreglo de RedSocial. Usar
//  enteros en vez de punteros permite guardar las listas de forma contigua,
//  que es lo que hace que un BFS sobre un millon de nodos sea viable.
// ============================================================================
#pragma once

#include "../estructuras/Arreglo.h"

namespace aed {

/// Candidato a nueva amistad junto con cuantos amigos comparte con el usuario.
struct SugerenciaAmistad {
    int indiceUsuario = -1;
    int amigosEnComun = 0;

    SugerenciaAmistad() = default;
    SugerenciaAmistad(int indice, int comunes)
        : indiceUsuario(indice), amigosEnComun(comunes) {}

    /// "Menor" significa PEOR sugerencia. Se define asi para poder usar un
    /// monticulo de minimos como filtro de los K mejores candidatos.
    bool operator<(const SugerenciaAmistad& otra) const {
        if (amigosEnComun != otra.amigosEnComun) return amigosEnComun < otra.amigosEnComun;
        return indiceUsuario > otra.indiceUsuario;  // desempate: gana el indice menor
    }
};

class GrafoAmistades {
public:
    GrafoAmistades();

    // ---- Nodos ------------------------------------------------------------
    void establecerCantidadNodos(int cantidad);
    int agregarNodo();
    int cantidadNodos() const { return _adyacencia.tamanio(); }
    long long cantidadAristas() const { return _cantidadAristas; }

    // ---- Aristas ----------------------------------------------------------
    /// Crea la amistad en ambos sentidos. Devuelve false si ya existia o si
    /// los indices no son validos. O(grado).
    bool agregarArista(int a, int b);

    /// Elimina la amistad en ambos sentidos. O(grado).
    bool eliminarArista(int a, int b);

    /// Busqueda binaria sobre la lista ordenada. O(log grado).
    bool sonAdyacentes(int a, int b) const;

    int grado(int nodo) const;
    const Arreglo<int>& vecinos(int nodo) const;

    /// Borra todas las aristas del nodo (tambien en la lista de sus vecinos).
    /// Es el paso previo a eliminar un usuario. O(suma de grados vecinos).
    void aislarNodo(int nodo);

    // ---- Carga masiva -----------------------------------------------------
    // Al construir un grafo de millones de aristas conviene primero agregarlas
    // todas al final de cada lista (O(1) cada una) y ordenar una sola vez al
    // terminar: O(m + n*g*log g) en vez de O(m*g) por insercion ordenada.
    void agregarAristaSinOrdenar(int a, int b);
    void ordenarYLimpiarListas();

    // ---- Algoritmos -------------------------------------------------------
    /// Camino de amistad mas corto mediante BFS BIDIRECCIONAL.
    /// Deja el camino completo (origen ... destino) en 'camino'.
    bool caminoMasCorto(int origen, int destino, Arreglo<int>& camino);

    /// Misma funcion con un BFS clasico desde el origen. Se conserva para
    /// comparar ambos enfoques en el banco de pruebas.
    bool caminoMasCortoBFSSimple(int origen, int destino, Arreglo<int>& camino);

    /// Interseccion de las dos listas de adyacencia ordenadas. O(gA + gB).
    void amigosEnComun(int a, int b, Arreglo<int>& resultado) const;

    /// Sugerencias de amistad por "amigos de mis amigos": cuenta cuantos
    /// amigos comunes tiene cada candidato y devuelve los mejores 'cantidad'.
    void sugerirAmistades(int nodo, int cantidad, Arreglo<SugerenciaAmistad>& salida);

    // ---- Estadisticas -----------------------------------------------------
    int gradoMaximo() const;
    double gradoPromedio() const;
    long long memoriaAproximadaBytes() const;

private:
    /// Prepara (y agranda si hizo falta) los arreglos auxiliares de los
    /// recorridos. Se reutilizan entre llamadas para no pedir memoria en cada
    /// consulta.
    void prepararEspaciosDeTrabajo();

    /// Expande un nivel completo del BFS bidireccional.
    /// Devuelve el mejor nodo de encuentro hallado en ese nivel, o -1.
    int expandirNivel(Arreglo<int>& frente, Arreglo<int>& selloPropio,
                      Arreglo<int>& distanciaPropia, Arreglo<int>& padrePropio,
                      const Arreglo<int>& selloOpuesto, const Arreglo<int>& distanciaOpuesta);

    void reconstruirCamino(int encuentro, Arreglo<int>& camino) const;

    Arreglo<Arreglo<int>> _adyacencia;
    Arreglo<int> _listaVacia;  // se devuelve ante indices invalidos
    long long _cantidadAristas;

    // -- Espacios de trabajo reutilizables ----------------------------------
    //
    // TECNICA DEL SELLO DE TIEMPO: en lugar de poner a cero un arreglo de un
    // millon de posiciones antes de cada busqueda (O(n) desperdiciado), se
    // guarda en cada posicion el numero de consulta que la marco. Una posicion
    // esta "visitada" solo si su sello coincide con el de la consulta actual.
    // Asi limpiar cuesta O(1): basta incrementar el contador.
    int _selloActual;
    Arreglo<int> _selloLadoA, _distanciaLadoA, _padreLadoA;
    Arreglo<int> _selloLadoB, _distanciaLadoB, _padreLadoB;
    Arreglo<int> _frenteA, _frenteB, _frenteAuxiliar;

    int _selloSugerencias;
    Arreglo<int> _marcaSugerencia;
    Arreglo<int> _conteoSugerencia;
    Arreglo<int> _candidatos;
};

}  // namespace aed
