\# HPC Parallel Sorting



\## Overview

This project presents a comprehensive implementation and performance evaluation of parallel sorting algorithms using multiple parallel programming paradigms. The goal is to compare scalability, efficiency, and runtime behavior across shared-memory and distributed-memory models on a real high-performance computing (HPC) system.



All implementations were executed and benchmarked on the \*\*SPbPU supercomputer\*\*, providing realistic performance measurements beyond theoretical or local-machine testing.



---



\## Implemented Approaches

The following parallel programming models are implemented and analyzed:



\- \*\*MPI (C)\*\*  

&nbsp; Distributed-memory parallel sorting using message passing across multiple processes.



\- \*\*OpenMP (C)\*\*  

&nbsp; Shared-memory parallel sorting using multithreading directives.



\- \*\*POSIX Threads (Pthreads, C)\*\*  

&nbsp; Low-level thread-based parallel sorting with explicit thread management.



\- \*\*mpi4py (Python)\*\*  

&nbsp; Python-based distributed parallel sorting using MPI bindings.



---



\## Project Structure



---



\## Experimental Setup

\- \*\*Platform:\*\* SPbPU High-Performance Computing System  

\- \*\*Programming Languages:\*\* C, Python  

\- \*\*Parallel Models:\*\* MPI, OpenMP, Pthreads  

\- \*\*Evaluation Metrics:\*\*

&nbsp; - Execution time

&nbsp; - Speedup

&nbsp; - Scalability

&nbsp; - Comparative performance across models



Benchmarks were conducted with varying input sizes and different numbers of threads/processes to evaluate both strong and weak scaling behavior.



---



\## Key Features

\- Real execution on a university supercomputer (not simulated)

\- Comparison of shared-memory vs distributed-memory paradigms

\- Unified problem definition across all implementations

\- Reproducible experiments using provided input data

\- Detailed performance analysis documented in the report



---



\## How to Run (Example)

> Compilation and execution commands depend on the target HPC environment and scheduler configuration.  

> Refer to `Report.pdf` for detailed build instructions, execution parameters, and benchmark settings.



---



\## Results

The results demonstrate clear performance differences between threading-based and message-passing approaches, highlighting the impact of communication overhead, memory access patterns, and scalability limits of each model.



Detailed numerical results and plots are available in:

\- `results/`

\- `plots/`

\- `Report.pdf`



---



\## Academic Context

This project was developed as part of coursework and practical training in \*\*High-Performance Computing\*\* at Saint Petersburg Polytechnic University (SPbPU).



---



\## Author

\*\*Matin Dastanboo\*\*  

MSc Student – Artificial Intelligence & Machine Learning 

Peter the Great St. Petersburg Polytechnic University

SAINT PETERSBURG - RUSSIA



