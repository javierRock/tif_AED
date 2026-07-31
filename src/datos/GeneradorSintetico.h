// ============================================================================
//  GeneradorSintetico.h
//  Construye un conjunto de datos realista de hasta millones de usuarios.
//
//  MODELO DE RED: BARABASI-ALBERT (conexion preferencial)
//  ------------------------------------------------------
//  Si las amistades se sortearan de forma uniforme se obtendria un grafo
//  aleatorio donde todo el mundo tiene aproximadamente el mismo numero de
//  amigos. Las redes sociales reales NO son asi: siguen una ley de potencias,
//  es decir la mayoria de la gente tiene pocos amigos y existen unos pocos
//  "hubs" con miles de conexiones.
//
//  Barabasi-Albert reproduce ese comportamiento con una regla muy simple:
//  cada usuario nuevo elige a sus m amigos con probabilidad proporcional a
//  cuantos amigos tiene ya cada candidato ("el rico se hace mas rico").
//
//  IMPLEMENTACION EFICIENTE: la "bolsa de repeticiones".
//  Elegir con probabilidad proporcional al grado seria O(n) por sorteo si
//  hubiera que recorrer los grados. En su lugar se mantiene un arreglo donde
//  cada nodo aparece TANTAS VECES COMO AMIGOS TIENE: sacar un elemento al azar
//  de esa bolsa ya cumple la probabilidad deseada, en O(1).
//
//  Este grafo es ademas el peor escenario posible para los algoritmos, porque
//  los hubs obligan al BFS a expandir vecindarios enormes: perfecto para
//  demostrar el valor de la busqueda bidireccional.
// ============================================================================
#pragma once

#include "../sistema/RedSocial.h"
#include "GeneradorAleatorio.h"

namespace aed {

struct ParametrosGeneracion {
    int cantidadUsuarios = 100000;
    int amistadesPorUsuario = 8;    // 'm' del modelo Barabasi-Albert
    int cantidadPublicaciones = 200000;
    int maximoLikesPorPublicacion = 5000;
    int maximoComentariosPorPublicacion = 4;
    unsigned long long semilla = 20260730ULL;
    bool mostrarProgreso = true;
};

class GeneradorSintetico {
public:
    /// Llena la red social con datos sinteticos segun los parametros dados.
    static void generar(RedSocial& red, const ParametrosGeneracion& parametros);

private:
    static void generarUsuarios(RedSocial& red, const ParametrosGeneracion& parametros,
                                GeneradorAleatorio& azar);
    static void generarAmistades(RedSocial& red, const ParametrosGeneracion& parametros,
                                 GeneradorAleatorio& azar);
    static void generarPublicaciones(RedSocial& red, const ParametrosGeneracion& parametros,
                                     GeneradorAleatorio& azar);
    static void generarInteracciones(RedSocial& red, const ParametrosGeneracion& parametros,
                                     GeneradorAleatorio& azar);
};

}  // namespace aed
