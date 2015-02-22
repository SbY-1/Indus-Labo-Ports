#ifndef GENVEHICLE_H
#define GENVEHICLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Ressources.h"
#include "Common.h"

#define TRUCK_PRIORITY 2
#define CAR_PRIORITY 1
#define VAN_PRIORITY 2

#define MAX_N_TRUCKS 5
#define MAX_N_CARS 10
#define MAX_N_VANS 15

Boat get_actual_boat(boat_p position, char* port, int nb_boats, Shm shm_boat);
char* getProp(const char *fileName, const char *propName);

#endif /* GENVEHICLE_H */
