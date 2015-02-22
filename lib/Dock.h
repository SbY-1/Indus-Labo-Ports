#ifndef DOCK_H
#define DOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "Ressources.h"
#include "Common.h"

void init_ressources(Semaphore* sem_dock, Semaphore* mutex_dock, Shm* shm_dock, char* port_name, int dock_index, int nb_docks);
void print_boat(char* port_name, int dock_index, int boat_index, char* msg);

#endif /* DOCK_H */
