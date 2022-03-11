#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char** argv){
    if(argc < 3){
        perror("Not enough argumetns\n");
        exit(1);
    }
    int numb_of_discharge;
    int remainder;
    int fl;
    int last;
    if ( ( fl =  atoi( argv[1] ) % atoi( argv[2] ) ) == 0 ){
        numb_of_discharge= atoi( argv[1] ) / atoi( argv[2] );
        last = atoi( argv[1] ) - numb_of_discharge + 1;
    }
    else{
        numb_of_discharge=  atoi( argv[1] ) / ( atoi( argv[2] ) );
        remainder= atoi( argv[1] ) % ( atoi( argv[2] ) );
        last= atoi( argv[1] ) - numb_of_discharge - remainder + 1;
    }

    int key = -1;
    if ((key = ftok("/tmp/sm_token.txt", 'R')) == -1) /*Here the file must exist */
    {
        printf("failed to create key: %s", strerror(errno));
        fflush(stdout);
    }

    int bit_count = atoi(argv[1]);
    int dskrptr = shmget(key, bit_count, IPC_CREAT | 0666 );
    if (dskrptr == -1) {
        printf("failed to create shared memory: %s", strerror(errno));
        fflush(stdout);
        return 1;
    }

    char buff1[1000], buff2[1000];
    for(int i = 0; i <= atoi( argv[1] ) / 2 ; i+= numb_of_discharge){ //возможные лишние итерации, выйдет по break
        if ( last - i < i ){ //так быть не должно, нужно выйти
            break;
        }
        else if ( i == last - i ){ //случай нечетного числа
            sprintf(buff1, "%d", i);
            sprintf(buff2, "%d", numb_of_discharge);

            if ( fork() == 0 )
                execlp("/Users/arturarifullin/Desktop/C_Programs/pi/BBP", "BBP", buff1, buff2, argv[1], (char*)0);
        }
        else{
            sprintf(buff1, "%d", i);
            sprintf(buff2, "%d", numb_of_discharge);
            if ( fork() == 0 )
                execlp("/Users/arturarifullin/Desktop/C_Programs/pi/BBP", "BBP", buff1, buff2, argv[1], (char*)0);
            sprintf(buff1, "%d", last - i);
            if ( i )
                sprintf(buff2, "%d", numb_of_discharge);
            else
                sprintf(buff2, "%d", numb_of_discharge+remainder);

            if ( fork() == 0 )
                execlp("/Users/arturarifullin/Desktop/C_Programs/pi/BBP", "BBP", buff1, buff2, argv[1], (char*)0);
        }
    }
    wait(NULL);
    printf("\n");
    char* c = shmat( dskrptr, NULL, 0 );
    for (int i = 0; i < atoi(argv[1]); i++){
        write(1, &c[i], sizeof(char));
    }
    return 0;

}