#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "structures.h"

int main(int args, char **argv){
    Vessel_info vessel;
    printf("New Vessel with pid %d and %d\n", getpid(), args);
    if(args != 10){
        printf("ERROR IN INSERTION\n");
        exit(2);
    }
    else{
        time_t arrival_time;

        int id = atoi(argv[9]);
        double parkperiod = atof(argv[5]);
        double mantime = atof(argv[7]);
        int ID = getpid();
        Init_Vessel_info(&vessel, ID, argv[1], argv[3], parkperiod, mantime, id);
        Print_Vessel_info(&vessel);

        int *shm;
        if ( ( shm = shmat(id, 0, 0) ) == (int *) -1) { 
            perror("shmat");
            exit(1);
        }
        SHARED_MEMORY *share_mem = (SHARED_MEMORY *)shm;
        sem_wait(&(share_mem->read_write));
        share_mem->waiting_vessels +=1;
        sem_post(&(share_mem->read_write));
        arrival_time = time(NULL);
        printf("Vessel %d tries to access first place's queue\n", vessel.ID);
        if (vessel.type == 'S'){
            sem_wait(&(share_mem->vessel_sem_S));
        }
        else if (vessel.type == 'M'){
            sem_wait(&(share_mem->vessel_sem_M));
        }
        else if (vessel.type == 'L'){
            sem_wait(&(share_mem->vessel_sem_L));
        }
        sem_wait(&(share_mem->read_write));     
        printf("Vessel %d will write on first place of queue\n", vessel.ID);
        if(vessel.type == 'L'){
            memcpy(&(share_mem->vessel_in_L), &vessel, sizeof(Vessel_info));
            share_mem->l_priority = ID;
            share_mem->arrival_time[0] = arrival_time;
        }
        else if(vessel.type == 'M'){
            memcpy(&(share_mem->vessel_in_M), &vessel, sizeof(Vessel_info));
            share_mem->m_priority = ID;
            share_mem->arrival_time[1] = arrival_time;
        }
        else if(vessel.type == 'S'){
            memcpy(&(share_mem->vessel_in_S), &vessel, sizeof(Vessel_info));
            share_mem->s_priority = ID;
            share_mem->arrival_time[2] = arrival_time;
        }
        sem_post(&(share_mem->read_write));
        printf("Vessel %d send message to Port\n", vessel.ID);
        sem_post(&(share_mem->vessel_sem_in));
        printf("Vessel %d tries to access road for port\n", vessel.ID);
        if(vessel.type == 'L')
            sem_wait(&(share_mem->portin_sem_L));
        else if(vessel.type == 'M')
            sem_wait(&(share_mem->portin_sem_M));
        else if(vessel.type == 'S')
            sem_wait(&(share_mem->portin_sem_S));
        //printf("Vessel %d unlock EMPTY1 for port master\n", vessel.ID);
        sem_post(&(share_mem->mutex2));
        printf("Vessel %d start mantime\n", vessel.ID);
        sleep((vessel.mantime)/10);
        printf("Vessel %d finish mantime and unlock port road\n", vessel.ID);
        sem_post(&(share_mem->port_sem_out));
        printf("Vessel %d start parking\n", vessel.ID);
        sleep(vessel.parkperiod/20);
        double cost = calc_vessel_cost_until_here(share_mem, &vessel, parkperiod/2);
        printf("Vessel %d wants its cost until here %lf\n", vessel.ID, cost);
        printf("Vessel %d continue parking\n", vessel.ID);
        sleep(vessel.parkperiod/20);
        //printf("Vessel %d finish parking and lock MUTEX\n", vessel.ID);
        sem_wait(&(share_mem->mutex1));
        sem_wait(&(share_mem->read_write));
        memcpy(&(share_mem->vessel_out), &vessel, sizeof(Vessel_info));
        sem_post(&(share_mem->read_write));
        printf("Vessel %d give message to port for leaving parking\n", vessel.ID);
        sem_post(&(share_mem->vessel_sem_out));
        //printf("Vessel %d wait for EMPTY2 for leaving the port\n", vessel.ID);
        printf("Vessel %d wait for leaving the port\n", vessel.ID);
        sem_wait(&(share_mem->mutex3));
        printf("Vessel %d cost for whole parkperiod %lf\n", vessel.ID, share_mem->vessel_sum_cost);
        printf("Vessel %d start mantime for leaving\n", vessel.ID);
        sleep(vessel.mantime/10);
        printf("Vessel %d finish mantime for leaving and unlock port road\n", vessel.ID);
        sem_post(&(share_mem->port_sem_out));

        int err = shmdt((void *) shm);
	    if (err == -1) perror ("Detachment.");
    
    }
}