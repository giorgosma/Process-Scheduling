#include "structures.h"

void Init_Vessel_info(Vessel_info *vessel,int ID, char *type, char *postype, double parkperiod, double mantime, int sh_mem_id){
    vessel->ID = ID;
    vessel->parkperiod = parkperiod;
    vessel->mantime = mantime;
    vessel->sh_mem_id = sh_mem_id;
    memcpy(&(vessel->type), type, sizeof(char));
    memcpy(&(vessel->postype), postype, sizeof(char));
}

void Print_Vessel_info(Vessel_info *vessel){
    printf("%d %c %c %f %f %d\n", vessel->ID, vessel->type, vessel->postype, vessel->parkperiod, vessel->mantime, vessel->sh_mem_id);
}

int compare_strings(char *name1, char *name2){
    int ret = strcmp(name1, name2);
    if (ret == 0)
        return 1;
    return 0;
}

void Init_sems(sem_t *sem, int value){
    int retval;
    if ( (retval = sem_init (sem, 1, value)) != 0) {
        perror ( " Couldn ’t initialize . " ) ;
        exit (3) ;
    }
}
void Print_sem_value(sem_t *sem){
    int whatisthevalue;
    sem_getvalue(sem, &whatisthevalue);
        printf("Value of Semaphore is %d\n",whatisthevalue);
}

void erase_the_stored(SHARED_MEMORY *shm, Vessel_info vessel){
    if (vessel.type == 'S'){
        (shm->vessel_in_S).ID = 0;
    }
    else if (vessel.type == 'M'){
        (shm->vessel_in_M).ID = 0;
    }
    else if (vessel.type == 'L'){
        (shm->vessel_in_L).ID = 0;
    }
}
int find_vessel_with_ID(SHARED_MEMORY *share_mem, int ID, int *flag){
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
    
    for(int i = 0; i < (share_mem->public_ledger).seats_size[0]; i++){
        if (((share_mem->public_ledger).Small_Seats[i]).vessel.ID == ID){
            *flag = 0;
            return i;
        }
    }
    for(int i = 0; i < (share_mem->public_ledger).seats_size[1]; i++){
        if (((share_mem->public_ledger).Medium_Seats[i]).vessel.ID == ID){
            *flag = 1;
            return i;
        }
    }
    for(int i = 0; i < (share_mem->public_ledger).seats_size[2]; i++){
        if (((share_mem->public_ledger).Large_Seats[i]).vessel.ID == ID){
            *flag = 2;
            return i;
        }
    }
    return -1;
}

void print_arrays(SHARED_MEMORY *share_mem){
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
    printf("SMALL ARRAY\n");
    for(int i = 0; i < (share_mem->public_ledger).seats_size[0]; i++){
        printf("%d \t", (share_mem->public_ledger.Small_Seats[i]).status);
        Print_Vessel_info(&(((share_mem->public_ledger).Small_Seats[i]).vessel));
    }
    printf("MEDIUM ARRAY\n");
    for(int i = 0; i < (share_mem->public_ledger).seats_size[1]; i++){
        printf("%d \t", (share_mem->public_ledger.Medium_Seats[i]).status);
        Print_Vessel_info(&(((share_mem->public_ledger).Medium_Seats[i]).vessel));
    }
    printf("LARGE ARRAY\n");
    for(int i = 0; i < (share_mem->public_ledger).seats_size[2]; i++){
        printf("%d \t", (share_mem->public_ledger.Large_Seats[i]).status);
        Print_Vessel_info(&(((share_mem->public_ledger).Large_Seats[i]).vessel));
    }
}

int print_current_arrays(SHARED_MEMORY *share_mem){
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
    int counter = 0;
    if (share_mem->max_count_of_Vessels[2] < (share_mem->public_ledger).seats_size[0]){
        printf("SMALL ARRAY\n");
        for(int i = 0; i < (share_mem->public_ledger).seats_size[0]; i++){
            if(((share_mem->public_ledger).Small_Seats[i]).status != 0){
                counter++;
                Print_Vessel_info(&(((share_mem->public_ledger).Small_Seats[i]).vessel));
            }
        }
    }
    if (share_mem->max_count_of_Vessels[1] < (share_mem->public_ledger).seats_size[1]){
        printf("MEDIUM ARRAY\n");
        for(int i = 0; i < (share_mem->public_ledger).seats_size[1]; i++){
            if(((share_mem->public_ledger).Medium_Seats[i]).status != 0){
                counter++;
                Print_Vessel_info(&(((share_mem->public_ledger).Medium_Seats[i]).vessel));
            }
        }
    }
    if (share_mem->max_count_of_Vessels[0] < (share_mem->public_ledger).seats_size[2]){
        printf("LARGE ARRAY\n");
        for(int i = 0; i < (share_mem->public_ledger).seats_size[2]; i++){
            if(((share_mem->public_ledger).Large_Seats[i]).status != 0){
                counter++;
                Print_Vessel_info(&(((share_mem->public_ledger).Large_Seats[i]).vessel));
            }
        }
    }
    return counter;
}

void enable_disable_monitor(SHARED_MEMORY *share_mem, int *monitor_key){
    if (*monitor_key == 1){
        sem_post(&(share_mem->monitor));
        *monitor_key = 0;
    }
    else if (*monitor_key == -1){
        sem_trywait(&(share_mem->monitor));
    }

}

void write_public_ledger_history(FILE *fp, SHARED_MEMORY *share_mem, Mooring_Area *seat, int seat_number){
    fprintf(fp, "%c [%d]: %d %c %c %lf %lf %lf %lf %lf %lf\n", seat->type, seat_number + 1, seat->vessel.ID, seat->vessel.type, seat->vessel.postype, seat->vessel.mantime, seat->vessel.parkperiod, seat->arrival_time, seat->service_time, seat->departure_time, share_mem->vessel_sum_cost);
}

void calc_vessels_cost(SHARED_MEMORY *share_mem, double cost, double parkperiod){
    double sum = 0;
    int time_per_30 = (int)(parkperiod);
    time_per_30 = parkperiod/30;
    sum = time_per_30*cost;
    if (( (parkperiod/30) - time_per_30 ) != 0){
        sum += cost;
    }
    share_mem->vessel_sum_cost = sum;
    share_mem->ports_total_income += share_mem->vessel_sum_cost;
}

void calc_average_waiting_time(SHARED_MEMORY *share_mem){
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
    double small_sum = 0;
    double medium_sum = 0;
    double large_sum = 0;
    int small_count = 0;
    int medium_count = 0;
    int large_count = 0;
    if (share_mem->max_count_of_Vessels[2] != (share_mem->public_ledger).seats_size[0])
        for (int i = 0; i < (share_mem->public_ledger).seats_size[0]; i++){
            if(((share_mem->public_ledger).Small_Seats[i]).status == 1){
                small_sum += (((share_mem->public_ledger).Small_Seats[i]).service_time - ((share_mem->public_ledger).Small_Seats[i]).arrival_time);
                small_count++;
            }
        }
    if(small_count > 0)
        printf("AVERAGE WAITING TIME FOR SMALL VESSELS %lf\n", small_sum/small_count);
    if (share_mem->max_count_of_Vessels[1] != (share_mem->public_ledger).seats_size[1])
        for (int i = 0; i < (share_mem->public_ledger).seats_size[1]; i++){
            if(((share_mem->public_ledger).Medium_Seats[i]).status == 1){
                medium_sum += (((share_mem->public_ledger).Medium_Seats[i]).service_time - ((share_mem->public_ledger).Medium_Seats[i]).arrival_time); 
                medium_count++;
            }
        }
    if(medium_count > 0)
        printf("AVERAGE WAITING TIME FOR MEDIUM VESSELS %lf\n", medium_sum/medium_count);
    if (share_mem->max_count_of_Vessels[0] != (share_mem->public_ledger).seats_size[2])
        for (int i = 0; i < (share_mem->public_ledger).seats_size[2]; i++){
            if(((share_mem->public_ledger).Large_Seats[i]).status == 1){
                large_sum += (((share_mem->public_ledger).Large_Seats[i]).service_time - ((share_mem->public_ledger).Large_Seats[i]).arrival_time); 
                large_count++;
            }
        }
    if(large_count > 0)
        printf("AVERAGE WAITING TIME FOR LARGE VESSELS %lf\n", large_sum/large_count);
    if((small_count + medium_count + large_count) > 0){
        printf("AVERAGE WAITING TIME FOR ALL VESSELS %lf\n\n", (small_sum + medium_sum + large_sum)/(small_count + medium_count + large_count));
        printf("AT THIS TIME PORT'S TOTAL INCOME %lf\n", share_mem->ports_total_income);
    }
}

double calc_vessel_cost_until_here(SHARED_MEMORY *share_mem, Vessel_info *vessel, double parkperiod_2){
    double cost; 
    int flag = -1;
    int seat_vessel = find_vessel_with_ID(share_mem, vessel->ID, &flag);
        if (flag == 0)
            cost = (share_mem->public_ledger).Small_Seats[seat_vessel].cost;
        else if (flag == 1)
            cost = (share_mem->public_ledger).Medium_Seats[seat_vessel].cost;
        else if (flag == 2)
            cost = (share_mem->public_ledger).Large_Seats[seat_vessel].cost;
    
    double sum = 0;
    int time_per_30 = (int)(parkperiod_2);
    time_per_30 = parkperiod_2/30;
    sum = time_per_30*cost;
    if (( (parkperiod_2/30) - time_per_30 ) != 0){
        sum += cost;
    }
    return sum;
}

int read_waiting_vessels(SHARED_MEMORY *sp, Waiting_Vessel *array_of_waiting_vessels){
    int i = 0;
    if((sp->vessel_in_S).ID != 0){
        memcpy(&((array_of_waiting_vessels[i]).vessel), &(sp->vessel_in_S), sizeof(Vessel_info));
        (array_of_waiting_vessels[i]).priority = sp->s_priority;
        (array_of_waiting_vessels[i]).arrival_time = sp->arrival_time[2];
        i++;
    }
    if((sp->vessel_in_M).ID != 0){
        memcpy(&((array_of_waiting_vessels[i]).vessel), &(sp->vessel_in_M), sizeof(Vessel_info));
        (array_of_waiting_vessels[i]).priority = sp->m_priority;
        (array_of_waiting_vessels[i]).arrival_time = sp->arrival_time[1];
        i++;
    }
    if((sp->vessel_in_L).ID != 0){
        memcpy(&((array_of_waiting_vessels[i]).vessel), &(sp->vessel_in_L), sizeof(Vessel_info));
        (array_of_waiting_vessels[i]).priority = sp->l_priority;
        (array_of_waiting_vessels[i]).arrival_time = sp->arrival_time[0];
        i++;
    }
    return i;
}

void swap(Waiting_Vessel *xp, Waiting_Vessel *yp)   // geeksforgeeks
{ 
    Waiting_Vessel temp;
    memcpy(&temp, xp, sizeof(Waiting_Vessel)); 
    memcpy(xp, yp, sizeof(Waiting_Vessel));
    memcpy(yp, &temp, sizeof(Waiting_Vessel));
} 

void sorting_the_array(Waiting_Vessel *array_of_waiting_vessels, int sz){   // geeksforgeeks
    int i, j; 
    for (i = 0; i < sz; i++)           
        for (j = 0; j < sz-i-1; j++)  
            if (array_of_waiting_vessels[j].priority > array_of_waiting_vessels[j+1].priority) 
                swap(&array_of_waiting_vessels[j], &array_of_waiting_vessels[j+1]);
}

int find_the_empty_seat(Mooring_Area *array_of_seats, int sz){
    int i = 0;
    while (i < sz){
        if(array_of_seats[i].status == 0)
            return i;
        i++;
    }
    return -1;
}
void store_vessel_to_seat(SHARED_MEMORY *share_mem, Vessel_info vessel, int flag, double arrival_time, double service_time){
    erase_the_stored(share_mem, vessel);
    if(vessel.type == 'L'){
        sem_post(&(share_mem->portin_sem_L));   // access to vessel for go inside port
        sem_post(&(share_mem->vessel_sem_L));   // access to next vessel of queue to write on its first place
    }
    else if(vessel.type == 'M'){
        sem_post(&(share_mem->portin_sem_M));
        sem_post(&(share_mem->vessel_sem_M));
    }
    else if(vessel.type == 'S'){
        sem_post(&(share_mem->portin_sem_S));
        sem_post(&(share_mem->vessel_sem_S));
    }
    sem_wait(&(share_mem->mutex2));
    //INIT SMALL, MEDIUM, LARGE SEATS
    (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
    (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
    (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
    int empty_seat = -1;
    if(vessel.type == 'L'){
        empty_seat = find_the_empty_seat((share_mem->public_ledger).Large_Seats, (share_mem->public_ledger).seats_size[2]);
        if (empty_seat == -1)
            printf("ERROR DIDNT FOUND EMPTY SEAT\n");
        else{
            (share_mem->public_ledger).Large_Seats[empty_seat].status = 1;
            memcpy(&((share_mem->public_ledger).Large_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
            (share_mem->public_ledger).Large_Seats[empty_seat].arrival_time = arrival_time;
            (share_mem->public_ledger).Large_Seats[empty_seat].service_time = service_time;
            share_mem->max_count_of_Vessels[0] -= 1;
        }
    }
    else if(vessel.type == 'M'){
        if(flag == 1){
            empty_seat = find_the_empty_seat((share_mem->public_ledger).Medium_Seats, (share_mem->public_ledger).seats_size[1]);
            if (empty_seat == -1)
                printf("ERROR DIDNT FOUND EMPTY SEAT\n");
            else{
                (share_mem->public_ledger).Medium_Seats[empty_seat].status = 1;
                memcpy(&((share_mem->public_ledger).Medium_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
                (share_mem->public_ledger).Medium_Seats[empty_seat].arrival_time = arrival_time;
                (share_mem->public_ledger).Medium_Seats[empty_seat].service_time = service_time;
                share_mem->max_count_of_Vessels[1] -= 1;
            }
        }
        else if(flag == 2){
            empty_seat = find_the_empty_seat((share_mem->public_ledger).Large_Seats, (share_mem->public_ledger).seats_size[2]);
            if (empty_seat == -1)
                printf("ERROR DIDNT FOUND EMPTY SEAT\n");
            else{
                (share_mem->public_ledger).Large_Seats[empty_seat].status = 1;
                memcpy(&((share_mem->public_ledger).Large_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
                (share_mem->public_ledger).Large_Seats[empty_seat].arrival_time = arrival_time;
                (share_mem->public_ledger).Large_Seats[empty_seat].service_time = service_time;
                share_mem->max_count_of_Vessels[0] -= 1;
            }
        } 
    }
    else if(vessel.type == 'S'){
        if (flag == 0){
            empty_seat = find_the_empty_seat((share_mem->public_ledger).Small_Seats, (share_mem->public_ledger).seats_size[0]);
            if (empty_seat == -1)
                printf("ERROR DIDNT FOUND EMPTY SEAT\n");
            else{
                (share_mem->public_ledger).Small_Seats[empty_seat].status = 1;
                memcpy(&((share_mem->public_ledger).Small_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
                (share_mem->public_ledger).Small_Seats[empty_seat].arrival_time = arrival_time;
                (share_mem->public_ledger).Small_Seats[empty_seat].service_time = service_time;
                share_mem->max_count_of_Vessels[2] -= 1;

            }
        }
        else if(flag == 1){
            empty_seat = find_the_empty_seat((share_mem->public_ledger).Medium_Seats, (share_mem->public_ledger).seats_size[1]);
            if (empty_seat == -1)
                printf("ERROR DIDNT FOUND EMPTY SEAT\n");
            else{
                (share_mem->public_ledger).Medium_Seats[empty_seat].status = 1;
                memcpy(&((share_mem->public_ledger).Medium_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
                (share_mem->public_ledger).Medium_Seats[empty_seat].arrival_time = arrival_time;
                (share_mem->public_ledger).Medium_Seats[empty_seat].service_time = service_time;
                share_mem->max_count_of_Vessels[1] -= 1;
            }
        }
        else if(flag == 2){
            empty_seat = find_the_empty_seat((share_mem->public_ledger).Large_Seats, (share_mem->public_ledger).seats_size[2]);
            if (empty_seat == -1)
                printf("ERROR DIDNT FOUND EMPTY SEAT\n");
            else{
                (share_mem->public_ledger).Large_Seats[empty_seat].status = 1;
                memcpy(&((share_mem->public_ledger).Large_Seats[empty_seat].vessel), &vessel, sizeof(Vessel_info));
                (share_mem->public_ledger).Large_Seats[empty_seat].arrival_time = arrival_time;
                (share_mem->public_ledger).Large_Seats[empty_seat].service_time = service_time;
                share_mem->max_count_of_Vessels[0] -= 1;
            }
        }

    }
    sem_wait(&(share_mem->read_write));
    share_mem->waiting_vessels -= 1;
    share_mem->parking_vessels += 1;
    sem_post(&(share_mem->read_write));
}

int check_empty_seat(SHARED_MEMORY *shm, Vessel_info vessel, int *flag){
    int status = 0;
    if(vessel.type == 'L'){                       // LARGE VESSEL
        sem_wait(&(shm->read_write));
        if(shm->max_count_of_Vessels[0] > 0){
            status = 1;
            *flag = 2; 
        }
        else 
            status = 0;
        sem_post(&(shm->read_write));
    }
    else if(vessel.type == 'M'){                  // MEDIUM VESSEL                 
        sem_wait(&(shm->read_write));
        if(shm->max_count_of_Vessels[1] > 0){
            status = 1;
            *flag = 1;
        }
        else if(vessel.postype == 'L'){         // CHECK IF MEDIUM HAS DONE UPGRADE TO LARGE
            printf("THERE IS NO SEAT FOR MEDIUM I WILL CHECK FOR LARGE POSTYPE\n");
            if(shm->max_count_of_Vessels[0] > 0){
                status = 1;
                *flag = 2;
            }
            else 
                status = 0;
        }
        else 
            status = 0;
        sem_post(&(shm->read_write));
    }
    else if(vessel.type == 'S'){                  // SMALL VESSEL
        sem_wait(&(shm->read_write));
        if(shm->max_count_of_Vessels[2] > 0){
            status = 1;
            *flag = 0;
        }
        else if(vessel.postype == 'M'){         // CHECK IF SMALL HAS DONE UPGRADE TO MEDIUM
            printf("THERE IS NO SEAT FOR SMALL I WILL CHECK FOR MEDIUM POSTYPE\n");
            if(shm->max_count_of_Vessels[1] > 0){
                status = 1;
                *flag = 1;
            }       
            else 
                status = 0;
        }
        else if(vessel.postype == 'L'){         // CHECK IF SMALL HAS DONE UPGRADE TO LARGE
            printf("THERE IS NO SEAT FOR SMALL I WILL CHECK FOR LARGE POSTYPE\n");
            if(shm->max_count_of_Vessels[0] > 0){
                status = 1;
                *flag = 2;
            }
            else 
                status = 0;
        }
        else 
            status = 0;
        sem_post(&(shm->read_write));
    }
    else 
        printf("ERROR IN TYPE VESSEL\n");
    return status;
}

int port_master_server(SHARED_MEMORY *share_mem, FILE *fp){
    int FLAG = 1;
    int monitor_key = 1;
    while(FLAG){
        int retval = sem_trywait(&(share_mem->vessel_sem_out));     // vessel to get out
        if (retval == -1){
            retval = sem_trywait(&(share_mem->vessel_sem_in));      // vessel to get in
            sem_wait(&(share_mem->read_write));
                int waiting = share_mem->waiting_vessels;
                int parking = share_mem->parking_vessels;
            sem_post(&(share_mem->read_write));
            if(retval == -1 && waiting == 0){
                if((waiting == 0) && (parking == 0)){
                    int whatisthevalue;
                    sem_getvalue(&(share_mem->port_sem_out), &whatisthevalue);
                    if (whatisthevalue != 1)
                        continue;   // if there is vessel doing mantime for living
                    enable_disable_monitor(share_mem, &monitor_key);            // enable the first time, monitor_key = 1 -> 0
                    printf("There are NOT Vessels to get in or get out the Port\nWait for New Vessels Or Exit? W/E ");
                    char c = getchar();
                    getchar();
                    if (c == 'E'){
                        FLAG = 0;
                        monitor_key = -1;
                        enable_disable_monitor(share_mem, &monitor_key);        // disable on exiting, monitor_key = -1
                    }
                    else if (c == 'W'){
                        continue;
                    }
                }
            }
            else{       // there are waiting vessels to get in
                time_t service_time;
                sem_wait(&(share_mem->read_write));
                int sum = share_mem->max_count_of_Vessels[0] + share_mem->max_count_of_Vessels[1] + share_mem->max_count_of_Vessels[2];
                sem_post(&(share_mem->read_write));
                if (sum == 0)
                    continue;
                printf("Insert Vessels\n");
                if(waiting > 0){
                    sem_wait(&(share_mem->read_write));
                    Waiting_Vessel array_of_waiting_vessels[3];
                    sem_post(&(share_mem->read_write));
                    printf("Reading the waiting vessels\n");
                    int sz = read_waiting_vessels(share_mem, array_of_waiting_vessels);
                    if (sz == 0)
                        continue;
                    else
                        printf("I READ %d waiting vessels\n", sz);
                    printf("Im going to sort them\n");
                    sorting_the_array(array_of_waiting_vessels, sz);
                    for(int i = 0; i <sz; i++){
                        printf("priority: %d \t ", (array_of_waiting_vessels[i]).priority);
                        Print_Vessel_info(&((array_of_waiting_vessels[i]).vessel));
                    }
                    int i = 0;
                    while(i < sz){
                        int flag = -1;
                        int status;
                        printf("check for empty seat for the vessel %d\n", ((array_of_waiting_vessels[i]).vessel).ID);
                        if( (status = check_empty_seat(share_mem, (array_of_waiting_vessels[i]).vessel, &flag )) == 1 ){
                            printf("found empty seat for vessel %d\n", ((array_of_waiting_vessels[i]).vessel).ID);
                            printf("Try to access port road or wait\n");
                            sem_wait(&(share_mem->port_sem_out));   // get access to the port
                            printf("Accessed port road\n");
                            service_time = time(NULL);
                            store_vessel_to_seat( share_mem, (array_of_waiting_vessels[i]).vessel, flag, (array_of_waiting_vessels[i]).arrival_time, service_time);
                            printf("Print array of parked vessels\n");
                            print_arrays(share_mem);
                            break;
                        }
                        else{
                            if(i == sz-1){      // there is no empty seat sleep for a litle
                                //i = 0;
                                printf("didn't found empty seat for any vessel 'wait' if any other vessel goes out\n"); 
                                sleep(3);
                                break;
                            }
                            else 
                                i++;
                        }
                    } 
                }
            }
        }
        else{           // there are waiting vessels to get out
            printf("FOR EXIT Try to access port road or wait\n"); 
            sem_wait(&(share_mem->port_sem_out));
            printf("FOR EXIT Accessed port road\n"); 
            sem_wait(&(share_mem->read_write));
            time_t departure_time;
            departure_time = time(NULL);
            int flag = -1;
            printf("FIND vessels %d seat in array of seats\n", share_mem->vessel_out.ID); 
            int seat_vessel = find_vessel_with_ID(share_mem, share_mem->vessel_out.ID, &flag);
            if (flag == 0)
                calc_vessels_cost(share_mem,(share_mem->public_ledger).Small_Seats[seat_vessel].cost, share_mem->vessel_out.parkperiod);
            else if (flag == 1)
                calc_vessels_cost(share_mem, (share_mem->public_ledger).Medium_Seats[seat_vessel].cost, share_mem->vessel_out.parkperiod);
            else if (flag == 2)
                calc_vessels_cost(share_mem, (share_mem->public_ledger).Large_Seats[seat_vessel].cost, share_mem->vessel_out.parkperiod);
            printf("UNLOCK EMPTY2 for leaving the port\n"); 
            sem_post(&(share_mem->mutex3));

            if (seat_vessel == -1){
                printf("ERROR dind't found it\n");
            }
            else{
                (share_mem->public_ledger).Small_Seats = (Mooring_Area *)((char *)share_mem + sizeof(SHARED_MEMORY));
                (share_mem->public_ledger).Medium_Seats = (Mooring_Area *)((char *)((share_mem->public_ledger).Small_Seats) + ((share_mem->public_ledger).seats_size[0])*sizeof(Mooring_Area));
                (share_mem->public_ledger).Large_Seats = (Mooring_Area *)((char *)(share_mem->public_ledger).Medium_Seats + ((share_mem->public_ledger).seats_size[1])*sizeof(Mooring_Area));
                if (flag == 0){
                    (share_mem->public_ledger).Small_Seats[seat_vessel].status = 0;
                    (share_mem->public_ledger).Small_Seats[seat_vessel].departure_time = departure_time;
                    write_public_ledger_history(fp, share_mem, &((share_mem->public_ledger).Small_Seats[seat_vessel]), seat_vessel);
                    share_mem->max_count_of_Vessels[2] += 1;
                }
                else if (flag == 1){
                    (share_mem->public_ledger).Medium_Seats[seat_vessel].status = 0;
                    (share_mem->public_ledger).Medium_Seats[seat_vessel].departure_time = departure_time;
                    write_public_ledger_history(fp, share_mem, &((share_mem->public_ledger).Medium_Seats[seat_vessel]), seat_vessel);
                    share_mem->max_count_of_Vessels[1] += 1;
                }
                else if (flag == 2){
                    (share_mem->public_ledger).Large_Seats[seat_vessel].status = 0;
                    (share_mem->public_ledger).Large_Seats[seat_vessel].departure_time = departure_time;
                    write_public_ledger_history(fp, share_mem, &((share_mem->public_ledger).Large_Seats[seat_vessel]), seat_vessel);
                    share_mem->max_count_of_Vessels[0] += 1;
                }
                share_mem->parking_vessels -= 1;
            }
            sem_post(&(share_mem->read_write));
            printf("UNLOCK MUTEX for the other exiting vessels\n");
            sem_post(&(share_mem->mutex1));
        }
    }

    return 1;
}