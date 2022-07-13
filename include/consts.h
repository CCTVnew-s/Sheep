#ifndef CONSTS_HEADER
#define CONSTS_HEADER

enum class CalculationLevel {Top = 0 ,Date = 100, Symbol = 200, SnapTime = 300, Signal = 400, Unknown = 999}; //  Signal Level shouldn't be used, at Date level, means you're working on certain date e.g. 2021.01.01, you will loop over symbols of the date

enum class MemoryLifeCyle{Global, Task, SingleTaskPhase};


#endif