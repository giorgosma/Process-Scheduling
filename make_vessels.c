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

typedef struct{
    char type, postype, parkperiod[50], mantime[50], p_id[50];
} Info_Vessel;

int read_id_from_file(char *filename){
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    int id;
    fread(&id,sizeof(id),1, fp);
    fclose(fp);
    return id;
}
void give_vessel_info(char *type, char *postype, char *parkperiod, char *mantime, char *p_id, int id){
    double p_period, m_time;
    printf("Select type: \t L for LARGE \t M for MEDIUM \t S for SMALL\n");
    int ret;
    scanf ("%s", type);
    if (*type == 'L'){
        memcpy(postype, "L", sizeof(char));
        p_period = 45;
        m_time = 30;
    }
    else if(*type == 'M'){
        printf("Select postype: \t L for LARGE \t M for MEDIUM\n");
        scanf ("%s", postype);
        p_period = 35;
        m_time = 20;
    }
    else if(*type == 'S'){
        printf("Select postype: \t L for LARGE \t M for MEDIUM \t S for SMALL\n");
        scanf ("%s", postype);
        p_period = 25;
        m_time = 10;
    }
    else{
        printf("NOTHING\n");
    }
    sprintf(parkperiod, "%f", p_period);
    sprintf(mantime, "%f", m_time);
    sprintf(p_id, "%d", id);
}

int make_processes(int id, int num_of_porcesses, Info_Vessel *info_vessel){
    pid_t pid;
    if (num_of_porcesses == 0)
        return 0;
    if ((pid = fork()) == 0){
        //for (int i = 0; i< num_of_porcesses; i++)
            //printf("%c %c %s %s %s\n", (info_vessel->type), (info_vessel->postype), info_vessel->parkperiod, info_vessel->mantime, info_vessel->p_id);
        execlp("./vessel", "-t", &(info_vessel->type), "-u", &(info_vessel->postype), "-p", info_vessel->parkperiod, "-m", info_vessel->mantime, "-s", info_vessel->p_id, (char *)NULL);
        printf("ERROR in EXEC\n");
    }
    else{
        // int ex = waitpid(pid,NULL,0);
        num_of_porcesses--;
        Info_Vessel *p_info = (Info_Vessel *)((char*)info_vessel + sizeof(Info_Vessel));
        //printf("%c %c %s %s %s\n", (p_info->type), (p_info->postype), p_info->parkperiod, p_info->mantime, p_info->p_id);
        sleep(1);
        make_processes(id, num_of_porcesses, p_info);
        int ex = waitpid(pid,NULL,0);
    }
    return 1;
}

int main(int args, char **argv){
    int id = read_id_from_file(argv[1]);
    //int id = 2345678;
    int num_of_porcesses = atoi(argv[2]);
    Info_Vessel info_vessel[num_of_porcesses];
    for (int i = 0; i< num_of_porcesses; i++){
        give_vessel_info(&(info_vessel[i].type), &(info_vessel[i].postype), info_vessel[i].parkperiod, info_vessel[i].mantime, info_vessel[i].p_id, id);
    }
    //for (int i = 0; i< num_of_porcesses; i++)
    //    printf("%c %c %s %s %s\n", (info_vessel[i].type), (info_vessel[i].postype), info_vessel[i].parkperiod, info_vessel[i].mantime, info_vessel[i].p_id);
    make_processes(id, num_of_porcesses, &(info_vessel[0]));
    return 0;
}