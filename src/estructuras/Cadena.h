// ============================================================================
//  Cadena.h
//  Cadena de caracteres dinamica implementada desde cero (reemplaza a
//  std::string, que tambien es un contenedor de la STL y por lo tanto queda
//  fuera de lo permitido por el enunciado).
//
//  Representacion interna:
//      _datos      -> bloque de memoria terminado en '\0'
//      _longitud   -> cantidad de caracteres utiles (sin contar el '\0')
//      _capacidad  -> cuantos caracteres utiles caben sin reservar de nuevo
//
//  Implementa la "regla de los cinco" (copia, movimiento y destructor) para
//  que sea seguro guardarla dentro de nuestros propios contenedores.
// ============================================================================
#pragma once

#include <iosfwd>

namespace aed {

class Cadena {
public:
    // ---- Construccion / destruccion ---------------------------------------
    Cadena();
    Cadena(const char* texto);
    Cadena(const char* texto, int longitud);
    Cadena(const Cadena& otra);
    Cadena(Cadena&& otra) noexcept;
    ~Cadena();

    Cadena& operator=(const Cadena& otra);
    Cadena& operator=(Cadena&& otra) noexcept;

    // ---- Consulta ---------------------------------------------------------
    int longitud() const { return _longitud; }
    bool vacia() const { return _longitud == 0; }
    int capacidad() const { return _capacidad; }

    /// Devuelve el texto en estilo C (nunca nulo: si esta vacia devuelve "").
    const char* texto() const { return _datos ? _datos : ""; }

    char operator[](int posicion) const { return _datos[posicion]; }
    char& operator[](int posicion) { return _datos[posicion]; }

    // ---- Modificacion -----------------------------------------------------
    void agregar(char caracter);
    void agregar(const char* texto);
    void agregar(const Cadena& otra);
    void limpiar();
    void reservar(int nuevaCapacidad);

    // ---- Operaciones de texto ---------------------------------------------
    Cadena subcadena(int inicio, int cantidad) const;
    Cadena aMinusculas() const;
    bool comienzaCon(const Cadena& prefijo) const;
    bool contiene(const Cadena& patron) const;

    /// Devuelve la posicion del caracter buscado o -1 si no aparece.
    /// Se usa para separar campos de un CSV o palabras de un nombre.
    int buscarCaracter(char caracter, int desde = 0) const;

    /// Funcion de dispersion FNV-1a de 64 bits: rapida, sencilla de explicar
    /// y con muy buena distribucion para textos cortos como correos o nombres.
    unsigned long long codigoHash() const;

    // ---- Conversiones -----------------------------------------------------
    static Cadena desdeEntero(long long valor);
    long long aEntero() const;

    // ---- Comparaciones ----------------------------------------------------
    friend bool operator==(const Cadena& a, const Cadena& b);
    friend bool operator!=(const Cadena& a, const Cadena& b);
    friend bool operator<(const Cadena& a, const Cadena& b);
    friend bool operator>(const Cadena& a, const Cadena& b);

private:
    char* _datos;
    int _longitud;
    int _capacidad;
};

std::ostream& operator<<(std::ostream& salida, const Cadena& cadena);

}  // namespace aed
