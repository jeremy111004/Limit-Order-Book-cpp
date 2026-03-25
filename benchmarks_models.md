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

