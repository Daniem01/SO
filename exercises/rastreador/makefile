# Variables de configuración
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = rastreador
SRC = rastreador.c

# Regla por defecto
all: $(TARGET)

# Regla de compilación
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Limpieza
clean:
	rm -f $(TARGET)

.PHONY: all clean