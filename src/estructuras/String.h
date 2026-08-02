// ============================================================================
//  String.h
//  Cadena de caracteres dinamica implementada desde cero. Es nuestra version
//  de std::string, que al ser un contenedor de la STL queda fuera de lo
//  permitido por el enunciado. Se llama String, sin acento y en ingles, para
//  que quede claro de un vistazo a que clase de la biblioteca estandar
//  sustituye.
//
//  Esta version esta escrita de forma deliberadamente rudimentaria: TODAS las
//  operaciones son bucles propios. No se usa strlen, memcpy, memcmp, strcmp
//  ni snprintf. Lo unico que se toma de la biblioteca estandar es <ostream>,
//  y solo para poder imprimir con "std::cout << cadena".
//
//  Representacion interna:
//
//      _datos      ->  [ 'H' ][ 'o' ][ 'l' ][ 'a' ][ '\0' ][ ? ][ ? ][ ? ]
//                        0      1      2      3
//      _longitud   =  4          caracteres utiles (sin contar el '\0')
//      _capacidad  =  7          cuantos caracteres utiles caben sin reservar
//                                de nuevo (el bloque mide _capacidad + 1 por
//                                el '\0' del final)
//
//  Si nunca se ha guardado nada, _datos vale nullptr y no se pide memoria.
//
//  Implementa la "regla de los cinco" (copia, movimiento y destructor) para
//  que sea seguro guardarla dentro de nuestros propios contenedores.
// ============================================================================
#pragma once

#include <iosfwd>

namespace aed {

class String {
public:
    // ---- Construccion / destruccion ---------------------------------------
    String();
    String(const char* texto);
    String(const char* texto, int longitud);
    String(const String& otra);
    String(String&& otra) noexcept;
    ~String();

    String& operator=(const String& otra);
    String& operator=(String&& otra) noexcept;

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
    void agregar(const String& otra);
    void limpiar();
    void reservar(int nuevaCapacidad);

    // ---- Operaciones de texto ---------------------------------------------
    String subcadena(int inicio, int cantidad) const;
    String aMinusculas() const;
    bool comienzaCon(const String& prefijo) const;
    bool contiene(const String& patron) const;

    /// Devuelve la posicion del caracter buscado o -1 si no aparece.
    /// Se usa para separar campos de un CSV o palabras de un nombre.
    int buscarCaracter(char caracter, int desde = 0) const;

    /// Funcion de dispersion FNV-1a de 64 bits: rapida, sencilla de explicar
    /// y con muy buena distribucion para textos cortos como correos o nombres.
    unsigned long long codigoHash() const;

    // ---- Conversiones -----------------------------------------------------
    static String desdeEntero(long long valor);
    long long aEntero() const;

    // ---- Comparaciones ----------------------------------------------------
    friend bool operator==(const String& a, const String& b);
    friend bool operator!=(const String& a, const String& b);
    friend bool operator<(const String& a, const String& b);
    friend bool operator>(const String& a, const String& b);

private:
    /// Nuestro propio strlen: cuenta caracteres hasta encontrar el '\0'.
    static int largoDeTexto(const char* texto);

    /// Garantiza que quepan 'caracteresExtra' mas a partir de _longitud.
    /// Es el UNICO sitio donde se decide cuanto crecer.
    void asegurarEspacioPara(int caracteresExtra);

    /// Comparacion alfabetica. Devuelve -1 si *this va antes que 'otra',
    /// 0 si son iguales y +1 si va despues. De aqui salen ==, !=, < y >.
    int compararCon(const String& otra) const;

    char* _datos;      // bloque terminado en '\0' (nullptr si nunca se reservo)
    int _longitud;     // caracteres utiles, sin contar el '\0'
    int _capacidad;    // cuantos caben sin volver a reservar
};

std::ostream& operator<<(std::ostream& salida, const String& cadena);

}  // namespace aed
