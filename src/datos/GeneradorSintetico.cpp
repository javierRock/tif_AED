#include "GeneradorSintetico.h"

#include <cstdio>

namespace aed {

namespace {





const char* const NOMBRES[] = {
    "Ana",    "Luis",   "Maria",  "Carlos", "Sofia",   "Jorge",   "Lucia",   "Miguel",
    "Elena",  "Pedro",  "Carmen", "Diego",  "Rosa",    "Javier",  "Paula",   "Andres",
    "Isabel", "Fernando", "Marta", "Ricardo", "Julia",  "Alberto", "Silvia",  "Raul",
    "Teresa", "Oscar",  "Beatriz", "Hugo",  "Natalia", "Sergio",  "Claudia", "Ivan",
    "Patricia", "Manuel", "Veronica", "Daniel", "Alicia", "Ruben", "Monica", "Pablo",
    "Gloria", "Esteban", "Rocio",  "Tomas",  "Adriana", "Felipe",  "Renata",  "Gabriel"};

const char* const APELLIDOS[] = {
    "Quispe",   "Mamani",   "Flores",   "Vargas",   "Rojas",    "Huaman",   "Ramos",
    "Torres",   "Chavez",   "Salazar",  "Cardenas", "Paredes",  "Zevallos", "Aguilar",
    "Cordova",  "Ticona",   "Apaza",    "Condori",  "Gutierrez", "Medina",  "Pacheco",
    "Bustamante", "Delgado", "Fernandez", "Cruz",   "Reyes",    "Valdivia", "Cabrera",
    "Sanchez",  "Ortega",   "Peralta",  "Benavides", "Escobar", "Guzman",   "Linares",
    "Montoya",  "Nunez",    "Ordonez",  "Portugal", "Quiroz",   "Rivera",   "Sotomayor",
    "Tapia",    "Ugarte",   "Velasquez", "Yupanqui", "Zamora",  "Barrios"};

const char* const PLANTILLAS_TEXTO[] = {
    "Hoy fue un gran dia en la universidad",
    "Alguien sabe donde queda la nueva biblioteca",
    "Terminando el proyecto de estructuras de datos",
    "Recomiendo mucho este libro que acabo de leer",
    "Que rico el almuerzo de hoy",
    "Buscando companeros para el trabajo grupal",
    "Por fin aprobe el examen mas dificil del ciclo",
    "Compartiendo fotos del viaje del fin de semana",
    "Empezando a entrenar para la maraton",
    "Feliz cumpleanos a mi mejor amigo",
    "El concierto de anoche estuvo increible",
    "Alguien tiene apuntes del curso de algoritmos",
    "Ya salieron las notas finales",
    "Preparando la sustentacion del proyecto",
    "Que opinan de la nueva cafeteria del campus",
    "Vendiendo mi bicicleta casi nueva",
    "Gracias a todos por los saludos",
    "Primer dia de practicas profesionales",
    "Nunca pense que un grafo pudiera ser tan grande",
    "Hoy toca maraton de estudio hasta tarde"};

const char* const COMENTARIOS[] = {
    "Totalmente de acuerdo",  "Que buena noticia",     "Felicitaciones",
    "Yo tambien pienso igual", "Cuentame mas",         "Excelente trabajo",
    "Gracias por compartir",  "No lo sabia",           "Increible",
    "Nos vemos pronto"};

const int CANTIDAD_NOMBRES = static_cast<int>(sizeof(NOMBRES) / sizeof(NOMBRES[0]));
const int CANTIDAD_APELLIDOS = static_cast<int>(sizeof(APELLIDOS) / sizeof(APELLIDOS[0]));
const int CANTIDAD_PLANTILLAS =
    static_cast<int>(sizeof(PLANTILLAS_TEXTO) / sizeof(PLANTILLAS_TEXTO[0]));
const int CANTIDAD_COMENTARIOS = static_cast<int>(sizeof(COMENTARIOS) / sizeof(COMENTARIOS[0]));


void informarAvance(const char* etapa, int hechos, int total, bool activo) {
    if (!activo || total <= 0) return;
    int paso = total / 10;
    if (paso == 0) paso = 1;
    if (hechos % paso != 0 && hechos != total) return;
    std::printf("\r    %-22s %3d %%", etapa, static_cast<int>(100LL * hechos / total));
    std::fflush(stdout);
}

Fecha fechaAleatoria(GeneradorAleatorio& azar, int anioMinimo, int anioMaximo) {
    int anio = azar.enteroEntre(anioMinimo, anioMaximo);
    int mes = azar.enteroEntre(1, 12);
    int dia = azar.enteroEntre(1, 28);
    return Fecha(anio, mes, dia);
}

}


void GeneradorSintetico::generar(RedSocial& red, const ParametrosGeneracion& parametros) {
    GeneradorAleatorio azar(parametros.semilla);

    red.reservarCapacidad(parametros.cantidadUsuarios, parametros.cantidadPublicaciones);
    generarUsuarios(red, parametros, azar);
    generarAmistades(red, parametros, azar);
    generarPublicaciones(red, parametros, azar);
    generarInteracciones(red, parametros, azar);
}





void GeneradorSintetico::generarUsuarios(RedSocial& red, const ParametrosGeneracion& parametros,
                                         GeneradorAleatorio& azar) {
    char bufferCorreo[96];

    for (int i = 0; i < parametros.cantidadUsuarios; ++i) {
        const char* nombrePila = NOMBRES[azar.enteroMenorQue(CANTIDAD_NOMBRES)];
        const char* apellido = APELLIDOS[azar.enteroMenorQue(CANTIDAD_APELLIDOS)];

        String nombreCompleto(nombrePila);
        nombreCompleto.agregar(" ");
        nombreCompleto.agregar(apellido);


        std::snprintf(bufferCorreo, sizeof(bufferCorreo), "%s.%s%d@redsocial.pe",
                      nombrePila, apellido, i);
        String correo(bufferCorreo);

        red.registrarUsuario(nombreCompleto, correo, fechaAleatoria(azar, 2008, 2026));
        informarAvance("Usuarios", i + 1, parametros.cantidadUsuarios, parametros.mostrarProgreso);
    }
    if (parametros.mostrarProgreso) std::printf("\n");
}




void GeneradorSintetico::generarAmistades(RedSocial& red, const ParametrosGeneracion& parametros,
                                          GeneradorAleatorio& azar) {
    GrafoAmistades& grafo = red.grafo();
    int cantidadNodos = grafo.cantidadNodos();
    int m = parametros.amistadesPorUsuario;

    if (cantidadNodos < 2 || m < 1) return;
    if (m >= cantidadNodos) m = cantidadNodos - 1;



    Arreglo<int> bolsa;
    bolsa.reservar(2 * cantidadNodos * m + 4 * m * m);




    int nucleo = m + 1;
    if (nucleo > cantidadNodos) nucleo = cantidadNodos;
    for (int i = 0; i < nucleo; ++i) {
        for (int j = i + 1; j < nucleo; ++j) {
            grafo.agregarAristaSinOrdenar(i, j);
            bolsa.agregar(i);
            bolsa.agregar(j);
        }
    }


    Arreglo<int> elegidos(m);
    for (int nodo = nucleo; nodo < cantidadNodos; ++nodo) {
        elegidos.limpiar();



        int intentos = 0;
        int maximoIntentos = 20 * m + 50;
        while (elegidos.tamanio() < m && intentos < maximoIntentos) {
            ++intentos;
            int candidato = bolsa[azar.enteroMenorQue(bolsa.tamanio())];
            if (candidato == nodo || elegidos.contiene(candidato)) continue;
            elegidos.agregar(candidato);
        }

        while (elegidos.tamanio() < m) {
            int candidato = azar.enteroMenorQue(nodo);
            if (!elegidos.contiene(candidato)) elegidos.agregar(candidato);
        }

        for (int k = 0; k < elegidos.tamanio(); ++k) {
            grafo.agregarAristaSinOrdenar(nodo, elegidos[k]);
            bolsa.agregar(nodo);
            bolsa.agregar(elegidos[k]);
        }
        informarAvance("Amistades", nodo + 1, cantidadNodos, parametros.mostrarProgreso);
    }
    bolsa.liberar();



    grafo.ordenarYLimpiarListas();
    red.sincronizarContadoresDeAmigos();
    if (parametros.mostrarProgreso) std::printf("\n");
}




void GeneradorSintetico::generarPublicaciones(RedSocial& red,
                                              const ParametrosGeneracion& parametros,
                                              GeneradorAleatorio& azar) {
    int totalUsuarios = red.cantidadPosicionesUsuario();
    if (totalUsuarios == 0) return;

    char bufferTexto[160];
    for (int i = 0; i < parametros.cantidadPublicaciones; ++i) {
        int indiceAutor = azar.enteroMenorQue(totalUsuarios);
        const Usuario& autor = red.usuarioEnIndice(indiceAutor);
        if (!autor.activo) continue;

        const char* plantilla = PLANTILLAS_TEXTO[azar.enteroMenorQue(CANTIDAD_PLANTILLAS)];
        std::snprintf(bufferTexto, sizeof(bufferTexto), "%s [#%d]", plantilla, i);

        red.crearPublicacion(autor.id, String(bufferTexto), fechaAleatoria(azar, 2020, 2026));
        informarAvance("Publicaciones", i + 1, parametros.cantidadPublicaciones,
                       parametros.mostrarProgreso);
    }
    if (parametros.mostrarProgreso) std::printf("\n");
}








void GeneradorSintetico::generarInteracciones(RedSocial& red,
                                              const ParametrosGeneracion& parametros,
                                              GeneradorAleatorio& azar) {
    int totalPublicaciones = red.cantidadPosicionesPublicacion();
    int totalUsuarios = red.cantidadPosicionesUsuario();
    if (totalPublicaciones == 0 || totalUsuarios == 0) return;

    for (int i = 0; i < totalPublicaciones; ++i) {
        const Publicacion& publicacion = red.publicacionEnIndice(i);
        if (!publicacion.activa) continue;
        int idPublicacion = publicacion.id;

        int likes = azar.sesgadoHaciaCero(parametros.maximoLikesPorPublicacion);
        if (likes > 0) red.agregarLikes(idPublicacion, likes);

        int cuantosComentarios = azar.sesgadoHaciaCero(parametros.maximoComentariosPorPublicacion + 1);
        for (int k = 0; k < cuantosComentarios; ++k) {
            int indiceAutor = azar.enteroMenorQue(totalUsuarios);
            const Usuario& autor = red.usuarioEnIndice(indiceAutor);
            if (!autor.activo) continue;

            const char* texto = COMENTARIOS[azar.enteroMenorQue(CANTIDAD_COMENTARIOS)];
            red.comentarPublicacion(idPublicacion, autor.id, String(texto),
                                    fechaAleatoria(azar, 2020, 2026));
        }
        informarAvance("Interacciones", i + 1, totalPublicaciones, parametros.mostrarProgreso);
    }
    if (parametros.mostrarProgreso) std::printf("\n");
}

}
