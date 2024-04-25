#!/bin/bash

# Cambiar al directorio donde está el archivo sala.c
cd lib/sala

# Compilar sala.c para generar el objeto sala.o
gcc -c sala.c -o sala.o

# Volver al directorio principal y crear una biblioteca estática libsalas.a que incluya sala.o
cd ../..
ar -crs lib/libsalas.a lib/sala/sala.o

# Cambiar al directorio fuentes/ para compilar el programa misala.c que utiliza la biblioteca libsalas.a
cd fuentes/
gcc misala.c -o misala -lsala -L../lib

# Ejecutar el programa compilado
echo "Compilando la biblioteca y compilando el programa misala"
echo ========================================
echo "Ejecutando el programa misala"
echo ========================================
echo "Ayuda del programa misala"
echo "./misala crea -f archivo.txt -c capacidad"
echo "./misala crea -f archivo.txt -c capacidad -o para sobreescribir un archivo"
echo "./misala reserva -f archivo.txt -n numero id-asiento"
echo "./misala anula -f archivo.txt -a id-asiento"
echo "./misala estado -f archivo.txt"
echo "./misala confirmaciones -f archivo.txt -a id-asiento"
echo "./misala elimina -f archivo.txt"

