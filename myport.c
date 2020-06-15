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

int read_configuration_file(Mooring_Area *mooring_area_info ,char *filename){
    int i = 0;
    char *InputLine = NULL;
    FILE *fp_in = NULL;
    size_t sz = 0;
    fp_in = fopen(filename,"r");
    if (fp_in == NULL){
        printf("ERROR IN OPENING\n");
        return 0;
    }
    while(getline(&InputLine, &sz, fp_in) != EOF){      //get the whole line from file
        sscanf(InputLine, "%c %d %lf",&(mooring_area_info[i].type), &(mooring_area_info[i].capacity), &(mooring_area_info[i].cost));    //break the line and store the information in the collumns
        i++;
    }
    free(InputLine);
    fclose(fp_in);
    return 1;
}
void print_mooring_area_info(Mooring_Area *mooring_area_info){
    for (int i=0; i<3; i++)
        printf("MOORING AREA INFO: \t %c, \t %d, \t %lf\n", mooring_area_info[i].type, mooring_area_info[i].capacity, mooring_area_info[i].cost);
}

void Init_array_of_types(Mooring_Area *array_of_types, char *type, int size_of_array, double cost){
    for (int i = 0; i < size_of_array; i++){
        memcpy(&(array_of_types[i]).type, type, sizeof(char));
        (array_of_types[i]).status = 0;
        (array_of_types[i]).cost = cost;
    }
}
void Print_array_of_types(Mooring_Area *array_of_types, int size_of_array){
    for (int i = 0; i < size_of_array; i++){
        printf("TYPE %d: %d %c and ID: %d\n",i, (array_of_types[i]).status, (array_of_types[i]).type, (array_of_types[i]).vessel.ID);
    }
}

int make_new_process(int key, SHARED_MEMORY *share_mem){
    pid_t pid;
    char p_key[100];
    sprintf(p_key, "%d", key);
    printf("key %d and p_key %s\n", key, p_key);
    if ((pid = fork()) == 0){
        execlp("./port_master", "-c", "charges.txt", p_key, (char *)NULL);
        printf("ERROR IN EXEC\n");
    }
    else{
        int ex = waitpid(pid,NULL,0);
    }
    return 1;
}
int write_id_to_file(long int id){
    FILE *fp = NULL;
    fp = fopen("shared_memory_id.csv","w");
    fwrite(&id, 1, sizeof(id), fp);
    fclose(fp);
}
//////////////////////////
long int read_id_from_file(char *filename){
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    long int id;
    fread(&id,sizeof(id), 1, fp);
    fclose(fp);
    return id;
}
////////////////////////////
int make_shared_memory(Mooring_Area *array_of_info){
    long int id;
    int sum_of_mooring_area = array_of_info[0].capacity + array_of_info[1].capacity + array_of_info[2].capacity;
    size_t SHARED_MEMORY_SIZE = 1*sizeof(SHARED_MEMORY) + sum_of_mooring_area*sizeof(Mooring_Area);
    if ((id = shmget(IPC_PRIVATE, SHARED_MEMORY_SIZE, 0666|IPC_CREAT)) == -1) {
       perror("Failed to create shared memory segment");
       return 1;
    }
    int *shm;
    if ( ( shm = shmat(id, 0, 0) ) == (int *) -1) { 
        perror("shmat");
        exit(1);
    }
    SHARED_MEMORY *share_mem = (SHARED_MEMORY *)shm;
    // Init semaphores
    Init_sems(&(share_mem->read_write), 1);
    Init_sems(&(share_mem->vessel_sem_M), 1);
    Init_sems(&(share_mem->vessel_sem_L), 1);
    Init_sems(&(share_mem->vessel_sem_S), 1);
    Init_sems(&(share_mem->vessel_sem_in), 0);
    Init_sems(&(share_mem->vessel_sem_out), 0);
    Init_sems(&(share_mem->portin_sem_M), 0);
    Init_sems(&(share_mem->portin_sem_L), 0);
    Init_sems(&(share_mem->portin_sem_S), 0);
    Init_sems(&(share_mem->port_sem_out), 1);
    Init_sems(&(share_mem->mutex1), 1);
    Init_sems(&(share_mem->mutex2), 0);
    Init_sems(&(share_mem->mutex3), 0);
    Init_sems(&(share_mem->monitor), 0);
    
    Init_sems(&(share_mem->read_sem), 1);
    Init_sems(&(share_mem->write_sem), 1);

    Init_sems(&(share_mem->Vessel_sem), 0);
    Init_sems(&(share_mem->ACCESS_Vessel_sem), 1);

    // Init sizes of arrays_if_Moorings_Areas
    share_mem->max_count_of_Vessels[0] = array_of_info[0].capacity;
    share_mem->max_count_of_Vessels[1] = array_of_info[1].capacity;
    share_mem->max_count_of_Vessels[2] = array_of_info[2].capacity;
    share_mem->waiting_vessels = 0;
    share_mem->parking_vessels = 0;
    share_mem->write_count = 0;
    share_mem->ports_total_income = 0;
    // Init waiting vessels and their priorities
    share_mem->vessel_in_S.ID = 0;
    share_mem->vessel_in_M.ID = 0;
    share_mem->vessel_in_L.ID = 0;
    share_mem->vessel_out.ID = 0;
    share_mem->s_priority = 0;
    share_mem->l_priority = 0;
    share_mem->m_priority = 0;

    (share_mem->public_ledger).seats_size[0] = array_of_info[2].capacity;       //MAX SMALL
    (share_mem->public_ledger).seats_size[1] = array_of_info[1].capacity;       //MAX MEDIUM
    (share_mem->public_ledger).seats_size[2] = array_of_info[0].capacity;       //MAX LARGE
    //Init arrays_of_Moorings_Areas
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    Init_array_of_types((share_mem->public_ledger).Small_Seats,"S",array_of_info[2].capacity, array_of_info[2].cost);
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + (array_of_info[2].capacity)*sizeof(Mooring_Area));
    Init_array_of_types((share_mem->public_ledger).Medium_Seats,"M",array_of_info[1].capacity, array_of_info[1].cost);   
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + (array_of_info[1].capacity)*sizeof(Mooring_Area));
    Init_array_of_types((share_mem->public_ledger).Large_Seats,"L",array_of_info[0].capacity, array_of_info[0].cost);
    
    // print arrays_of_Moorings_Areas
    // Print_array_of_types((share_mem->public_ledger).Small_Seats, array_of_info[2].capacity);    
    // Print_array_of_types((share_mem->public_ledger).Medium_Seats, array_of_info[1].capacity);   
    // Print_array_of_types((share_mem->public_ledger).Large_Seats, array_of_info[0].capacity);
    // print semaphores
    // Print_sem_value(&(share_mem->read_write));
    // Print_sem_value(&(share_mem->vessel_sem_M));
    // Print_sem_value(&(share_mem->vessel_sem_L));
    // Print_sem_value(&(share_mem->vessel_sem_S));
    // Print_sem_value(&(share_mem->vessel_sem_in));
    // Print_sem_value(&(share_mem->vessel_sem_out));
    // Print_sem_value(&(share_mem->portin_sem_M));
    // Print_sem_value(&(share_mem->portin_sem_L));
    // Print_sem_value(&(share_mem->portin_sem_S));
    // Print_sem_value(&(share_mem->port_sem_out));
    // Print_sem_value(&(share_mem->mutex1));
    // Print_sem_value(&(share_mem->mutex2));
    // Print_sem_value(&(share_mem->mutex3));
    // write shared memory id to file
    write_id_to_file(id);
    make_new_process(id, share_mem);
    /* Remove Semaphores .*/
    sem_destroy(&(share_mem->read_write));
    sem_destroy(&(share_mem->vessel_sem_M));
    sem_destroy(&(share_mem->vessel_sem_L));
    sem_destroy(&(share_mem->vessel_sem_S));
    sem_destroy(&(share_mem->vessel_sem_in));
    sem_destroy(&(share_mem->vessel_sem_out));
    sem_destroy(&(share_mem->portin_sem_M));
    sem_destroy(&(share_mem->portin_sem_L));
    sem_destroy(&(share_mem->portin_sem_S));
    sem_destroy(&(share_mem->port_sem_out));
    sem_destroy(&(share_mem->mutex1));
    sem_destroy(&(share_mem->mutex2));
    sem_destroy(&(share_mem->mutex3));
    sem_destroy(&(share_mem->monitor));

    sem_destroy(&(share_mem->Vessel_sem));
    sem_destroy(&(share_mem->ACCESS_Vessel_sem));
    sem_destroy(&(share_mem->read_sem));
    sem_destroy(&(share_mem->write_sem));
    /* Remove segment . */
    int err = shmctl ( id , IPC_RMID , 0) ;
    if ( err == -1) perror ( " Removal . " ) ;
    else printf ( " Removed . %d\n " , err ) ;

}

int main(int args, char **argv){
    if (args != 3){
        printf("ERROR IN INSERTION\n");
        exit(2);
    }
    Mooring_Area *mooring_area_info = malloc(3*sizeof(Mooring_Area));
    if (compare_strings(argv[1], "-l"))
        if(read_configuration_file(mooring_area_info,argv[2])){
            print_mooring_area_info(mooring_area_info);
            printf("READ IS COMPLETED\n"); 
        }
    make_shared_memory(mooring_area_info);
    free(mooring_area_info);
}