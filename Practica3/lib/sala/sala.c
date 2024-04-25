#include <stdlib.h>
#include <stdio.h>
#include "sala.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_SALAS 10

#define LIBRE 0

int* asientos;
static int capacidad_actual = 0;
static int asientosOcupados = 0;
int id;
int* capacidad_sala;
int i;
int PLAZAS;

typedef struct {
    pid_t pid;
    char ciudad[100];
    int activa;
} Sala;

Sala salas[MAX_SALAS];
int numero_salas = 0;

// Función para crear una sala con una capacidad dada
void crea_sala(int capacidad){
    PLAZAS = capacidad;
    capacidad_sala = (int*)malloc(capacidad * sizeof(int));
    if (capacidad_sala == NULL){
        fprintf(stderr, "Error de Memoria: %s\n", strerror(errno));
    }
    for (i = 0; i < PLAZAS; i++){
        capacidad_sala[i] = LIBRE;
    }
}

// Función para eliminar una sala
int elimina_sala() {
    if (asientos == NULL) {
        return -1;
    }
    free(asientos);
    asientos = NULL;
    capacidad_actual = 0;
    asientosOcupados = 0;
    return 0;
}

// Función para reservar un asiento para una persona
int reserva_asiento(int id){
    if (id <= 0){
        fprintf(stderr, "Ha habido algún error al reservar el asiento.\n");
        return -1;
    }
    for (i = 0; i < PLAZAS; i++){
        if (capacidad_sala[i] == LIBRE){
            capacidad_sala[i] = id;
            return i;
        }
    }
    fprintf(stderr, "No se pueden reservar más asientos.\n");
    return -1;
}

// Función para liberar un asiento reservado
int libera_asiento(int asiento){
    if (asiento < 0 || asiento >= PLAZAS){
        fprintf(stderr, "Ha habido algún error: %s\n", strerror(errno));
        return -1;
    }
    if (capacidad_sala[asiento] > LIBRE){
        int variable = capacidad_sala[asiento];
        capacidad_sala[asiento] = LIBRE;
        return variable;
    } else {
        fprintf(stderr, "Ha habido algún error: %s\n", strerror(errno));
        return -1;
    }
}

// Función para obtener el estado de un asiento
int estado_asiento(int asiento){
    if (asiento < 0 || asiento >= PLAZAS){
        fprintf(stderr, "Ha habido algún error: %s\n", strerror(errno));
        return -1;
    } else if (capacidad_sala[asiento] >= 0){
        return capacidad_sala[asiento];
    }
    return 0;
}

// Función para obtener la cantidad de asientos libres
int asientos_libres(){
    int asiento_libre = 0;
    for (i = 0; i < PLAZAS; i++){
        if (capacidad_sala[i] == LIBRE){
            asiento_libre++;
        }
    }
    return asiento_libre;
}

// Función para obtener la cantidad de asientos ocupados
int asientos_ocupados(){
    int asiento_ocupado = 0;
    for (i = 0; i < PLAZAS; i++){
        if (capacidad_sala[i] > LIBRE){
            asiento_ocupado++;
        }
    }
    return asiento_ocupado;
}

// Función para obtener la capacidad de la sala
int capacidad(){
    return asientos_libres() + asientos_ocupados();
}

// Función para limpiar todos los asientos de la sala, haciéndolos disponibles
void limpia_sala() {
    if (asientos == NULL) {
        printf("No hay una sala creada para limpiar.\n");
        return;
    }

    for (int i = 0; i < capacidad_actual; i++) {
        asientos[i] = -1; // Marcar cada asiento como disponible
    }
    asientosOcupados = 0; // Actualizar el contador de asientos ocupados
    printf("Todos los asientos de la sala han sido liberados.\n");
}

// Función para manejar la señal SIGCHLD
void signal_handler(int sig, siginfo_t *siginfo, void *context) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        int final_status = WEXITSTATUS(status);
        printf("La sala con PID %d ha finalizado. Estado final del cierre: %d\n", pid, final_status);
        printf("Introduce el nombre de la nueva sala (o 'salir' para terminar): ");
        fflush(stdout);
    }
}

// Función para configurar el manejador de señales
void setup_signal_handler() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = &signal_handler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

// Función para crear una sucursal con una ciudad y capacidad dadas
void crea_sucursal(const char* ciudad, int capacidad) {
    if (numero_salas >= MAX_SALAS) {
        printf("Se ha alcanzado el número máximo de salas.\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso hijo (fork failed)");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char str_capacidad[10];
        sprintf(str_capacidad, "%d", capacidad);

        execlp("xterm", "xterm", "-e", "./gestion_sala", "gestion", ciudad, str_capacidad, (char *)NULL);

        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        salas[numero_salas].pid = pid;
        strncpy(salas[numero_salas].ciudad, ciudad, sizeof(salas[numero_salas].ciudad) - 1);
        salas[numero_salas].activa = 1;
        numero_salas++;
        printf("La sucursal '%s' ha sido creada con éxito. PID del proceso hijo: %d\n", ciudad, pid);
    }
}

// Función para comprobar el estado de las salas
void comprobar_salas() {
    for (int i = 0; i < numero_salas; i++) {
        if (salas[i].activa) {
            int status;
            pid_t result = waitpid(salas[i].pid, &status, WNOHANG);
            if (result > 0) { 
                printf("La sala '%s' ha finalizado su ejecución.\n", salas[i].ciudad);
                salas[i].activa = 0;
            } else if (result == -1) {
                perror("Error al comprobar el estado de la sala");
                salas[i].activa = 0;
            }
        }
    }
}

// Función para cerrar todas las salas
void cerrar_todas_las_salas() {
    for (int i = 0; i < numero_salas; i++) {
        if (salas[i].activa) {
            kill(salas[i].pid, SIGTERM);
            salas[i].activa = 0;
        }
    }
}

int guarda_estado_sala(const char* ruta_fichero){
    int archivo = open(ruta_fichero, O_WRONLY|O_CREAT|O_TRUNC, 00666);
    if (archivo == -1){
        fprintf(stderr, "Error abriendo el archivo: %s\n", strerror(errno));
        return -1;
    }
    char* almacen_capacidad = (char*)malloc(1025*sizeof(char));
    sprintf(almacen_capacidad, "%d\n", capacidad());
    fillbuffer(almacen_capacidad);
    ssize_t capacidad_escrita = write(archivo, almacen_capacidad, strlen(almacen_capacidad));
    free(almacen_capacidad);
    if (capacidad_escrita == -1){
        close(archivo);
        fprintf(stderr, "Error escribiendo la capacidad de la sala en el archivo: %s\n", strerror(errno));
        return -1;
    }
    for (int i = 0; i < PLAZAS; i++){
        int asientos = capacidad_sala[i];
        char* buffer = (char*)malloc(1025*sizeof(char));
        sprintf(buffer, "%d,%d\n", i, asientos);
        fillbuffer(buffer);
        ssize_t estado_asiento = write(archivo, buffer, strlen(buffer));
        free(buffer);
        if (estado_asiento == -1){
            close(archivo);
            fprintf(stderr, "Error al escribir el estado de cada asiento en el archivo: %s\n", strerror(errno));
            return -1;
        }
    }
    if (close(archivo) == -1){
        fprintf(stderr, "Error al cerrar el archivo: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int recupera_estado_sala(const char* ruta_fichero){
    int archivo = open(ruta_fichero, O_RDONLY);
    if (archivo == -1){
        fprintf(stderr, "Error abriendo el archivo: %s\n", strerror(errno));
        return -1;
    }
    int tempNumber = 0;
    int auxIndex = -1;
    char currentChar[1];
    int first = 0;
    int num_lineas = 0;
    ssize_t capacidad_leida = read(archivo, currentChar, 1);
    while (capacidad_leida != 0){
        if (currentChar[0] != '\n' && currentChar[0] != ','){
            tempNumber = (tempNumber * 10) + atoi(currentChar);
        }
        else if (currentChar[0] == '\n' && !first){
            first = 1;
            crea_sala(tempNumber);
            tempNumber = 0;
        }
        else if(currentChar[0] == ','){
            auxIndex = tempNumber;
            tempNumber = 0;
        }
        else if(currentChar[0] == '\n' && auxIndex >= 0){
            capacidad_sala[auxIndex] = tempNumber;
            tempNumber = 0;
            auxIndex = -1;
            num_lineas++;
        }
        capacidad_leida = read(archivo, currentChar, 1);
    }
    if (capacidad_leida == -1){
        close(archivo);
        fprintf(stderr, "Error leyendo la capacidad de la sala en el archivo: %s\n", strerror(errno));
        return -1;
    }
    if(num_lineas != capacidad()){
        fprintf(stderr, "Error la capacidad del archivo no coincide con la capacidad actual de la sala: %s\n", strerror(errno));
        return -1;
    }
    if(close(archivo) == -1){
        fprintf(stderr, "Error al cerrar el archivo: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    int archivo = open(ruta_fichero, O_RDWR);
    if (archivo == -1) {
        fprintf(stderr, "Error abriendo el archivo: %s\n", strerror(errno));
        return -1;
    }

    for (size_t i = 0; i < num_asientos; i++) {
        int asiento_id = id_asientos[i];
        if (asiento_id < 0 || asiento_id >= PLAZAS) {
            fprintf(stderr, "Error: ID de asiento fuera de rango.\n");
            close(archivo);
            return -1;
        }

        // Calcula la posición en el archivo basada en el ID del asiento
        off_t pos = lseek(archivo, (asiento_id * 1025), SEEK_SET);
        if (pos == -1) {
            fprintf(stderr, "Error moviendo el cursor en el archivo: %s\n", strerror(errno));
            close(archivo);
            return -1;
        }

        char buffer[1025];
        sprintf(buffer, "%d,%d\n", asiento_id, capacidad_sala[asiento_id]);
        fillbuffer(buffer);  // Asegura que el buffer tenga siempre el mismo tamaño
        ssize_t written = write(archivo, buffer, strlen(buffer));
        if (written == -1) {
            fprintf(stderr, "Error escribiendo en el archivo: %s\n", strerror(errno));
            close(archivo);
            return -1;
        }
    }

    if (close(archivo) == -1) {
        fprintf(stderr, "Error al cerrar el archivo: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    int archivo = open(ruta_fichero, O_RDONLY);
    if (archivo == -1) {
        fprintf(stderr, "Error abriendo el archivo: %s\n", strerror(errno));
        return -1;
    }

    for (size_t i = 0; i < num_asientos; i++) {
        int asiento_id = id_asientos[i];
        if (asiento_id < 0 || asiento_id >= PLAZAS) {
            fprintf(stderr, "Error: ID de asiento fuera de rango.\n");
            close(archivo);
            return -1;
        }

        // Calcula la posición en el archivo basada en el ID del asiento
        off_t pos = lseek(archivo, (asiento_id * 1025), SEEK_SET);
        if (pos == -1) {
            fprintf(stderr, "Error moviendo el cursor en el archivo: %s\n", strerror(errno));
            close(archivo);
            return -1;
        }

        char buffer[1025] = {0};
        ssize_t read_bytes = read(archivo, buffer, sizeof(buffer)-1);
        if (read_bytes == -1) {
            fprintf(stderr, "Error leyendo el archivo: %s\n", strerror(errno));
            close(archivo);
            return -1;
        }

        int asiento, estado;
        if (sscanf(buffer, "%d,%d", &asiento, &estado) != 2) {
            fprintf(stderr, "Error parseando datos del archivo.\n");
            close(archivo);
            return -1;
        }
        capacidad_sala[asiento] = estado;  // Actualiza el estado en memoria
    }

    if (close(archivo) == -1) {
        fprintf(stderr, "Error al cerrar el archivo: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
 