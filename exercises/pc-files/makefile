# Variables de configuración
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = pc-files
SRC = pc-files.c

# Regla por defecto (se ejecuta al escribir 'make' a secas)
all: $(TARGET)

# Regla para compilar el ejecutable vinculando la librería de hilos
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para limpiar los archivos generados y dejar la carpeta impecable
clean:
	rm -f $(TARGET)

.PHONY: all clean