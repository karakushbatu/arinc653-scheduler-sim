# System Working Principle Report

## 1. Purpose of the Simulation
This simulation demonstrates the core ideas behind an ARINC 653-style partition scheduler in a compact, understandable C++ project. Its purpose is not to reproduce a certified avionics platform in full detail, but to show the main architectural ideas clearly:

- fixed time windows
- isolated partition behavior
- deterministic execution
- controlled fault handling

The result is suitable for learning, demonstration, and presentation.

## 2. System Components
The system is built from six main components:

- `IPartition`
- `FlightControl`
- `DisplaySystem`
- `Scheduler`
- `Logger`
- `HealthMonitor`
- `Config`

Together, these components model how a scheduler can run multiple partitions in a controlled order while preserving responsibility boundaries.

## 3. Role of Each Class
### `IPartition`
Defines the common execution contract for partitions. The scheduler only depends on this interface, which keeps orchestration separate from partition-specific logic.

### `FlightControl`
Represents the high-criticality partition. It owns its own private state such as altitude and velocity and updates that state deterministically on each cycle.

### `DisplaySystem`
Represents the low-criticality partition. It owns its own private display data and intentionally throws an exception on a configured cycle to simulate a controlled partition failure.

### `Scheduler`
Implements the major-cycle execution logic. It decides which partition runs, in what order, and for how long. It does not modify partition internals.

### `Logger`
Produces readable runtime output. It can include deterministic simulated timestamps, which makes logs easier to analyze without introducing random wall-clock variation.

### `HealthMonitor`
Stores partition fault records and prints a concise summary after execution. It acts as a small supervisory monitoring mechanism.

### `Config`
Stores simulation settings such as cycle count, timing windows, failure cycle, and logging options. It can also read values from a simple `config.txt` file.

## 4. Major Cycle Structure
The scheduler uses a **100 ms major cycle**:

- `Flight Control` runs first for `60 ms`
- `Display System` runs second for `40 ms`

This order repeats for each cycle. The simulation runs for at least five cycles by default.

The major cycle is a simplified version of a static schedule table. It reflects the ARINC 653 idea that CPU access is planned in advance rather than assigned dynamically.

## 5. Execution Flow Step by Step
1. `main.cpp` creates the configuration, logger, health monitor, and scheduler.
2. `FlightControl` and `DisplaySystem` objects are created through the `IPartition` abstraction.
3. The scheduler receives both partitions together with their window durations.
4. The scheduler starts cycle 1 and logs the cycle header.
5. It activates `Flight Control`, executes its logic, and waits until the 60 ms window is complete.
6. It activates `Display System`, executes its logic, and waits until the 40 ms window is complete.
7. The same fixed order repeats in the next cycle.
8. If the display partition reaches its configured failure cycle, it throws an exception.
9. The scheduler catches the exception, logs the failure, and records it in the health monitor.
10. Because the display partition is non-critical, the scheduler continues with the remaining cycles.
11. At the end, the health monitor prints a summary of any recorded failures.

## 6. How Time Partitioning Is Represented
Time partitioning is represented by the scheduler's static execution table:

- one major cycle
- two fixed windows
- constant partition order
- optional `sleep_for` timing emulation using `std::chrono`

This means that the runtime behavior is predictable before execution begins. Every cycle follows the same sequence, which is a key property of deterministic systems.

## 7. How Space Partitioning Is Represented
Space partitioning is represented through software structure rather than OS-level memory protection:

- partition state is private
- partitions expose behavior through a small interface
- the scheduler only calls `execute()`
- there are no shared global variables

`FlightControl` cannot directly read or write `DisplaySystem` internals, and the scheduler cannot modify either partition's private members. This models the isolation mindset used in partitioned systems.

## 8. How Fault Isolation Works
Fault isolation is demonstrated by the display partition failure scenario:

- the display partition throws a controlled exception on a selected cycle
- the scheduler catches the exception locally
- the system logs the fault without terminating the process
- the health monitor stores the fault context
- later cycles continue, so the flight-control partition remains operational

This is a simplified representation of containment. In real avionics systems, the platform would use stronger mechanisms than a C++ exception alone, but the educational idea is the same: isolate the fault and protect the rest of the system.

## 9. How Health Monitoring Works
The health monitor records:

- which partition failed
- in which cycle the failure happened
- the failure message
- whether the partition is critical

At the end of the simulation, it prints a compact summary. This provides traceability and makes abnormal behavior easy to discuss in a presentation.

## 10. Why the System Is Deterministic
The system is deterministic because:

- the execution order is fixed
- timing windows are fixed
- partition behavior is predefined
- there is no random input
- there is no concurrency
- simulated timestamps are derived from schedule progression, not real wall-clock time

The same configuration produces the same logical execution sequence and the same fault point every time.

## 11. How Configuration and Logging Are Handled
Configuration is intentionally lightweight. Default values are compiled into the `Config` structure, and an optional `config.txt` file can override them. This keeps the project easy to understand while still showing a configurable design.

Logging is handled by the `Logger` class. It prints:

- cycle headers
- active partition information
- partition faults
- final health monitor summaries

When timestamps are enabled, the logger uses simulated elapsed time such as `T+0200 ms`. This improves readability while staying deterministic.

## 12. Why the Architecture Is Aligned with ARINC 653 Ideas
This architecture aligns with ARINC 653 ideas in the following simplified ways:

- it uses a static major cycle
- it separates high- and low-criticality functions
- it keeps partition state isolated
- it contains a non-critical partition fault
- it preserves deterministic execution order

It does not claim full standard compliance, but it reflects the key conceptual goals of partitioned avionics software.

## 13. Limitations Compared to Real Avionics Systems
This project is an educational simulation, so it has clear limitations:

- it does not implement OS-level memory protection
- it does not implement real ARINC 653 processes inside each partition
- it does not include inter-partition communication services
- it uses exceptions for fault demonstration, which is simpler than real avionics fault handling
- it runs in user space on a general-purpose computer rather than on a certified RTOS

These limitations are acceptable for the assignment because the objective is to demonstrate understanding of the concepts with clean, explainable code.
