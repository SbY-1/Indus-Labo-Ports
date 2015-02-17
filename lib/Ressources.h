#ifndef RESSOURCES_H
#define RESSOURCES_H

#include <stdio.h>
#include <stdlib.h>

// Semaphore
void ouvertureSemaphore(Semaphore *sem);
void waitSem(Semaphore sem);
void signalSem(Semaphore sem);
void fermetureSemaphore(Semaphore sem);
void destructionSemaphore(Semaphore sem);

// Shm
void ouvertureMemoirePartagee(Shm *memPartagee);
void mappingMemoirePartagee(Shm *memPartagee, size_t size);
void fermetureMemoiePartagee(Shm memPartagee);

// Mq
void ouvertureMessageQueue(MessageQueue *mq, struct mq_attr attr);
void fermetureMessageQueue(MessageQueue mq);
void envoieMessage(mqd_t msgq,const char * message,int taille,unsigned int msg_prio);
int receptionMessage(mqd_t msgq,char * message,int taille,unsigned int *msg_prio);

typedef struct Shm_t
{
    int    	fdShm;
    caddr_t pShm;
	mode_t 	mode;
    char 	shmName[SHM_NAME_LENGTH];
    size_t 	sizeofShm;
} Shm;

typedef struct Semaphore_t
{
    sem_t*  p_sem;
    int     oflag;
    mode_t  mode;
    char    semname[SEM_NAME_LENGTH];
    unsigned int value;
} Semaphore;

typedef struct MessageQueue_t
{
    mqd_t	mq;
    int     oflag;
    mode_t  mode;
    char    name[MQ_NAME_LENGTH];
} MessageQueue;

#endif /* RESSOURCES_H */
