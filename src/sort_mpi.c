#define _GNU_SOURCE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int cmp_int(const void *a, const void *b){
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static inline double now_s(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + 1e-9*(double)ts.tv_nsec;
}

/* ===== Min-heap for k-way merge (O(N log P)) ===== */
typedef struct { int val; int src; int idx; } heap_item;

static void heap_swap(heap_item *h, int i, int j){ heap_item t=h[i]; h[i]=h[j]; h[j]=t; }

static void heap_sift_down(heap_item *h, int n, int i){
    for(;;){
        int l=2*i+1, r=2*i+2, m=i;
        if(l<n && h[l].val < h[m].val) m=l;
        if(r<n && h[r].val < h[m].val) m=r;
        if(m==i) break;
        heap_swap(h,i,m);
        i=m;
    }
}

static void heap_sift_up(heap_item *h, int i){
    while(i>0){
        int p=(i-1)/2;
        if(h[p].val <= h[i].val) break;
        heap_swap(h,p,i);
        i=p;
    }
}

static void heap_push(heap_item *h, int *n, heap_item it){
    h[*n]=it;
    (*n)++;
    heap_sift_up(h, (*n)-1);
}

static heap_item heap_pop(heap_item *h, int *n){
    heap_item top=h[0];
    (*n)--;
    h[0]=h[*n];
    heap_sift_down(h, *n, 0);
    return top;
}

static void merge_sorted_blocks(int *all, int *counts, int *displs, int P, int *out){
    heap_item *heap = (heap_item*)malloc((size_t)P * sizeof(heap_item));
    int hn = 0;

    for(int p=0; p<P; p++){
        if(counts[p] > 0){
            heap_item it;
            it.src = p;
            it.idx = 0;
            it.val = all[displs[p]];
            heap_push(heap, &hn, it);
        }
    }

    int N = 0;
    for(int p=0; p<P; p++) N += counts[p];

    for(int i=0; i<N; i++){
        heap_item cur = heap_pop(heap, &hn);
        out[i] = cur.val;

        int p = cur.src;
        int next_idx = cur.idx + 1;
        if(next_idx < counts[p]){
            heap_item nxt;
            nxt.src = p;
            nxt.idx = next_idx;
            nxt.val = all[displs[p] + next_idx];
            heap_push(heap, &hn, nxt);
        }
    }
    free(heap);
}

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);

    int rank=0, size=1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *in_path = NULL;
    const char *out_path = NULL;

    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--input") && i+1<argc) in_path = argv[++i];
        else if(!strcmp(argv[i],"--output") && i+1<argc) out_path = argv[++i];
    }

    if(!in_path || !out_path){
        if(rank==0) fprintf(stderr,"Usage: %s --input data/input_500.txt --output results/mpi_sorted.txt\n", argv[0]);
        MPI_Finalize();
        return 2;
    }

    int *full = NULL;
    int N = 0;

    if(rank==0){
        FILE *f = fopen(in_path,"r");
        if(!f){ perror("fopen input"); MPI_Abort(MPI_COMM_WORLD, 1); }
        int cap=1024; full=(int*)malloc((size_t)cap*sizeof(int));
        while(1){
            int x;
            if(fscanf(f,"%d",&x)!=1) break;
            if(N>=cap){ cap*=2; full=(int*)realloc(full,(size_t)cap*sizeof(int)); }
            full[N++] = x;
        }
        fclose(f);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(N==0){ MPI_Finalize(); return 0; }

    int *counts = (int*)malloc((size_t)size*sizeof(int));
    int *displs = (int*)malloc((size_t)size*sizeof(int));
    for(int p=0; p<size; p++){
        int l = (p * N) / size;
        int r = ((p+1) * N) / size;
        counts[p] = r - l;
        displs[p] = l;
    }

    int local_n = counts[rank];
    int *local = (int*)malloc((size_t)local_n*sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = now_s();

    MPI_Scatterv(full, counts, displs, MPI_INT,
                 local, local_n, MPI_INT,
                 0, MPI_COMM_WORLD);

    qsort(local, (size_t)local_n, sizeof(int), cmp_int);

    int *all_sorted = NULL;
    if(rank==0) all_sorted = (int*)malloc((size_t)N*sizeof(int));

    MPI_Gatherv(local, local_n, MPI_INT,
                all_sorted, counts, displs, MPI_INT,
                0, MPI_COMM_WORLD);

    if(rank==0){
        int *out = (int*)malloc((size_t)N*sizeof(int));
        merge_sorted_blocks(all_sorted, counts, displs, size, out);

        FILE *g = fopen(out_path,"w");
        if(!g){ perror("fopen output"); MPI_Abort(MPI_COMM_WORLD, 2); }
        for(int i=0;i<N;i++){
            fprintf(g, "%d%s", out[i], (i+1<N) ? " " : "\n");
        }
        fclose(g);

        free(out);
        free(all_sorted);
        free(full);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = now_s();

    if(rank==0){
        printf("mpi_sort: N=%d tasks=%d time_s=%.6f\n", N, size, (t1-t0));
    }

    free(local);
    free(counts);
    free(displs);

    MPI_Finalize();
    return 0;
}
