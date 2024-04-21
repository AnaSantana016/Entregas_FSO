#ifndef SALA_H
#define SALA_H

#include <sys/types.h>
#include <sys/wait.h> 

#define CAPACIDAD_MAXIMA 100
#define MAX_SALAS 10

typedef struct {
    pid_t pid;
    char ciudad[100];
    int activa;
} Sala;

extern Sala salas[MAX_SALAS];
extern int numero_salas;

int crea_sala(int capacidad);
int elimina_sala(void);
int reserva_asiento(int id_persona);
int libera_asiento(int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres(void);
int asientos_ocupados(void);
int capacidad_sala(void);

void crea_sucursal(const char* ciudad, int capacidad);
void gestion_sala_shell(void);
void setup_signal_handler(void);
void cerrar_todas_las_salas(void);
void comprobar_salas(void);

#endif // SALA_H
