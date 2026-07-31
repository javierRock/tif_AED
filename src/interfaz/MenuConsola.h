// ============================================================================
//  MenuConsola.h
//  Interfaz de texto del sistema. Es la unica parte del programa que habla
//  con el usuario; toda la logica vive en RedSocial y en GrafoAmistades.
//
//  Esa separacion es intencional: permite que el banco de pruebas y las
//  pruebas automaticas usen exactamente el mismo codigo que el menu, sin
//  ninguna dependencia de la entrada y salida.
// ============================================================================
#pragma once

#include "../estructuras/Cadena.h"
#include "../sistema/RedSocial.h"

namespace aed {

class MenuConsola {
public:
    explicit MenuConsola(RedSocial& red);

    /// Bucle principal: muestra el menu y atiende opciones hasta que el
    /// usuario elige salir o se acaba la entrada.
    void ejecutar();

private:
    void mostrarMenu() const;

    // --- Usuarios ----------------------------------------------------------
    void registrarUsuario();
    void eliminarUsuario();
    void buscarUsuario();
    void listarAlfabeticamente();

    // --- Amistades ---------------------------------------------------------
    void agregarAmigo();
    void eliminarAmigo();
    void verAmigos();
    void caminoDeAmistad();
    void amigosEnComun();
    void sugerenciasDeAmistad();

    // --- Publicaciones -----------------------------------------------------
    void crearPublicacion();
    void eliminarPublicacion();
    void verPublicacionesDeUsuario();
    void interactuarConPublicacion();

    // --- Rankings y analisis -----------------------------------------------
    void usuariosMasActivos();
    void publicacionesConMasReacciones();
    void mostrarEstadisticas();
    void ejecutarBancoDePruebas();
    void exportarDatos();
    void importarDatos();

    // --- Utilidades de presentacion ----------------------------------------
    void mostrarFichaDeUsuario(int indice) const;
    void mostrarLineaDeUsuario(int indice) const;
    void mostrarPublicacion(int indice, bool conComentarios) const;

    // --- Entrada de datos --------------------------------------------------
    Cadena leerTexto(const char* mensaje);
    int leerEntero(const char* mensaje);

    RedSocial& _red;
    bool _salir;
    bool _entradaCerrada;
};

}  // namespace aed
