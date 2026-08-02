

#pragma once

#include <iosfwd>

namespace aed
{

    class String
    {
    public:
        String();
        String(const char *texto);
        String(const char *texto, int longitud);
        String(const String &otra);
        String(String &&otra) noexcept;
        ~String();

        String &operator=(const String &otra);
        String &operator=(String &&otra) noexcept;

        int longitud() const { return _longitud; }
        bool vacia() const { return _longitud == 0; }
        int capacidad() const { return _capacidad; }

        const char *texto() const { return _datos ? _datos : ""; }

        char operator[](int posicion) const { return _datos[posicion]; }
        char &operator[](int posicion) { return _datos[posicion]; }

        void agregar(char caracter);
        void agregar(const char *texto);
        void agregar(const String &otra);
        void limpiar();
        void reservar(int nuevaCapacidad);

        String subcadena(int inicio, int cantidad) const;
        String aMinusculas() const;
        bool comienzaCon(const String &prefijo) const;
        bool contiene(const String &patron) const;

        int buscarCaracter(char caracter, int desde = 0) const;

        unsigned long long codigoHash() const;

        static String desdeEntero(long long valor);
        long long aEntero() const;

        friend bool operator==(const String &a, const String &b);
        friend bool operator!=(const String &a, const String &b);
        friend bool operator<(const String &a, const String &b);
        friend bool operator>(const String &a, const String &b);

    private:
        static int largoDeTexto(const char *texto);

        void asegurarEspacioPara(int caracteresExtra);

        int compararCon(const String &otra) const;

        char *_datos;
        int _longitud;
        int _capacidad;
    };

    std::ostream &operator<<(std::ostream &salida, const String &cadena);

}
