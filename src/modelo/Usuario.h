// ============================================================================
//  Usuario.h
//  Usuario de la red social.
//
//  Campos exigidos por el enunciado: ID unico, nombre, correo, fecha de
//  registro, lista de amigos, publicaciones realizadas, cantidad de
//  seguidores, cantidad de amigos y reacciones recibidas.
//
//  DONDE ESTA LA LISTA DE AMIGOS
//  -----------------------------
//  No se guarda dentro del Usuario, sino en la clase GrafoAmistades. Motivo:
//  la amistad es una relacion entre DOS usuarios y los algoritmos que la
//  explotan (camino mas corto, amigos en comun, sugerencias) son algoritmos
//  de grafos. Tenerla centralizada permite recorrer las listas de adyacencia
//  de forma contigua en memoria, que es lo que hace viable un BFS sobre un
//  millon de nodos. El campo 'cantidadAmigos' que si vive aqui es el grado
//  del nodo, y RedSocial lo mantiene sincronizado al crear o borrar amistades.
//
//  BORRADO LOGICO: al eliminar un usuario se marca 'activo = false'. Su
//  posicion en el arreglo se conserva porque el grafo referencia usuarios por
//  indice; la posicion queda en una lista de huecos libres para reutilizarla.
// ============================================================================
#pragma once

#include "../estructuras/Arreglo.h"
#include "../estructuras/Cadena.h"
#include "Fecha.h"

namespace aed {

struct Usuario {
    int id = -1;
    Cadena nombre;
    Cadena correo;
    Fecha fechaRegistro;

    /// Posiciones (no ids) de sus publicaciones dentro del arreglo global.
    /// Guardar indices en vez de punteros evita que un crecimiento del
    /// arreglo de publicaciones invalide las referencias.
    Arreglo<int> indicesPublicaciones;

    int cantidadAmigos = 0;      // grado del nodo en el grafo de amistades
    int cantidadSeguidores = 0;
    long long reaccionesRecibidas = 0;
    int comentariosRealizados = 0;
    bool activo = true;

    Usuario() = default;

    Usuario(int idUsuario, const Cadena& nombreUsuario, const Cadena& correoUsuario,
            const Fecha& registro)
        : id(idUsuario), nombre(nombreUsuario), correo(correoUsuario), fechaRegistro(registro) {}

    int cantidadPublicaciones() const { return indicesPublicaciones.tamanio(); }

    /// Puntaje usado en el ranking de "usuarios mas activos".
    ///
    /// Combina lo que el usuario PRODUCE con el impacto que consigue:
    ///     publicacion = 3 puntos, comentario = 2 puntos, reaccion = 1 punto.
    ///
    /// Nota para la sustentacion: como una publicacion popular acumula miles
    /// de reacciones, en la practica el tercer termino es el de mayor
    /// magnitud. El efecto es que encabezan el ranking quienes publican mucho
    /// Y ademas generan interaccion, no quienes solo publican. Si se quisiera
    /// medir unicamente la produccion de contenido bastaria con quitar el
    /// ultimo sumando; el resto del sistema no cambia.
    long long puntajeActividad() const {
        return static_cast<long long>(cantidadPublicaciones()) * 3 +
               static_cast<long long>(comentariosRealizados) * 2 + reaccionesRecibidas;
    }
};

}  // namespace aed
