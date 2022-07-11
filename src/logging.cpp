#include "logging.h"

 std::map<std::thread::id, int> LogByThread::lookuptable = std::map<std::thread::id, int>();
 std::map<int, std::fstream *> LogByThread::logs = std::map<int, std::fstream *>();
 const int LogByThread::DEFAULTSTREAM = 999;