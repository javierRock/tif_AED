// ============================================================================
//  BancoPruebas.h
//  Banco de pruebas de rendimiento ("analisis de rendimiento" del enunciado).
//
//  Mide el costo real de cada operacion del sistema y lo contrasta con la
//  complejidad teorica esperada. Las comparaciones mas ilustrativas son:
//
//      TablaHash O(1)     frente a   ArbolAVL O(log n)   buscando usuarios
//      BFS bidireccional  frente a   BFS clasico         buscando caminos
//      Quicksort          frente a   Mergesort           ordenando enteros
//      Monticulo O(n log K) frente a ordenar todo O(n log n)  en el top K
//
//  Los resultados se imprimen en una tabla y se exportan a CSV para poder
//  graficarlos despues (el enunciado permite generar graficos estadisticos).
// ============================================================================
#pragma once

#include "../datos/GeneradorSintetico.h"
#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"
#include "../sistema/RedSocial.h"

namespace aed {

struct ResultadoPrueba {
    String operacion;
    long long escala = 0;             // numero de usuarios de la prueba
    long long repeticiones = 0;       // cuantas veces se ejecuto la operacion
    double milisegundosTotales = 0.0;

    /// Costo medio de UNA ejecucion, en microsegundos.
    double microsegundosPorOperacion() const {
        if (repeticiones <= 0) return 0.0;
        return (milisegundosTotales * 1000.0) / static_cast<double>(repeticiones);
    }
};

class BancoPruebas {
public:
    /// Mide todas las operaciones sobre una red ya construida.
    static void medirOperaciones(RedSocial& red, Arreglo<ResultadoPrueba>& resultados);

    /// Construye una red con los parametros dados midiendo la carga, y luego
    /// mide las operaciones sobre ella.
    static void ejecutarEscala(const ParametrosGeneracion& parametros,
                               Arreglo<ResultadoPrueba>& resultados);

    /// Ejecuta la bateria completa en varias escalas crecientes para poder
    /// observar como evoluciona cada operacion al aumentar n.
    static void ejecutarSerieDeEscalas(int usuariosMaximos, const char* rutaCsv);

    static void imprimirTabla(const Arreglo<ResultadoPrueba>& resultados);
    static bool exportarCSV(const Arreglo<ResultadoPrueba>& resultados, const char* ruta);
};

}  // namespace aed
