#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <omp.h>

static int cmp_int(const void *p, const void *q) {
    int x = *(const int*)p, y = *(const int*)q;
    return (x > y) - (x < y);
}

static inline double now_s(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + 1e-9*(double)ts.tv_nsec;
}

/* k-way merge for small N=500 */
static void merge_k(int *a, int n, int k){
    int *L = (int*)malloc((size_t)k * sizeof(int));
    int *R = (int*)malloc((size_t)k * sizeof(int));
    int *idx = (int*)malloc((size_t)k * sizeof(int));
    int *out = (int*)malloc((size_t)n * sizeof(int));

    for(int t=0;t<k;t++){
        L[t] = (t*n)/k;
        R[t] = ((t+1)*n)/k;
        idx[t] = L[t];
    }

    for(int i=0;i<n;i++){
        int best_t = -1;
        int best_v = 0;
        for(int t=0;t<k;t++){
            if(idx[t] < R[t]){
                int v = a[idx[t]];
                if(best_t < 0 || v < best_v){
                    best_t = t;
                    best_v = v;
                }
            }
        }
        out[i] = best_v;
        idx[best_t]++;
    }

    memcpy(a, out, (size_t)n*sizeof(int));
    free(L); free(R); free(idx); free(out);
}

int main(int argc, char **argv){
    const char *in_path = NULL, *out_path = NULL;
    int threads = 1;

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--input") && i+1<argc) in_path = argv[++i];
        else if(!strcmp(argv[i],"--output") && i+1<argc) out_path = argv[++i];
        else if(!strcmp(argv[i],"--threads") && i+1<argc) threads = atoi(argv[++i]);
    }

    if(!in_path || !out_path || threads < 1){
        fprintf(stderr,"Usage: %s --input data/input_500.txt --output results/out.txt --threads T\n", argv[0]);
        return 2;
    }

    FILE *f = fopen(in_path,"r");
    if(!f){ perror("fopen input"); return 1; }

    int cap = 1024, n = 0;
    int *a = (int*)malloc((size_t)cap*sizeof(int));
    while(1){
        int x;
        if(fscanf(f,"%d",&x) != 1) break;
        if(n >= cap){
            cap *= 2;
            a = (int*)realloc(a, (size_t)cap*sizeof(int));
        }
        a[n++] = x;
    }
    fclose(f);

    if(threads > n) threads = n;

    double t0 = now_s();

    #pragma omp parallel num_threads(threads)
    {
        int tid = omp_get_thread_num();
        int T = omp_get_num_threads();
        int l = (tid * n) / T;
        int r = ((tid + 1) * n) / T;
        qsort(a + l, (size_t)(r - l), sizeof(int), cmp_int);
    }

    merge_k(a, n, threads);

    double t1 = now_s();

    FILE *g = fopen(out_path,"w");
    if(!g){ perror("fopen output"); return 1; }
    for(int i=0;i<n;i++){
        fprintf(g, "%d%s", a[i], (i+1<n) ? " " : "\n");
    }
    fclose(g);

    printf("openmp_sort: n=%d threads=%d time_s=%.6f\n", n, threads, (t1-t0));

    free(a);
    return 0;
}
