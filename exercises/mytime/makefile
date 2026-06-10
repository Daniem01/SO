# Variables de configuración
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = mytime
SRC = mytime.c

# Regla por defecto (la que se ejecuta al escribir 'make' a secas)
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para limpiar los archivos generados y dejar la carpeta impecable
clean:
	rm -f $(TARGET)

.PHONY: all clean