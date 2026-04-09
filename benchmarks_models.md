Google benchMark + perf for model 1 (Naive implementation baseline)  for 1,000,000,000 orders.
Running /home/jeremy1110/LOB_engine/lob_benchmark
Run on (32 X 2401 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 1024 KiB (x16)
  L3 Unified 32768 KiB (x2)
Load Average: 0.37, 0.41, 0.36
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
terminate called after throwing an instance of 'std::bad_alloc'
  what():  std::bad_alloc
/home/jeremy1110/LOB_engine/lob_benchmark: Aborted

 Performance counter stats for '/home/jeremy1110/LOB_engine/lob_benchmark':

   254,689,803,672      instructions                                                          
   128,058,093,547      cycles                                                                
    62,337,579,661      L1-dcache-loads                                                       
     4,133,047,680      L1-dcache-load-misses                                                 
   <not supported>      LLC-loads                                                             
   <not supported>      LLC-load-misses                                                       

      56.701438956 seconds time elapsed

      26.870721000 seconds user
      29.681959000 seconds sys



MODEL 2 LOB CONTIGUOUS+  + ARENA 
LOB PERF : jeremy1110@fedora:~$ cd LOB_engine
jeremy1110@fedora:~/LOB_engine$ perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./lob_bench
2026-04-09T17:43:10+02:00
Running ./lob_bench
Run on (32 X 5140.13 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 1024 KiB (x16)
  L3 Unified 32768 KiB (x2)
Load Average: 0.41, 0.56, 0.45
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
LOB_Naive_Continuous     416815 ns       416579 ns         1662 items_per_second=24.0051M/s

 Performance counter stats for './lob_bench':

     4,841,866,481      cycles:u                                                                (83.33%)
     9,807,377,891      instructions:u                                                          (83.31%)
       156,444,035      cache-references:u                                                      (83.30%)
         1,987,750      cache-misses:u                                                          (83.33%)
     2,144,469,247      branches:u                                                              (83.38%)
        62,966,965      branch-misses:u                                                         (83.34%)

       1.228811391 seconds time elapsed

       0.997306000 seconds user
       0.231070000 seconds sys


       Metric	Value	Analytical Significance
Instructions Per Cycle (IPC)	2.02	Solid but CPU does wait sometimes
Branch-Miss Rate	2.93%	High 
Cache-Miss Rate	1.27%	Best performance here, the Arena is working data is contiguous 
Instructions per Order	~590	High, a lot of verbose
