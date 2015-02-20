#ifndef PORT_H
#define PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "Common.h"
#include "Ressources.h"

Boat get_actual_boat(boat_p position, char* port, int nb_boats, Shm shm_boat);
char* getProp(const char *fileName, const char *propName);

#endif /* PORT_H */
