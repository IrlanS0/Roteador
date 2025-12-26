# --- Configuração Portátil ---
CC = gcc
CFLAGS = -Wall -g -std=c99
LDFLAGS = -lm 

# --- Detecção de Sistema ---
EXECUTABLE = main
# Assume comandos Unix por padrão
RM = rm -f
RM_DIR = rm -rf
MKDIR_CMD = @mkdir -p $(OUTPUT_DIR)
EXEC_PREFIX = ./

# Se o 'make' detectar que está no Windows...
ifeq ($(OS),Windows_NT)
	EXECUTABLE := $(EXECUTABLE).exe
	RM = del /f /q
	RM_DIR = rmdir /s /q
	MKDIR_CMD = @mkdir $(OUTPUT_DIR) 2>NUL || exit 0
	EXEC_PREFIX = .\
else
	EXEC_PREFIX = ./
endif
# Fim do bloco ifeq

# --- Arquivos do Projeto ---
OUTPUT_DIR = build

# --- Definir TODOS os arquivos .c e .h ---
SRCS = src/index.c

# --- Descoberta Automática de Testes ---
INPUTS = $(wildcard tests/*.input)
OUTPUTS = $(patsubst tests/%.input, $(OUTPUT_DIR)/%.out, $(INPUTS))

# --- Targets ---
.PHONY: all test clean

all: $(EXECUTABLE)
$(EXECUTABLE): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)
	

# --- Regra de Testes ---
test: all $(OUTPUTS)
	@echo "--- Todos os testes foram gerados! ---"

# Esta regra está perfeita
$(OUTPUT_DIR)/%.out: tests/%.input $(EXECUTABLE)
	$(MKDIR_CMD)
	@echo "Rodando teste: $< ..."
	$(EXEC_PREFIX)$(EXECUTABLE) $< $@

# --- MUDANÇA: Limpar os arquivos .o também ---
clean:
	@echo "Limpando..."
	@-del /f /q $(EXECUTABLE)
	@-del /f /q
	@-rmdir /s /q $(OUTPUT_DIR)
	@echo "Limpo!"