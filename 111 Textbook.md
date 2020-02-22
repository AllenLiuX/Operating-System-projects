# 111 Textbook

## 1 Dialogue on the Book

- Virtualization, Concurrency, and Persistence

## 2 Introduction to Operation System

- Running Program fetches, decode, and executes
- Policy: eg. if two programs want to run at a particular time, which should run?
- Mechanisms: eg. the ability to run multiple programs at once

- Virtualizaion of memory: physical memory is a shared resource.
- atomic: all at once.
- GOALs: protection, isolation, reliability, energy-efficiency, security, mobility.

## 3. A Dialogue on Virtualization

- eg. the illusion of many virtual peaches, one peach for each person.

## 4 The abstraction: The Process

- process = running program.
- time sharing -> virtualizing CPU. Run as many process as user like.
- Mechanism: low-level methods or protocols that implement a needed piece of functionaliteg. Context-switch, time-sharing.
- Policy: algorithms for making some kind of decision within the OS. Eg. scheduling policy.
- **Machine state:** what a program can read or update when it is running.
  - memory, registers, PC(instruction pointer, which instruction of the prgram is currently being execute), stack pointer, frame pointer,

### 4.3 Process Creation

- Load code and static data into memory, in executable format. 
- Allocate memory for heap (for dynamically-allocated data), malloc(), free().
- Initialization tasks including IO

### 4.4 Process States

- Running (on a processor), Ready, Blocked.
- Ready to running: scheduled.

### 4.5 Data structure

- OS keeps process list.
- Register context be saved and restored -> context switch.

## 5 Interlude: Process API

#### 5.1 fork()

- PID: Process identifier.
- Fork() creates a child(rc==0) with new PID, own copy of the address space, register, PC, etc.
- getpid() returns process id.
- sequence is not **deterministic**. 
- Non-determinism leads to multi-threaded programs, which have concurrency problems.

#### 5.2 wait()

- Wait(): the parent process calls wait() to delay its execution until the child finishes executing. When the child is done, wait() returns to the parent.
- wait(NULL) wait the child process returns first, and then return the PID of child process.

#### 5.3 exec()

```c
char *myargs[3]; 
myargs[0] = strdup("wc"); // program: "wc" (word count) 
myargs[1] = strdup("p3.c"); // argument: file to count 
myargs[2] = NULL; // marks end of array
execvp(myargs[0], myargs); //run another program
```

- it loads code (and static data) from that executable and overwrites its current code segment (and current static data) with it
- the heap and stack and other parts of the memory space of the program are re-initialized. 
- Then the OS simply runs that program, passing in any arguments as the argv of that process.
- a successful call to exec() never returns.



## 6 Mechanism: Limited Direct Execution

- Centual concerns of virtualization: Performance and Control.
- **Limited Directed Execution:** make sure program not do anything harmful & OS can stop it for time sharing. (保护宝宝)
- **User mode**: can't isisue I/O, result in raising an exception
- **Kernel mode**: run in OS(kernel), do privileged operation.
- **system call**: when user mode want privileged operation. (execute trap instruction: into kernel, and return-from-trap instruction to return user-mode)

### Switching Between Processes

How can OS regain control of CPU?

- **Cooperative** Approach: wait for system calls
  - processes are assumed to periodicallt give up CPU.
- **Non-Cooperative** approach: OS takes control
  - timer interrupt. Raise interrupt periodically.
- **Context switch**: save register and stack pointer to kernel stack, and jump to trap handler.
- When timer interrupt, saved by HW; When OS decides to switch, saved by SW(ie the OS) into memory in the process structure of the process.
- 如果一个interrupt在执行，则disable interrupt，or locking.

## 7 Scheduling: introduction(policy)

- 7.1 Wordload assumptions: Run-time known, arrive at the same time, runs to completion, only use CPU
- 7.2 Scheduling metrics: $T_{turnaround} = T_{completion}-T_{arrival}$
- 7.3 FIFO(first in first out) or FCFS(First come first serve)：排队。
  - Convoy effect: 一个顾客占时太久，耽误大量小顾客。
- 7.4 Shortest Job First(SJF)
  - Optimal in theory. 但是如果小顾客稍微来晚点就也要等很久。
  - Non-preemptive.
- 7.5 Shortest Time-to-Completion First (STCF) / Preemptive Shortest Job First (PSJF)
  -  it can **preempt** job A and decide to run another job, perhaps continuing A later.
- 7.6 $T_{response} = T_{firstrun}-T_{arrival}$
- 7.7 Round Robin(time slicing).  instead of running jobs to completion, RR runs a job for a time slice (scheduling quantum) and then switches to the next job in the run queue.
  -  amortize(分摊） the cost

## 8 Scheduling: The Multi-Level Feedback Queue

- optimize turnaround time. (Shorter job first)
- minimize response time. 

####  How To Change Priority

- has a number of distinct queues, each assigned a different **priority level**.
- varies the priority of a job based on its observed behavior
- RULES
  - 1&2: Priority更小，run first；同样，RR。
  - 3: When job enters, placed at the highest priority.
  - 4A: If used entire time slice, priority reduced
  - 4B: If gives up CPU befure time slice, priority unchanged
  -  if an interactive job, for example, is doing a lot of I/O,  it will relinquish the CPU before its time slice is complete. Don't penalize it.
- Problems
  - starvation. Too many interactive jobs.
  -  game the scheduler. issue a I/O before the slce ends -> monoply CPU.

#### The Priority Boost

- RULE 5: periodically boost the priority of all the jobs: After some time period S, move all the jobs in the system to the topmost queue

#### Better Accounting

- RULE 4:  Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU), its priority is reduced (i.e., it moves down one queue).

## 12 A Dialogue on Memory Virtualization

-  every address generated by a user program is a virtual address
- the OS will give each program the view that it has a large contiguous address space to put its code and data into

## 13 The Abstraction: Address Spaces

- **multiprogramming**: multiple processes were ready to run at a giventime, and the OS would switch between them. -> time sharing
-  **address space**: the running program’s view of memory in the system.
- AS contains code, stack ,and heap.
- **virtualizing memory**. Eg. process A perform a load at address 0 -> virtual address.
  - All of address we see is virtual
- GOALS:
  - Transparency: program behaves like having own physical address
  - Efficiency: time and space.
  - Protection (isolation)

## 14 Interlude: Memory API

-  Long-lived memory: heap.
- allocate a pointer to an integer on the heap:

```C
#include <stdlib.h> 
void func() {
int *x=(int*)malloc(sizeof(int));
char* r = "hello";
char* s = (char*) malloc(strlen(s) + 1); //must be allocated
strcpy(s, r);
...
free(x);
}
```

- not enough memory: buffer overflow
- didn't initialized: uninitialized read.
- Not free memory: memory leak.
- free earlier than done with it: adangling pointer.
- double free

### Other calls

- calloc() allocates memory and also zeroes it be- fore returning
- realloc() makes a new larger region of memory, copies the old region into it, and returns the pointer to the new region.

## 15 Mechanism: Address Translation

### LDE: limited direct execution(LDE)

- **LDE**: let the program run directly on the hardware; however, at certain key points in time (such as when a process issues a system call, or a timer interrupt occurs), arrange so that the OS gets involved and makes sure the “right” thing happens.
- **address translation**: the hardware takes a virtual address the process thinks it is referencing and transforms it into a physical address which is where the data actually reside. (+base register)
- **dynamic relocation**:relocation of the address happens at runtime.
- **Base & bound registers**. Check within the bound first, to ensure it is legal. -> exception handler(into kernel mode) -> save base and bounds registers in **process control block**(PCB))
- **free list**: a data struction for finding room for new address space and mark as used.

## 16 Segmentation

- segmentation: With a base and bounds pair per segment, we can place each segment independently in physical memory. (code, heap, stack)
- **segmentation fault:** tried to refer to an illegal address, which is beyond the end of the heap. HW detects address out of bounds, traps into the OS.
- **virtual address = segment+offset**. Segment 00: code segment -> refers to the base&bound pair, and plus the offset.
- hardware also has a bit to indicate "growing positive".
- Protection bits: defines read-write-execute for code sharing.

#### External Fragmentation

- Compact: copy data to one contiguous region of memory. But too expensive and memory-intensive.

## 17 Free-Space Management

- **Internal fragmentation**: an allocator hands out chunks of memory bigger than that requested, any unasked for (unused) space in such a chunk.
- **Free list**: A linked list with each entry recording free space's address and length.
- **Splitting**: a free chunk of memory that can satisfy the request and split it into two. The first chunk it will return to the caller; the second chunk will remain on the list.
- **Coalescing**:  when returning a free chunk in memory, look carefully at the addresses of the chunk you are returning as well as the nearby chunks of free space; if the newly- freed space sits right next to one (or two, as in this example) existing free chunks, merge them into a single larger free chunk.
- Best fit, worst fit, first fit, next fit(a pointer pointing to the location last looked at)
- external fragmentation always exists.!!
- **Segregated List**:  having a chunk of memory dedicated for one particular size of requests. 
- **Slab allocator**: allocates a number of object caches for kernel objects that are likely to be requested frequently 根据frequency变更slabs大小
- **binary buddy allocator**：free memory 慢慢变大找2^N。-> internal fragment.

## 18 Paging: Introduction

- Segmentation ->  variable-sized pieces -> fragment problems
- Paging -> fixed-sized pieces(page) -> no external fragmentation
- **Page table**: store address translations for each of the virtual pages of the address space
- virtual address = VPN(virtual page number) 4 bits(16 pages) + Offset 4bits(16 bytes)
- PFN: physical frame number, PTE: page tabl entry.
- MMU: memory management Unit.  the part of the processor that helps with address translation.
- PTE构成：PFN + valid bit, protection bit, present bit(physical memory or disk), dirty bit(whether the page has been modified since it was brought into memory), reference bit( whether a page has been accessed, popular or not), user/supervisor bit (U/S)

## 19 Paging: Faster Translations (TLBs)

- TLB: translation-lookaside buffer. part of MMU.
- TLB hit: TLB holds the translation for this VPN. 
- TLB miss: If VPN is not found in TLB -> the hardware locates the page table in memory (using the page table base register) and **looks up the page table entry** (PTE) for this page using the VPN as an index. If the page is valid and present in physical memory, the hardware extracts the PFN from the PTE, installs it in the TLB, and **retries** the instruction -> TLB hit
- Eg. access an array: the first time will be a TLB miss, and the second time will be a TLB hit. When comes to new page, it will have a new TLB miss.
- TLB rate is high due to  spatial locality and temporal locality.
- A TLB entry = VPN | PFN | valid bit+protection bits+dirty bit
- WHen context switch, TLB's translations are only valid for the currently running process.
  - 方法1 flush the TLB。清空
  - 方法2 address space identifier (ASID), ≈ PID.

## 21 Beyond Physical Memory: Mechanisms

- **swap space**:  reserve some space on the disk for moving pages back and forth
- **page fault**: accessing a page that is not in physical memory
- When the OS receives a page fault for a page, it looks in the PTE to find the address, and issues the request to **disk** to fetch the page into memory
-  **page-replacement policy**: process of picking a page to kick out or replace when memory is full.
- **control flow for fetch data in memory**: 1. Access TLB; 2. look up page table for address in physical memory; 3. page fault, replace page in physical memory with page in disk.
- **low watermark** (LW ): when too many pages in memory.  high watermark (HW): evict pages until here.

## 22 Beyond Physical Memory: Policies

- **cache hits**:  the number of times a page that is accessed is found in memory.
-  **average memory access time** (AMAT) = (P_Hit · T_Mem )+(PMiss · T_Disk)
- **cold-start miss** (or compulsory miss): the cache begins in an empty state
- FIFO ≈ Random
- Least-Frequently-Used (LFU). & Least-Recently- Used (LRU).
- **Clock algorithm**: A clock hand points to some particular page to begin with (it doesn’t really matter which). When a replacement must occur, the OS checks if the currently-pointed to page P has a use bit of 1 or 0. If 1, this implies that page P was recently used and thus is not a good candidate for replacement. Thus, the use bit for P set to 0 (cleared), and the clock hand is incremented to the next page (P +1). The algorithm continues until it finds a use bit that is set to 0, implying this page has not been recently used.
- dirty bit: whether a page has been modified or not while in memory. VM systems prefer to evict clean pages over dirty pages. 因为evict不用重新write to disk.

