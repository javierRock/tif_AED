cmake_minimum_required(VERSION 3.16)
project(RedSocialAED CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra)

# ---------------------------------------------------------------------------
# Todo el codigo, salvo el main y las pruebas, vive en esta biblioteca. Asi el
# ejecutable del sistema y el de las pruebas comparten exactamente el mismo
# codigo compilado.
# ---------------------------------------------------------------------------
add_library(nucleo STATIC
    src/estructuras/Cadena.cpp
    src/estructuras/Trie.cpp
    src/modelo/Fecha.cpp
    src/sistema/GrafoAmistades.cpp
    src/sistema/RedSocial.cpp
    src/datos/GeneradorSintetico.cpp
    src/datos/ArchivoCSV.cpp
    src/analisis/BancoPruebas.cpp
    src/interfaz/MenuConsola.cpp
)
target_include_directories(nucleo PUBLIC src)

add_executable(redsocial src/main.cpp)
target_link_libraries(redsocial PRIVATE nucleo)

add_executable(pruebas_aed pruebas/pruebas.cpp)
target_link_libraries(pruebas_aed PRIVATE nucleo)

enable_testing()
add_test(NAME pruebas_unitarias COMMAND pruebas_aed)
