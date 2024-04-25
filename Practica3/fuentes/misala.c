#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../cabeceras/sala.h"

//Función misala.c

int misala_crea(char* ruta, int capacidad, int sobreescribir){
	
	if(ruta == NULL){
		fprintf(stderr, "Error: no se proporcionó la ruta del archivo.\n");
		return -1;
	}
	
	
	if(access(ruta, F_OK) == 0 && sobreescribir == 0){
		fprintf(stderr, "Error: el archivo ya existe, se debe utilizar la opcion -o para sobreescribir el archivo.\n");
		return -1;
	}
	
	//Crea la sala con la capacidad indicada.
	crea_sala(capacidad);
	if(capacidad <= 0){
		fprintf(stderr, "Error: la capacidad debe ser mayor de 0.\n");
		elimina_sala();
		return -1;
	}
	
	//Guarda el estado inicial de la sala en el archivo.
	if (guarda_estado_sala(ruta) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo guardar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	elimina_sala();
	return 0;
}

int misala_reserva(char* ruta, int num_asientos, int* id_personas){

	if(ruta == NULL){
		fprintf(stderr, "Error: no se proporcionó la ruta del archivo.\n");
		return -1;
	}

	if(access(ruta, W_OK) != 0){
		fprintf(stderr, "Error: no hay permisos de escritura\n");
		return -1;
	}
	
	//Recupera el estado parcial de la sala
	if (recupera_estado_sala(ruta) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo recuperar el estado parcial de la sala en el archivo.\n");
		return -1;
	}

  	//Se reservan los asientos de la sala.

  	int* ids = malloc(num_asientos * sizeof(int));
	if (ids == NULL) {
  		fprintf(stderr, "Error: no se pudo asignar memoria para el arreglo de IDs\n");
  		return -1;
	}

	int num_ids = 0;

	for (int i = 0; i < num_asientos; i++) {
  	// Verifica si los IDs proporcionados por parámetro hay alguno que se repite.
  		for (int j = 0; j < num_ids; j++) {
    			if (id_personas[i] == ids[j]) {
      				fprintf(stderr, "Error: Los ids pasados por parámetros no pueden coincidir.\n");
      				free(ids);
      				return -1;
    			}
  		}
  
  	// Agrega el ID de persona al arreglo de IDs
  	ids[num_ids++] = id_personas[i];
 
  	// Escribe el estado de cada asiento en el archivo.
  	reserva_asiento(id_personas[i]);
	}
	
	// Liberar la memoria dinámica del arreglo de IDs leídos.
	free(ids);
	
	//Si el numero de asientos es mayor que la capacidad de la sala devuelve un error.
	if (num_asientos > capacidad()){
		fprintf(stderr, "Error: el número de asientos no puede superar la sala.\n");
		printf("La capacidad es: %d\n", capacidad());
		return -1;
	}
	
	//Guarda el estado inicial de la sala en el archivo.
	if (guarda_estado_sala(ruta) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo guardar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	elimina_sala();
	return 0;
}

int misala_anula(char* ruta, int num_asientos, int* id_asientos){

	if(ruta == NULL){
		fprintf(stderr, "Error: no se proporcionó la ruta del archivo.\n");
		return -1;
	}
	
	if(access(ruta, W_OK) != 0){
		fprintf(stderr, "Error: no hay permisos de escritura\n");
		return -1;
	}
	
	//Recupera el estado inicial de la sala en el archivo.
	int capacidad_sala[num_asientos];
	if (recupera_estadoparcial_sala(ruta, num_asientos, id_asientos) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo recuperar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	for (int i = 0; i < num_asientos; i++){
		libera_asiento(id_asientos[i]);
	
	}
	
	//Guarda el estado de la sala en el archivo.
	if (guarda_estadoparcial_sala(ruta, num_asientos, id_asientos) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo guardar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	
	elimina_sala();
	return 0;
}

int misala_confirmaciones(char* ruta, int num_asientos, int* id_asientos){
	
	//Te elimina todo lo demás que tu no selecciones
	if(ruta == NULL){
		fprintf(stderr, "Error: no se proporcionó la ruta del archivo.\n");
		return -1;
	}
	
	if(access(ruta, W_OK) != 0){
		fprintf(stderr, "Error: no hay permisos de escritura\n");
		return -1;
	}
	
	//Recupera el estado inicial de la sala en el archivo.
	if (recupera_estadoparcial_sala(ruta, num_asientos, id_asientos) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo recuperar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	//Guarda el estado parcial de la sala en el archivo.
	if (guarda_estado_sala(ruta) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo guardar el estado parcial de la sala en el archivo.\n");
		return -1;
	}
	
	elimina_sala();
	return 0;
}

int misala_compara(char* ruta1, char* ruta2) {
	int capacidad_sala[capacidad()];
    if (ruta1 == NULL || ruta2 == NULL) {
        fprintf(stderr, "Error: Rutas de archivo no proporcionadas.\n");
        return -1;
    }

    if (access(ruta1, R_OK) != 0 || access(ruta2, R_OK) != 0) {
        fprintf(stderr, "Error: Problemas de acceso con los archivos.\n");
        return -1;
    }

    if (recupera_estado_sala(ruta1) == -1 || recupera_estado_sala(ruta2) == -1) {
        fprintf(stderr, "Error: No se pudo cargar el estado de alguna de las salas.\n");
        return -1;
    }

    // Asumimos que las funciones de recuperación de estado configuran una estructura global `capacidad_sala`
    if (capacidad() != capacidad()) {
        fprintf(stderr, "Las capacidades de las salas no coinciden.\n");
        return 1;
    }

    for (int i = 0; i < capacidad(); i++) {
        if (capacidad_sala[i] != capacidad_sala[i]) {
            return 1;  // Las salas son diferentes
        }
    }

    return 0;  // Las salas son iguales
}


int misala_estado(char* ruta){
	
	if(ruta == NULL){
		fprintf(stderr, "Error: no se proporcionó la ruta del archivo.\n");
		return -1;
	}
	
	
	if(access(ruta, R_OK) != 0){
		fprintf(stderr, "Error: no hay permisos de lectura\n");
		return -1;
	}
	
	if(access(ruta, F_OK) != 0){
		fprintf(stderr, "Error: no existe fichero.\n");
		return -1;
	}
	
	//Recupera el estado inicial de la sala en el archivo.
	if (recupera_estado_sala(ruta) == -1){
		elimina_sala();
		fprintf(stderr, "Error: no se pudo recuperar el estado de la sala en el archivo.\n");
		return -1;
	}
	
	
	int* capacidad_sala = get_sala();
	printf("La capacidad es: %d\n", capacidad());
	for(int i = 0; i < capacidad(); i++){
		printf("El asiento %d tiene el estado %d\n",i,capacidad_sala[i]);
	}
	
	elimina_sala();
	return 0;
	
}

int elimina_sala_y_archivo(char* ruta) {
    if (ruta == NULL) {
        fprintf(stderr, "Error: No se proporcionó la ruta del archivo.\n");
        return -1;
    }

    // Comprobar si el archivo existe
    if (access(ruta, F_OK) != 0) {
        fprintf(stderr, "Error: El archivo especificado no existe.\n");
        return -1;
    }

    // Intentar eliminar el archivo
    if (remove(ruta) != 0) {
        perror("Error al eliminar el archivo");
        return -1;
    }

    // Eliminar la sala (limpiar recursos de memoria)
    elimina_sala();

    printf("Archivo y sala eliminados correctamente.\n");
    return 0;
}


int main(int argc, char *argv[]){

	if(argc < 3){
		fprintf(stderr, "Número de parámetros introducidos incorrectos: %s \nUso correcto de parámetros: misala Orden Argumento\n", strerror(errno));
		exit(-1);
	}
	
	int flag_f, flag_o, flag_c, flag_i, flag_n, flag_a, flag_inc = 0;
	int sobreescribir, capacidad = 0;
	char caracter;
	char* ruta;
	int* id_asientos;
	int* id_personas;
	int num_asientos;
	
	
	
	if(strcmp(argv[1], "crea") == 0){
	
		while ((caracter=getopt(argc, argv, "f:c:o::")) != -1){
			
			switch(caracter) {
				
				case 'f':
					flag_f = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -f\n");
						return -1;
					}
					ruta = strdup(optarg);
					break;
				
				case 'c':
					flag_c = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -c\n");
						return -1;
					
					}
					capacidad = atoi(optarg);
					break;
					
				case 'o':
					flag_o = 1;
					sobreescribir = 1;
					break;
					
				case '?':
					flag_inc = 1;
					printf("Opción inesperada: %c\n", optopt);
					break;
					
				default:
					fprintf(stderr, "Uso: ./misala %s -f[o] ruta -c capacidad\n", argv[1]);
					break;
			}
		}
		
		if(flag_f == 0 || flag_c == 0){
			fprintf(stderr, "Error: no se proporcionan los argumentos necesarios.\n");			
			return -1;
		}
		
		if(flag_o == 0){
			sobreescribir = 0;
		}
		
		if(flag_inc == 1){
			fprintf(stderr, "Error: no se proporcionan los argumentos adecuados.\n");			
			return -1;
		}
		
		if(misala_crea(ruta, capacidad, sobreescribir) == -1){
			fprintf(stderr, "Error: no se pudo crear la sala.\n");
			return -1;
		}
		
		free(ruta);
		return 0;
			
	
	}else if(strcmp(argv[1], "reserva") == 0){
	
	
		while ((caracter=getopt(argc, argv, "f:n:")) != -1){
			
			switch(caracter) {
				
				case 'f':
					flag_f = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -f\n");
						return -1;
					}
					ruta = strdup(optarg);
					break;
					
				case 'n':
                			flag_n = 1;
                			int id;
					num_asientos = atoi(optarg);
					if(num_asientos <= 0){
						fprintf(stderr, "Error: el número de asientos debe ser un número positivo\n");
						return -1;
					}
				
					id_personas = malloc((argc - optind) * sizeof(int));
					if(id_personas <= 0){
						fprintf(stderr, "Error: deben haber ID positivos.\n");
						free(id_personas);
						return -1;
					}
					for(int i = optind; i < argc; i++){
						id_personas[id] = atoi(argv[i]);
						id++;
					} 
					
					if(id != num_asientos){
						fprintf(stderr, "Error: el número de IDs de asientos no coincide con el especificado: %d\n", num_asientos);
						return -1;
					}
					
                			break;
					
				case '?':
					flag_inc = 1;
					printf("Opción inesperada: %c\n", optopt);
					break;
					
				default:
					fprintf(stderr, "Uso: ./misala %s -f ruta -n número_de_asientos id_persona1 id_persona2... \n" , argv[1]);
					break;
			}
		}
		
		if(flag_f == 0 || flag_n == 0){
			fprintf(stderr, "Error: no se proporcionan los argumentos necesarios.\n");			
			return -1;
		}
		
		if(flag_inc == 1){
			fprintf(stderr, "Error: no se proporcionan los argumentos adecuados.\n");			
			return -1;
		}
		
		if(misala_reserva(ruta, num_asientos, id_personas) == -1){
			fprintf(stderr, "Error: no se pudo reservar los asientos.\n");
			return -1;
		}
		
		free(ruta);
		free(id_personas);
		return 0;
	
	}else if(strcmp(argv[1], "anula") == 0){
		
		while ((caracter=getopt(argc, argv, "f:a:")) != -1){
			
			switch(caracter) {
				
				case 'f':
					flag_f = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -f\n");
						return -1;
					}
					ruta = strdup(optarg);
					break;
					
				case 'a':
    					flag_a = 1;
    					num_asientos = argc - optind+1;
    					if(num_asientos <= 0){
        					fprintf(stderr, "Error: debe especificar al menos un ID de asiento para anular\n");
        					return -1;
    					}

    					id_asientos = malloc(num_asientos * sizeof(int));
    					if(id_asientos == NULL){
        					fprintf(stderr, "Error: no se pudo asignar memoria\n");
        					free(id_asientos);
        					return -1;
    					}
    
    					id_asientos[0] = atoi(argv[optind -1]);
    					for(int i = optind; i < argc; i++){
        					id_asientos[i - optind+1] = atoi(argv[i]);
    					}
    					break;
					
				case '?':
					flag_inc = 1;
					printf("Opción inesperada: %c\n", optopt);
					break;
					
					
				default:
					fprintf(stderr, "Uso: ./misala %s -f ruta -a id_asiento1 [id_asiento2...] \n" , argv[1]);
					break;
			}
		}
		
		if(flag_f == 0 || flag_a == 0){
			fprintf(stderr, "Error: no se proporcionan los argumentos necesarios.\n");			
			return -1;
			
		}
		
		if(flag_inc == 1){
			fprintf(stderr, "Error: no se proporcionan los argumentos adecuados.\n");			
			return -1;
		}
		
		if(misala_anula(ruta, num_asientos, id_asientos) == -1){
			fprintf(stderr, "Error: no se pudo anular asientos.\n");
			return -1;
		}
	
		free(id_asientos);
		free(ruta);
		return 0;
	
	
	}else if(strcmp(argv[1], "estado") == 0){
	
	
		while ((caracter=getopt(argc, argv, "f:")) != -1){
			
			switch(caracter) {
				
				case 'f':
					flag_f = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -f.\n");
						return -1;
					}
					ruta = strdup(optarg);
					break;
					
				case '?':
					flag_inc = 1;
					printf("Opción inesperada: %c\n", optopt);
					break;
					
				default:
					fprintf(stderr, "Uso: ./misala %s -f ruta.\n", argv[1]);
					break;
			}
		}
		
		if(flag_f == 0){
			fprintf(stderr, "Error: no se proporcionan los argumentos necesarios.\n");			
			return -1;
		}
		
		if(flag_inc == 1){
			fprintf(stderr, "Error: no se proporcionan los argumentos adecuados.\n");			
			return -1;
		}
		
		if(misala_estado(ruta) == -1){
			fprintf(stderr, "Error: no se pudo saber el estado de la sala.\n");
			return -1;
		}
		
		free(ruta);
		return 0;
	
	
	}else if(strcmp(argv[1], "confirmaciones") == 0){
	
	
		while ((caracter=getopt(argc, argv, "f:a:")) != -1){
			
			switch(caracter) {
				
				case 'f':
					flag_f = 1;
					if (optarg == NULL){
						fprintf(stderr, "Error: argumento vacío para -f\n");
						return -1;
					}
					ruta = strdup(optarg);
					break;
					
				case 'a':
    					flag_a = 1;
    					num_asientos = argc - optind+1;
    					if(num_asientos <= 0){
        					fprintf(stderr, "Error: debe especificar al menos un ID de asiento para anular\n");
        					return -1;
    					}

    					id_asientos = malloc(num_asientos * sizeof(int));
    					if(id_asientos == NULL){
        					fprintf(stderr, "Error: no se pudo asignar memoria\n");
        					free(id_asientos);
        					return -1;
    					}
    
    					id_asientos[0] = atoi(argv[optind -1]);
    					for(int i = optind; i < argc; i++){
        					id_asientos[i - optind+1] = atoi(argv[i]);
    					}
    					break;
					
				case '?':
					flag_inc = 1;
					printf("Opción inesperada: %c\n", optopt);
					break;
					
					
				default:
					fprintf(stderr, "Uso: ./misala %s -f ruta -a id_asiento1 [id_asiento2...] \n" , argv[1]);
					break;
			}
		}
		
		if(flag_f == 0 || flag_a == 0){
			fprintf(stderr, "Error: no se proporcionan los argumentos necesarios.\n");			
			return -1;
			
		}
		
		if(flag_inc == 1){
			fprintf(stderr, "Error: no se proporcionan los argumentos adecuados.\n");			
			return -1;
		}
		
		if(misala_confirmaciones(ruta, num_asientos, id_asientos) == -1){
			fprintf(stderr, "Error: no se pudo confirmar los asientos.\n");
			return -1;
		}
	
		free(id_asientos);
		free(ruta);
		return 0;
		
	
	
	}else if (strcmp(argv[1], "elimina") == 0) {
		char* ruta = NULL;
		int flag_f = 0;
		char caracter;

		while ((caracter = getopt(argc, argv, "f:")) != -1) {
			switch (caracter) {
				case 'f':
					flag_f = 1;
					ruta = strdup(optarg);
					break;
				case '?':
					fprintf(stderr, "Opción desconocida: %c\n", optopt);
					return -1;
				default:
					fprintf(stderr, "Uso: %s elimina -f ruta\n", argv[0]);
					return -1;
			}
		}

		if (!flag_f || ruta == NULL) {
			fprintf(stderr, "Error: se requiere la ruta del archivo con la opción -f.\n");
			return -1;
		}

		if (elimina_sala_y_archivo(ruta) == -1) {
			fprintf(stderr, "Error al eliminar la sala y el archivo.\n");
			free(ruta);
			return -1;
		}

		free(ruta);
	}else if (strcmp(argv[1], "compara") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Uso: %s compara ruta1 ruta2\n", argv[0]);
			return -1;
		}
		char* ruta1 = argv[2];
		char* ruta2 = argv[3];

		int resultado = misala_compara(ruta1, ruta2);
		if (resultado == -1) {
			fprintf(stderr, "Error comparando los archivos de las salas.\n");
			return -1;
		} else if (resultado == 1) {
			printf("Las salas son diferentes.\n");
			return 1;
		} else {
			printf("Las salas son iguales.\n");
			return 0;
		}
	}else{
		fprintf(stderr, "Orden desconocida: %s\n", strerror(errno));
		exit(-1);
	}

	exit(0);
}
