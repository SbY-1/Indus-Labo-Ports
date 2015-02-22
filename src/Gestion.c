#include "Gestion.h"

int main()
{
	Semaphore mutex_boat;
	Shm shm_boat;

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

	while(1)
		pause();

	return EXIT_SUCCESS;
}
