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

typedef int FLAG;
// int ENTER = 1;
// int EXIT = -1;

typedef struct{
    int ID;
    char type;
    char postype;
    double parkperiod;
    double mantime;
    int sh_mem_id;
} Vessel_info;

typedef struct{
    int status;
    char type;
    int capacity;
    double cost;

    Vessel_info vessel;
    double arrival_time;
    double service_time;
    double departure_time;
} Mooring_Area;

typedef struct{
    int seats_size[3];
    Mooring_Area *Small_Seats;
    Mooring_Area *Medium_Seats;
    Mooring_Area *Large_Seats;

} Public_Ledger;

typedef struct{
    Vessel_info vessel;
    int priority;
    double arrival_time;
} Waiting_Vessel;

typedef struct{
    sem_t read_write;
    sem_t read_sem;
    sem_t write_sem;
    sem_t vessel_sem_in;
    sem_t vessel_sem_out;
    sem_t vessel_sem_L;
    sem_t vessel_sem_M;
    sem_t vessel_sem_S;
    sem_t portin_sem_L;
    sem_t portin_sem_M;
    sem_t portin_sem_S;
    sem_t port_sem_out;
    sem_t mutex1;
    sem_t mutex2;
    sem_t mutex3;
    sem_t monitor;
    sem_t Vessel_sem;
    sem_t ACCESS_Vessel_sem;

    int write_count;
    int max_count_of_Vessels[3];
    int waiting_vessels;
    int parking_vessels;
    Vessel_info vessel_in_S, vessel_in_M, vessel_in_L;
    double arrival_time[3];
    int s_priority, m_priority, l_priority;
    Vessel_info vessel_out;
    double vessel_sum_cost;
    double ports_total_income;
    Public_Ledger public_ledger;

} SHARED_MEMORY;


void Init_Vessel_info(Vessel_info *vessel,int ID, char *type, char *postype, double parkperiod, double mantime, int sh_mem_id);
int compare_strings(char *name1, char *name2);
void Print_Vessel_info(Vessel_info *vessel);
void Init_sems(sem_t *sem, int value);
void Print_sem_value(sem_t *sem);
void erase_the_stored(SHARED_MEMORY *shm, Vessel_info vessel);
int find_vessel_with_ID(SHARED_MEMORY *share_mem, int ID, int *flag);
void print_arrays(SHARED_MEMORY *share_mem);
int print_current_arrays(SHARED_MEMORY *share_mem);
void enable_disable_monitor(SHARED_MEMORY *share_mem, int *monitor_key);

void write_public_ledger_history(FILE *fp, SHARED_MEMORY *share_mem, Mooring_Area *seat, int seat_number);
void calc_vessels_cost(SHARED_MEMORY *share_mem, double cost, double parkperiod);
void calc_average_waiting_time(SHARED_MEMORY *share_mem);
double calc_vessel_cost_until_here(SHARED_MEMORY *share_mem, Vessel_info *vessel, double parkperiod_2);

int read_waiting_vessels(SHARED_MEMORY *sp, Waiting_Vessel *array_of_waiting_vessels);
void swap(Waiting_Vessel *xp, Waiting_Vessel *yp);
void sorting_the_array(Waiting_Vessel *array_of_waiting_vessels, int sz);
int find_the_empty_seat(Mooring_Area *array_of_seats, int sz);
void store_vessel_to_seat(SHARED_MEMORY *share_mem, Vessel_info vessel, int flag, double arrival_time, double service_time);
int check_empty_seat(SHARED_MEMORY *shm, Vessel_info vessel, int *flag);
int port_master_server(SHARED_MEMORY *share_mem, FILE *fp);




