#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_SALAS 10

static int *asientos = NULL;
static int capacidad_actual = 0;
static int asientosOcupados = 0;

typedef struct {
    pid_t pid;
    char ciudad[100];
    int activa;
} Sala;

Sala salas[MAX_SALAS];
int numero_salas = 0;

// Función para crear una sala con una capacidad dada
int crea_sala(int capacidad) {
    if (asientos != NULL || capacidad <= 0 || capacidad > CAPACIDAD_MAXIMA) {
        return -1;
    }
    asientos = (int *)malloc(capacidad * sizeof(int));
    if (asientos == NULL) {
        return -1;
    }
    for (int i = 0; i < capacidad; i++) {
        asientos[i] = -1;
    }
    capacidad_actual = capacidad;
    asientosOcupados = 0;
    return capacidad;
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
int reserva_asiento(int id_persona) {
    if (asientos == NULL || id_persona <= 0 || asientos_ocupados() >= capacidad_actual) {
        return -1; // Sala llena o entrada inválida
    }
    for (int i = 0; i < capacidad_actual; i++) {
        if (asientos[i] == -1) { // Asiento libre encontrado
            asientos[i] = id_persona; // Reservar asiento
            asientosOcupados++;
            return i + 1; // Retornar el número de asiento reservado
        }
    }
    return -1; // No debería llegar aquí si hay asientos libres
}


// Función para liberar un asiento reservado
int libera_asiento(int id_asiento) {
    if (asientos == NULL || id_asiento < 1 || id_asiento > capacidad_actual) {
        return -1;
    }
    int index = id_asiento - 1;
    if (asientos[index] == -1) {
        return -1;
    }
    int id_persona = asientos[index];
    asientos[index] = -1;
    asientosOcupados--;
    return id_persona;
}

// Función para obtener el estado de un asiento
int estado_asiento(int id_asiento) {
    if (asientos == NULL || id_asiento < 1 || id_asiento > capacidad_actual) {
        return -1;
    }
    int index = id_asiento - 1;
    return asientos[index];
}

// Función para obtener la cantidad de asientos libres
int asientos_libres() {
    if (asientos == NULL) {
        return -1;
    }
    return capacidad_actual - asientosOcupados;
}

// Función para obtener la cantidad de asientos ocupados
int asientos_ocupados() {
    if (asientos == NULL) {
        return -1;
    }
    return asientosOcupados;
}

// Función para obtener la capacidad de la sala
int capacidad_sala() {
    return capacidad_actual;
}

int reserva_multiple_asientos(int cantidad) {
    // Verificar que hay suficientes asientos libres y que la entrada es válida
    if (asientos == NULL || cantidad <= 0 || cantidad > asientos_libres()) {
        return -1;
    }
    int reservados = 0;
    for (int i = 0; i < capacidad_actual && reservados < cantidad; i++) {
        if (asientos[i] == -1) { // Asiento libre encontrado
            asientos[i] = 0; // Reservar asiento (ajustar según la lógica de identificación de usuario)
            reservados++;
            asientosOcupados++; // Incrementar el contador de asientos ocupados
        }
    }
    return reservados; // Devolver el número de asientos realmente reservados
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

void gestion_sala_shell(void) {
    char comando[256];
    int id, resultado;

    printf("Mini-shell de gestión de sala iniciado. Esperando comandos...\n");
    while (1) {
        printf("> ");
        if (scanf("%255s", comando) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (strcmp(comando, "reserva") == 0) {
            if (scanf("%d", &id) == 1) {
                resultado = reserva_asiento(id);
                if (resultado != -1) {
                    printf("Asiento %d reservado para la persona con ID %d.\n", resultado, id);
                } else {
                    printf("No se pudo reservar el asiento para la persona con ID %d.\n", id);
                }
            } else {
                printf("Error en la lectura del ID para reserva.\n");
                while (getchar() != '\n');
            }
        } else if (strcmp(comando, "libera") == 0) {
            if (scanf("%d", &id) == 1) {
                resultado = libera_asiento(id);
                if (resultado != -1) {
                    printf("Asiento %d liberado que estaba reservado por la persona con ID %d.\n", id, resultado);
                } else {
                    printf("No se pudo liberar el asiento %d.\n", id);
                }
            } else {
                printf("Error en la lectura del ID para liberar.\n");
                while (getchar() != '\n');
            }
        } else if (strcmp(comando, "estado_asiento") == 0) {
            if (scanf("%d", &id) == 1) {
                resultado = estado_asiento(id);
                if (resultado != -1) {
                    printf("Asiento %d está ocupado por la persona con ID %d.\n", id, resultado);
                } else {
                    printf("Asiento %d está libre.\n", id);
                }
            } else {
                printf("Error en la lectura del ID para estado de asiento.\n");
                while (getchar() != '\n');
            }
        } else if (strcmp(comando, "estado_sala") == 0) {
            printf("Asientos ocupados: %d\n", asientos_ocupados());
            printf("Asientos libres: %d\n", asientos_libres());
            printf("Capacidad de la sala: %d\n", capacidad_sala());
        } else if (strcmp(comando, "cerrar_sala") == 0) {
            int final_status = asientos_libres() == 0 ? 0 : 1;
            elimina_sala();
            printf("Sala cerrada con estado %d. Terminando mini-shell...\n", final_status);
            exit(final_status);
        } else if (strcmp(comando, "limpia_sala") == 0) {
            limpia_sala();
        } else if (strcmp(comando, "reserva_multiple") == 0) {
            int cantidad;
            if (scanf("%d", &cantidad) == 1) {
                resultado = reserva_multiple_asientos(cantidad);
                if (resultado > 0) {
                    printf("%d asientos reservados exitosamente.\n", resultado);
                } else {
                    printf("No se pudieron reservar %d asientos.\n", cantidad);
                }
            } else {
                printf("Error en la lectura de la cantidad para reserva múltiple.\n");
                while (getchar() != '\n');
            }
        } else {
            printf("Comando no reconocido.\n");
        }
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