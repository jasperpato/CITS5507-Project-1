#define N 2000
#define NUM_SITES N*N
#define MAX_CLUSTERS N % 2 == 0 ? NUM_SITES/2 : (N-1)*(N-1)/2+1
#define N_THREADS 1