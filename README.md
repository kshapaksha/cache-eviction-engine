# Cache Management System

**Cache Management System: Comparative Analysis of FIFO, LRU and Optimal
Page Replacement with TTL-Based Cache Expiry**

A C++ console application with two modes: a page-replacement simulator and a
browser cache manager with LRU eviction and TTL expiry.

## Authors
- Pranjal Papnoi
- Sheetal Mehta

Graphic Era Hill University, Haldwani Campus. PBL Project.

## Modes
| Mode | Description |
|------|-------------|
| Mode 1 | Simulates FIFO, LRU and Optimal page replacement step by step and compares their faults and hit ratios |
| Mode 2 | Browser cache manager: visit, view, search, delete, expire and clear pages, with live statistics |



## How Mode 2 works
- A **doubly linked list** keeps pages ordered by recent use.
- A **hash map** finds any page by URL instantly.
- When the cache is full, the **least recently used** page is evicted.
- Every page has a **TTL**. Expired pages are removed on access or by manual cleanup.

## Build and run
```bash
g++ -std=c++17 -Wall main.cpp mode1.cpp mode2.cpp -o cache
./cache
```

## Sample output
(paste a screenshot or a short terminal session here)

## Contributions
- Mode 1 (page replacement): Sheetal Mehta
- Mode 2 (cache manager) and main program: Pranjal Papnoi

## License
MIT
