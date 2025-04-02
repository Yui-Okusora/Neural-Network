#pragma once


//---------- IO ----------

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iomanip>

//------------------------

//------- Threads --------

#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <shared_mutex>
#include <mutex>

//------------------------

//--- Data Structure -----

#include <vector>
#include <queue>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

//------------------------

//------- Others ---------

#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>
#include <Windows.h>
#include <chrono>
#include "Timer.hpp"

//------------------------

//---- Self-Defined ------

#ifndef NeuroSoraCore_ThreadPool
#include "ThreadPool/ThreadPool.hpp"
#endif
