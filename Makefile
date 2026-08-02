# ============================================================================
#  Compilacion en Linux y en Windows 11.
#
#  Linux / macOS / WSL:      make
#  Windows con MSYS2, MinGW64 o Git Bash:      make
#  Windows con cmd.exe o PowerShell (MinGW-w64):      mingw32-make
#
#  Objetivos:  todo (por defecto) · ejecutar · probar · banco · limpiar
#              entorno   muestra el sistema y el compilador detectados
# ============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
DIR_OBJ  := build

# --- Deteccion del sistema --------------------------------------------------
# En Windows la variable OS siempre vale Windows_NT, tanto en cmd.exe como
# dentro de MSYS2 o Git Bash. MSYSTEM solo esta definida en esos entornos tipo
# Unix, y es lo que distingue si podemos usar mkdir -p y rm -rf.
ifeq ($(OS),Windows_NT)
    SUFIJO := .exe
    LDFLAGS += -static-libgcc -static-libstdc++
    ifdef MSYSTEM
        ENTORNO := Windows-MSYS
    else
        ENTORNO := Windows-cmd
        USA_CMD := 1
        SHELL := cmd.exe
        .SHELLFLAGS := /C
    endif
else
    SUFIJO :=
    ENTORNO := $(shell uname -s)
endif

# --- Ordenes que cambian segun el interprete de comandos --------------------
ifdef USA_CMD
    RUTA       = $(subst /,\,$(patsubst %/,%,$(1)))
    CREAR_DIR  = @if not exist "$(call RUTA,$(1))" mkdir "$(call RUTA,$(1))"
    BORRAR_DIR = @if exist "$(call RUTA,$(1))" rmdir /s /q "$(call RUTA,$(1))"
    BORRAR     = @if exist $(1) del /q $(1)
    PREFIJO    :=
else
    CREAR_DIR  = @mkdir -p $(1)
    BORRAR_DIR = @rm -rf $(1)
    BORRAR     = @rm -f $(1)
    PREFIJO    := ./
endif

BIN_SISTEMA := redsocial$(SUFIJO)
BIN_PRUEBAS := pruebas_aed$(SUFIJO)

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

.PHONY: todo ejecutar probar banco limpiar entorno

todo: $(BIN_SISTEMA) $(BIN_PRUEBAS)

$(BIN_SISTEMA): $(OBJETOS_NUCLEO) $(DIR_OBJ)/src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_PRUEBAS): $(OBJETOS_NUCLEO) $(DIR_OBJ)/pruebas/pruebas.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(DIR_OBJ)/%.o: %.cpp
	$(call CREAR_DIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

ejecutar: $(BIN_SISTEMA)
	$(PREFIJO)$(BIN_SISTEMA)

probar: $(BIN_PRUEBAS)
	$(PREFIJO)$(BIN_PRUEBAS)

banco: $(BIN_SISTEMA)
	$(PREFIJO)$(BIN_SISTEMA) --banco --maximo 1000000

limpiar:
	$(call BORRAR_DIR,$(DIR_OBJ))
	$(call BORRAR,$(BIN_SISTEMA))
	$(call BORRAR,$(BIN_PRUEBAS))
	$(call BORRAR,*.csv)

entorno:
	@echo Entorno .... $(ENTORNO)
	@echo Compilador . $(CXX)
	@echo Binarios ... $(BIN_SISTEMA) $(BIN_PRUEBAS)
