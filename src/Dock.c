#include "Dock.h"

int main(int argc, char** argv)
{
	Semaphore sem_dock;	
	Semaphore mutex_dock;
	Shm shm_dock;
	int dock_index;
	int nb_docks;
	int stop = 0;
	char* port_name = argv[1];

	sscanf(argv[2], "%d", &dock_index);
	sscanf(argv[3], "%d", &nb_docks);

	init_ressources(&sem_dock, &mutex_dock, &shm_dock, port_name, dock_index, nb_docks);

	// Mise a zero des info de la Shm
	Dock dock;
	dock.index = dock_index;
	dock.boat_index = -1;
	wait_sem(mutex_dock);
	memcpy(shm_dock.pShm + (dock_index * sizeof(Dock)), &dock, sizeof(Dock));
	signal_sem(mutex_dock);

	while (!stop)
	{
		// Attente d'un bateau
		printf("Quai %s %d > Attente d'un bateau\n", port_name, dock_index);
		wait_sem(sem_dock);

		wait_sem(mutex_dock);
		memcpy(&dock, shm_dock.pShm + (dock_index * sizeof(Dock)), sizeof(Dock));
		signal_sem(mutex_dock);

		printf("Quai %s %d > Bateau %d a quai \n", port_name, dock_index, dock.boat_index);

	}
	return 0;
}

void init_ressources(Semaphore* sem_dock, Semaphore* mutex_dock, Shm* shm_dock, char* port_name, int dock_index, int nb_docks)
{
	// SEM_DOCK
	sem_dock->oflag = (O_CREAT | O_RDWR);
    sem_dock->mode  = 0644;
    sem_dock->value = 0;
    sprintf(sem_dock->semname,"%s%s%d", SEM_DOCK, port_name, dock_index);

	// MUTEX_DOCK
	mutex_dock->oflag = O_RDWR;
    mutex_dock->mode  = 0644;
    mutex_dock->value = 1;
    sprintf(mutex_dock->semname,"%s%s", MUTEX_DOCK, port_name);

	// SHM_DOCK
	shm_dock->sizeofShm = sizeof(Dock) * nb_docks;
	shm_dock->mode = O_RDWR;
	sprintf(shm_dock->shmName,"%s%s", SHM_DOCK, port_name);

	sem_unlink(sem_dock->semname);

	open_sem(sem_dock);
	open_sem(mutex_dock);

	open_shm(shm_dock);
	mapping_shm(shm_dock, sizeof(Dock) * nb_docks);
}
