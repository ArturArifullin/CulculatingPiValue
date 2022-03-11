#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

double series(int, int);
double expm(double, double);
int ser_numb = 100;

double series(int m, int id){
        int k;
        double ak, eps, p, s, t;
#define eps 1e-17
        s = 0.;
        for(k=0; k < id; k++){
                ak = 8*k+m;
                p = id-k;
                t = expm(p, ak);
                s += t/ak;
                s -= (int)s;
        }

        for(k = id; k <= id + ser_numb; k++){
                ak = 8 * k + m;
                t = pow(16., (double)(id - k)) / ak;
                if(t < eps) break;
                s = s + t;
                s -= (int)s;
        }
        return s;

}


double expm(double p, double ak){
        int i, j;
        double p1, pt, r;
#define ntp 25
static double tp[ntp];
        static int tp1 = 0;
        if(tp1 == 0){
                tp1 = 1;
                tp[0] = 1.;

                for (i = 1; i < ntp; i++)
                        tp[i] = 2. * tp[i - 1];

        }

        if(ak ==1.) return 0;

        for(i = 0; i < ntp; i++)
                if (tp[i] > p)
                        break;

        pt = tp[i - 1];
        p1 = p;
        r = 1.;

        for(j = 1; j <= i; j++){
                if(p1 >= pt){
                        r = 16. * r;
                        r = r - (int)(r / ak) * ak;
                        p1 -= pt;
                }
                pt *= 0.5;
                if (pt >= 1.){
                        r *= r;
                        r -= (int)(r / ak) * ak;
                }

        }

        return r;
}

int main(int argc, char** argv){
        double pid, s1, s2, s3, s4;
        char* out = (char*)malloc(atoi(argv[2])+ 1);
        char hx[] = "0123456789ABCDF";
        for(int i = 0; i < atoi(argv[2]); i++) {
                int id = atoi(argv[1]) + i;
                s1 = series(1, id);
                s2 = series(4, id);
                s3 = series(5, id);
                s4 = series(6, id);
                pid = 4. * s1 - 2.* s2 - s3 - s4;
                pid = pid - (int)pid + 1.;
                out[i] = hx[(int)(16. * (fabs(pid) - floor(pid)))];
        }
        int key = -1;
        if ((key = ftok("/tmp/sm_token.txt", 'R')) == -1) /*Here the file must exist */
        {
            printf("failed to create key: %s", strerror(errno));
            fflush(stdout);
        }

        int dskrptr= shmget(key, atoi(argv[3]), IPC_CREAT | 0666 );
        
        char* c= shmat( dskrptr, NULL, 0 );
        
        for(int i = 0; i < atoi(argv[2]); i++)
                c[i+atoi(argv[1])]= out[i];
        shmdt(c);
        return 0;
}