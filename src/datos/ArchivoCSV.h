// ============================================================================
//  ArchivoCSV.h
//  Lectura y escritura de archivos CSV, implementadas sobre las funciones de
//  archivo del lenguaje C (fopen / fread / fwrite).
//
//  El enunciado permite usar bibliotecas externas para leer CSV, pero se
//  prefirio escribirlo aqui porque son pocas lineas y asi el proyecto no
//  depende de nada externo.
//
//  RENDIMIENTO: no se lee caracter por caracter con fgetc, sino en bloques de
//  64 KB que luego se recorren en memoria. Con archivos de millones de filas
//  la diferencia es de un orden de magnitud.
//
//  LIMITACION ASUMIDA: no se soportan campos entrecomillados. Al escribir, las
//  comas y saltos de linea del texto se sustituyen por espacios. Es suficiente
//  para los datos de este proyecto y mantiene el codigo simple.
// ============================================================================
#pragma once

#include <cstdio>

#include "../estructuras/Arreglo.h"
#include "../estructuras/String.h"

namespace aed {

class RedSocial;

// ---------------------------------------------------------------------------
class LectorCSV {
public:
    LectorCSV();
    ~LectorCSV();

    LectorCSV(const LectorCSV&) = delete;
    LectorCSV& operator=(const LectorCSV&) = delete;

    bool abrir(const char* ruta);
    void cerrar();
    bool estaAbierto() const { return _archivo != nullptr; }

    /// Lee la siguiente fila y la separa en campos.
    /// Devuelve false cuando ya no quedan filas.
    bool leerFila(Arreglo<String>& campos);

private:
    /// Siguiente caracter del archivo, o -1 si se llego al final.
    int siguienteCaracter();

    std::FILE* _archivo;
    char* _buffer;
    int _bytesEnBuffer;
    int _posicion;
};

// ---------------------------------------------------------------------------
class EscritorCSV {
public:
    EscritorCSV();
    ~EscritorCSV();

    EscritorCSV(const EscritorCSV&) = delete;
    EscritorCSV& operator=(const EscritorCSV&) = delete;

    bool abrir(const char* ruta);
    void cerrar();
    bool estaAbierto() const { return _archivo != nullptr; }

    void campo(const String& texto);
    void campo(const char* texto);
    void campo(long long numero);
    void finDeFila();

private:
    void escribirSeparador();

    std::FILE* _archivo;
    bool _primerCampoDeLaFila;
};

// ---------------------------------------------------------------------------
//  Volcado y carga completa de la red social.
//
//  Formato de los tres archivos:
//      usuarios.csv      id, nombre, correo, fecha, seguidores, reacciones,
//                        comentarios_realizados
//      amistades.csv     id_a, id_b
//      publicaciones.csv id, id_autor, fecha, likes, texto
// ---------------------------------------------------------------------------
namespace archivo {

bool guardarUsuarios(const RedSocial& red, const char* ruta);
bool guardarAmistades(const RedSocial& red, const char* ruta);
bool guardarPublicaciones(const RedSocial& red, const char* ruta);

bool cargarUsuarios(RedSocial& red, const char* ruta);
bool cargarAmistades(RedSocial& red, const char* ruta);
bool cargarPublicaciones(RedSocial& red, const char* ruta);

}  // namespace archivo

}  // namespace aed
