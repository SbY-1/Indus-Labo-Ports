#include "Gestion.h"

int main()
{
	int i;
	int stop = 0;
	int nb_ports = atoi(getProp(PROP_FILE, "nb_ports"));
	int nb_boats = atoi(getProp(PROP_FILE, "nb_boats"));
	char* ports_name[] = {"Douvre", "Calais", "Dunkerque"};
	pid_t child_pid;

	printf("Nb_boats : %d \n", nb_boats);
	printf("Nb_ports : %d \n", nb_ports);

	mutex_boat.oflag = (O_CREAT | O_RDWR);
    mutex_boat.mode  = 0600;
    mutex_boat.value = 1;
    strcpy(mutex_boat.semname, MUTEX_BOAT);

	sem_unlink(mutex_boat.semname);

	open_sem(&mutex_boat);

	shm_boat.sizeofShm = sizeof(Boat) * nb_boats;
	shm_boat.mode = O_CREAT | O_RDWR;
	strcpy(shm_boat.shmName, SHM_BOAT);

	open_shm(&shm_boat);
	mapping_shm(&shm_boat, sizeof(Boat) * nb_boats);

	/*for (i = 0; i < 10; i++)
	{
		wait_sem(mutex_boat);
		printf("Test de section\n");
		signal_sem(mutex_boat);
	}*/
	

	// Création des ressources nécessaires
	//init_ressources(nb_boats);

	// Création des bateaux
	for (i = 0; i < nb_boats; i++)
	{
		if ((child_pid = fork()) < 0)
		{
			perror("fork failure");
			exit(1);
		}

		if (child_pid == 0)
		{
			char* p = malloc(sizeof(p));
			sprintf(p, "%d", i);
			execl("Boat", "BOAT", p, NULL);
		}
	}

	
	// Création des ports
	for (i = 0; i < nb_ports; i++)
	{
		if ((child_pid = fork()) < 0)
		{
			perror("fork failure");
			exit(1);
		}

		if (child_pid == 0)
		{
		  char* p = malloc(sizeof(p));
		  sprintf(p, "%d", (i == 0) ? 3 : 2);
			execl("Port", "PORT", ports_name[i], p, NULL);
		}
	}

	// Lecture des données
	Boat tmpBoat;
	while (!stop)
	{
		for (i = 0; i < nb_boats; i++)
		{
			wait_sem(mutex_boat);
			memcpy(&tmpBoat, shm_boat.pShm + (i * sizeof(Boat)), sizeof(Boat));
			if (tmpBoat.state_changed == 1)
			{
				//printf("Boat %d - pid = %d - position : %d - direction %d - state %d\n", i, tmpBoat.pid, tmpBoat.position, tmpBoat.direction, tmpBoat.state_changed);

				tmpBoat.state_changed = 0;
				memcpy(shm_boat.pShm + (i * sizeof(Boat)), &tmpBoat, sizeof(Boat));
			}
			signal_sem(mutex_boat);
		}
	}

	return EXIT_SUCCESS;
}

void init_ressources(int nb_boats)
{
	// MUTEX_BATEAU
	/*mutex_boat.oflag = (O_CREAT | O_RDWR);
    mutex_boat.mode  = 0600;
    mutex_boat.value = 1;
    strcpy(mutex_boat.semname, MUTEX_BOAT);

	open_sem(&mutex_boat);

	// SHM_BATEAU
	shm_boat.sizeofShm = sizeof(Boat) * nb_boats;
	shm_boat.mode = O_CREAT | O_RDWR;
	strcpy(shm_boat.shmName, SHM_BOAT);

	open_shm(&shm_boat);
	mapping_shm(&shm_boat, sizeof(Boat) * nb_boats);*/
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
