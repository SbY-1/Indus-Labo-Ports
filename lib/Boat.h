#ifndef BATEAU_H
#define BATEAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "Ressources.h"
#include "Common.h"

void handler(int sig);
void print_boat(int index, char* msg);

#endif /* BATEAU_H */
