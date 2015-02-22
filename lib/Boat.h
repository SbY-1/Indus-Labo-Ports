#ifndef BATEAU_H
#define BATEAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "Ressources.h"
#include "Common.h"

void init_ressources(Semaphore* mutex_sync, Semaphore* mutex_boat, Shm* shm_boat, int index);
void open_port_ressources(Semaphore* sem_port, Semaphore* mutex_dep, Semaphore* mutex_arr, Shm* shm_dep, Shm* shm_arr, char* port_name);
void open_dock_ressources(Semaphore* mutex_dock, Shm* shm_dock, char* port_name, int nb_docks);
void handler(int sig);
void print_boat(int index, char* msg);

#endif /* BATEAU_H */
