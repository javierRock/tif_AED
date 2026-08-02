








#pragma once

#include "../estructuras/String.h"
#include "../sistema/RedSocial.h"

namespace aed {

class MenuConsola {
public:
    explicit MenuConsola(RedSocial& red);



    void ejecutar();

private:
    void mostrarMenu() const;


    void registrarUsuario();
    void eliminarUsuario();
    void buscarUsuario();
    void listarAlfabeticamente();


    void agregarAmigo();
    void eliminarAmigo();
    void verAmigos();
    void caminoDeAmistad();
    void amigosEnComun();
    void sugerenciasDeAmistad();


    void crearPublicacion();
    void eliminarPublicacion();
    void verPublicacionesDeUsuario();
    void interactuarConPublicacion();


    void usuariosMasActivos();
    void publicacionesConMasReacciones();
    void mostrarEstadisticas();
    void ejecutarBancoDePruebas();
    void exportarDatos();
    void importarDatos();


    void mostrarFichaDeUsuario(int indice) const;
    void mostrarLineaDeUsuario(int indice) const;
    void mostrarPublicacion(int indice, bool conComentarios) const;


    String leerTexto(const char* mensaje);
    int leerEntero(const char* mensaje);

    RedSocial& _red;
    bool _salir;
    bool _entradaCerrada;
};

}
