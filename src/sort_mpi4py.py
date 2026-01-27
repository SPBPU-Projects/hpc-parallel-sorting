from mpi4py import MPI
import argparse
import heapq
import time

def read_ints(path: str):
    with open(path, "r") as f:
        return list(map(int, f.read().split()))

def write_ints(path: str, arr):
    with open(path, "w") as f:
        f.write(" ".join(map(str, arr)) + "\n")

def chunkify(a, size):
    n = len(a)
    base = n // size
    rem = n % size
    chunks = []
    start = 0
    for r in range(size):
        add = 1 if r < rem else 0
        end = start + base + add
        chunks.append(a[start:end])
        start = end
    return chunks

def kway_merge(sorted_lists):
    # heap: (value, list_id, index_in_list)
    heap = []
    for li, lst in enumerate(sorted_lists):
        if lst:
            heap.append((lst[0], li, 0))
    heapq.heapify(heap)

    out = []
    while heap:
        val, li, idx = heapq.heappop(heap)
        out.append(val)
        idx += 1
        if idx < len(sorted_lists[li]):
            heapq.heappush(heap, (sorted_lists[li][idx], li, idx))
    return out

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--input", required=True)
    ap.add_argument("--output", required=True)
    ap.add_argument("--echo", action="store_true", help="print sorted numbers to stdout (use only for small N)")
    args = ap.parse_args()

    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    t0 = MPI.Wtime()

    if rank == 0:
        data = read_ints(args.input)
        N = len(data)
        chunks = chunkify(data, size)
    else:
        N = None
        chunks = None

    N = comm.bcast(N, root=0)
    local = comm.scatter(chunks, root=0)

    local.sort()
    gathered = comm.gather(local, root=0)

    if rank == 0:
        sorted_all = kway_merge(gathered)
        t1 = MPI.Wtime()
        print(f"mpi4py_sort: N={N} tasks={size} time_s={t1 - t0:.6f}")

        write_ints(args.output, sorted_all)

        if args.echo:
            print("OUTPUT:")
            print(" ".join(map(str, sorted_all)))

if __name__ == "__main__":
    main()

