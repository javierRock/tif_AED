# ============================================================================
#  Makefile alternativo (por si se prefiere no usar CMake).
#
#      make            compila el sistema y las pruebas
#      make ejecutar   compila y lanza el sistema
#      make probar     compila y lanza las pruebas automaticas
#      make banco      ejecuta el analisis de rendimiento
#      make limpiar    borra los archivos generados
# ============================================================================
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
DIR_OBJ  := build

FUENTES_NUCLEO := \
	src/estructuras/String.cpp \
	src/estructuras/Trie.cpp \
	src/modelo/Fecha.cpp \
	src/sistema/GrafoAmistades.cpp \
	src/sistema/RedSocial.cpp \
	src/datos/GeneradorSintetico.cpp \
	src/datos/ArchivoCSV.cpp \
	src/analisis/BancoPruebas.cpp \
	src/interfaz/MenuConsola.cpp

OBJETOS_NUCLEO := $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(FUENTES_NUCLEO))

.PHONY: todo ejecutar probar banco limpiar

todo: redsocial pruebas_aed

redsocial: $(OBJETOS_NUCLEO) $(DIR_OBJ)/src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

pruebas_aed: $(OBJETOS_NUCLEO) $(DIR_OBJ)/pruebas/pruebas.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

ejecutar: redsocial
	./redsocial

probar: pruebas_aed
	./pruebas_aed

banco: redsocial
	./redsocial --banco --maximo 1000000

limpiar:
	rm -rf $(DIR_OBJ) redsocial pruebas_aed *.csv
