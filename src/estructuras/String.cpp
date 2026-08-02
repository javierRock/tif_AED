#include "String.h"

#include <ostream>

namespace aed {







int String::largoDeTexto(const char* texto) {
    if (texto == nullptr) return 0;
    int largo = 0;
    while (texto[largo] != '\0') {
        ++largo;
    }
    return largo;
}




String::String() : _datos(nullptr), _longitud(0), _capacidad(0) {}


String::String(const char* texto) : _datos(nullptr), _longitud(0), _capacidad(0) {
    agregar(texto);
}


String::String(const char* texto, int longitud) : _datos(nullptr), _longitud(0), _capacidad(0) {
    if (texto == nullptr || longitud <= 0) return;
    reservar(longitud);

    char* destino = _datos;
    for (int i = 0; i < longitud; ++i) {
        destino[i] = texto[i];
    }
    _longitud = longitud;
    _datos[_longitud] = '\0';
}

String::String(const String& otra) : _datos(nullptr), _longitud(0), _capacidad(0) {
    agregar(otra);
}



String::String(String&& otra) noexcept
    : _datos(otra._datos), _longitud(otra._longitud), _capacidad(otra._capacidad) {
    otra._datos = nullptr;
    otra._longitud = 0;
    otra._capacidad = 0;
}

String::~String() {
    delete[] _datos;
}

String& String::operator=(const String& otra) {
    if (this == &otra) return *this;
    limpiar();
    agregar(otra);
    return *this;
}

String& String::operator=(String&& otra) noexcept {
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




void String::reservar(int nuevaCapacidad) {
    if (nuevaCapacidad <= _capacidad) return;


    char* nuevos = new char[nuevaCapacidad + 1];


    const char* viejos = _datos;
    int largo = _longitud;
    for (int i = 0; i < largo; ++i) {
        nuevos[i] = viejos[i];
    }
    nuevos[largo] = '\0';

    delete[] _datos;
    _datos = nuevos;
    _capacidad = nuevaCapacidad;
}


void String::asegurarEspacioPara(int caracteresExtra) {
    int necesaria = _longitud + caracteresExtra;
    if (necesaria <= _capacidad) return;




    int nueva = (_capacidad == 0) ? 8 : _capacidad * 2;
    if (nueva < necesaria) nueva = necesaria;
    reservar(nueva);
}

void String::limpiar() {
    _longitud = 0;
    if (_datos != nullptr) _datos[0] = '\0';
}




void String::agregar(char caracter) {
    asegurarEspacioPara(1);
    _datos[_longitud] = caracter;
    ++_longitud;
    _datos[_longitud] = '\0';
}

void String::agregar(const char* texto) {
    int largo = largoDeTexto(texto);
    if (largo == 0) return;

    asegurarEspacioPara(largo);

    char* destino = _datos + _longitud;
    for (int i = 0; i < largo; ++i) {
        destino[i] = texto[i];
    }
    _longitud += largo;
    _datos[_longitud] = '\0';
}

void String::agregar(const String& otra) {




    int largo = otra._longitud;
    if (largo == 0) return;

    asegurarEspacioPara(largo);

    const char* origen = otra._datos;
    char* destino = _datos + _longitud;
    for (int i = 0; i < largo; ++i) {
        destino[i] = origen[i];
    }
    _longitud += largo;
    _datos[_longitud] = '\0';
}




String String::subcadena(int inicio, int cantidad) const {
    if (inicio < 0 || inicio >= _longitud || cantidad <= 0) return String();
    if (inicio + cantidad > _longitud) cantidad = _longitud - inicio;
    return String(_datos + inicio, cantidad);
}

String String::aMinusculas() const {
    String resultado(*this);
    for (int i = 0; i < resultado._longitud; ++i) {
        char c = resultado._datos[i];
        if (c >= 'A' && c <= 'Z') resultado._datos[i] = static_cast<char>(c - 'A' + 'a');
    }
    return resultado;
}

bool String::comienzaCon(const String& prefijo) const {
    if (prefijo._longitud > _longitud) return false;
    for (int i = 0; i < prefijo._longitud; ++i) {
        if (_datos[i] != prefijo._datos[i]) return false;
    }
    return true;
}

bool String::contiene(const String& patron) const {
    if (patron._longitud == 0) return true;
    if (patron._longitud > _longitud) return false;



    for (int i = 0; i + patron._longitud <= _longitud; ++i) {
        bool coincide = true;
        for (int k = 0; k < patron._longitud; ++k) {
            if (_datos[i + k] != patron._datos[k]) {
                coincide = false;
                break;
            }
        }
        if (coincide) return true;
    }
    return false;
}

int String::buscarCaracter(char caracter, int desde) const {
    for (int i = desde; i < _longitud; ++i) {
        if (_datos[i] == caracter) return i;
    }
    return -1;
}



unsigned long long String::codigoHash() const {
    const unsigned long long SEMILLA = 1469598103934665603ULL;
    const unsigned long long PRIMO = 1099511628211ULL;

    unsigned long long valor = SEMILLA;
    for (int i = 0; i < _longitud; ++i) {
        valor ^= static_cast<unsigned char>(_datos[i]);
        valor *= PRIMO;
    }
    return valor;
}






String String::desdeEntero(long long valor) {
    if (valor == 0) return String("0");

    bool negativo = (valor < 0);

    String alReves;
    while (valor != 0) {
        long long digito = valor % 10;



        if (digito < 0) digito = -digito;
        alReves.agregar(static_cast<char>('0' + digito));
        valor /= 10;
    }
    if (negativo) alReves.agregar('-');

    String resultado;
    resultado.reservar(alReves._longitud);
    for (int i = alReves._longitud - 1; i >= 0; --i) {
        resultado.agregar(alReves._datos[i]);
    }
    return resultado;
}

long long String::aEntero() const {
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








int String::compararCon(const String& otra) const {
    int i = 0;
    while (i < _longitud && i < otra._longitud) {


        unsigned char a = static_cast<unsigned char>(_datos[i]);
        unsigned char b = static_cast<unsigned char>(otra._datos[i]);
        if (a != b) return (a < b) ? -1 : 1;
        ++i;
    }
    if (_longitud == otra._longitud) return 0;
    return (_longitud < otra._longitud) ? -1 : 1;
}

bool operator==(const String& a, const String& b) {
    if (a._longitud != b._longitud) return false;
    return a.compararCon(b) == 0;
}

bool operator!=(const String& a, const String& b) {
    return !(a == b);
}

bool operator<(const String& a, const String& b) {
    return a.compararCon(b) < 0;
}

bool operator>(const String& a, const String& b) {
    return a.compararCon(b) > 0;
}

std::ostream& operator<<(std::ostream& salida, const String& cadena) {
    salida << cadena.texto();
    return salida;
}

}
