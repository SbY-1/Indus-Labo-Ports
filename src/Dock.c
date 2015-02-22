#include "Dock.h"

int main(int argc, char** argv)
{
	Semaphore sem_dock;
	Semaphore mutex_dock;
	Semaphore sem_gen_v;
	Shm shm_dock;
	struct mq_attr attr1;
	struct mq_attr attr2;
	mqd_t mqd_trucks;
	mqd_t mqd_cars_vans;
	char mq1_name[MQ_NAME_LENGTH];
	char mq2_name[MQ_NAME_LENGTH];
	char* port_name = argv[1];
	char* msg = malloc(sizeof(msg));
	int dock_index;
	int nb_docks;
	int stop = 0;
	int num_read = 0;
	void *buffer;

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
		printf("\t\t Quai %s %d > En attente %s\n", port_name, dock_index, sem_dock.semname);
		wait_sem(sem_dock);

		wait_sem(mutex_dock);
		memcpy(&dock, shm_dock.pShm + (dock_index * sizeof(Dock)), sizeof(Dock));
		signal_sem(mutex_dock);

		//printf("Quai %s %d > Bateau %d a quai \n", port_name, dock_index, dock.boat_index);
		sprintf(msg, "Bateau %d a quai", dock.boat_index);
		print_boat(port_name, dock_index, dock.boat_index, msg);
		// Ouverture MQ --- TODO: refactor with open_mq, ...
		sprintf(mq1_name, "%s%d", MQ_TRUCKS, dock.boat_index);
		sprintf(mq2_name, "%s%d", MQ_CARS_VANS, dock.boat_index);
		mqd_trucks = mq_open(mq1_name, O_RDONLY | O_NONBLOCK);
		mqd_cars_vans = mq_open(mq2_name, O_RDONLY | O_NONBLOCK);
		
		if (mqd_trucks == (mqd_t) -1 || mqd_cars_vans == (mqd_t) -1)
		{
			perror("Error when opening MQs (trucks, cars, vans)");
		}
		if (mq_getattr(mqd_trucks, &attr1) == -1 || mq_getattr(mqd_cars_vans, &attr2) == -1)
		{
			perror("Erreur when mq_getattr\n");
		}
		buffer = malloc(attr1.mq_msgsize);
		printf("[DEBARQUEMENT]\n");
		printf("CURMSGS TRUCKS: %ld\n", attr1.mq_curmsgs);
		if(attr1.mq_curmsgs > 0)
		{
			while(num_read != -1)
			{
				num_read = mq_receive(mqd_trucks, buffer, attr1.mq_msgsize, NULL);
				printf("Sortie de %s\n", (char *)buffer);
				nanosleep((struct timespec[]){{0, 250000000}}, NULL);
			}
		}
		num_read = 0;
		printf("CURMSGS CARS & VANS: %ld\n", attr2.mq_curmsgs);
		if(attr2.mq_curmsgs > 0)
		{
			while(num_read != -1)
			{
				num_read = mq_receive(mqd_cars_vans, buffer, attr2.mq_msgsize, NULL);
				printf("Sortie de %s\n", (char *)buffer);
				nanosleep((struct timespec[]){{0, 250000000}}, NULL);
			}
		}
		sem_gen_v.oflag = 0;
		sprintf(sem_gen_v.semname,"%s%s", SEM_GEN_V, argv[1]);
		open_sem(&sem_gen_v);
		signal_sem(sem_gen_v);
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

void print_boat(char* port_name, int dock_index, int boat_index, char* msg)
{
	char* color[] = {"\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m"};
	char* reset = "\033[0m";

	printf("\t\tQuai %s %d > %s%s%s\n", port_name, dock_index, color[boat_index], msg, reset);
}
