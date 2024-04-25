#ifndef SALA_H
#define SALA_H
#define CAPACIDAD_MAXIMA 100

// Practica 1
void crea_sala(int);
int elimina_sala(void);
int reserva_asiento(int id_persona);
int libera_asiento(int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres(void);
int asientos_ocupados(void);
int capacidad(void);

// Practica 2
void crea_sucursal (const char* ciudad,	int	capacidad);
void gestion_sala_shell(void);

// Practica 3
int guarda_estado_sala(const char*);
int recupera_estado_sala(const char*);
int guarda_estadoparcial_sala(const char*, size_t, int*);
int recupera_estadoparcial_sala(const char*, size_t, int*);

int contar_digitos(int);
void fillbuffer(char*);
int* get_sala();

#endif
