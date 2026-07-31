#include "ArchivoCSV.h"

#include "../sistema/RedSocial.h"

namespace aed {

namespace {
const int TAMANIO_BUFFER = 64 * 1024;  // 64 KB por lectura
}

// ===========================================================================
//  LectorCSV
// ===========================================================================
LectorCSV::LectorCSV()
    : _archivo(nullptr), _buffer(nullptr), _bytesEnBuffer(0), _posicion(0) {}

LectorCSV::~LectorCSV() {
    cerrar();
}

bool LectorCSV::abrir(const char* ruta) {
    cerrar();
    _archivo = std::fopen(ruta, "rb");
    if (_archivo == nullptr) return false;
    _buffer = new char[TAMANIO_BUFFER];
    _bytesEnBuffer = 0;
    _posicion = 0;
    return true;
}

void LectorCSV::cerrar() {
    if (_archivo != nullptr) {
        std::fclose(_archivo);
        _archivo = nullptr;
    }
    delete[] _buffer;
    _buffer = nullptr;
    _bytesEnBuffer = 0;
    _posicion = 0;
}

int LectorCSV::siguienteCaracter() {
    // Cuando se agota el bloque en memoria se pide el siguiente al disco.
    if (_posicion >= _bytesEnBuffer) {
        _bytesEnBuffer = static_cast<int>(
            std::fread(_buffer, 1, static_cast<unsigned long>(TAMANIO_BUFFER), _archivo));
        _posicion = 0;
        if (_bytesEnBuffer <= 0) return -1;
    }
    return static_cast<unsigned char>(_buffer[_posicion++]);
}

bool LectorCSV::leerFila(Arreglo<Cadena>& campos) {
    if (_archivo == nullptr) return false;

    campos.limpiar();
    Cadena actual;
    bool hayContenido = false;

    while (true) {
        int caracter = siguienteCaracter();

        if (caracter == -1) {
            if (!hayContenido) return false;  // fin de archivo sin datos
            campos.agregar(mover(actual));
            return true;
        }
        if (caracter == '\n') {
            campos.agregar(mover(actual));
            return true;
        }
        if (caracter == '\r') continue;  // finales de linea de Windows

        hayContenido = true;
        if (caracter == ',') {
            campos.agregar(mover(actual));
            actual = Cadena();
        } else {
            actual.agregar(static_cast<char>(caracter));
        }
    }
}

// ===========================================================================
//  EscritorCSV
// ===========================================================================
EscritorCSV::EscritorCSV() : _archivo(nullptr), _primerCampoDeLaFila(true) {}

EscritorCSV::~EscritorCSV() {
    cerrar();
}

bool EscritorCSV::abrir(const char* ruta) {
    cerrar();
    _archivo = std::fopen(ruta, "wb");
    _primerCampoDeLaFila = true;
    return _archivo != nullptr;
}

void EscritorCSV::cerrar() {
    if (_archivo != nullptr) {
        std::fclose(_archivo);
        _archivo = nullptr;
    }
}

void EscritorCSV::escribirSeparador() {
    if (!_primerCampoDeLaFila) std::fputc(',', _archivo);
    _primerCampoDeLaFila = false;
}

void EscritorCSV::campo(const Cadena& texto) {
    if (_archivo == nullptr) return;
    escribirSeparador();
    // Se neutralizan los caracteres que romperian el formato.
    for (int i = 0; i < texto.longitud(); ++i) {
        char c = texto[i];
        if (c == ',' || c == '\n' || c == '\r') c = ' ';
        std::fputc(c, _archivo);
    }
}

void EscritorCSV::campo(const char* texto) {
    campo(Cadena(texto));
}

void EscritorCSV::campo(long long numero) {
    if (_archivo == nullptr) return;
    escribirSeparador();
    std::fprintf(_archivo, "%lld", numero);
}

void EscritorCSV::finDeFila() {
    if (_archivo == nullptr) return;
    std::fputc('\n', _archivo);
    _primerCampoDeLaFila = true;
}

// ===========================================================================
//  Volcado de la red social
// ===========================================================================
namespace archivo {

bool guardarUsuarios(const RedSocial& red, const char* ruta) {
    EscritorCSV escritor;
    if (!escritor.abrir(ruta)) return false;

    escritor.campo("id");
    escritor.campo("nombre");
    escritor.campo("correo");
    escritor.campo("fecha_registro");
    escritor.campo("seguidores");
    escritor.campo("reacciones_recibidas");
    escritor.campo("comentarios_realizados");
    escritor.finDeFila();

    for (int i = 0; i < red.cantidadPosicionesUsuario(); ++i) {
        const Usuario& usuario = red.usuarioEnIndice(i);
        if (!usuario.activo) continue;

        escritor.campo(usuario.id);
        escritor.campo(usuario.nombre);
        escritor.campo(usuario.correo);
        escritor.campo(usuario.fechaRegistro.aTexto());
        escritor.campo(usuario.cantidadSeguidores);
        escritor.campo(usuario.reaccionesRecibidas);
        escritor.campo(usuario.comentariosRealizados);
        escritor.finDeFila();
    }
    return true;
}

bool guardarAmistades(const RedSocial& red, const char* ruta) {
    EscritorCSV escritor;
    if (!escritor.abrir(ruta)) return false;

    escritor.campo("id_a");
    escritor.campo("id_b");
    escritor.finDeFila();

    const GrafoAmistades& grafo = red.grafo();
    for (int i = 0; i < grafo.cantidadNodos(); ++i) {
        const Usuario& usuario = red.usuarioEnIndice(i);
        if (!usuario.activo) continue;

        const Arreglo<int>& amigos = grafo.vecinos(i);
        for (int k = 0; k < amigos.tamanio(); ++k) {
            // Cada amistad se escribe una sola vez (la del indice menor).
            if (amigos[k] <= i) continue;
            escritor.campo(usuario.id);
            escritor.campo(red.usuarioEnIndice(amigos[k]).id);
            escritor.finDeFila();
        }
    }
    return true;
}

bool guardarPublicaciones(const RedSocial& red, const char* ruta) {
    EscritorCSV escritor;
    if (!escritor.abrir(ruta)) return false;

    escritor.campo("id");
    escritor.campo("id_autor");
    escritor.campo("fecha");
    escritor.campo("likes");
    escritor.campo("comentarios");
    escritor.campo("texto");
    escritor.finDeFila();

    for (int i = 0; i < red.cantidadPosicionesPublicacion(); ++i) {
        const Publicacion& publicacion = red.publicacionEnIndice(i);
        if (!publicacion.activa) continue;

        escritor.campo(publicacion.id);
        escritor.campo(publicacion.idPropietario);
        escritor.campo(publicacion.fecha.aTexto());
        escritor.campo(publicacion.numeroLikes);
        escritor.campo(publicacion.numeroComentarios());
        escritor.campo(publicacion.texto);
        escritor.finDeFila();
    }
    return true;
}

// ---------------------------------------------------------------------------
bool cargarUsuarios(RedSocial& red, const char* ruta) {
    LectorCSV lector;
    if (!lector.abrir(ruta)) return false;

    Arreglo<Cadena> campos;
    bool esCabecera = true;

    while (lector.leerFila(campos)) {
        if (esCabecera) {  // la primera fila son los nombres de columna
            esCabecera = false;
            continue;
        }
        if (campos.tamanio() < 4) continue;

        int id = red.registrarUsuario(campos[1], campos[2], Fecha::desdeTexto(campos[3]));
        if (id < 0) continue;

        Usuario* usuario = red.buscarPorId(id);
        if (usuario == nullptr) continue;
        if (campos.tamanio() > 4) usuario->cantidadSeguidores = static_cast<int>(campos[4].aEntero());
        if (campos.tamanio() > 5) usuario->reaccionesRecibidas = campos[5].aEntero();
        if (campos.tamanio() > 6) usuario->comentariosRealizados = static_cast<int>(campos[6].aEntero());
    }
    return true;
}

bool cargarAmistades(RedSocial& red, const char* ruta) {
    LectorCSV lector;
    if (!lector.abrir(ruta)) return false;

    Arreglo<Cadena> campos;
    bool esCabecera = true;

    // Se cargan sin ordenar y se ordena todo al final: mismo motivo que en el
    // generador sintetico, es mucho mas rapido para millones de aristas.
    while (lector.leerFila(campos)) {
        if (esCabecera) {
            esCabecera = false;
            continue;
        }
        if (campos.tamanio() < 2) continue;

        int indiceA = red.indiceDeUsuario(static_cast<int>(campos[0].aEntero()));
        int indiceB = red.indiceDeUsuario(static_cast<int>(campos[1].aEntero()));
        if (indiceA < 0 || indiceB < 0) continue;
        red.grafo().agregarAristaSinOrdenar(indiceA, indiceB);
    }
    red.grafo().ordenarYLimpiarListas();
    red.sincronizarContadoresDeAmigos();
    return true;
}

bool cargarPublicaciones(RedSocial& red, const char* ruta) {
    LectorCSV lector;
    if (!lector.abrir(ruta)) return false;

    Arreglo<Cadena> campos;
    bool esCabecera = true;

    while (lector.leerFila(campos)) {
        if (esCabecera) {
            esCabecera = false;
            continue;
        }
        if (campos.tamanio() < 6) continue;

        int idAutor = static_cast<int>(campos[1].aEntero());
        int idPublicacion = red.crearPublicacion(idAutor, campos[5], Fecha::desdeTexto(campos[2]));
        if (idPublicacion < 0) continue;

        int likes = static_cast<int>(campos[3].aEntero());
        if (likes > 0) red.agregarLikes(idPublicacion, likes);
    }
    return true;
}

}  // namespace archivo

}  // namespace aed
