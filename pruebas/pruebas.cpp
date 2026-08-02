









#include <cstdio>

#include "../src/datos/GeneradorSintetico.h"
#include "../src/estructuras/ArbolAVL.h"
#include "../src/estructuras/Arreglo.h"
#include "../src/estructuras/String.h"
#include "../src/estructuras/Cola.h"
#include "../src/estructuras/ListaEnlazada.h"
#include "../src/estructuras/MonticuloMinimo.h"
#include "../src/estructuras/Ordenamiento.h"
#include "../src/estructuras/Pila.h"
#include "../src/estructuras/TablaHash.h"
#include "../src/estructuras/Trie.h"
#include "../src/sistema/RedSocial.h"

using namespace aed;




namespace {

int totalComprobaciones = 0;
int comprobacionesFallidas = 0;
const char* grupoActual = "";

void comenzarGrupo(const char* nombre) {
    grupoActual = nombre;
    std::printf("\n  %s\n", nombre);
}

void comprobar(bool condicion, const char* descripcion) {
    ++totalComprobaciones;
    if (condicion) {
        std::printf("    [ OK ] %s\n", descripcion);
    } else {
        ++comprobacionesFallidas;
        std::printf("    [FALLA] %s   (grupo: %s)\n", descripcion, grupoActual);
    }
}

}




void probarString() {
    comenzarGrupo("String");

    String vacia;
    comprobar(vacia.vacia() && vacia.longitud() == 0, "una cadena recien creada esta vacia");

    String saludo("Hola");
    saludo.agregar(" mundo");
    comprobar(saludo.longitud() == 10, "agregar concatena y actualiza la longitud");
    comprobar(String(saludo.texto()) == String("Hola mundo"), "el contenido es el esperado");

    String copia(saludo);
    comprobar(copia == saludo, "el constructor de copia duplica el contenido");

    String movida(mover(copia));
    comprobar(movida == saludo && copia.vacia(), "el movimiento traslada y deja vacia la origen");

    comprobar(saludo.subcadena(0, 4) == String("Hola"), "subcadena extrae el trozo pedido");
    comprobar(saludo.comienzaCon(String("Hola")), "comienzaCon detecta el prefijo");
    comprobar(!saludo.comienzaCon(String("mundo")), "comienzaCon rechaza lo que no es prefijo");
    comprobar(saludo.contiene(String("mun")), "contiene encuentra el patron interno");
    comprobar(String("ABC").aMinusculas() == String("abc"), "aMinusculas convierte el texto");

    comprobar(String("ana") < String("beto"), "el orden alfabetico funciona");
    comprobar(String::desdeEntero(-1234) == String("-1234"), "desdeEntero convierte numeros");
    comprobar(String("4321").aEntero() == 4321, "aEntero interpreta el texto");

    comprobar(String("hola").codigoHash() == String("hola").codigoHash(),
              "el hash es igual para cadenas iguales");
    comprobar(String("hola").codigoHash() != String("holb").codigoHash(),
              "el hash cambia si cambia el texto");


    String reutilizada("texto muy largo de ejemplo");
    reutilizada = String("ab");
    comprobar(reutilizada.longitud() == 2 && reutilizada == String("ab"),
              "asignar una cadena corta sobre una larga la trunca bien");
}




void probarArreglo() {
    comenzarGrupo("Arreglo dinamico");

    Arreglo<int> numeros;
    for (int i = 0; i < 1000; ++i) numeros.agregar(i);
    comprobar(numeros.tamanio() == 1000, "agregar 1000 elementos deja tamanio 1000");
    comprobar(numeros[0] == 0 && numeros[999] == 999, "los elementos conservan su valor");

    numeros.eliminarEn(0);
    comprobar(numeros.tamanio() == 999 && numeros[0] == 1,
              "eliminarEn conserva el orden de los demas");

    int ultimoValor = numeros.ultimo();
    numeros.eliminarRapido(0);
    comprobar(numeros[0] == ultimoValor, "eliminarRapido trae el ultimo al hueco");

    Arreglo<int> ordenado;
    int valores[] = {50, 10, 40, 20, 30};
    for (int i = 0; i < 5; ++i) ordenado.insertarOrdenado(valores[i]);
    bool creciente = true;
    for (int i = 1; i < ordenado.tamanio(); ++i) {
        if (ordenado[i - 1] > ordenado[i]) creciente = false;
    }
    comprobar(creciente, "insertarOrdenado mantiene el arreglo ordenado");
    comprobar(ordenado.buscarBinario(40) == 3, "la busqueda binaria encuentra la posicion");
    comprobar(ordenado.buscarBinario(99) == -1, "la busqueda binaria devuelve -1 si no esta");

    Arreglo<String> textos;
    textos.agregar(String("uno"));
    textos.agregar(String("dos"));
    Arreglo<String> copiaTextos(textos);
    comprobar(copiaTextos.tamanio() == 2 && copiaTextos[1] == String("dos"),
              "el arreglo copia correctamente objetos con memoria propia");
}




void probarOrdenamiento() {
    comenzarGrupo("Ordenamiento (quicksort y mergesort)");

    GeneradorAleatorio azar(12345);
    const int N = 20000;

    Arreglo<int> conQuicksort(N);
    Arreglo<int> conMergesort(N);
    for (int i = 0; i < N; ++i) {
        int valor = azar.enteroMenorQue(100000);
        conQuicksort.agregar(valor);
        conMergesort.agregar(valor);
    }

    ordenarRapido(conQuicksort);
    ordenarPorMezcla(conMergesort, MenorQue());

    bool quicksortOrdenado = true;
    bool coinciden = true;
    for (int i = 1; i < N; ++i) {
        if (conQuicksort[i - 1] > conQuicksort[i]) quicksortOrdenado = false;
        if (conQuicksort[i] != conMergesort[i]) coinciden = false;
    }
    comprobar(quicksortOrdenado, "quicksort deja el arreglo ordenado");
    comprobar(coinciden, "quicksort y mergesort producen el mismo resultado");


    Arreglo<int> yaOrdenado(5000);
    for (int i = 0; i < 5000; ++i) yaOrdenado.agregar(i);
    ordenarRapido(yaOrdenado);
    comprobar(yaOrdenado[0] == 0 && yaOrdenado[4999] == 4999,
              "quicksort soporta un arreglo ya ordenado (mediana de tres)");

    Arreglo<int> conRepetidos;
    for (int i = 0; i < 100; ++i) conRepetidos.agregar(i % 5);
    ordenarRapido(conRepetidos);
    eliminarDuplicadosOrdenados(conRepetidos);
    comprobar(conRepetidos.tamanio() == 5, "eliminarDuplicadosOrdenados deja solo los distintos");
}




void probarListaPilaCola() {
    comenzarGrupo("ListaEnlazada, Pila y Cola");

    ListaEnlazada<int> lista;
    for (int i = 1; i <= 5; ++i) lista.agregarAlFinal(i);
    lista.agregarAlInicio(0);
    comprobar(lista.tamanio() == 6 && lista.primero()->valor == 0,
              "la lista agrega al inicio y al final");

    bool eliminado = lista.eliminarPrimeroQue([](const int& v) { return v == 3; });
    comprobar(eliminado && lista.tamanio() == 5, "eliminarPrimeroQue quita el elemento buscado");


    lista.eliminarPrimeroQue([](const int& v) { return v == 5; });
    lista.agregarAlFinal(9);
    const ListaEnlazada<int>::Nodo* nodo = lista.primero();
    while (nodo->siguiente != nullptr) nodo = nodo->siguiente;
    comprobar(nodo->valor == 9, "la cola de la lista queda correcta tras borrar el ultimo");

    Pila<int> pila;
    for (int i = 1; i <= 3; ++i) pila.apilar(i);
    comprobar(pila.desapilar() == 3 && pila.desapilar() == 2, "la pila devuelve en orden LIFO");

    Cola<int> cola;
    for (int i = 1; i <= 3; ++i) cola.encolar(i);
    comprobar(cola.desencolar() == 1 && cola.desencolar() == 2, "la cola devuelve en orden FIFO");



    Cola<int> circular(4);
    bool ordenCorrecto = true;
    int siguienteEsperado = 0;
    for (int vuelta = 0; vuelta < 100; ++vuelta) {
        circular.encolar(vuelta);
        if (vuelta % 2 == 1) {
            if (circular.desencolar() != siguienteEsperado++) ordenCorrecto = false;
        }
    }
    comprobar(circular.tamanio() == 50, "el arreglo circular gestiona bien el envolvimiento");
    comprobar(ordenCorrecto, "el orden FIFO se mantiene al redimensionar la cola");
}




void probarTablaHash() {
    comenzarGrupo("TablaHash (direccionamiento abierto)");

    TablaHash<int, int> tabla;
    const int N = 50000;
    for (int i = 0; i < N; ++i) tabla.insertar(i, i * 2);

    comprobar(tabla.tamanio() == N, "se insertaron todas las claves");
    bool todasCorrectas = true;
    for (int i = 0; i < N; ++i) {
        const int* valor = tabla.buscar(i);
        if (valor == nullptr || *valor != i * 2) todasCorrectas = false;
    }
    comprobar(todasCorrectas, "todas las claves se recuperan con su valor tras redimensionar");
    comprobar(tabla.buscar(N + 1) == nullptr, "una clave inexistente devuelve nullptr");

    comprobar(!tabla.insertar(5, 999), "insertar una clave existente devuelve false");
    comprobar(*tabla.buscar(5) == 999, "y actualiza su valor");


    for (int i = 0; i < N; i += 2) tabla.eliminar(i);
    comprobar(tabla.tamanio() == N / 2, "el tamanio refleja los borrados");

    bool imparesIntactos = true;
    for (int i = 1; i < N; i += 2) {
        if (tabla.buscar(i) == nullptr) imparesIntactos = false;
    }
    comprobar(imparesIntactos, "tras borrar la mitad, el resto sigue siendo accesible (tumbas)");

    bool paresAusentes = true;
    for (int i = 0; i < N; i += 2) {
        if (tabla.buscar(i) != nullptr) paresAusentes = false;
    }
    comprobar(paresAusentes, "las claves borradas ya no se encuentran");

    TablaHash<String, int> porTexto;
    porTexto.insertar(String("ana@correo.com"), 1);
    porTexto.insertar(String("luis@correo.com"), 2);
    comprobar(*porTexto.buscar(String("luis@correo.com")) == 2, "funciona con claves de texto");
    comprobar(porTexto.buscar(String("nadie@correo.com")) == nullptr,
              "no confunde claves de texto distintas");
}




void probarMonticulo() {
    comenzarGrupo("MonticuloMinimo");

    GeneradorAleatorio azar(777);
    MonticuloMinimo<int> monticulo;
    for (int i = 0; i < 5000; ++i) monticulo.insertar(azar.enteroMenorQue(100000));

    bool creciente = true;
    int anterior = -1;
    while (!monticulo.vacio()) {
        int actual = monticulo.extraerMinimo();
        if (actual < anterior) creciente = false;
        anterior = actual;
    }
    comprobar(creciente, "extraerMinimo devuelve los elementos de menor a mayor");


    const int K = 10;
    MonticuloMinimo<int> mejores(K);
    for (int i = 1; i <= 1000; ++i) {
        if (mejores.tamanio() < K) {
            mejores.insertar(i);
        } else if (mejores.minimo() < i) {
            mejores.reemplazarMinimo(i);
        }
    }
    Arreglo<int> resultado;
    mejores.volcarDeMayorAMenor(resultado);
    comprobar(resultado.tamanio() == K && resultado[0] == 1000 && resultado[K - 1] == 991,
              "el filtro top-K devuelve los 10 mayores en orden descendente");
}




namespace {


struct RecolectorDeClaves {
    Arreglo<int> claves;
    bool operator()(const int& clave, const int&  ) {
        claves.agregar(clave);
        return true;
    }
};

}

void probarArbolAVL() {
    comenzarGrupo("ArbolAVL");

    ArbolAVL<int, int> arbol;
    const int N = 10000;



    for (int i = 0; i < N; ++i) arbol.insertar(i, i * 10);
    comprobar(arbol.tamanio() == N, "se insertaron todas las claves");


    comprobar(arbol.altura() <= 20, "el arbol se mantiene balanceado (altura <= 20)");

    bool todasCorrectas = true;
    for (int i = 0; i < N; ++i) {
        const int* valor = arbol.buscar(i);
        if (valor == nullptr || *valor != i * 10) todasCorrectas = false;
    }
    comprobar(todasCorrectas, "todas las claves se recuperan con su valor");

    RecolectorDeClaves recolector;
    arbol.recorrerEnOrden(recolector);
    bool enOrden = (recolector.claves.tamanio() == N);
    for (int i = 1; i < recolector.claves.tamanio(); ++i) {
        if (recolector.claves[i - 1] >= recolector.claves[i]) enOrden = false;
    }
    comprobar(enOrden, "el recorrido en orden devuelve las claves ordenadas");

    for (int i = 0; i < N; i += 2) arbol.eliminar(i);
    comprobar(arbol.tamanio() == N / 2, "eliminar reduce el tamanio correctamente");
    comprobar(arbol.altura() <= 20, "el arbol sigue balanceado despues de eliminar");
    comprobar(arbol.buscar(0) == nullptr && arbol.buscar(1) != nullptr,
              "solo desaparecen las claves eliminadas");

    ArbolAVL<String, int> porNombre;
    porNombre.insertar(String("Zoe"), 1);
    porNombre.insertar(String("Ana"), 2);
    porNombre.insertar(String("Marco"), 3);
    comprobar(*porNombre.buscar(String("Ana")) == 2, "funciona con claves de texto");
}




void probarTrie() {
    comenzarGrupo("Trie de prefijos");

    Trie trie;
    trie.insertar(String("Maria"), 1);
    trie.insertar(String("Mariana"), 2);
    trie.insertar(String("Marco"), 3);
    trie.insertar(String("Ana"), 4);

    Arreglo<int> resultado;
    trie.buscarPorPrefijo(String("Mar"), 10, resultado);
    comprobar(resultado.tamanio() == 3, "el prefijo 'Mar' encuentra tres coincidencias");

    trie.buscarPorPrefijo(String("Maria"), 10, resultado);
    comprobar(resultado.tamanio() == 2, "el prefijo 'Maria' encuentra a Maria y Mariana");

    trie.buscarPorPrefijo(String("mar"), 10, resultado);
    comprobar(resultado.tamanio() == 3, "la busqueda no distingue mayusculas de minusculas");

    trie.buscarPorPrefijo(String("Zzz"), 10, resultado);
    comprobar(resultado.vacio(), "un prefijo inexistente no devuelve nada");

    trie.buscarPorPrefijo(String("Mar"), 2, resultado);
    comprobar(resultado.tamanio() == 2, "se respeta el limite de resultados");

    comprobar(trie.eliminar(String("Marco"), 3), "eliminar encuentra y quita el id");
    trie.buscarPorPrefijo(String("Mar"), 10, resultado);
    comprobar(resultado.tamanio() == 2, "tras eliminar quedan dos coincidencias");
}




void probarGrafo() {
    comenzarGrupo("GrafoAmistades");

    GrafoAmistades grafo;
    grafo.establecerCantidadNodos(8);


    for (int i = 0; i < 6; ++i) grafo.agregarArista(i, i + 1);
    comprobar(grafo.cantidadAristas() == 6, "se contaron las aristas correctamente");
    comprobar(!grafo.agregarArista(0, 1), "no se puede duplicar una amistad existente");
    comprobar(!grafo.agregarArista(3, 3), "no se admiten amistades de un usuario consigo mismo");

    comprobar(grafo.sonAdyacentes(2, 3) && grafo.sonAdyacentes(3, 2),
              "la amistad es simetrica en ambas listas");
    comprobar(!grafo.sonAdyacentes(0, 5), "usuarios no conectados no son adyacentes");

    Arreglo<int> camino;
    comprobar(grafo.caminoMasCorto(0, 6, camino), "existe camino entre los extremos");
    comprobar(camino.tamanio() == 7 && camino[0] == 0 && camino[6] == 6,
              "el BFS bidireccional devuelve el camino completo y correcto");

    Arreglo<int> caminoSimple;
    grafo.caminoMasCortoBFSSimple(0, 6, caminoSimple);
    comprobar(caminoSimple.tamanio() == camino.tamanio(),
              "el BFS clasico halla un camino de la misma longitud");

    comprobar(!grafo.caminoMasCorto(0, 7, camino), "no hay camino hacia un nodo aislado");
    comprobar(grafo.caminoMasCorto(3, 3, camino) && camino.tamanio() == 1,
              "el camino de un usuario a si mismo tiene un solo elemento");


    Arreglo<int> comunes;
    grafo.amigosEnComun(0, 2, comunes);
    comprobar(comunes.tamanio() == 1 && comunes[0] == 1, "amigosEnComun halla la interseccion");

    grafo.amigosEnComun(0, 6, comunes);
    comprobar(comunes.vacio(), "sin amigos compartidos la interseccion queda vacia");


    Arreglo<SugerenciaAmistad> sugerencias;
    grafo.sugerirAmistades(0, 5, sugerencias);
    comprobar(!sugerencias.vacio() && sugerencias[0].indiceUsuario == 2,
              "la mejor sugerencia para el 0 es el 2");

    bool sinAmigosActuales = true;
    for (int i = 0; i < sugerencias.tamanio(); ++i) {
        if (sugerencias[i].indiceUsuario == 1 || sugerencias[i].indiceUsuario == 0) {
            sinAmigosActuales = false;
        }
    }
    comprobar(sinAmigosActuales, "las sugerencias excluyen al propio usuario y a sus amigos");

    comprobar(grafo.eliminarArista(2, 3), "eliminarArista devuelve true si existia");
    comprobar(!grafo.sonAdyacentes(2, 3) && !grafo.sonAdyacentes(3, 2),
              "la arista desaparece de las dos listas");
    comprobar(!grafo.caminoMasCorto(0, 6, camino), "al cortar la linea ya no hay camino");

    grafo.aislarNodo(1);
    comprobar(grafo.grado(1) == 0 && grafo.grado(0) == 0,
              "aislarNodo borra tambien las referencias de vuelta");
}




void probarEquivalenciaDeBFS() {
    comenzarGrupo("BFS bidireccional frente a BFS clasico (grafo grande)");

    RedSocial red;
    ParametrosGeneracion parametros;
    parametros.cantidadUsuarios = 20000;
    parametros.amistadesPorUsuario = 4;
    parametros.cantidadPublicaciones = 0;
    parametros.mostrarProgreso = false;
    GeneradorSintetico::generar(red, parametros);

    GrafoAmistades& grafo = red.grafo();
    GeneradorAleatorio azar(4242);

    Arreglo<int> caminoBidireccional;
    Arreglo<int> caminoSimple;
    bool longitudesIguales = true;
    bool caminosValidos = true;

    for (int consulta = 0; consulta < 300; ++consulta) {
        int origen = azar.enteroMenorQue(grafo.cantidadNodos());
        int destino = azar.enteroMenorQue(grafo.cantidadNodos());

        bool hayBi = grafo.caminoMasCorto(origen, destino, caminoBidireccional);
        bool haySimple = grafo.caminoMasCortoBFSSimple(origen, destino, caminoSimple);

        if (hayBi != haySimple) longitudesIguales = false;
        if (!hayBi) continue;
        if (caminoBidireccional.tamanio() != caminoSimple.tamanio()) longitudesIguales = false;


        if (caminoBidireccional.primero() != origen ||
            caminoBidireccional.ultimo() != destino) {
            caminosValidos = false;
        }
        for (int i = 1; i < caminoBidireccional.tamanio(); ++i) {
            if (!grafo.sonAdyacentes(caminoBidireccional[i - 1], caminoBidireccional[i])) {
                caminosValidos = false;
            }
        }
    }

    comprobar(longitudesIguales,
              "en 300 consultas ambos BFS coinciden en la longitud del camino minimo");
    comprobar(caminosValidos, "cada paso del camino devuelto es una amistad real");
}




void probarRedSocial() {
    comenzarGrupo("RedSocial");

    RedSocial red;
    int ana = red.registrarUsuario(String("Ana Torres"), String("ana@correo.com"), Fecha(2020, 1, 5));
    int luis = red.registrarUsuario(String("Luis Rojas"), String("luis@correo.com"), Fecha(2021, 3, 8));
    int mara = red.registrarUsuario(String("Maria Flores"), String("maria@correo.com"), Fecha(2022, 6, 1));

    comprobar(ana > 0 && luis > 0 && mara > 0, "los tres usuarios se registraron");
    comprobar(red.cantidadUsuariosActivos() == 3, "el contador de activos es correcto");
    comprobar(red.registrarUsuario(String("Otra"), String("ana@correo.com"), Fecha()) == -1,
              "no se admiten dos usuarios con el mismo correo");

    comprobar(red.buscarPorId(ana) != nullptr, "busqueda por ID");
    comprobar(red.buscarPorCorreo(String("luis@correo.com"))->id == luis, "busqueda por correo");

    Arreglo<int> encontrados;
    red.buscarPorNombre(String("Ana Torres"), encontrados);
    comprobar(encontrados.tamanio() == 1, "busqueda por nombre exacto en el AVL");

    red.buscarPorPrefijo(String("mar"), 10, encontrados);
    comprobar(encontrados.tamanio() == 1, "busqueda por prefijo en el trie");

    red.buscarPorPrefijo(String("flo"), 10, encontrados);
    comprobar(encontrados.tamanio() == 1, "el trie tambien indexa el apellido");


    comprobar(red.agregarAmigo(ana, luis), "se crea la amistad");
    comprobar(red.agregarAmigo(luis, mara), "se crea la segunda amistad");
    comprobar(!red.agregarAmigo(ana, luis), "no se duplica una amistad");
    comprobar(red.buscarPorId(luis)->cantidadAmigos == 2, "el contador de amigos se actualiza");

    Arreglo<int> camino;
    comprobar(red.caminoDeAmistad(ana, mara, camino) && camino.tamanio() == 3,
              "el camino de amistad entre Ana y Maria pasa por Luis");

    Arreglo<int> comunes;
    red.amigosEnComun(ana, mara, comunes);
    comprobar(comunes.tamanio() == 1, "Ana y Maria tienen un amigo en comun");

    Arreglo<SugerenciaAmistad> sugerencias;
    red.sugerenciasDeAmistad(ana, 5, sugerencias);
    comprobar(sugerencias.tamanio() == 1 && sugerencias[0].amigosEnComun == 1,
              "se sugiere a Maria por compartir un amigo");


    int pub1 = red.crearPublicacion(ana, String("Hola a todos"), Fecha(2026, 1, 1));
    int pub2 = red.crearPublicacion(ana, String("Segunda publicacion"), Fecha(2026, 1, 2));
    comprobar(pub1 > 0 && pub2 > 0, "se crearon las publicaciones");

    red.agregarLikes(pub1, 50);
    red.comentarPublicacion(pub1, luis, String("Muy bueno"), Fecha(2026, 1, 3));
    comprobar(red.buscarPublicacion(pub1)->numeroLikes == 50, "los likes se acumulan");
    comprobar(red.buscarPublicacion(pub1)->numeroComentarios() == 1, "el comentario se guarda");
    comprobar(red.buscarPorId(ana)->reaccionesRecibidas == 51,
              "las reacciones recibidas suman likes y comentarios");
    comprobar(red.buscarPorId(luis)->comentariosRealizados == 1,
              "se contabiliza el comentario a su autor");

    Arreglo<int> publicaciones;
    red.publicacionesDe(ana, publicaciones);
    comprobar(publicaciones.tamanio() == 2, "Ana tiene dos publicaciones");

    Arreglo<ElementoRanking> ranking;
    red.publicacionesConMasReacciones(5, ranking);
    comprobar(!ranking.vacio() && red.publicacionEnIndice(ranking[0].indice).id == pub1,
              "la publicacion con mas reacciones encabeza el ranking");

    red.usuariosMasActivos(5, ranking);
    comprobar(!ranking.vacio() && red.usuarioEnIndice(ranking[0].indice).id == ana,
              "Ana es la usuaria mas activa");

    comprobar(red.eliminarPublicacion(pub2), "se elimina una publicacion");
    red.publicacionesDe(ana, publicaciones);
    comprobar(publicaciones.tamanio() == 1, "la publicacion eliminada ya no aparece");
    comprobar(red.buscarPublicacion(pub2) == nullptr, "no se puede recuperar por ID");


    comprobar(red.eliminarUsuario(luis), "se elimina a Luis");
    comprobar(red.cantidadUsuariosActivos() == 2, "el contador de activos baja");
    comprobar(red.buscarPorId(luis) == nullptr, "Luis ya no se encuentra por ID");
    comprobar(red.buscarPorCorreo(String("luis@correo.com")) == nullptr,
              "su correo queda liberado del indice");
    comprobar(red.buscarPorId(ana)->cantidadAmigos == 0,
              "los amigos de Luis pierden esa amistad");
    comprobar(!red.caminoDeAmistad(ana, mara, camino),
              "sin Luis no queda camino entre Ana y Maria");

    red.buscarPorNombre(String("Luis Rojas"), encontrados);
    comprobar(encontrados.vacio(), "su nombre desaparece del AVL");


    int nuevo = red.registrarUsuario(String("Pedro Vargas"), String("pedro@correo.com"),
                                     Fecha(2026, 2, 2));
    comprobar(nuevo > 0 && red.cantidadUsuariosActivos() == 3, "se registra un usuario nuevo");
    comprobar(red.cantidadPosicionesUsuario() == 3,
              "se reutilizo el hueco del usuario eliminado");
    comprobar(red.buscarPorId(nuevo)->cantidadAmigos == 0,
              "el usuario nuevo no hereda las amistades del anterior");


    int gemela = red.registrarUsuario(String("Ana Torres"), String("ana.gemela@correo.com"),
                                      Fecha(2023, 4, 9));
    comprobar(gemela > 0, "se admite un segundo usuario con el mismo nombre");
    red.buscarPorNombre(String("Ana Torres"), encontrados);
    comprobar(encontrados.tamanio() == 2, "el AVL devuelve los dos homonimos");
    comprobar(red.eliminarUsuario(gemela), "se elimina uno de los homonimos");
    red.buscarPorNombre(String("Ana Torres"), encontrados);
    comprobar(encontrados.tamanio() == 1, "al quitar el homonimo queda solo el original");
}




void probarGeneradorSintetico() {
    comenzarGrupo("Generador sintetico (Barabasi-Albert)");

    RedSocial red;
    ParametrosGeneracion parametros;
    parametros.cantidadUsuarios = 30000;
    parametros.amistadesPorUsuario = 6;
    parametros.cantidadPublicaciones = 20000;
    parametros.mostrarProgreso = false;
    GeneradorSintetico::generar(red, parametros);

    comprobar(red.cantidadUsuariosActivos() == 30000, "se generaron todos los usuarios");
    comprobar(red.cantidadPublicacionesActivas() == 20000, "se generaron las publicaciones");


    TablaHash<String, int> nombresVistos;
    nombresVistos.reservarPara(30000);
    int repetidos = 0;
    bool tresPalabras = true;
    for (int i = 0; i < red.cantidadPosicionesUsuario(); ++i) {
        const Usuario& usuario = red.usuarioEnIndice(i);
        if (!usuario.activo) continue;

        if (nombresVistos.buscar(usuario.nombre) != nullptr) {
            ++repetidos;
        } else {
            nombresVistos.insertar(usuario.nombre, i);
        }

        int espacios = 0;
        for (int k = 0; k < usuario.nombre.longitud(); ++k) {
            if (usuario.nombre[k] == ' ') ++espacios;
        }
        if (espacios != 2) tresPalabras = false;
    }
    comprobar(repetidos == 0, "ningun usuario generado repite el nombre completo");
    comprobar(tresPalabras, "el nombre lleva nombre de pila y dos apellidos");

    const GrafoAmistades& grafo = red.grafo();
    comprobar(grafo.gradoPromedio() > 5.0 && grafo.gradoPromedio() < 14.0,
              "el grado promedio esta en el rango esperado (~2m)");


    comprobar(grafo.gradoMaximo() > 20 * static_cast<int>(grafo.gradoPromedio()),
              "existen 'hubs' con un grado muy superior al promedio (ley de potencias)");


    bool contadoresCoherentes = true;
    bool listasOrdenadas = true;
    for (int i = 0; i < red.cantidadPosicionesUsuario(); ++i) {
        if (red.usuarioEnIndice(i).cantidadAmigos != grafo.grado(i)) contadoresCoherentes = false;
        const Arreglo<int>& vecinos = grafo.vecinos(i);
        for (int k = 1; k < vecinos.tamanio(); ++k) {
            if (vecinos[k - 1] >= vecinos[k]) listasOrdenadas = false;
        }
    }
    comprobar(contadoresCoherentes, "cantidadAmigos coincide con el grado en el grafo");
    comprobar(listasOrdenadas, "todas las listas de adyacencia estan ordenadas y sin duplicados");


    bool simetrico = true;
    GeneradorAleatorio azar(31337);
    for (int consulta = 0; consulta < 5000; ++consulta) {
        int nodo = azar.enteroMenorQue(grafo.cantidadNodos());
        const Arreglo<int>& vecinos = grafo.vecinos(nodo);
        if (vecinos.vacio()) continue;
        int vecino = vecinos[azar.enteroMenorQue(vecinos.tamanio())];
        if (!grafo.sonAdyacentes(vecino, nodo)) simetrico = false;
    }
    comprobar(simetrico, "el grafo es simetrico: toda amistad esta en ambos sentidos");
}


int main() {
    std::printf("\n============================================================\n");
    std::printf("  PRUEBAS AUTOMATICAS DEL PROYECTO\n");
    std::printf("============================================================\n");

    probarString();
    probarArreglo();
    probarOrdenamiento();
    probarListaPilaCola();
    probarTablaHash();
    probarMonticulo();
    probarArbolAVL();
    probarTrie();
    probarGrafo();
    probarRedSocial();
    probarEquivalenciaDeBFS();
    probarGeneradorSintetico();

    std::printf("\n============================================================\n");
    std::printf("  RESULTADO: %d comprobaciones, %d fallidas\n", totalComprobaciones,
                comprobacionesFallidas);
    std::printf("============================================================\n\n");

    return (comprobacionesFallidas == 0) ? 0 : 1;
}
