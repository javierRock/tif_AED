#include "BancoPruebas.h"

#include <cstdio>

#include "../datos/ArchivoCSV.h"
#include "../datos/GeneradorAleatorio.h"
#include "../estructuras/Ordenamiento.h"
#include "Cronometro.h"

namespace aed {

namespace {

void anotar(Arreglo<ResultadoPrueba>& resultados, const char* operacion, long long escala,
            long long repeticiones, double milisegundos) {
    ResultadoPrueba resultado;
    resultado.operacion = String(operacion);
    resultado.escala = escala;
    resultado.repeticiones = repeticiones;
    resultado.milisegundosTotales = milisegundos;
    resultados.agregar(mover(resultado));
}


void elegirUsuariosAlAzar(RedSocial& red, GeneradorAleatorio& azar, int cantidad,
                          Arreglo<int>& indices) {
    indices.limpiar();
    int posiciones = red.cantidadPosicionesUsuario();
    if (posiciones == 0) return;

    int intentos = 0;
    int maximoIntentos = cantidad * 10 + 100;
    while (indices.tamanio() < cantidad && intentos < maximoIntentos) {
        ++intentos;
        int indice = azar.enteroMenorQue(posiciones);
        if (red.usuarioEnIndice(indice).activo) indices.agregar(indice);
    }
}

}


void BancoPruebas::medirOperaciones(RedSocial& red, Arreglo<ResultadoPrueba>& resultados) {
    GeneradorAleatorio azar(987654321ULL);
    const long long escala = red.cantidadUsuariosActivos();
    if (escala == 0) return;

    Cronometro reloj;


    const int CONSULTAS = 100000;
    const int CONSULTAS_PESADAS = 200;

    Arreglo<int> muestra;
    elegirUsuariosAlAzar(red, azar, CONSULTAS, muestra);
    if (muestra.vacio()) return;




    long long encontrados = 0;
    reloj.iniciar();
    for (int i = 0; i < muestra.tamanio(); ++i) {
        int id = red.usuarioEnIndice(muestra[i]).id;
        if (red.buscarPorId(id) != nullptr) ++encontrados;
    }
    anotar(resultados, "Buscar por ID (TablaHash)", escala, muestra.tamanio(), reloj.milisegundos());




    Arreglo<String> correos;
    Arreglo<String> nombres;
    int muestraTexto = muestra.tamanio() < 20000 ? muestra.tamanio() : 20000;
    correos.reservar(muestraTexto);
    nombres.reservar(muestraTexto);
    for (int i = 0; i < muestraTexto; ++i) {
        correos.agregar(red.usuarioEnIndice(muestra[i]).correo);
        nombres.agregar(red.usuarioEnIndice(muestra[i]).nombre);
    }

    reloj.iniciar();
    for (int i = 0; i < correos.tamanio(); ++i) {
        if (red.buscarPorCorreo(correos[i]) != nullptr) ++encontrados;
    }
    anotar(resultados, "Buscar por correo (TablaHash)", escala, correos.tamanio(),
           reloj.milisegundos());











    Arreglo<int> coincidencias;
    reloj.iniciar();
    for (int i = 0; i < nombres.tamanio(); ++i) {
        red.buscarPorNombre(nombres[i], coincidencias);
    }
    anotar(resultados, "Buscar nombre+homonimos (AVL)", escala, nombres.tamanio(),
           reloj.milisegundos());




    const int CONSULTAS_PREFIJO = 5000;
    reloj.iniciar();
    for (int i = 0; i < CONSULTAS_PREFIJO; ++i) {
        const String& nombre = nombres[i % nombres.tamanio()];
        red.buscarPorPrefijo(nombre.subcadena(0, 3), 10, coincidencias);
    }
    anotar(resultados, "Buscar por prefijo (Trie)", escala, CONSULTAS_PREFIJO,
           reloj.milisegundos());









    {
        int cantidadClaves = static_cast<int>(escala);
        const int CONSULTAS_COMPARATIVA = 200000;



        Arreglo<int> claves(cantidadClaves);
        for (int i = 0; i < cantidadClaves; ++i) claves.agregar(i);
        for (int i = cantidadClaves - 1; i > 0; --i) {
            intercambiar(claves[i], claves[azar.enteroMenorQue(i + 1)]);
        }

        TablaHash<int, int> tabla(16);
        tabla.reservarPara(cantidadClaves);
        reloj.iniciar();
        for (int i = 0; i < cantidadClaves; ++i) tabla.insertar(claves[i], i);
        anotar(resultados, "Construir TablaHash", escala, cantidadClaves, reloj.milisegundos());

        ArbolAVL<int, int> arbol;
        reloj.iniciar();
        for (int i = 0; i < cantidadClaves; ++i) arbol.insertar(claves[i], i);
        anotar(resultados, "Construir ArbolAVL", escala, cantidadClaves, reloj.milisegundos());

        reloj.iniciar();
        for (int i = 0; i < CONSULTAS_COMPARATIVA; ++i) {
            const int* valor = tabla.buscar(claves[i % cantidadClaves]);
            if (valor != nullptr) ++encontrados;
        }
        anotar(resultados, "Consultar TablaHash O(1)", escala, CONSULTAS_COMPARATIVA,
               reloj.milisegundos());

        reloj.iniciar();
        for (int i = 0; i < CONSULTAS_COMPARATIVA; ++i) {
            const int* valor = arbol.buscar(claves[i % cantidadClaves]);
            if (valor != nullptr) ++encontrados;
        }
        anotar(resultados, "Consultar ArbolAVL O(log n)", escala, CONSULTAS_COMPARATIVA,
               reloj.milisegundos());
    }




    reloj.iniciar();
    for (int i = 0; i + 1 < muestra.tamanio(); i += 2) {
        red.grafo().sonAdyacentes(muestra[i], muestra[i + 1]);
    }
    anotar(resultados, "Comprobar amistad (bin.)", escala, muestra.tamanio() / 2,
           reloj.milisegundos());




    reloj.iniciar();
    for (int i = 0; i + 1 < muestra.tamanio(); i += 2) {
        red.grafo().amigosEnComun(muestra[i], muestra[i + 1], coincidencias);
    }
    anotar(resultados, "Amigos en comun", escala, muestra.tamanio() / 2, reloj.milisegundos());




    Arreglo<int> camino;
    int paresPesados = CONSULTAS_PESADAS;
    if (paresPesados * 2 > muestra.tamanio()) paresPesados = muestra.tamanio() / 2;

    reloj.iniciar();
    for (int i = 0; i < paresPesados; ++i) {
        red.grafo().caminoMasCorto(muestra[2 * i], muestra[2 * i + 1], camino);
    }
    anotar(resultados, "Camino BFS bidireccional", escala, paresPesados, reloj.milisegundos());

    reloj.iniciar();
    for (int i = 0; i < paresPesados; ++i) {
        red.grafo().caminoMasCortoBFSSimple(muestra[2 * i], muestra[2 * i + 1], camino);
    }
    anotar(resultados, "Camino BFS clasico", escala, paresPesados, reloj.milisegundos());




    Arreglo<SugerenciaAmistad> sugerencias;
    reloj.iniciar();
    for (int i = 0; i < paresPesados; ++i) {
        red.grafo().sugerirAmistades(muestra[i], 10, sugerencias);
    }
    anotar(resultados, "Sugerencias de amistad", escala, paresPesados, reloj.milisegundos());




    Arreglo<ElementoRanking> ranking;
    const int REPETICIONES_RANKING = 5;

    reloj.iniciar();
    for (int i = 0; i < REPETICIONES_RANKING; ++i) red.usuariosMasActivos(10, ranking);
    anotar(resultados, "Top 10 usuarios activos", escala, REPETICIONES_RANKING,
           reloj.milisegundos());

    reloj.iniciar();
    for (int i = 0; i < REPETICIONES_RANKING; ++i) red.publicacionesConMasReacciones(10, ranking);
    anotar(resultados, "Top 10 publicaciones", escala, REPETICIONES_RANKING, reloj.milisegundos());




    const int OPERACIONES_AMISTAD = 20000;
    int pares = OPERACIONES_AMISTAD;
    if (pares * 2 > muestra.tamanio()) pares = muestra.tamanio() / 2;

    reloj.iniciar();
    for (int i = 0; i < pares; ++i) {
        red.grafo().agregarArista(muestra[2 * i], muestra[2 * i + 1]);
    }
    anotar(resultados, "Agregar amistad", escala, pares, reloj.milisegundos());

    reloj.iniciar();
    for (int i = 0; i < pares; ++i) {
        red.grafo().eliminarArista(muestra[2 * i], muestra[2 * i + 1]);
    }
    anotar(resultados, "Eliminar amistad", escala, pares, reloj.milisegundos());




    int cantidadOrdenar = static_cast<int>(escala);
    Arreglo<int> original(cantidadOrdenar);
    for (int i = 0; i < cantidadOrdenar; ++i) original.agregar(azar.enteroMenorQue(1000000000));

    Arreglo<int> copia(original);
    reloj.iniciar();
    ordenarRapido(copia);
    anotar(resultados, "Quicksort propio", escala, cantidadOrdenar, reloj.milisegundos());

    copia = original;
    reloj.iniciar();
    ordenarPorMezcla(copia, MenorQue());
    anotar(resultados, "Mergesort propio", escala, cantidadOrdenar, reloj.milisegundos());



    if (encontrados < 0) std::printf(" ");
}


void BancoPruebas::ejecutarEscala(const ParametrosGeneracion& parametros,
                                  Arreglo<ResultadoPrueba>& resultados) {
    RedSocial red;
    Cronometro reloj;

    ParametrosGeneracion silencioso = parametros;
    silencioso.mostrarProgreso = false;

    reloj.iniciar();
    GeneradorSintetico::generar(red, silencioso);
    anotar(resultados, "Construccion completa", red.cantidadUsuariosActivos(), 1,
           reloj.milisegundos());

    medirOperaciones(red, resultados);
}


void BancoPruebas::ejecutarSerieDeEscalas(int usuariosMaximos, const char* rutaCsv) {
    Arreglo<ResultadoPrueba> resultados;




    int escalas[] = {10000, 100000, 1000000, 2000000};
    int cantidadEscalas = static_cast<int>(sizeof(escalas) / sizeof(escalas[0]));

    for (int i = 0; i < cantidadEscalas; ++i) {
        if (escalas[i] > usuariosMaximos) break;

        ParametrosGeneracion parametros;
        parametros.cantidadUsuarios = escalas[i];
        parametros.amistadesPorUsuario = 8;
        parametros.cantidadPublicaciones = escalas[i] * 2;
        parametros.mostrarProgreso = false;

        std::printf("  Escala de %d usuarios... ", escalas[i]);
        std::fflush(stdout);

        Cronometro reloj;
        ejecutarEscala(parametros, resultados);
        std::printf("listo (%.1f s)\n", reloj.segundos());
    }

    std::printf("\n");
    imprimirTabla(resultados);

    if (rutaCsv != nullptr && exportarCSV(resultados, rutaCsv)) {
        std::printf("\n  Resultados exportados a: %s\n", rutaCsv);
    }
}


void BancoPruebas::imprimirTabla(const Arreglo<ResultadoPrueba>& resultados) {
    std::printf("  %-30s %12s %12s %14s %16s\n", "OPERACION", "ESCALA", "REPETIC.",
                "TOTAL (ms)", "POR OPER. (us)");
    std::printf("  %s\n",
                "----------------------------------------------------------"
                "--------------------------------");

    for (int i = 0; i < resultados.tamanio(); ++i) {
        const ResultadoPrueba& r = resultados[i];
        std::printf("  %-30s %12lld %12lld %14.2f %16.4f\n", r.operacion.texto(), r.escala,
                    r.repeticiones, r.milisegundosTotales, r.microsegundosPorOperacion());
    }
}

bool BancoPruebas::exportarCSV(const Arreglo<ResultadoPrueba>& resultados, const char* ruta) {
    EscritorCSV escritor;
    if (!escritor.abrir(ruta)) return false;

    escritor.campo("operacion");
    escritor.campo("escala_usuarios");
    escritor.campo("repeticiones");
    escritor.campo("milisegundos_totales");
    escritor.campo("microsegundos_por_operacion");
    escritor.finDeFila();

    for (int i = 0; i < resultados.tamanio(); ++i) {
        const ResultadoPrueba& r = resultados[i];
        char numero[64];

        escritor.campo(r.operacion);
        escritor.campo(r.escala);
        escritor.campo(r.repeticiones);
        std::snprintf(numero, sizeof(numero), "%.4f", r.milisegundosTotales);
        escritor.campo(numero);
        std::snprintf(numero, sizeof(numero), "%.6f", r.microsegundosPorOperacion());
        escritor.campo(numero);
        escritor.finDeFila();
    }
    return true;
}

}
