#include "GenVehicle.h"

// argv[1] = port name
int main(int argc, char** argv)
{
	Semaphore sem_gen_v;
	Semaphore mutex_boat;
	Semaphore mutex_sync;
	Shm shm_boat;
	Boat boat;
	mqd_t mqd_trucks;
	mqd_t mqd_cars_vans;

	char buffer[MQ_MSGSIZE];
	int i = 0;
	int nb_trucks = 0, nb_cars = 0, nb_vans = 0;
	int nb_boats = atoi(getProp(PROP_FILE, "nb_boats"));

	srand(getpid());

	// SEM_GEN_V
	sem_gen_v.oflag = (O_CREAT | O_RDWR);
	sem_gen_v.mode  = 0644;
	sem_gen_v.value = 0;
	sprintf(sem_gen_v.semname,"%s%s", SEM_GEN_V, argv[1]);
	open_sem(&sem_gen_v);

	// Preparing mutex for shm_boat access
	mutex_boat.oflag = O_RDWR;
	mutex_boat.mode  = 0644;
	mutex_boat.value = 1;
	strcpy(mutex_boat.semname, MUTEX_BOAT);
	open_sem(&mutex_boat);

	// Preparing shm_boat access
	shm_boat.sizeofShm = sizeof(Boat) * 6;
	shm_boat.mode = O_RDWR;
	strcpy(shm_boat.shmName, SHM_BOAT);
	open_shm(&shm_boat);
	mapping_shm(&shm_boat, sizeof(Boat) * 6);

	while(1)
	{
		// Waiting signal_sem on sem_gen_v from Docks processes.
		wait_sem(sem_gen_v);
		printf("---> GEN VEHICLE FOR %s UNLOCKED\n", argv[1]);
		// Waiting for access on shm_boat
		wait_sem(mutex_boat);
		boat = get_actual_boat(DOCK, argv[1], nb_boats, shm_boat);
		signal_sem(mutex_boat);

		// MUTEX_SYNC
		mutex_sync.oflag = 0;
		sprintf(mutex_sync.semname,"%s%d", MUTEX_SYNC, boat.index);
		open_sem(&mutex_sync);

		// Ouverture MQs
		mqd_trucks = mq_open(boat.mq1.name, O_WRONLY);
		mqd_cars_vans = mq_open(boat.mq2.name, O_WRONLY);

		nb_cars = rand() % MAX_N_CARS + 1;
		nb_vans = rand() % MAX_N_VANS + 1;
		nb_trucks = rand()% MAX_N_TRUCKS + 1;

		memset(buffer, 0, MQ_MSGSIZE);
		printf("[BEGINNING BOARDING] > Boat %d\n", boat.index);
		for(i = 0; i < nb_cars; i++)
		{
			sprintf(buffer, "Car %d", i + 1);
			if(mq_send(mqd_cars_vans, buffer, strlen(buffer), CAR_PRIORITY) == -1)
			{
				mq_close(mqd_cars_vans);
				mq_unlink(boat.mq1.name);
				perror("Error occured when mq_send (cars & vans)\n");
				exit(EXIT_FAILURE);
			}
			printf("%s on board\n", buffer);
			// Sleep 1/4s -- TODO Paramétrable.
			nanosleep((struct timespec[]){{0, 250000000}}, NULL);
		}
		printf("\t%d cars entered the boat %d.\n", nb_cars, boat.index);
		for(i = 0; i < nb_vans; i++)
		{
			sprintf(buffer, "Van %d", i);
			if(mq_send(mqd_cars_vans, buffer, strlen(buffer), VAN_PRIORITY) == -1)
			{
				mq_close(mqd_cars_vans);
				mq_unlink(boat.mq1.name);
				perror("Error occured when mq_send (cars & vans)\n");
				exit(EXIT_FAILURE);
			}
			printf("%s on board\n", buffer);
			// Sleep 1/4s
			nanosleep((struct timespec[]){{0, 250000000}}, NULL);
		}
		printf("\t%d vans entered the boat %d.\n", nb_cars, boat.index);
		for(i = 0; i < nb_trucks; i++)
		{
			sprintf(buffer, "Truck %d", i + 1);
			if(mq_send(mqd_trucks, buffer, strlen(buffer), TRUCK_PRIORITY) == -1)
			{
				mq_close(mqd_trucks);
				mq_unlink(boat.mq2.name);
				perror("Error occured when mq_send (trucks)\n");
				exit(EXIT_FAILURE);
			}
			printf("%s on board\n", buffer);
			nanosleep((struct timespec[]){{0, 250000000}}, NULL);
		}
		printf("\t%d trucks entered the boat %d.\n", nb_trucks, boat.index);
		printf("[ENDING BOARDING] for Boat [%d]", boat.index);
		// Récupération de la mutex_sync
		mutex_sync.oflag = 0;
		sprintf(mutex_sync.semname,"%s%d", MUTEX_SYNC, boat.index);
		// Signal le bateau qu'il peut y aller
		signal_sem(mutex_sync);
	}

	return 0;
}

// NB : Dupliquées de Port.c
Boat get_actual_boat(boat_p position, char* port, int nb_boats, Shm shm_boat)
{
	int i;
	int found;
	char* ports_name[] = {"Douvre", "Calais", "Dunkerque"};
	Boat tmp;
	boat_d direction;

	for (i = 0, found = 0; i < 3 && !found; i++)
	{
		if (strcmp(port, ports_name[i]) == 0)
		{
			found = 1;
			direction = i + 1;
			//printf("Port %s > Nom port %d\n", port, direction);
		}
	}

	for (i = 0, found = 0; i < nb_boats && !found; i++)
	{
		memcpy(&tmp, shm_boat.pShm + (i * sizeof(Boat)), sizeof(Boat));
		if (tmp.position == position && tmp.direction == direction)
		{
			found = 1;
			printf("GenVehicle %s > Bateau trouve %d\n", port, tmp.pid);
		}
	}	
	
	return tmp;
}

char* getProp(const char *fileName, const char *propName)
{
	FILE* 	file = NULL;
	char* 	token = NULL;
	char 	line[128];
	char	sep[2] = "=";
	int 	i;
	int 	loginFound = 0;

	if ((file = fopen(fileName, "r")) == NULL)
	{
		perror("Opening file\n");
		exit(errno);
	}
	else
	{
		while (fgets(line, sizeof line, file) != NULL)
		{
			token = strtok(line, sep);
			i = 0;

			while(token != NULL) 
			{
				if (i == 0)
				{
					if (strcmp(token, propName) == 0)
					loginFound++;
				}
				else if (i != 0 && loginFound == 1)
				{
					char *password = malloc(sizeof(char *) * 30);
					strcpy(password, token);
					fclose(file);
					return password;
				}
				token = strtok(NULL, sep);
				i++;
			}
		}
	}

	fclose(file);
	return NULL;
}
