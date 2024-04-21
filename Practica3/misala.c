#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

// Prototipo de funciones
void parse_args(int argc, char* argv[]);
void crea_sala_cmd(char* filepath, int capacity, int overwrite);
void reserva_asientos_cmd(char* filepath, int* ids, int count);
void anula_reservas_cmd(char* filepath, int* ids, int count);
void muestra_estado_cmd(char* filepath);

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    return 0;
}

void parse_args(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso incorrecto de los argumentos.\n");
        exit(EXIT_FAILURE);
    }

    char* cmd = argv[1];

    if (strcmp(cmd, "crea") == 0) {
        char* filepath = NULL;
        int capacity = 0, overwrite = 0;
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
                filepath = argv[++i];
            } else if (strcmp(argv[i], "-fo") == 0 && i + 1 < argc) {
                filepath = argv[++i];
                overwrite = 1;
            } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
                capacity = atoi(argv[++i]);
            }
        }
        if (!filepath || capacity <= 0) {
            fprintf(stderr, "Argumentos insuficientes o incorrectos para crear una sala.\n");
            exit(EXIT_FAILURE);
        }
        crea_sala_cmd(filepath, capacity, overwrite);
    } else if (strcmp(cmd, "reserva") == 0) {
        char* filepath = argv[3];
        int ids[argc - 4];
        for (int i = 4; i < argc; i++) {
            ids[i - 4] = atoi(argv[i]);
        }
        reserva_asientos_cmd(filepath, ids, argc - 4);
    } else if (strcmp(cmd, "anula") == 0) {
        char* filepath = argv[3];
        int ids[argc - 4];
        for (int i = 4; i < argc; i++) {
            ids[i - 4] = atoi(argv[i]);
        }
        anula_reservas_cmd(filepath, ids, argc - 4);
    } else if (strcmp(cmd, "estado") == 0) {
        char* filepath = argv[3];
        muestra_estado_cmd(filepath);
    } else {
        fprintf(stderr, "Comando no reconocido.\n");
        exit(EXIT_FAILURE);
    }
}

void crea_sala_cmd(char* filepath, int capacity, int overwrite) {
    int flags = O_WRONLY | O_CREAT;
    if (overwrite) {
        flags |= O_TRUNC;
    } else {
        flags |= O_EXCL;
    }

    int fd = open(filepath, flags, 0666);
    if (fd == -1) {
        perror("Error al abrir/crear el archivo");
        exit(EXIT_FAILURE);
    }

    // Escribir la capacidad al inicio del archivo
    if (write(fd, &capacity, sizeof(int)) != sizeof(int)) {
        perror("Error al escribir la capacidad en el archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Inicializar todos los asientos como libres (-1)
    int asiento = -1;
    for (int i = 0; i < capacity; i++) {
        if (write(fd, &asiento, sizeof(int)) != sizeof(int)) {
            perror("Error al inicializar asientos");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    printf("Sala creada con éxito en %s\n", filepath);
}

void reserva_asientos_cmd(char* filepath, int* ids, int count) {
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Supongamos que la primera parte del archivo contiene la capacidad de la sala
    int capacidad;
    if (read(fd, &capacidad, sizeof(capacidad)) != sizeof(capacidad)) {
        fprintf(stderr, "Error al leer la capacidad del archivo\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Comprobar si hay suficientes asientos disponibles antes de proceder
    int asiento, disponibles = 0;
    lseek(fd, sizeof(int), SEEK_SET); // Saltar la capacidad
    for (int i = 0; i < capacidad; i++) {
        if (read(fd, &asiento, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "Error al leer el estado de un asiento\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if (asiento == -1) disponibles++;
    }

    if (disponibles < count) {
        fprintf(stderr, "No hay suficientes asientos disponibles\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Reserva de asientos
    lseek(fd, sizeof(int), SEEK_SET);
    for (int i = 0; i < count; i++) {
        int offset = ids[i] - 1; // Convertir ID a índice base-0
        lseek(fd, offset * sizeof(int), SEEK_CUR);
        if (write(fd, ids + i, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "Error al reservar un asiento\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    printf("Reserva completada con éxito.\n");
}

void anula_reservas_cmd(char* filepath, int* ids, int count) {
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int capacidad;
    if (read(fd, &capacidad, sizeof(capacidad)) != sizeof(capacidad)) {
        fprintf(stderr, "Error al leer la capacidad del archivo\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int asiento;
    for (int i = 0; i < count; i++) {
        int asientoIndex = ids[i] - 1; // Ajustando índice basado en usuario a índice basado en C
        if (asientoIndex >= 0 && asientoIndex < capacidad) {
            lseek(fd, sizeof(int) + asientoIndex * sizeof(int), SEEK_SET);
            asiento = -1; // Marcando asiento como libre
            if (write(fd, &asiento, sizeof(int)) != sizeof(int)) {
                fprintf(stderr, "Error al anular un asiento\n");
                close(fd);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "ID de asiento %d fuera de rango\n", ids[i]);
        }
    }
    close(fd);
    printf("Anulación completada con éxito.\n");
}

void muestra_estado_cmd(char* filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int capacidad;
    if (read(fd, &capacidad, sizeof(capacidad)) != sizeof(capacidad)) {
        fprintf(stderr, "Error al leer la capacidad del archivo\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int asiento;
    printf("Estado de los asientos:\n");
    for (int i = 0; i < capacidad; i++) {
        if (read(fd, &asiento, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "Error al leer el estado de un asiento\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
        printf("Asiento %d: %s\n", i + 1, asiento == -1 ? "Libre" : "Ocupado");
    }
    close(fd);
}
