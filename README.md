# ARINC 653-Style Partition Scheduler Simulation

## 1. Project Title
ARINC 653-Style Partition Scheduler Simulation

## 2. Author Information
- **Author:** Batuhan Karakuş
- **Program:** TUSAŞ SKY Experience Program

## 3. Project Overview
This project simulates a simplified ARINC 653-style partition scheduler in modern C++. The goal is to demonstrate how a deterministic major-cycle scheduler can manage multiple software partitions with fixed execution windows, isolated internal state, and fault containment behavior.

The project was built as an educational and presentation-ready engineering assignment. It focuses on clarity, explainability, and clean object-oriented design rather than on framework complexity. The simulation models two avionics-inspired partitions:

- **Flight Control** as the high-criticality partition
- **Display System** as the low-criticality partition

The implementation demonstrates:

- deterministic scheduling
- time partitioning
- space partitioning
- fault isolation
- basic health monitoring
- modular C++ class design

## 4. ARINC 653 Background
ARINC 653 is a software interface standard used in Integrated Modular Avionics (IMA) systems. Its purpose is to let multiple avionics applications with different criticality levels run on the same computing platform without interfering with each other.

The standard is strongly associated with two core partitioning ideas:

- **Time partitioning:** each partition receives a predefined CPU time window.
- **Space partitioning:** each partition is isolated in memory and internal state.

This matters in avionics because modern aircraft host many functions on shared computing resources. A flight-critical function such as flight control must not lose deterministic access to processing time because of a lower-criticality function such as a display refresh task. Likewise, a fault in one partition should be contained so that it does not corrupt another partition's data or control flow.

## 5. Time Partitioning in This Project
This simulation uses a fixed **major cycle of 100 ms**. Inside each major cycle, the scheduler always executes partitions in the same order:

1. **Flight Control** for **60 ms**
2. **Display System** for **40 ms**

This schedule repeats for at least five cycles. The order never changes, and there is no random or dynamic dispatch decision. That makes the behavior deterministic and easy to analyze.

From an ARINC 653 perspective, this represents a simplified static schedule table. The scheduler enforces the order and timing windows, while each partition only executes its own internal logic.

## 6. Space Partitioning in This Project
Space partitioning is represented through private class state and clear ownership boundaries:

- `FlightControl` stores private state such as `altitudeMeters_` and `velocityMetersPerSecond_`
- `DisplaySystem` stores its own private `screenData_`
- `Scheduler` does not directly modify partition internals
- no shared global variables are used

This is important because it mirrors the safety argument behind partitioned systems: one subsystem should not be able to accidentally overwrite or manipulate the state of another subsystem. Even though this project is a software simulation rather than a memory-protected RTOS, the class design reflects the same system thinking.

## 7. Fault Isolation Strategy
The project introduces a controlled failure in the **Display System** partition on a configured cycle. When that cycle is reached, the partition throws an exception.

The scheduler wraps partition execution in `try-catch` blocks. If a failure occurs:

- the fault is caught safely
- the error is logged
- the failed partition and cycle number are recorded by the health monitor
- the overall program continues
- the Flight Control partition keeps running in later cycles

This models the basic idea of fault containment. A non-critical partition fault is visible, traceable, and isolated, but it does not bring down the full system.

## 8. Health Monitoring
The `HealthMonitor` class keeps a list of partition failures. Each record contains:

- partition name
- cycle number
- fault message
- criticality

At the end of execution, the health monitor produces a concise summary. This makes the system easier to inspect during testing or presentation and reflects the role of supervisory fault reporting in safety-oriented platforms.

## 9. Design Decisions
### Why this class structure?
The class design keeps responsibilities separate:

- `IPartition` defines the contract for all partitions
- `FlightControl` and `DisplaySystem` implement partition-specific behavior
- `Scheduler` controls execution order and window timing
- `Logger` produces deterministic readable output
- `HealthMonitor` stores and reports failures
- `Config` centralizes timing and simulation settings

### Why OOP?
Object-oriented design is useful here because the scheduler should work with partitions through a common interface rather than with hard-coded behavior. This improves readability, extensibility, and presentation clarity. A new partition type could be added later with minimal scheduler change.

### Why single-threaded?
The simulation is intentionally single-threaded because the assignment emphasizes deterministic scheduling and explainability. Real concurrent execution would add complexity without improving the learning objective.

### Why configurable timing and logging?
The project includes a lightweight `Config` structure and optional `config.txt` support to keep values adjustable without introducing complex parsing. Logging includes deterministic simulated timestamps to make the output easier to follow while still remaining repeatable.

## 10. Project Structure
```text
.
|-- CMakeLists.txt
|-- README.md
|-- .gitignore
|-- include
|   |-- Config.h
|   |-- DisplaySystem.h
|   |-- FlightControl.h
|   |-- HealthMonitor.h
|   |-- IPartition.h
|   |-- Logger.h
|   `-- Scheduler.h
|-- src
|   |-- DisplaySystem.cpp
|   |-- FlightControl.cpp
|   |-- HealthMonitor.cpp
|   |-- Logger.cpp
|   |-- Scheduler.cpp
|   `-- main.cpp
`-- docs
    `-- system_working_principle_report.md
```

## 11. Build Instructions
### Requirements
- CMake 3.10 or newer
- A C++ compiler with C++11 support or later

### Build Steps
```bash
cmake -S . -B build
cmake --build build
```

## 12. Run Instructions
After building, run the executable:

```bash
./build/arinc653_scheduler_sim
```

On Windows with a multi-configuration generator, use:

```bash
.\build\Debug\arinc653_scheduler_sim.exe
```

Optional configuration is supported through a simple `config.txt` file in the project root. If the file does not exist, default values are used.

This means the project **still runs normally without `config.txt`**. In that case, the built-in defaults from `Config` are used automatically.

Example:

```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=0
display_failure_cycle=3
timestamps_enabled=true
emulate_timing=true
```

### How to Use `config.txt`
The program automatically looks for a file named `config.txt` in the repository root. The easiest workflow is:

1. Start from [config.txt.example](C:/Users/Batu/Documents/GitHub/arinc653-scheduler-sim/config.txt.example)
2. Copy one profile block from that file
3. Paste only that block into a new `config.txt` in the project root
4. Run the executable again
5. Observe how the execution trace changes

The supported keys are:

- `cycle_count`: how many major cycles the scheduler will run
- `major_cycle_ms`: total major cycle duration
- `flight_control_window_ms`: Flight Control window duration
- `display_window_ms`: Display System window duration
- `flight_control_failure_cycle`: which cycle should trigger the Flight Control fault, `0` means disabled
- `display_failure_cycle`: which cycle should trigger the display fault
- `timestamps_enabled`: `true` or `false`
- `emulate_timing`: `true` or `false`

Important validation rule:

- `flight_control_window_ms + display_window_ms` must equal `major_cycle_ms`

### Config Scenarios You Can Try
#### 1. Move the display fault earlier
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=0
display_failure_cycle=2
timestamps_enabled=true
emulate_timing=true
```
Expected effect:

- the error appears during cycle 2 instead of cycle 3
- Flight Control still continues in later cycles

#### 2. Run more cycles
```text
cycle_count=8
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=0
display_failure_cycle=5
timestamps_enabled=true
emulate_timing=true
```
Expected effect:

- the schedule repeats for 8 cycles
- the fault summary is still reported once

#### 3. Turn timestamps off
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=0
display_failure_cycle=3
timestamps_enabled=false
emulate_timing=true
```
Expected effect:

- logs remain deterministic
- the `T+xxxx ms` prefix disappears

#### 4. Disable real waiting for faster testing
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=0
display_failure_cycle=3
timestamps_enabled=true
emulate_timing=false
```
Expected effect:

- the printed schedule remains the same
- the program finishes quickly because it does not sleep for each window

#### 5. Change window distribution while preserving the major cycle
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=70
display_window_ms=30
flight_control_failure_cycle=0
display_failure_cycle=3
timestamps_enabled=true
emulate_timing=true
```
Expected effect:

- the partition order stays the same
- simulated timestamps change according to the new static windows
- the schedule is still valid because `70 + 30 = 100`

#### 6. Trigger a critical Flight Control fault
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=4
display_failure_cycle=0
timestamps_enabled=true
emulate_timing=true
```
Expected effect:

- Flight Control fails in cycle 4
- the fault is recorded as critical
- the scheduler stops after detecting the critical failure
- later cycles do not run

#### 7. Compare non-critical and critical fault behavior together
```text
cycle_count=5
major_cycle_ms=100
flight_control_window_ms=60
display_window_ms=40
flight_control_failure_cycle=4
display_failure_cycle=2
timestamps_enabled=true
emulate_timing=true
```
Expected effect:

- Display fails in cycle 2 and the system continues
- Flight Control fails in cycle 4 and the scheduler stops
- this demonstrates mixed-criticality fault handling clearly

## 13. Example Output
```text
[T+0000 ms] [INFO] Scheduler started.
[T+0000 ms] [INFO] [Cycle 1]
[T+0000 ms] [INFO] Active partition: Flight Control | window=60 ms
Flight Control running | cycle=1 | altitude=1012.5 m | velocity=249.2 m/s
[T+0060 ms] [INFO] Active partition: Display System | window=40 ms
Display System running | cycle=1 | screenData="Primary Flight Display frame updated for cycle 1"
[T+0100 ms] [INFO] [Cycle 2]
[T+0100 ms] [INFO] Active partition: Flight Control | window=60 ms
Flight Control running | cycle=2 | altitude=1025 m | velocity=248.4 m/s
[T+0160 ms] [INFO] Active partition: Display System | window=40 ms
Display System running | cycle=2 | screenData="Primary Flight Display frame updated for cycle 2"
[T+0200 ms] [INFO] [Cycle 3]
[T+0200 ms] [INFO] Active partition: Flight Control | window=60 ms
Flight Control running | cycle=3 | altitude=1037.5 m | velocity=247.6 m/s
[T+0260 ms] [INFO] Active partition: Display System | window=40 ms
[T+0260 ms] [ERROR] Display System ERROR: Display refresh failure detected.
[T+0500 ms] [INFO] Health Monitor Summary: 1 failure(s) recorded.
[T+0500 ms] [ERROR] Fault #1 | partition=Display System | cycle=3 | critical=no | message=Display refresh failure detected.
[T+0500 ms] [INFO] Scheduler finished.
```

## 14. Verified Build and Run
This project was verified with the following local toolchain:

- **Generator:** Visual Studio 17 2022
- **Compiler:** MSVC 19.44.x
- **Build type used for validation:** `Debug`

Validation commands:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' -S . -B build -G "Visual Studio 17 2022" -A x64
& 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build --config Debug
.\build\Debug\arinc653_scheduler_sim.exe
```

Observed runtime behavior during verification:

- the scheduler executed 5 full cycles
- execution order stayed fixed as `Flight Control -> Display System`
- the display partition failed on cycle 3 as designed
- the failure was logged and recorded by `HealthMonitor`
- the system continued because the failed partition was non-critical
- the run ended with a one-fault summary

## 15. Possible Future Improvements
- Add a third partition to demonstrate mixed-criticality growth.
- Replace simple exception handling with policy-based recovery actions.
- Add unit tests for scheduler order, health monitor records, and configuration validation.
- Export fault reports to a text or CSV file for offline analysis.
- Extend the model toward ARINC 653 process scheduling inside each partition.

## 16. Theoretical Questions and Answers

### A. IMODE Fundamentals
#### What is an Interface in IMODE?
In IMODE-style software thinking, an interface is the defined interaction boundary between components. It describes how one module, subsystem, or service can be used by another without exposing internal implementation details. In practical engineering terms, an interface answers questions such as:

- what operations are available
- what inputs are accepted
- what outputs are produced
- what rules or contracts must be respected

An interface is therefore not only a technical connection point but also a design contract. It allows teams to reason about systems in terms of responsibilities and dependencies instead of tightly coupled code.

#### At least two purposes of an interface
**1. Decoupling components**  
An interface reduces direct dependency on concrete implementations. If one component only knows the interface, the internal implementation can change without forcing changes in all consumers. This makes maintenance easier and reduces ripple effects.

**2. Supporting modular development**  
Interfaces let different parts of a system be developed, tested, and reasoned about independently. One team can implement a subsystem while another integrates with its agreed contract.

**3. Improving testability**  
Interfaces make it easier to replace real components with test doubles or mock implementations. This is useful when validating behavior in isolation.

**4. Clarifying responsibilities**  
A good interface defines what a component should do and what it should not expose. This improves system understanding and reduces accidental misuse.

In this project, `IPartition` plays exactly that role. The scheduler knows how to run a partition, but it does not know or manipulate the internal state of `FlightControl` or `DisplaySystem`.

### B. Java Fundamentals
#### Explain the difference between JDK, JRE, and JVM in detail
**JVM (Java Virtual Machine)**  
The JVM is the execution engine that runs Java bytecode. It is responsible for loading classes, verifying bytecode, managing memory, and executing instructions. It provides platform independence because the same bytecode can run on different operating systems as long as a compatible JVM exists for that platform.

**JRE (Java Runtime Environment)**  
The JRE is the runtime package needed to run Java applications. It includes:

- the JVM
- core Java class libraries
- supporting runtime files

If a user only wants to run Java software and does not need to compile code, the JRE is the relevant runtime environment.

**JDK (Java Development Kit)**  
The JDK is the full development toolkit for Java. It includes:

- the JRE
- the JVM
- compiler tools such as `javac`
- debugging and development utilities

If a developer wants to write, compile, debug, and package Java applications, the JDK is required.

#### How they relate to one another
The relationship can be summarized as:

- **JDK** contains the tools to develop Java programs
- **JRE** contains the environment to run Java programs
- **JVM** is the engine inside the runtime that actually executes bytecode

So the hierarchy is:

**JDK -> JRE -> JVM**

#### Source code -> compilation -> bytecode -> JVM execution flow
The Java execution model typically works as follows:

1. A developer writes Java source code in `.java` files.
2. The Java compiler `javac`, which is part of the JDK, compiles the source code.
3. The compiler produces platform-independent `.class` files containing Java bytecode.
4. At runtime, the JVM loads the bytecode.
5. The JVM verifies the bytecode for safety and correctness rules.
6. The JVM interprets or JIT-compiles the bytecode into native machine instructions.
7. The program executes on the host operating system.

This model is important because Java does not compile directly to one operating system's machine code during normal compilation. Instead, it compiles to bytecode first, and the JVM provides the platform abstraction layer that makes Java portable.

### C. ARINC 653 Concepts
#### Why are time partitioning and space partitioning critical in ARINC 653 systems?
These two mechanisms are critical because avionics systems are safety-sensitive, mixed-criticality, and strongly timing-dependent.

**Time partitioning** guarantees that each hosted function receives CPU access during predefined execution windows. A high-criticality function does not have to compete unpredictably with a lower-priority or faulty function. This supports deterministic timing analysis and certification-oriented reasoning.

**Space partitioning** ensures that one function cannot corrupt another function's internal memory or state. Even if a subsystem behaves incorrectly, the damage should remain contained within its assigned partition boundary.

#### What real avionics problem do they solve?
Modern aircraft consolidate many functions onto shared computing modules. Without partitioning, several dangerous problems can appear:

- one application may consume too much CPU time
- one faulty module may corrupt another module's memory
- certification becomes harder because interference paths are unclear
- mixed-criticality software becomes unsafe to host on common hardware

ARINC 653 solves this by creating predictable and isolated execution environments. It enables shared platforms without sacrificing the safety expectations of critical applications.

#### Safety, predictability, fault containment, and mixed-criticality explanation
**Safety**  
Aircraft functions such as flight guidance, control, navigation, and crew displays do not all have the same safety impact. Partitioning ensures that critical functions remain protected from less critical ones.

**Predictability**  
Certification and systems engineering require knowing when software will run and how it will behave under normal and abnormal conditions. Static time windows make execution patterns analyzable.

**Fault containment**  
If one subsystem fails, the system should prevent that failure from spreading. Space partitioning and supervisory monitoring help localize faults.

**Mixed-criticality support**  
An aircraft may host mission-critical, safety-critical, and convenience-related software together. Partitioning allows these to coexist on the same platform while maintaining controlled separation.

This project captures those concepts in simplified form: a fixed major cycle, private partition state, a contained low-criticality display failure, and continued operation of the flight-control partition.
