# CS 111

[TOC]



## 1 Interface Stability

#### The Criticality of Interfaces in Architecture

-  defines the major components of the system, the functionality of each, and the interfaces between them. 先规定好，再独自设计，然后交互
-  A software interface specification is a form of contract.
- "Posix file semantics". 不遵守的话会发生很多问题。

#### Interfaces vs. Implementations

- Interfaces are fixed all the time.
- Implementations  are flexible.

#### Program vs. User Interfaces

- Human beings are amazingly robust.
- Code is nowhere nearly so robust.
- you have to trust that what ever platform they run it on will correctly implement all of the interfaces on which your program depends.

#### Is every interface carved in stone?

- Interface Polymorphism (Version number)
- Versioned Interfaces（Backwards comptibility）
- specific support commitments (eg support for next 5 years)

#### Interface Stability and Design

- upwards-compatible way
-  consider all of the different types of change that are likely to happen over the life of this syste

## 2 Software Interface Standards

#### 现实因素

- The cost of building different versions of an application for different hardware platforms
- Computer sales were driven by the applications they could support.
- Software Portability 影响产品生死。

#### 发展

- Client changed into normal public.
  - New applications have to work on both old and new devices. 
  - Software upgrades cannot break existing applications.
- GOAL: whatever combination of software components wind up on a particular device will work together
- SOLUTION: detailed specifications & comprehensive compliance testing.

#### Software Interface Standardization

**优点**

- details well considered
- platform-neutral
- very clear and complete specifications
-  freedom to explore alternative implementations

**缺点**

- constrain the range of possible implementations
- constraints on their consumers
- many different stake-holders => difficult to evolve standards

#### 问题1 Confusing interfaces with Implementations

- interface standards should not specify design. Rather they should specify behavior
- interface specifications are written after the fact

#### 问题 2 The rate of evolution, for both technology and applications

#### Proprietary vs Open Standards

- A Proprietary interface is one that is developed and controlled by a single organization
-  An Open Standard is one that is developed and controlled by a consortium of providers and/or consumers (e.g. the IETF network protocols)

#### API: Application Programming Interfaces

-  they are written at the source programming level (e.g. C, C++ , Java, Python). eg. write()
- API specifications are a basis for software portability, but applications must be recompiled for each platform
- +++ an application written to a particular API should easily recompile and correctly execute on any platform that supports that API
- +++  any application written to supported APIs should easily port to their platform.
- ---  the API must be defined in a platform-independent way (to have the above benefits)
  - Eg defined some type as int (implicitly assuming that to be at least 64 bits wide) might not work on a 32 bit machine. 
  - accessed individual bytes within an int might not work on a big-endian machine. 
  -  assumed a particular feature implementation (e.g. fork(2)) might not be implementable on some platforms (e.g. Windows)

#### ABI: Application Binary Interfaces

<u>**An Application Binary Interface is the binding of an Application Programming Interface to an Instruction Set Architecture.**</u>

Contains:

- binary representation of key data types 
- call to and return from a subroutine 
- Stack-frame structure
-  parameters, return values, registers, system calls, and signal deliveries.
-  formats of load modules, shared objects, and dynamically loaded libraries

#####  portability benefits +++

The resulting binary load module should correctly run, unmodified, on any platform that supports that ABI.

Used by compiler, linkage editor, program loader, operating system, or programmer who writes assembly language subroutines.

## 3 Object Modules, Linkage Editing, Libraries 

### components of software tool chain

#### Compiler (-> source.s)

- Reads source modules and included header files, parses the input language (e.g. C or Java), and infers the intended computations, for which it will generate lower level code (assembly code). 

#### Assembler (-> object.o)

-  each line of code often translating directly to a single **machine language** instruction or data item.
- allows the declaration of variables, the use of macros, and references to externally defined code and data
- func & local symbols not have yet been assigned hard in-memory addresses. Only be expressed as offsets

#### Linkage editor  (static library.a -> executable)

- 把所有东西都放进virtual address space.

-  reads a specified set of object modules, placing them consecutively into a virtual address space, and noting where (in that virtual address space) each was placed.
- searches a specified set of libraries to find object modules that can satisfy those references, and places them in the **evolving virtual address space** as well.
- The resulting bits represent a program that is ready to be loaded into memory and executed

#### Program loader

-  part of the operating system
- examines the information in a load module, creates an appropriate virtual address space, and reads the instructions and initialized data values from the load module into that virtual address space.
- map shared libraries into VAS.

### object modules

#### executable (load) modules

- the code (machine language instructions) within an object module are Instruction Set Architecture (ISA) specific. 
- ELF (Executable and Linkable Format) makes the formats common across ISAs.
  - header section
  - Code and Data section
  - symbol table (for external)
  - A collection of relocation entries

#### relocatable object modules

- incomplete. They make references to code or data items that must be supplied from other modules. 
- not yet been determined where (at which addresses). Only have relative address.

### Libraries

- Libraries are not always orthogonal and independent. (可以call 其他libraries)
- 顺序： If we call a function from library A, and library A calls functions from library B, we may need to search library A before searching library B。

### Linkage Editing

 **filling in all of the linkages (connections) between the loaded modules (by linkage editor, ld(1)):**

- Resolution: search the specified librarie。
- Loading: text & data -> single virtual address space
- Relocation: go through all of the relocation entries, correctly reflect the chosen addresses.

### Load Modules

- ≈ object modules,  contains multiple sections (code, data, symbol table).
- complete and requires no relocation.
- load new program (exec):
  - Consults the load module
  - Allocates the appropriate segments in VAS
  - Reads the contents from LM to memory
  - Creates a stack segment
- have a symbol table to receive an exception.

### Shared Libraries (run-time loadable)

- Reserve an address for each shared library. 
- SL -> read-only code segment.
- Assign a number (0-n) to each routine + put redirection table at the beginning of that shared segment
- stub library, that defines symbols for every entry point in the shared library
- Linkage edit the client program with the stub library.
- When run,  OS notices that the program requires shared libraries, open the associated (sharable, read-only) code segments, and map into address space. (in  ld.so(8))

**优点**

- single copy implementation
- version controlled by a library path environment variable
- program not affected by changes in the sizes of library routines or the addition of new modules.
- stub modules ->  one shared library to make calls into another.

**缺点**

- read-only code -> no static data
- cannot make any static calls or reference global variables to/in the client program.

### Dynamically Loaded Libraries

- delay the loading of a module until it is actually needed. 
- User mode: dlopen(3). OS mode:  dynamically load device drivers or file system implementations.
  - +++ pre-compiled applications can exploit plug-ins to support functionality that was not implemented.
- Calls from the client application into a shared or Dynamically Loaded library are generally handled through a table or vector of entry points

**Process:**

- program choose and load that library into its address space
-  calls the supplied initialization entry point, app and DLL bind to each other
- requests services from the DLL by making calls through the dynamically established vector of service entry points.
- When doesn't need,  calls the shut-down method, and un-load.

### SL vs DLL

- linkage-edit time, run time.
- SL  consume memory for the entire time, DLL can unload.
- SL impose numerous constraints and simple interfaces on the code. DLL can perform complex initialization and support complex (bi-directional) calls between the client application and library. 
- SL is indistinguishable from static library. DLL  require considerably more work (from the client application) to load the library, register the interfaces, and establish work sessions.
- SL +++ efficiently binding libraries to client applications. DLL +++  extend the functionality of a client application

## 4 Stack Frames and Linkage Conventions

### Stack

- Last-In-First-Out
- 自动化 automatically allocated & automatically deallocated
- 封闭性 only visible to code within that procedure
- ^ Long lived resources in Heaps.

### Subroutine Linkage Conventions

- ISA  specific.
- subroutine linkage contains: 
  - parameter passing
  - subroutine call(save the return address)
  - register saving
  - allocating space for the local variables

### Traps and Interrupts

#### Procedure vs interrupts

- Procedure expects function performed and returned value.
- interrupts are initiated and defined by HW.
- computer state should be restored after interrupts.

#### Interrupt mechanism

- TRAP vector table: 连接对应的possible external interrupt/execution exception, Program Counter, 和Processor Status.
- CPU在table里load新的PC/PS，并push到CPU stack，并继续新的PC指定位置的execution
- First level handler save registers, and choose appropriate second level handler.
- Second level handler deals with the event, and return to FLH, which restores registers, execute 'return', reload PC/PS, and resumes execution at the interruption.
- **优点**：translates HW-driven call to FLH (higher level language) -> SLH.
- 缺点：user mode -> processor mode, new address space(context switch), 会很慢，并loss of CPU caches.

## 5 Real Time Scheduling

### Real-Time System

- Correctness depending on: Timeliness, predictability, feasibility, hard real-time, soft real-time.
- Know the length of tasks.
- Starvation of low prority task is OK.
- Work-load be relatively fixed, to simultaneously achieve high utilization & good response time(就像高速同时车流很大而不堵车)

### Scheduling Algorithms

#### No scheduler: 排队

#### Static scheduling：fixed schedule

#### Dynamic scheduling

##### Preempty

问题

- 中断运行的task会miss ddl
- task fixed, 没必要
- infinite loop很少



## 6 Garbage Collection and Defragmentation 

### Garbage Collection

- seeking out no-longer-in-use resources and recycling
- Commands: close(), free(), delete, return, exit.
- 共享资源时：Resouce manager: automatically free the object when the reference count reaches zero.
- 取消手动档：reference can be copied or deleted without notify the OS; 有些语言减少难度；太久没用自动删；记录allocation/release太多太费时。
- 方法：建一个list of existed resources, 从头到尾只要是reachable resource，就remove; 最后free所有还在list上的
- 缺点：电脑可能一直运行流畅直到突然中断来做GC

### Defragmentation

- reassigning and relocating previously allocated resources to eliminate external fragmentation
- **Coalescing** is only effective if adjacent memory chunks happen to be free at the same time.
- In SSD, erasure can only be done in large(64MB) units. 所以先找到一块，把在用的4K blocks移到新的地方，再删这块并加入free list
- Clean up the free space（像汉诺塔）：移出所有有用的block，清理，再把blocks放回去，达成连续，并不断重复。

## 7 Working Sets

- LRU成功原因：most programs exhibit some degree of temporal and spatial locality (Only for single program/process).
- 和Round-Robin互相排斥，因为RR runs periodic.
- RR时采用per-process LRU。
- Thrashing：The dramatic degradation in system performance associated with not having enough memory to efficiently run all of the ready processes。内存变少，Page fault变多。
- working set size：增加number of page frames对表现没啥影响，而减少则可见的变差。
- WS大小：不同process以及不同时候都不一样，可以根据page faults数量来观察：minimize page faults(overhead), maximize throughput(efficiency).

### Implementation

- 类似Clock Algorithm：循环pages，如果最近被referenced，记下现在accumulated time，并标记为非referenced；如果没有，则算它的age，如果大于target age则return（否则小于就换算是thrashing）。
- Age取决于accumulated CPU time，只有当ower运行并没有referencing它的时候才增加。
- 如果没有任何小于target age的，就too many process了。replace oldest或者减少process。

### Dynamic Equilibrium to the rescue

- page stealing algorithm
- Every process is continuously losing pages and stealing pages.
- working set会根据reference page的多少和频率来变大变小。



## 8 Inter-Process Communication

### Process Interactions

-  **coordination of operations** with other processes
  - Synchronization: mutexes, condition variables
  - Signals exchange: kill
  - control operation: fork, wait, ptrace
- **data exchange** between processes
  - Uni-Directional streams
  - Bi-directional interactions

### Uni-Directional Byte Streams

- program accepts a byte-stream input, and produces a byte-stream output (**suitable input to the next**), and works independently
- byte streams are unstructured, 可被存进file再放入pipeline
- Pipes are temporary files with a few special features -> recognize the difference between a file and an inter-process data stream
  - 如果reader用尽了pipe的data，但还有open write file descripter, 这个reader就没得到EOF。要么reader blocked直到更多data来，要么writer blocked
  - Available buffering of pipe may be limited -> **FLOW CONTROL**： 如果writer太快，就block writer直到reader追上来
  - 如果给没有open read file descriptor的pipe写，则illegal并SIGPIPE
  - reader writer both closed, the file automatically deleted
- Pipeline -> closed system, no authentication or encrypttion.

### Named Pipes

- A named pipe can be used as a **rendezvous** point for unrelated processes.
- Readers and writers cannot authenticate each other
- Writes from multiple writers may be intersperse，分不清bytes从哪里来
- They do not enable clean fail-overs from a failed reader to its successor
- All readers and writers must be running on the same node

### Mailboxes

- Each write is stored and delivered as a distinct message
- Each write 伴随sender的authentication identification
- Unprocessed messages remain in the mailbox after the death of a reader and can be retrieved by the next reader.
- Still subject to single node/single operating system restrictions

### General Network Connections

- Linux **API:** socket(IPC end-point + protocol + data model), bind(socket + local network address), connect, listen. accept, send, recv
- options: byte streams over reliable connections (e.g. TCP) , best effort datagrams (e.g. UDP)
- Higher level: Remote Procedure Calls, RESTful service models, Publish/Subscribe services
- Complexities: Interoperablity, security, noeection and node failures, server address

### Shared Memory

- High performance of IPC = efficiency + trhoughput + latency
- Best performance: not buffering, but Shared Memory:
  - create a file -- each process maps the file into VAS(virtual address space) --  shared segment locked-down and never paged out -- process agree on data structures(eg. poll) in SS --anything written to SS will be immediately available.
- LIMITATION: Only for processes on same memory bus; a bug in one process can destroy the communication; No authentication of data from which process.

### Network Conntection and Out-of-Band Signals

- Allow an important message to go directly to front of the line.
- **Out-of-band**: Sending a signal that could invoke a registered handler, and flush (without processing) all of the bufferred data -> different channel from buffer, recipient was local
- 多信道：one heavily used channel for normal requests + another reserved for out-of-band requests。 Periodically polls the out-of-band channel -> allow preempting queued operations.



## 9 User-Mode Thread Implementation

### Thread

- is an independently schedulable unit of execution. 
- runs within the address space of a process. 
- has access to all of the system resources owned by that process. 
- has its own general registers. 
- has its own stack (within the owning process' address space).

- a process is a container for an address space and resources. a thread is the unit of scheduled execution.

### Simple Threads Library (User Mode)

- Create: allocate memory for thread-private stack from the heap, create thread descriptor with identification, scheduling, pointer to stack
- Yield or Sleep: save general registers, remove from ready queue(sleep), select next thread on queue
- Dispatch: restore saved registers and return to ready queue
- Exit: free stack and descriptor
- **Scheduling**: SIGALARM as timer signals, can interrupt(yield) thread running too long.
- Sigprocmask: temporarily block signals

### Kernel implemented threads

Solves the following problems:

- when a thread blocks, all threads(within that process) stop executing. OS cannot know thrds
- OS cannot execute threads in parallel on multiple cores if OS not aware of multiple thrds.
- futex(7) approach: uses an atomic instruction to attempt to get a lock in user-mode, call OS if failed.

### Performance

- **Light weight thread**: non-preenptive scheduling下user-mode更快，不用context switch
- Multi-processor下kernel-mode更快，加大throughput大于损失的context switch



## 10 Deadlock Avoidance

- 不能避免的情况：
  - mutual exclusion is fundamental. 
  - hold and block are inevitable. 
  - preemption is unacceptable. 
  - the resource dependency networks are imponderable.
- Deadlock例子：
  - main memory is exhausted. 
  - we need to swap some processes out to secondary storage to free up memory. 
  - swapping processes out involves the creation of new I/O requests descriptors, which must be allocated from main memory.
  - some process will free up resources when it completes, but the process needs more resources in order to complete.
- **Deadlock Avoidance**: keep track of free resources, and refuse to grant requests that would put the system into a dangerously resource-depleted state

### Reservations

- 运行中拒绝requests不好handle，所以ask processes to reserve resources before actually need them.
- **sbrk**: 没有allocate更多memory，而是让OS改变data segment的大小in VAS。直到refer新pages才allocate.
- 如果能确定在over-tex memory，就可以抛出error来deal，而不是真正运行时kill process
- 类似情况reservation：闪存少->refuse create new file, thrashing -> refuse new process, network traffic saturate agreement -> refuse crease or bind socket

### Over-Booking

- Grant more reservations than actually resources we have -> **Relative-safe**
- 情况：airlines; network brandwidth handle 125% traffic and maintain 99% performance
- kill random process太可怕，所以OS一般under-book(10%)

### Dealing with Rejection

failed with clean error -> process can try to manage in the most graceful way:

- A **simple** program might log an error message and exit. 
- A **stubborn** program might continue retrying the request (in hope tht the problem is transient). 
- A **more robust** program might return errors for requsts that cannot be processed (for want of resources), but continue trying to serve new requests (in the hope that the problem is transient). 
- A **more civic-minded** program might attempt to reduce its resource use (and therefore the number of requests it can serve).



### 11 Health Monitoring and Recovery





## Lec 1

### Operating System

#### What it does

- Manage hardware for programs
- abstracts the hardware (work directly with HW)
- provides new abstractions for applications (via abstractions)
- constrained by many non-technical factors

Applcations SW --binary interface--> system services/libraries --system call--> OS  --privileged inst--> HW

### Instruction Set Architetures(ISAs)

privileged vs general

- any code can execute gerneral
- processor must be in a special mode to execute privileged

### Types of OS Resources

#### Serially reusable resources 轮流用

- one at a time
- access control, exclusive use
- graceful transitions

#### Partitionable resoures 有限共享

- access control
  - Containment
  - Privacy
- graceful transitions (not permanently allocated)

#### Sharable resources 无限共享

- don't wait or own
- invlove limitless resources

## Lec 2

### Abstractions

- CPU/Memory abstractions
  - Process, thread, virtual machines
  - Virtual address apaces, shared segments
- Persistent storage abstractions
  - Files and file systems
- I/O abstractions
  - Virtual terminal sessions, windows
  - Sockets, pipes, VPNs, signals (as interrupts)

### Services: Higher level abstractions

#### 分类

- Cooperating parallel processes
- Security
- User interface

#### 性质

- not directly visible
- Enclosure management
- Software updates, configuration registry
- Dynamic resource allocation and scheduling
- Networks, protocols, domain services

#### 方法

Applications do: (each in different layers)

- #### call subroutines (functions -> other program)

  - jump, return values in registers on the stack
  - √ Fast, run-time binding
  - ^ all services in same address space
  - ^ limited ability to combine different languages
  - ^ cannot usually use privieged inst (diff programs have diff limited address space(?))

- #### via libraries

  - one subroutine delivery approach
  - a library is a collection of object modules(a single file)
  - √ standard utility

- #### make system calls

  - force an entry into OS
  - √ able to allocate/use new/privileged resources
  - √ able to share/communicate with other processes
  - ^ implemented on local node
  - ^ much slower than subroutine calls

- #### via Kernel

  - 主要是functions require privilege
    - Privilege instr (interrupt, IO)
    - allocate physic resource (memory)
    - ensuring privacy and containment
    - ensuring integrity of critical resource
  - operation may be out-sourced (?)
  - plugins may be less trusted

- #### Outside Kernel

  - Not all trusted code must be in the kernel
  - Some are actually somewhat privileged process
  - some are merely trusted

- #### send messages to SW to perform the services

  - exchange messages with a server(via sys calls)
  - √ Server can be  anywhere on earch
  - √ sevice highly scalable and available
  - √ user-mode code ok
  - ^ 1000x slower than subroutine
  - Limited ability to operate on process resources

- #### via Middleware

  - SW is key part of application or service platform, but not of OS
  - Kernel code is expensive and dangerous
  - User mode is easy to build, test, debug. portable, crash and restarted

### Library

#### Static

- include in load module at link time
- each load module has its own copy of each library
- must relink to incorporate new library

#### Shared

- map into address space at exec time (linker needs to know where the library is)
- One in-memory copy, shared by all processes
- OS loads library along with program(运行程序时才加载library)
- √ save memory
- √ faster program start-up (如果已经在memory里，不用再次加载）
- √ simplified  updates
- ^ cannot include global data storage
- ^ added into program memory (不管需不需要）
- ^ called routines must by known at compile-time (只是把load延迟到运行时间而已）
- ^ Dynamically loadable libraries are more general

#### Dynamic

- choose and load at run-time
- Load only the library functions are called, 方法只在被调用时才load

### API (Application Program Interfaces)

- A source level interface
- An API compliant program will compile & run on any compliant system (for programmer)
- Normal libraries(shared and otherwise) and DLL are accessed through API

### ABI (Applcation Binary Interfaces)

- A binary interface, specifying dynamically loadable libraries(DLL), data formats, calling sequences, linkage conventions
- the binding of an API to a HW architecture(ISA)
- An ABI compliant program will run(unmodified) on any compliant system (for users)
- The dynamic loading mechanism is ABI-specific

## Lec 3

### Process

- Characterized by its properties (state) & operations.

#### State

- A mode or condition of being
  - Scheduling priority of a process 
  - Current pointer into a file 
  - Completion condition of an I/O operation 
  - List of memory pages allocated to a process 

#### Process address space

- A process’ address space is made up of all memory locations that the process can address.
- code + data + stack.

#### Code segment

- Code must be loaded into memory.
- Code segments are read/execute only and sharable.

#### Data segment

-  must be initialized in address space
- read/write, and process private
- can grow/shrink (sbrk())

#### Stack segment

- Each procedure call allocates a new stack frame
- Size of stack depends on program activities
- OS manages the process’ stack segment
- read/write and process private

#### Process descriptor

- Stores all information relevant to the process
  - State to restore (ID, state, pointer, priority ...)
  -  References to allocated resources 
  - Information to support process operations

#### other process state ( not in PD)

- on the stack & in registers
- linux has supervisor-mode stack.
  -  retain the state of in-progress system calls 
  - save the state of an interrupt preempted process

## Lec 4

### Scheduling

- GOALS: throughput(operations/second), average time-to-completion, fairness, priority, real-time deadline, response time
- Preempty vs non-preempty
- graceful degradation: degraded performance / rejecting work.
- Hard & soft ddl
- clock interrupt: ensure that runaway process doesn't keep control forever. -> preemptive scheduling
- starvation
- Hard & soft priority

