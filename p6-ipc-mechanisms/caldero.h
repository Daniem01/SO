#ifndef CALDERO_H
#define CALDERO_H

// Constantes numéricas 
#define M 10
#define NUMITER 3

// Nombres de los recursos
#define SEM_FILL "/sem_llenado"
#define SEM_EMPTY "/sem_vacio"
#define SEM_MUTEX "/sem_mutex"
#define SHM_NAME "/shm_caldero"

// Estructura compartida
struct memoria_compartida {
    int raciones;
};

#endif 