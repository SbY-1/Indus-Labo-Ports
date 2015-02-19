#include "Boat.h"

int main(int argc, char* argv[])
{
	Semaphore mutex_boat;
	Shm shm_boat;
	int index;
	sscanf(argv[1], "%d", &index);
	int stop = 0;
	Boat boat;

	srand(getpid());

	// MUTEX_BATEAU
	mutex_boat.oflag = O_RDWR;
    mutex_boat.mode  = 0644;
    mutex_boat.value = 1;
    strcpy(mutex_boat.semname, MUTEX_BOAT);

	open_sem(&mutex_boat);

	shm_boat.sizeofShm = sizeof(Boat) * 6;
	shm_boat.mode = O_RDWR;
	strcpy(shm_boat.shmName, SHM_BOAT);

	open_shm(&shm_boat);
	mapping_shm(&shm_boat, sizeof(Boat) * 6);

	// Placement de l'état par défaut du bateau
	boat.pid = getpid();
	boat.position = SEA;
	boat.direction = UNDEFINED;
	boat.state_changed = 0;
	
	wait_sem(mutex_boat);
	memcpy(shm_boat.pShm + (index * sizeof(Boat)), &boat, sizeof(Boat));
	signal_sem(mutex_boat);
	
	while (!stop)
	{
		// Lecture de l'état du bateau
		wait_sem(mutex_boat);
		memcpy(&boat, shm_boat.pShm + (index * sizeof(Boat)), sizeof(Boat));
		signal_sem(mutex_boat);				

		switch(boat.position)
		{
			case SEA:
				// Premier voyage 
				if (boat.direction == UNDEFINED)
					boat.direction = rand() % 3 + 1;
				// Les bateaux viennent d'un port
				else
				{
					if (boat.direction == CALAIS || boat.direction == DUNKERQUE)
						boat.direction = DOVER;
					else
						boat.direction =  rand() % (3 - 2 + 1) + 2;
				}

				// Simulation de la traversée
				int duration = rand() % (30 - 15 + 1) + 15;
				printf("Traversée de bateau %d pendant %d seconde\n", index, duration);
				sleep(duration);

				boat.state_changed = 1;
				boat.position = ENTERS_PORT;
				break;
			case ENTERS_PORT:
				printf("Entree dans le port du bateau%d\n", index);
				pause();
				boat.position = DOCK;
				break;
			case DOCK:
				boat.position = LEAVES_PORT;
				break;
			case LEAVES_PORT:
				boat.position = SEA;
				break;
		}

		// Copie de la structure
		wait_sem(mutex_boat);
		memcpy(shm_boat.pShm + (index * sizeof(Boat)), &boat, sizeof(Boat));
		signal_sem(mutex_boat);
	}
	return EXIT_SUCCESS;
}
