# Opciones de compilación
CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -pthread

# Nombre del ejecutable final
TARGET = disco

# Archivo fuente
SRC = disco.c

# Regla por defecto: compila el ejecutable
all: $(TARGET)

# Regla para enlazar y compilar el binario
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Limpieza del binario generado
clean:
	rm -f $(TARGET)

.PHONY: all clean