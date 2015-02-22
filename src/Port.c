#include "Port.h"

int main(int argc, char** argv)
{
	Semaphore sem_port;
	Semaphore mutex_boat;
	Semaphore mutex_dep;
	Semaphore mutex_dock;
	Semaphore mutex_arr;
	Semaphore mutex_sync;

	Shm shm_dep;
	Shm shm_arr; 
	Shm shm_boat;
	Shm shm_dock;

	Boat boat;
	char* port_name = argv[1];
	char* msg = malloc(sizeof(msg));
	int cpt_arr = 0;
	int cpt_dep = 0;
	int stop = 0;
	int nb_boats = atoi(getProp(PROP_FILE, "nb_boats"));
 	int nb_docks = 0;
  	int i;

  	sscanf(argv[2], "%d", &nb_docks);

	// Création des processus fils
	create_processes(nb_docks, port_name);	

	// Initialisation des ressources
	init_ressources(&mutex_boat, &sem_port, &mutex_dep, &mutex_dock, &mutex_arr, &shm_dep, &shm_arr, &shm_dock, &shm_boat, port_name, nb_docks, nb_boats);

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
		printf("\tPort %s > En attente de bateau\n", port_name);
		wait_sem(sem_port);

		// Compteur de depart
		wait_sem(mutex_dep);
		memcpy(&cpt_dep, shm_dep.pShm, sizeof(int));
		if (cpt_dep > 0)
		{
			// Recherche du bateau
			wait_sem(mutex_boat);
			boat = get_actual_boat(LEAVES_PORT, port_name, nb_boats, shm_boat);
			signal_sem(mutex_boat);

			sprintf(msg, "Bateau %d sort", boat.index);
			print_boat(port_name, boat.index, msg);
			
			// Décrémente le compteur
			cpt_dep--;
			memcpy(shm_dep.pShm, &cpt_dep, sizeof(int));

			signal_sem(mutex_dep);

			// Autorise le bateau à sortir
			mutex_sync.oflag = 0;
			sprintf(mutex_sync.semname,"%s%d", MUTEX_SYNC, boat.index);
			open_sem(&mutex_sync);
			signal_sem(mutex_sync);
			close_sem(mutex_sync);
		}
		else
		{
			signal_sem(mutex_dep);

			// Compteur d'arrivée
			wait_sem(mutex_arr);
			memcpy(&cpt_arr, shm_arr.pShm, sizeof(int));
			if (cpt_arr > 0)
			{			
				// Décrémente le compteur
				cpt_arr--;
				memcpy(shm_arr.pShm, &cpt_arr, sizeof(int));

				signal_sem(mutex_arr);

				// Recherche du bateau
				wait_sem(mutex_boat);
				boat = get_actual_boat(ENTERS_PORT, port_name, nb_boats, shm_boat);
				signal_sem(mutex_boat);

				sprintf(msg, "Bateau %d entre", boat.index);
				print_boat(port_name, boat.index, msg);

				// TODO Reservation du quai
				int found = 0;
				wait_sem(mutex_dock);
				for (i = 0; i < nb_docks && !found; i++)
				{
					Dock tmpDock;
					memcpy(&tmpDock, shm_dock.pShm + (i * sizeof(Dock)), sizeof(Dock));
					//printf("Port %s > Bateau - %d Quai %d - %d\n", port_name, boat.index, tmpDock.index, tmpDock.boat_index);
					sprintf(msg, "Quai %d", tmpDock.index);
					print_boat(port_name, boat.index, msg);
					// Recherche du premier quai disponible
					if (tmpDock.boat_index == -1)
					{
						tmpDock.boat_index = boat.index;
						memcpy(shm_dock.pShm + (i * sizeof(Dock)), &tmpDock, sizeof(Dock));
						found = 1;
					}
				}
				signal_sem(mutex_dock);

				// Envoie d'un signal au bateau
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
	}

	return EXIT_SUCCESS;
}

void create_processes(int nb_docks, char* port_name)
{
	pid_t child_pid;
	int i;

	// Création des quais
	for (i = 0; i < nb_docks; i++)
	{
		if ((child_pid = fork()) < 0)
		{
			perror("fork failure");
			exit(1);
		}

		if (child_pid == 0)
		{
			char* p = malloc(sizeof(p));
			char* d = malloc(sizeof(d));
			sprintf(p, "%d", i);
			sprintf(d, "%d", nb_docks);
			execl("Dock", "DOCK", port_name, p, d, NULL);
		}
	}

	// Création de GenVehicle

	if ((child_pid = fork()) < 0)
	{
		perror("fork failure");
		exit(1);
	}

	if (child_pid == 0)
	{
		execl("GenVehicle", "GENVEHICLE", port_name, NULL);
	}
}

void init_ressources(Semaphore* mutex_boat, Semaphore* sem_port, Semaphore* mutex_dep, Semaphore* mutex_dock, Semaphore* mutex_arr, Shm* shm_dep, Shm* shm_arr, Shm* shm_dock, Shm* shm_boat, char* port_name, int nb_docks, int nb_boats)
{
	// MUTEX_BATEAU
	mutex_boat->oflag = O_RDWR;
    mutex_boat->mode  = 0644;
    mutex_boat->value = 1;
    strcpy(mutex_boat->semname, MUTEX_BOAT);

	// SEM_PORT
	sem_port->oflag = (O_CREAT | O_RDWR);
    sem_port->mode  = 0644;
    sem_port->value = 0;
    sprintf(sem_port->semname,"%s%s", SEM_PORT, port_name);

	// MUTEX_DEP
	mutex_dep->oflag = (O_CREAT | O_RDWR);
    mutex_dep->mode  = 0644;
    mutex_dep->value = 1;
    sprintf(mutex_dep->semname,"%s%s", MUTEX_DEP, port_name);	

	// MUTEX_DOCK
	mutex_dock->oflag = (O_CREAT | O_RDWR);
    mutex_dock->mode  = 0644;
    mutex_dock->value = 1;
    sprintf(mutex_dock->semname,"%s%s", MUTEX_DOCK, port_name);	

	// MUTEX_ARR
	mutex_arr->oflag = (O_CREAT | O_RDWR);
    mutex_arr->mode  = 0644;
    mutex_arr->value = 1;
    sprintf(mutex_arr->semname,"%s%s", MUTEX_ARR, port_name);	

	// SHM_DEP
	shm_dep->sizeofShm = sizeof(int);
	shm_dep->mode = O_CREAT | O_RDWR;
	sprintf(shm_dep->shmName,"%s%s", SHM_DEP, port_name);

	// SHM_ARR
	shm_arr->sizeofShm = sizeof(int);
	shm_arr->mode = O_CREAT | O_RDWR;
	sprintf(shm_arr->shmName,"%s%s", SHM_ARR, port_name);

	// SHM_DOCK
	shm_dock->sizeofShm = sizeof(Dock) * nb_docks;
	shm_dock->mode = O_CREAT | O_RDWR;
	sprintf(shm_dock->shmName,"%s%s", SHM_DOCK, port_name);

	shm_boat->sizeofShm = sizeof(Boat) * nb_boats;
	shm_boat->mode = O_RDWR;
	strcpy(shm_boat->shmName, SHM_BOAT);

	sem_unlink(sem_port->semname);
	sem_unlink(mutex_dep->semname);
	sem_unlink(mutex_dock->semname);
	sem_unlink(mutex_arr->semname);
	sem_unlink(mutex_dock->semname);

	open_sem(sem_port);
	open_sem(mutex_dep);
	open_sem(mutex_dock);
	open_sem(mutex_arr);
	open_sem(mutex_boat);
	open_sem(mutex_dock);

	open_shm(shm_boat);
	mapping_shm(shm_boat, sizeof(Boat) * nb_boats);

	open_shm(shm_dock);
	mapping_shm(shm_dock, sizeof(Dock) * nb_docks);

	open_shm(shm_dep);
	mapping_shm(shm_dep, sizeof(int));

	open_shm(shm_arr);
	mapping_shm(shm_arr, sizeof(int));
}

void print_boat(char* port_name, int boat_index, char* msg)
{
	char* color[] = {"\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m"};
	char* reset = "\033[0m";

	printf("\tPort %s > %s%s%s\n", port_name, color[boat_index], msg, reset);
}

Boat get_actual_boat(boat_p position, char* port, int nb_boats, Shm shm_boat)
{
	// Parcours de la shm pour trouver le bateau concerné
	int i;
	int found;
	char* ports_name[] = {"Douvre", "Calais", "Dunkerque"};
	Boat *tmp = malloc(sizeof(Boat));
	boat_d direction;

	// Recherche le nom du port pour l'enum
	for (i = 0, found = 0; i < 3 && !found; i++)
	{
		if (strcmp(port, ports_name[i]) == 0)
		{
			found = 1;
			direction = i + 1;
		}
	}

	// Recherche le bateau aux portes du port
	for (i = 0, found = 0; i < nb_boats && !found; i++)
	{
		memcpy(tmp, shm_boat.pShm + (i * sizeof(Boat)), sizeof(Boat));
		printf("Recherche: Bateau : %d - Port %d - Position %d\n", tmp->index, tmp->direction, tmp->position);
		if (tmp->position == position && tmp->direction == direction)
			found = 1;
	}	
	
	return *tmp;
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
