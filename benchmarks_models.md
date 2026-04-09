## Benchmarking Results

### Model 1: Naive Batch Processing
* **Implementation:** `std::map` / `std::unordered_map` (Global Heap)
* **Scale:** 1,000,000,000 Orders
* **Result:** `CRITICAL FAILURE` - `std::bad_alloc`
* **Diagnostics:** * Memory exhaustion reached before completion.
    * `sys` time (29.68s) > `user` time (26.87s): Indicates kernel-level thrashing/page faults.
    * L1 Load Misses: 4.13B (Cache pollution).
  This was created more as a test than a real comparison.

### Model 2: Arena Baseline (Naive Logic)
* **Implementation:** Contiguous Arena (`Order` objects) + `std::map` (Logic)
* **Status:** Baseline for next ameliorations
* **Throughput:** 24.00 M/s

| Metric | Value | Note |
| :--- | :--- | :--- |
| **IPC** | 2.02 | Okay for baseline but there is an ovbious pipeline stall |
| **Branch-Miss** | 2.93% | Logic bottleneck possibly due to verbosity |
| **Cache-Miss** | 1.27% | Arena locality success|
| **Inst/Order** | ~590 | High verbosity / Map overhead |

// Analysis:
// Model 1 serves as a good baseline with credible results.
// Possible bottleneck: Instruction density and branch misprediction in std::map.
// Next optimization: Replace maps with intrusive lists and flat price arrays, and reduce verbosity.
