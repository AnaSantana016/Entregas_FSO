#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int *asientos = NULL;
static int capacidad_actual = 0;
static int asientosOcupados = 0;

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

int reserva_asiento(int id_persona) {
    if (asientos == NULL || id_persona <= 0) {
        return -1;
    }
    for (int i = 0; i < capacidad_actual; i++) {
        if (asientos[i] == -1) {
            asientos[i] = id_persona;
            asientosOcupados++;
            return i + 1;
        }
    }
    return -1;
}

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

int estado_asiento(int id_asiento) {
    if (asientos == NULL || id_asiento < 1 || id_asiento > capacidad_actual) {
        return -1;
    }
    int index = id_asiento - 1;
    return asientos[index];
}

int asientos_libres() {
    if (asientos == NULL) {
        return -1;
    }
    return capacidad_actual - asientosOcupados;
}

int asientos_ocupados() {
    if (asientos == NULL) {
        return -1;
    }
    return asientosOcupados;
}

int capacidad_sala() {
    return capacidad_actual;
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
        }
        else if (strcmp(comando, "libera") == 0) {
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
        }
        else if (strcmp(comando, "estado_sala") == 0) {
            printf("Asientos ocupados: %d\n", asientos_ocupados());
            printf("Asientos libres: %d\n", asientos_libres());
            printf("Capacidad de la sala: %d\n", capacidad_sala());
        }
        else if (strcmp(comando, "cerrar_sala") == 0) {
            elimina_sala();
            printf("Sala cerrada. Terminando mini-shell...\n");
            break;
        }
        else {
            printf("Comando no reconocido.\n");
        }
    }
}

void crea_sucursal(const char* ciudad, int capacidad) {
    int estadoSala = crea_sala(capacidad);
    if (estadoSala == -1) {
        fprintf(stderr, "Error al crear la sala con capacidad %d.\n", capacidad);
        exit(EXIT_FAILURE);
    }

    printf("La sucursal '%s' ha sido creada con éxito.\n", ciudad);

    for (int i = 0; i < 4; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error al crear el proceso hijo (fork failed)");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            execlp("gnome-terminal", "gnome-terminal", "--", "bash", "-c",
                   "exec ./gestion_sala shell; exec bash", NULL);
            perror("Error al ejecutar gnome-terminal");
            exit(EXIT_FAILURE);
        }
    }

    int status;
    while ((wait(&status)) > 0);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "shell") == 0) {
        gestion_sala_shell();
        return 0;
    }

    printf("Programa de gestión de salas iniciado.\n");
    char ciudad[100];
    int capacidad;
    
    while (1) {
        printf("Introduce el nombre de la nueva sala (o 'salir' para terminar): ");
        fflush(stdout);

        if (scanf("%99s", ciudad) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (strcmp(ciudad, "salir") == 0) {
            printf("Saliendo del programa.\n");
            break;
        }

        printf("Introduce la capacidad de la sala: ");
        fflush(stdout);

        if (scanf("%d", &capacidad) != 1 || capacidad <= 0 || capacidad > CAPACIDAD_MAXIMA) {
            printf("Entrada inválida. La capacidad debe ser un número entre 1 y %d.\n", CAPACIDAD_MAXIMA);
            while (getchar() != '\n');
            continue;
        }

        crea_sucursal(ciudad, capacidad);
        break;
    }

    return 0;
}