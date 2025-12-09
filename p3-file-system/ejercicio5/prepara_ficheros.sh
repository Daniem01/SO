#!/bin/bash

# Directorio proporcionado como argumento
DIR_NAME=$1

# GESTIÓN DEL ARGUMENTO
if [ -z "$DIR_NAME" ]; then
    echo "Error: Debe proporcionar un nombre de directorio como argumento."
    exit 1
fi

# COMPROBACIÓN Y LIMPIEZA
if [ -d "$DIR_NAME" ]; then
    echo "Directorio '$DIR_NAME' existe. Limpiando contenido..."
    # Borra todos los archivos, subdirectorios y archivos ocultos 
    rm -rf "$DIR_NAME"/* 2>/dev/null
    rm -rf "$DIR_NAME"/.[!.]* 2>/dev/null
else
    echo "Directorio '$DIR_NAME' no existe. Creándolo..."
    mkdir "$DIR_NAME"
fi

# Accedemos al directorio
cd "$DIR_NAME" || { echo "Error al acceder al directorio '$DIR_NAME'."; exit 1; }

echo "--- CREANDO ESTRUCTURA DE FICHEROS ---"
# CREACIÓN DE FICHEROS Y ENLACES
mkdir subdir                       
touch fichero1                      
echo "1234567890" > fichero2         
ln -s fichero2 enlaceS              
ln fichero2 enlaceH                

echo ""
echo "--- RESULTADOS DE STAT ---"
# RECORRER Y MOSTRAR ATRIBUTOS
stat * .* 2>/dev/null 