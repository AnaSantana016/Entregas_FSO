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
echo "========================================"
echo "Para probar el programa misala, ejecute los siguientes comandos:"
echo "========================================"
echo "cd Practica3/fuentes/"
echo "./misala ... "
echo "========================================"
echo "Ayuda del programa misala"
echo "./misala crea -f archivo.txt -c 10"
echo "./misala crea -f archivo.txt -c 10 -o"
echo "./misala reserva -f archivo.txt id_persona1 id_persona2…"
echo "./misala anula -f archivo.txt -a id-asiento1 id-asiento2…"
echo "./misala estado -f archivo.txt"
echo "./misala elimina -f archivo.txt"
echo "./misala compara archivo.txt archivo2.txt"
echo "========================================"
