#ifndef COMMON_H
#define COMMON_H

#include "Ressources.h"

#define PROP_FILE	"../Config.cfg"

#define MUTEX_BOAT	"mutexBoat"
#define SHM_BOAT	"shmBoat"
#define SHM_ARR		"shmArr"
#define SHM_DEP		"shmDep"
#define SHM_DOCK	"shmDock"
#define SEM_PORT	"semPort"
#define SEM_DOCK	"semDock"
#define MUTEX_DEP	"mutexDep"
#define MUTEX_DOCK	"mutexDock"
#define MUTEX_ARR	"mutexArr"
#define MUTEX_SYNC	"mutexSync"
#define MUTEX_DOCK  "mutexDock"

typedef enum {SEA, ENTERS_PORT, DOCK, LEAVES_PORT} boat_p;
typedef enum {UNDEFINED, DOVER, CALAIS, DUNKERQUE} boat_d;

typedef struct Boat_t
{
	pid_t 			pid;
	int				index;
	boat_p			position;
	boat_d			direction;
	int 			state_changed;
	MessageQueue	mq1;
	MessageQueue 	mq2;
} Boat;

typedef struct Dock_t
{
	int index;
	int boat_index;
} Dock;

#endif /* COMMON_H */
