// ============================================================================
//  main.cpp
//  Punto de entrada del sistema.
//
//  Uso tipico:
//      ./redsocial                          -> demo con 100 000 usuarios
//      ./redsocial --usuarios 1000000       -> escala de un millon
//      ./redsocial --banco --maximo 1000000 -> solo analisis de rendimiento
//      ./redsocial --vacia                  -> arranca sin datos
// ============================================================================
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "analisis/BancoPruebas.h"
#include "analisis/Cronometro.h"
#include "datos/GeneradorSintetico.h"
#include "interfaz/MenuConsola.h"
#include "sistema/RedSocial.h"

using namespace aed;

namespace {

struct OpcionesDeLinea {
    ParametrosGeneracion generacion;
    bool soloBancoDePruebas = false;
    bool arrancarVacia = false;
    int maximoDelBanco = 1000000;
};

void mostrarAyuda() {
    std::printf(
        "\nRED SOCIAL - Proyecto Final de Algoritmos y Estructuras de Datos\n\n"
        "  --usuarios N        Cantidad de usuarios a generar (por defecto 100000)\n"
        "  --amistades M       Amistades por usuario nuevo, modelo Barabasi-Albert\n"
        "                      (por defecto 8; el total de aristas es aprox. N*M)\n"
        "  --publicaciones P   Cantidad de publicaciones (por defecto 2*N)\n"
        "  --semilla S         Semilla del generador aleatorio (datos reproducibles)\n"
        "  --vacia             No genera datos: la red arranca sin usuarios\n"
        "  --banco             Ejecuta solo el banco de pruebas y termina\n"
        "  --maximo N          Escala maxima del banco de pruebas\n"
        "  --ayuda             Muestra este mensaje\n\n");
}

/// Interpreta los argumentos. Devuelve false si hay que terminar el programa.
bool interpretarArgumentos(int cantidad, char** argumentos, OpcionesDeLinea& opciones) {
    bool publicacionesIndicadas = false;

    for (int i = 1; i < cantidad; ++i) {
        const char* actual = argumentos[i];
        bool quedaValor = (i + 1 < cantidad);

        if (std::strcmp(actual, "--ayuda") == 0 || std::strcmp(actual, "-h") == 0) {
            mostrarAyuda();
            return false;
        } else if (std::strcmp(actual, "--vacia") == 0) {
            opciones.arrancarVacia = true;
        } else if (std::strcmp(actual, "--banco") == 0) {
            opciones.soloBancoDePruebas = true;
        } else if (std::strcmp(actual, "--usuarios") == 0 && quedaValor) {
            opciones.generacion.cantidadUsuarios = std::atoi(argumentos[++i]);
        } else if (std::strcmp(actual, "--amistades") == 0 && quedaValor) {
            opciones.generacion.amistadesPorUsuario = std::atoi(argumentos[++i]);
        } else if (std::strcmp(actual, "--publicaciones") == 0 && quedaValor) {
            opciones.generacion.cantidadPublicaciones = std::atoi(argumentos[++i]);
            publicacionesIndicadas = true;
        } else if (std::strcmp(actual, "--semilla") == 0 && quedaValor) {
            opciones.generacion.semilla =
                static_cast<unsigned long long>(std::atoll(argumentos[++i]));
        } else if (std::strcmp(actual, "--maximo") == 0 && quedaValor) {
            opciones.maximoDelBanco = std::atoi(argumentos[++i]);
        } else {
            std::printf("Argumento no reconocido: %s\n", actual);
            mostrarAyuda();
            return false;
        }
    }

    // Por defecto se generan dos publicaciones por usuario.
    if (!publicacionesIndicadas) {
        opciones.generacion.cantidadPublicaciones = opciones.generacion.cantidadUsuarios * 2;
    }
    return true;
}

void mostrarResumenDeCarga(const RedSocial& red, double segundos) {
    const GrafoAmistades& grafo = red.grafo();
    double megas = static_cast<double>(red.memoriaAproximadaBytes()) / (1024.0 * 1024.0);

    std::printf("\n  Datos generados en %.2f segundos\n", segundos);
    std::printf("  ------------------------------------------------------------\n");
    std::printf("   Usuarios ............ %d\n", red.cantidadUsuariosActivos());
    std::printf("   Amistades ........... %lld\n", grafo.cantidadAristas());
    std::printf("   Grado promedio ...... %.2f  (maximo: %d)\n", grafo.gradoPromedio(),
                grafo.gradoMaximo());
    std::printf("   Publicaciones ....... %d\n", red.cantidadPublicacionesActivas());
    std::printf("   Memoria aproximada .. %.1f MB\n", megas);
    std::printf("  ------------------------------------------------------------\n\n");
}

}  // namespace

int main(int cantidadArgumentos, char** argumentos) {
    OpcionesDeLinea opciones;
    opciones.generacion.cantidadUsuarios = 100000;

    if (!interpretarArgumentos(cantidadArgumentos, argumentos, opciones)) return 0;

    // --- Modo exclusivo de analisis de rendimiento -------------------------
    if (opciones.soloBancoDePruebas) {
        std::printf("\n  BANCO DE PRUEBAS DE RENDIMIENTO\n");
        std::printf("  ------------------------------------------------------------\n");
        BancoPruebas::ejecutarSerieDeEscalas(opciones.maximoDelBanco, "benchmark.csv");
        return 0;
    }

    RedSocial red;

    if (!opciones.arrancarVacia) {
        std::printf("\n  Generando datos sinteticos (%d usuarios, %d publicaciones)...\n",
                    opciones.generacion.cantidadUsuarios,
                    opciones.generacion.cantidadPublicaciones);

        Cronometro reloj;
        GeneradorSintetico::generar(red, opciones.generacion);
        mostrarResumenDeCarga(red, reloj.segundos());
    }

    MenuConsola menu(red);
    menu.ejecutar();
    return 0;
}
