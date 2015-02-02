#ifndef GESTION_H
#define GESTION_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define PROP_FILE	"Config.cfg"

char* getProp(const char *fileName, const char *propName);

#endif /* GESTION_H */
