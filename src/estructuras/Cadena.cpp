#include "Cadena.h"

#include <cstdio>
#include <cstring>
#include <ostream>

namespace aed {

// ---------------------------------------------------------------------------
// Construccion
// ---------------------------------------------------------------------------
Cadena::Cadena() : _datos(nullptr), _longitud(0), _capacidad(0) {}

Cadena::Cadena(const char* texto) : _datos(nullptr), _longitud(0), _capacidad(0) {
    if (texto == nullptr) return;
    int largo = static_cast<int>(std::strlen(texto));
    if (largo == 0) return;
    reservar(largo);
    std::memcpy(_datos, texto, static_cast<unsigned long>(largo));
    _longitud = largo;
    _datos[_longitud] = '\0';
}

Cadena::Cadena(const char* texto, int longitud) : _datos(nullptr), _longitud(0), _capacidad(0) {
    if (texto == nullptr || longitud <= 0) return;
    reservar(longitud);
    std::memcpy(_datos, texto, static_cast<unsigned long>(longitud));
    _longitud = longitud;
    _datos[_longitud] = '\0';
}

Cadena::Cadena(const Cadena& otra) : _datos(nullptr), _longitud(0), _capacidad(0) {
    if (otra._longitud == 0) return;
    reservar(otra._longitud);
    std::memcpy(_datos, otra._datos, static_cast<unsigned long>(otra._longitud));
    _longitud = otra._longitud;
    _datos[_longitud] = '\0';
}

// El constructor de movimiento "roba" el bloque de memoria de la otra cadena
// y la deja vacia. No hay ninguna copia de caracteres: es O(1).
Cadena::Cadena(Cadena&& otra) noexcept
    : _datos(otra._datos), _longitud(otra._longitud), _capacidad(otra._capacidad) {
    otra._datos = nullptr;
    otra._longitud = 0;
    otra._capacidad = 0;
}

Cadena::~Cadena() {
    delete[] _datos;
}

Cadena& Cadena::operator=(const Cadena& otra) {
    if (this == &otra) return *this;
    _longitud = 0;
    if (otra._longitud > 0) {
        reservar(otra._longitud);
        std::memcpy(_datos, otra._datos, static_cast<unsigned long>(otra._longitud));
        _longitud = otra._longitud;
    }
    if (_datos != nullptr) _datos[_longitud] = '\0';
    return *this;
}

Cadena& Cadena::operator=(Cadena&& otra) noexcept {
    if (this == &otra) return *this;
    delete[] _datos;
    _datos = otra._datos;
    _longitud = otra._longitud;
    _capacidad = otra._capacidad;
    otra._datos = nullptr;
    otra._longitud = 0;
    otra._capacidad = 0;
    return *this;
}

// ---------------------------------------------------------------------------
// Manejo de memoria
// ---------------------------------------------------------------------------
void Cadena::reservar(int nuevaCapacidad) {
    if (nuevaCapacidad <= _capacidad) return;

    // Reservamos un byte extra para el terminador '\0'.
    char* nuevos = new char[nuevaCapacidad + 1];
    if (_longitud > 0) {
        std::memcpy(nuevos, _datos, static_cast<unsigned long>(_longitud));
    }
    nuevos[_longitud] = '\0';
    delete[] _datos;
    _datos = nuevos;
    _capacidad = nuevaCapacidad;
}

void Cadena::limpiar() {
    _longitud = 0;
    if (_datos != nullptr) _datos[0] = '\0';
}

// ---------------------------------------------------------------------------
// Modificacion
// ---------------------------------------------------------------------------
void Cadena::agregar(char caracter) {
    if (_longitud == _capacidad) {
        // Crecimiento geometrico: duplicar evita reservar en cada agregado.
        reservar(_capacidad == 0 ? 8 : _capacidad * 2);
    }
    _datos[_longitud++] = caracter;
    _datos[_longitud] = '\0';
}

void Cadena::agregar(const char* texto) {
    if (texto == nullptr) return;
    int largo = static_cast<int>(std::strlen(texto));
    if (largo == 0) return;
    if (_longitud + largo > _capacidad) {
        int objetivo = (_capacidad == 0 ? 8 : _capacidad * 2);
        if (objetivo < _longitud + largo) objetivo = _longitud + largo;
        reservar(objetivo);
    }
    std::memcpy(_datos + _longitud, texto, static_cast<unsigned long>(largo));
    _longitud += largo;
    _datos[_longitud] = '\0';
}

void Cadena::agregar(const Cadena& otra) {
    agregar(otra.texto());
}

// ---------------------------------------------------------------------------
// Operaciones de texto
// ---------------------------------------------------------------------------
Cadena Cadena::subcadena(int inicio, int cantidad) const {
    if (inicio < 0 || inicio >= _longitud || cantidad <= 0) return Cadena();
    if (inicio + cantidad > _longitud) cantidad = _longitud - inicio;
    return Cadena(_datos + inicio, cantidad);
}

Cadena Cadena::aMinusculas() const {
    Cadena resultado(*this);
    for (int i = 0; i < resultado._longitud; ++i) {
        char c = resultado._datos[i];
        if (c >= 'A' && c <= 'Z') resultado._datos[i] = static_cast<char>(c - 'A' + 'a');
    }
    return resultado;
}

bool Cadena::comienzaCon(const Cadena& prefijo) const {
    if (prefijo._longitud > _longitud) return false;
    if (prefijo._longitud == 0) return true;
    return std::memcmp(_datos, prefijo._datos, static_cast<unsigned long>(prefijo._longitud)) == 0;
}

bool Cadena::contiene(const Cadena& patron) const {
    if (patron._longitud == 0) return true;
    if (patron._longitud > _longitud) return false;
    // Busqueda directa: suficiente para textos cortos como nombres o correos.
    for (int i = 0; i + patron._longitud <= _longitud; ++i) {
        if (std::memcmp(_datos + i, patron._datos,
                        static_cast<unsigned long>(patron._longitud)) == 0) {
            return true;
        }
    }
    return false;
}

int Cadena::buscarCaracter(char caracter, int desde) const {
    for (int i = desde; i < _longitud; ++i) {
        if (_datos[i] == caracter) return i;
    }
    return -1;
}

// FNV-1a: se parte de una semilla y por cada byte se hace XOR + multiplicacion
// por un primo grande. Es una sola pasada sobre el texto, O(longitud).
unsigned long long Cadena::codigoHash() const {
    const unsigned long long SEMILLA = 1469598103934665603ULL;
    const unsigned long long PRIMO = 1099511628211ULL;

    unsigned long long valor = SEMILLA;
    for (int i = 0; i < _longitud; ++i) {
        valor ^= static_cast<unsigned char>(_datos[i]);
        valor *= PRIMO;
    }
    return valor;
}

// ---------------------------------------------------------------------------
// Conversiones
// ---------------------------------------------------------------------------
Cadena Cadena::desdeEntero(long long valor) {
    char temporal[32];
    std::snprintf(temporal, sizeof(temporal), "%lld", valor);
    return Cadena(temporal);
}

long long Cadena::aEntero() const {
    long long resultado = 0;
    int i = 0;
    bool negativo = false;
    if (_longitud > 0 && (_datos[0] == '-' || _datos[0] == '+')) {
        negativo = (_datos[0] == '-');
        i = 1;
    }
    for (; i < _longitud; ++i) {
        char c = _datos[i];
        if (c < '0' || c > '9') break;
        resultado = resultado * 10 + (c - '0');
    }
    return negativo ? -resultado : resultado;
}

// ---------------------------------------------------------------------------
// Comparaciones
// ---------------------------------------------------------------------------
bool operator==(const Cadena& a, const Cadena& b) {
    if (a._longitud != b._longitud) return false;
    if (a._longitud == 0) return true;
    return std::memcmp(a._datos, b._datos, static_cast<unsigned long>(a._longitud)) == 0;
}

bool operator!=(const Cadena& a, const Cadena& b) {
    return !(a == b);
}

bool operator<(const Cadena& a, const Cadena& b) {
    return std::strcmp(a.texto(), b.texto()) < 0;
}

bool operator>(const Cadena& a, const Cadena& b) {
    return std::strcmp(a.texto(), b.texto()) > 0;
}

std::ostream& operator<<(std::ostream& salida, const Cadena& cadena) {
    salida << cadena.texto();
    return salida;
}

}  // namespace aed
