

#pragma once

#include "Arreglo.h"
#include "Utilidades.h"

namespace aed
{

    const int UMBRAL_INSERCION = 16;

    template <typename T, typename Comparador>
    void ordenarPorInsercion(T *datos, int inicio, int fin, Comparador esMenor)
    {
        for (int i = inicio + 1; i <= fin; ++i)
        {
            T actual = mover(datos[i]);
            int j = i - 1;
            while (j >= inicio && esMenor(actual, datos[j]))
            {
                datos[j + 1] = mover(datos[j]);
                --j;
            }
            datos[j + 1] = mover(actual);
        }
    }

    template <typename T, typename Comparador>
    int particionar(T *datos, int inicio, int fin, Comparador esMenor)
    {
        int medio = inicio + (fin - inicio) / 2;

        if (esMenor(datos[medio], datos[inicio]))
            intercambiar(datos[medio], datos[inicio]);
        if (esMenor(datos[fin], datos[inicio]))
            intercambiar(datos[fin], datos[inicio]);
        if (esMenor(datos[fin], datos[medio]))
            intercambiar(datos[fin], datos[medio]);
        intercambiar(datos[medio], datos[fin - 1]);

        T pivote = datos[fin - 1];
        int i = inicio;
        int j = fin - 1;

        while (true)
        {
            while (esMenor(datos[++i], pivote))
            {
            }
            while (esMenor(pivote, datos[--j]))
            {
            }
            if (i >= j)
                break;
            intercambiar(datos[i], datos[j]);
        }
        intercambiar(datos[i], datos[fin - 1]);
        return i;
    }

    template <typename T, typename Comparador>
    void ordenarRapidoRecursivo(T *datos, int inicio, int fin, Comparador esMenor)
    {
        while (fin - inicio + 1 > UMBRAL_INSERCION)
        {
            int posicionPivote = particionar(datos, inicio, fin, esMenor);

            if (posicionPivote - inicio < fin - posicionPivote)
            {
                ordenarRapidoRecursivo(datos, inicio, posicionPivote - 1, esMenor);
                inicio = posicionPivote + 1;
            }
            else
            {
                ordenarRapidoRecursivo(datos, posicionPivote + 1, fin, esMenor);
                fin = posicionPivote - 1;
            }
        }
        ordenarPorInsercion(datos, inicio, fin, esMenor);
    }

    template <typename T, typename Comparador>
    void ordenarRapido(T *datos, int cantidad, Comparador esMenor)
    {
        if (cantidad < 2)
            return;
        ordenarRapidoRecursivo(datos, 0, cantidad - 1, esMenor);
    }

    template <typename T>
    void ordenarRapido(T *datos, int cantidad)
    {
        ordenarRapido(datos, cantidad, MenorQue());
    }

    template <typename T, typename Comparador>
    void ordenarRapido(Arreglo<T> &arreglo, Comparador esMenor)
    {
        ordenarRapido(arreglo.datos(), arreglo.tamanio(), esMenor);
    }

    template <typename T>
    void ordenarRapido(Arreglo<T> &arreglo)
    {
        ordenarRapido(arreglo.datos(), arreglo.tamanio(), MenorQue());
    }

    template <typename T, typename Comparador>
    void mezclar(T *datos, T *auxiliar, int inicio, int medio, int fin, Comparador esMenor)
    {
        int i = inicio;
        int j = medio + 1;
        int k = inicio;

        while (i <= medio && j <= fin)
        {

            if (!esMenor(datos[j], datos[i]))
            {
                auxiliar[k++] = mover(datos[i++]);
            }
            else
            {
                auxiliar[k++] = mover(datos[j++]);
            }
        }
        while (i <= medio)
            auxiliar[k++] = mover(datos[i++]);
        while (j <= fin)
            auxiliar[k++] = mover(datos[j++]);
        for (int p = inicio; p <= fin; ++p)
            datos[p] = mover(auxiliar[p]);
    }

    template <typename T, typename Comparador>
    void ordenarPorMezclaRecursivo(T *datos, T *auxiliar, int inicio, int fin, Comparador esMenor)
    {
        if (inicio >= fin)
            return;
        int medio = inicio + (fin - inicio) / 2;
        ordenarPorMezclaRecursivo(datos, auxiliar, inicio, medio, esMenor);
        ordenarPorMezclaRecursivo(datos, auxiliar, medio + 1, fin, esMenor);
        mezclar(datos, auxiliar, inicio, medio, fin, esMenor);
    }

    template <typename T, typename Comparador>
    void ordenarPorMezcla(T *datos, int cantidad, Comparador esMenor)
    {
        if (cantidad < 2)
            return;
        T *auxiliar = new T[cantidad];
        ordenarPorMezclaRecursivo(datos, auxiliar, 0, cantidad - 1, esMenor);
        delete[] auxiliar;
    }

    template <typename T>
    void ordenarPorMezcla(T *datos, int cantidad)
    {
        ordenarPorMezcla(datos, cantidad, MenorQue());
    }

    template <typename T, typename Comparador>
    void ordenarPorMezcla(Arreglo<T> &arreglo, Comparador esMenor)
    {
        ordenarPorMezcla(arreglo.datos(), arreglo.tamanio(), esMenor);
    }

    template <typename T>
    void eliminarDuplicadosOrdenados(Arreglo<T> &arreglo)
    {
        if (arreglo.tamanio() < 2)
            return;
        int escritura = 1;
        for (int i = 1; i < arreglo.tamanio(); ++i)
        {
            if (!(arreglo[i] == arreglo[escritura - 1]))
            {
                arreglo[escritura++] = arreglo[i];
            }
        }
        arreglo.redimensionar(escritura);
    }

}
