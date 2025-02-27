#pragma once

#include <vector>
#include <iostream>
//#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

enum ActivationType
{
	ReLU,
	LReLU,
	Sigmoid,
	Tanh,
    SoftMax
};