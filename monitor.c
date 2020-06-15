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

void print_statimes(SHARED_MEMORY *share_mem){
    sem_wait(&(share_mem->read_write));
    calc_average_waiting_time(share_mem);
    sem_post(&(share_mem->read_write));
    
}

int print_times(SHARED_MEMORY *share_mem){
    sem_wait(&(share_mem->read_write));
        int status = print_current_arrays(share_mem);
    sem_post(&(share_mem->read_write));
    return status;
}
int read_id_from_file(char *filename){
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    int id;
    fread(&id,sizeof(id),1, fp);
    fclose(fp);
    return id;
}

int main(int args, char **argv){
    if (args != 7){
        printf("ERROR IN INSERTION\n");
        exit(2);
    }
    else{
        int times = atoi(argv[2]);
        int statimes = atoi(argv[4]);
        int id = read_id_from_file(argv[6]);
        //printf("monitor %d %d %d\n", times, statimes, id);
        int *shm;
        if ( ( shm = shmat(id, 0, 0) ) == (int *) -1) { 
            perror("shmat");
            exit(1);
        }
        SHARED_MEMORY *share_mem = (SHARED_MEMORY *)shm;
        int whatisthevalue;
        int flag = 1;
        sem_wait(&(share_mem->monitor));    // wait to be enabled 0->1->0
        sem_post(&(share_mem->monitor));    // 0->1
        int count =0;
        while(flag){
                sleep(times); 
	            int status = print_times(share_mem);
                if (status > 0){
                    printf("\n");
                    count++;
                }
            int st = statimes - (count*times);
            if(st < times){
                if (st > 0)
                    sleep(st);
                print_statimes(share_mem);
                count =0;
            }
            sem_getvalue(&(share_mem->monitor), &whatisthevalue);
            if(whatisthevalue == 0){
                flag = 0;
            }
        }

        int err = shmdt((void *) shm);
	    if (err == -1) perror ("Detachment.");
    }
    return 0;
}