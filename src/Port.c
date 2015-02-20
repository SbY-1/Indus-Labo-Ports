#include "Port.h"

int main(int argc, char** argv)
{
	int stop = 0;
	Semaphore sem_port;
	Semaphore mutex_boat;
	Semaphore mutex_dep;
	Semaphore mutex_dock;
	Semaphore mutex_arr;
	Semaphore mutex_sync;
	Shm shm_dep;
	Shm shm_arr; 
	Shm shm_boat;
	Boat boat;
	int cpt_arr = 0;
	int cpt_dep = 0;
	int nb_boats = atoi(getProp(PROP_FILE, "nb_boats"));

	// MUTEX_BATEAU
	mutex_boat.oflag = O_RDWR;
    mutex_boat.mode  = 0644;
    mutex_boat.value = 1;
    strcpy(mutex_boat.semname, MUTEX_BOAT);

	// SEM_PORT
	sem_port.oflag = (O_CREAT | O_RDWR);
    sem_port.mode  = 0644;
    sem_port.value = 0;
    sprintf(sem_port.semname,"%s%s", SEM_PORT, argv[1]);

	// MUTEX_DEP
	mutex_dep.oflag = (O_CREAT | O_RDWR);
    mutex_dep.mode  = 0644;
    mutex_dep.value = 1;
    sprintf(mutex_dep.semname,"%s%s", MUTEX_DEP, argv[1]);	

	// MUTEX_DOCK
	mutex_dock.oflag = (O_CREAT | O_RDWR);
    mutex_dock.mode  = 0644;
    mutex_dock.value = 1;
    sprintf(mutex_dock.semname,"%s%s", MUTEX_DOCK, argv[1]);	

	// MUTEX_ARR
	mutex_arr.oflag = (O_CREAT | O_RDWR);
    mutex_arr.mode  = 0644;
    mutex_arr.value = 1;
    sprintf(mutex_arr.semname,"%s%s", MUTEX_ARR, argv[1]);	

	// SHM_DEP
	shm_dep.sizeofShm = sizeof(int);
	shm_dep.mode = O_CREAT | O_RDWR;
	sprintf(shm_dep.shmName,"%s%s", SHM_DEP, argv[1]);

	// SHM_ARR
	shm_arr.sizeofShm = sizeof(int);
	shm_arr.mode = O_CREAT | O_RDWR;
	sprintf(shm_arr.shmName,"%s%s", SHM_ARR, argv[1]);

	shm_boat.sizeofShm = sizeof(Boat) * nb_boats;
	shm_boat.mode = O_RDWR;
	strcpy(shm_boat.shmName, SHM_BOAT);

	open_shm(&shm_boat);
	mapping_shm(&shm_boat, sizeof(Boat) * nb_boats);

	sem_unlink(sem_port.semname);
	sem_unlink(mutex_dep.semname);
	sem_unlink(mutex_dock.semname);
	sem_unlink(mutex_arr.semname);

	open_sem(&sem_port);
	open_sem(&mutex_dep);
	open_sem(&mutex_dock);
	open_sem(&mutex_arr);
	open_sem(&mutex_boat);

	open_shm(&shm_dep);
	mapping_shm(&shm_dep, sizeof(int));

	open_shm(&shm_arr);
	mapping_shm(&shm_arr, sizeof(int));

	// Mise a 0 des compteurs
	wait_sem(mutex_dep);
	memcpy(shm_dep.pShm, &cpt_dep, sizeof(int));
	signal_sem(mutex_dep);

	wait_sem(mutex_arr);
	memcpy(shm_arr.pShm, &cpt_arr, sizeof(int));
	signal_sem(mutex_arr);
	
	while (!stop)
	{
		// En attente de bateau
		printf("Port %s > En attente de bateau\n", argv[1]);
		wait_sem(sem_port);

		// Compteur de depart
		wait_sem(mutex_dep);
		memcpy(&cpt_dep, shm_dep.pShm, sizeof(int));
		if (cpt_dep > 0)
		{
			// Recherche du bateau
			wait_sem(mutex_boat);
			boat = get_actual_boat(LEAVES_PORT, argv[1], nb_boats, shm_boat);
			signal_sem(mutex_boat);

			printf("Port %s > Bateau %d sort\n", argv[1], boat.pid);
			
			// Décrémente le compteur
			cpt_dep--;
			memcpy(shm_dep.pShm, &cpt_dep, sizeof(int));

			signal_sem(mutex_dep);

			// TODO Envoie d'un signal au bateau
			//kill(boat.pid, SIGUSR2);
			mutex_sync.oflag = O_RDWR;
			mutex_sync.mode  = 0644;
			mutex_sync.value = 1;
			sprintf(mutex_sync.semname,"%s%d", MUTEX_SYNC, boat.index);

			open_sem(&mutex_sync);
			sleep(1);
			signal_sem(mutex_sync);
			close_sem(mutex_sync);
		}
		else
			signal_sem(mutex_dep);

		// TODO Reservation du quai
		//sleep(2);

		// Compteur d'arrivée
		wait_sem(mutex_arr);
		memcpy(&cpt_arr, shm_arr.pShm, sizeof(int));
		if (cpt_arr > 0)
		{
			// Recherche du bateau
			wait_sem(mutex_boat);
			boat = get_actual_boat(ENTERS_PORT, argv[1], nb_boats, shm_boat);
			signal_sem(mutex_boat);

			printf("Port %s > Bateau %d entre\n", argv[1], boat.pid);
			
			// Décrémente le compteur
			cpt_arr--;
			memcpy(shm_arr.pShm, &cpt_arr, sizeof(int));

			signal_sem(mutex_arr);

			// TODO Envoie d'un signal au bateau
			//kill(boat.pid, SIGUSR1);
			mutex_sync.oflag = O_RDWR;
			mutex_sync.mode  = 0644;
			mutex_sync.value = 1;
			sprintf(mutex_sync.semname,"%s%d", MUTEX_SYNC, boat.index);

			open_sem(&mutex_sync);
			sleep(1);
			signal_sem(mutex_sync);
			close_sem(mutex_sync);
		}
		else
			signal_sem(mutex_arr);

	}

	return EXIT_SUCCESS;
}

Boat get_actual_boat(boat_p position, char* port, int nb_boats, Shm shm_boat)
{
	// TODO Parcours de la shm pour trouver le batau concerné
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
			printf("Port %s > Nom port %d\n", port, direction);
		}
	}

	for (i = 0, found = 0; i < nb_boats && !found; i++)
	{
		memcpy(&tmp, shm_boat.pShm + (i * sizeof(Boat)), sizeof(Boat));
		if (tmp.position == position && tmp.direction == direction)
		{
			found = 1;
			printf("Port %s > Bateau trouve %d\n", port, tmp.pid);
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




