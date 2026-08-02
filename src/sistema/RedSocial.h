

#pragma once

#include "../estructuras/ArbolAVL.h"
#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"
#include "../estructuras/TablaHash.h"
#include "../estructuras/Trie.h"
#include "../modelo/Publicacion.h"
#include "../modelo/Usuario.h"
#include "GrafoAmistades.h"

namespace aed
{

    struct ElementoRanking
    {
        int indice = -1;
        long long puntaje = 0;

        ElementoRanking() = default;
        ElementoRanking(int i, long long p) : indice(i), puntaje(p) {}

        bool operator<(const ElementoRanking &otro) const
        {
            if (puntaje != otro.puntaje)
                return puntaje < otro.puntaje;
            return indice > otro.indice;
        }
    };

    class RedSocial
    {
    public:
        RedSocial();

        RedSocial(const RedSocial &) = delete;
        RedSocial &operator=(const RedSocial &) = delete;

        int registrarUsuario(const String &nombre, const String &correo, const Fecha &fechaRegistro);

        bool eliminarUsuario(int id);

        Usuario *buscarPorId(int id);
        Usuario *buscarPorCorreo(const String &correo);

        void buscarPorNombre(const String &nombre, Arreglo<int> &indices);

        void buscarPorPrefijo(const String &prefijo, int limite, Arreglo<int> &indices);

        void listarEnOrdenAlfabetico(int limite, Arreglo<int> &indices);

        bool agregarAmigo(int idA, int idB);
        bool eliminarAmigo(int idA, int idB);
        bool sonAmigos(int idA, int idB);

        bool caminoDeAmistad(int idA, int idB, Arreglo<int> &indicesDelCamino);

        void amigosEnComun(int idA, int idB, Arreglo<int> &indicesComunes);

        void sugerenciasDeAmistad(int id, int cantidad, Arreglo<SugerenciaAmistad> &sugerencias);

        void amigosDe(int id, Arreglo<int> &indicesAmigos);

        int crearPublicacion(int idAutor, const String &texto, const Fecha &fecha);
        bool eliminarPublicacion(int idPublicacion);
        bool darLike(int idPublicacion);

        bool agregarLikes(int idPublicacion, int cantidad);
        int comentarPublicacion(int idPublicacion, int idAutor, const String &texto,
                                const Fecha &fecha);

        Publicacion *buscarPublicacion(int idPublicacion);

        void publicacionesDe(int idUsuario, Arreglo<int> &indicesPublicaciones);

        void usuariosMasActivos(int cantidad, Arreglo<ElementoRanking> &ranking);

        void publicacionesConMasReacciones(int cantidad, Arreglo<ElementoRanking> &ranking);

        int cantidadPosicionesUsuario() const { return _usuarios.tamanio(); }
        int cantidadUsuariosActivos() const { return _usuariosActivos; }
        int cantidadPosicionesPublicacion() const { return _publicaciones.tamanio(); }
        int cantidadPublicacionesActivas() const { return _publicacionesActivas; }

        Usuario &usuarioEnIndice(int indice) { return _usuarios[indice]; }
        const Usuario &usuarioEnIndice(int indice) const { return _usuarios[indice]; }
        Publicacion &publicacionEnIndice(int indice) { return _publicaciones[indice]; }
        const Publicacion &publicacionEnIndice(int indice) const { return _publicaciones[indice]; }

        int indiceDeUsuario(int id) const;

        GrafoAmistades &grafo() { return _grafo; }
        const GrafoAmistades &grafo() const { return _grafo; }

        int alturaDelArbolDeNombres() const { return _indicePorNombre.altura(); }
        int nombresDistintos() const { return _indicePorNombre.tamanio(); }
        int nodosDelTrie() const { return _indicePrefijos.cantidadNodos(); }

        long long memoriaAproximadaBytes() const;

        void reservarCapacidad(int usuariosEsperados, int publicacionesEsperadas);

        void construirIndicesDeTexto(bool activo) { _indicesDeTextoActivos = activo; }

        void sincronizarContadoresDeAmigos();

    private:
        void indexarTextos(int indice);
        void desindexarTextos(int indice);

        template <typename Accion>
        static void porCadaPalabra(const String &texto, Accion accion)
        {
            int inicio = 0;
            for (int i = 0; i <= texto.longitud(); ++i)
            {
                bool finDePalabra = (i == texto.longitud()) || (texto[i] == ' ');
                if (finDePalabra)
                {
                    if (i > inicio)
                        accion(texto.subcadena(inicio, i - inicio));
                    inicio = i + 1;
                }
            }
        }

        Arreglo<Usuario> _usuarios;
        Arreglo<Publicacion> _publicaciones;
        GrafoAmistades _grafo;

        TablaHash<int, int> _indicePorId;
        TablaHash<String, int> _indicePorCorreo;
        TablaHash<int, int> _indicePublicacionPorId;
        ArbolAVL<String, Arreglo<int>> _indicePorNombre;
        Trie _indicePrefijos;

        Arreglo<int> _posicionesLibres;
        int _siguienteIdUsuario;
        int _siguienteIdPublicacion;
        int _siguienteIdComentario;
        int _usuariosActivos;
        int _publicacionesActivas;
        bool _indicesDeTextoActivos;
    };

}
