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
    printf("portmaster args %d\n", args);
    if (args != 3)
        printf("ERROR FROM INSERTION\n");
    else{
        //read charges - not 
        int id = atoi(argv[2]);
        printf("id %d\n", id);
        int *shm;
        if ( ( shm = shmat(id, 0, 0) ) == (int *) -1) { 
            perror("shmat");
            exit(1);
        }
        SHARED_MEMORY *share_mem = (SHARED_MEMORY *)shm;
        FILE * fp;
        fp = fopen ("public_ledger.txt", "a");
        port_master_server(share_mem, fp);
        fclose(fp);
        int err = shmdt((void *) shm);
	    if (err == -1) perror ("Detachment.");	
    }
	return 0;
}