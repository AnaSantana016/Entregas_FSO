#ifndef SALA_H
#define SALA_H
#define CAPACIDAD_MAXIMA 100

// Práctica 1
int crea_sala(int capacidad);
int elimina_sala(void);
int reserva_asiento(int id_persona);
int libera_asiento(int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres(void);
int asientos_ocupados(void);
int capacidad_sala(void);

// Práctica 2
void crea_sucursal (const char* ciudad,	int	capacidad);
void gestion_sala_shell(void);
#endif
