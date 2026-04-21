
## UML Class Diagram
Sistemin yapısını görsel olarak özetlemek için şu UML diyagramı kullanılabilir:

```mermaid
classDiagram
    class IPartition {
        <<interface>>
        +getName() string
        +isCritical() bool
        +execute(cycleNumber) string
    }

    class FlightControl {
        -failureCycle_ size_t
        -altitudeMeters_ double
        -velocityMetersPerSecond_ double
        +getName() string
        +isCritical() bool
        +execute(cycleNumber) string
    }

    class DisplaySystem {
        -failureCycle_ size_t
        -screenData_ string
        +getName() string
        +isCritical() bool
        +execute(cycleNumber) string
    }

    class Scheduler {
        -config_ Config
        -logger_ Logger&
        -healthMonitor_ HealthMonitor&
        -schedule_ vector
        -simulatedTime_ milliseconds
        +addPartition(partition, windowMs)
        +run()
    }

    class Logger {
        -timestampsEnabled_ bool
        +logInfo(message, simulatedTime)
        +logError(message, simulatedTime)
        +logCycleHeader(cycleNumber, simulatedTime)
    }

    class HealthMonitor {
        -faultRecords_ vector
        +recordFailure(partitionName, cycleNumber, message, critical)
        +hasFailures() bool
        +hasCriticalFailures() bool
        +logSummary(logger, simulatedTime)
    }

    class Config {
        +cycleCount size_t
        +majorCycleMs int
        +flightControlWindowMs int
        +displayWindowMs int
        +flightControlFailureCycle size_t
        +displayFailureCycle size_t
        +timestampsEnabled bool
        +emulateTiming bool
        +fromFile(path) Config
        +validate()
    }

    IPartition <|.. FlightControl
    IPartition <|.. DisplaySystem
    Scheduler --> IPartition : executes
    Scheduler --> Logger : logs through
    Scheduler --> HealthMonitor : reports faults
    Scheduler --> Config : uses timing
    HealthMonitor --> Logger : writes summary
```

Bu diyagramı anlatırken şu kısa çerçeve kullanılabilir:

- `IPartition`, ortak contract'tır.
- `FlightControl` ve `DisplaySystem`, bu contract'ı implement eder.
- `Scheduler`, merkezi orkestrasyon sınıfıdır.
- `Logger` ve `HealthMonitor`, destek servisleri gibi çalışır.
- `Config`, sistem davranışını dışarıdan ayarlamak için kullanılır.

