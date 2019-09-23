#include <fstream>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <iterator>
#include <queue>
#include <memory>
#include "Results.h"
#define char_int(c) ((int)c - (int)'a')

class BoggleImpl;

class BoggleSolver
{  
public:
	BoggleSolver(const char* DictionaryPath);
	~BoggleSolver();

	// movable:
	BoggleSolver(BoggleSolver && rhs) noexcept;
	BoggleSolver& operator=(BoggleSolver && rhs) noexcept;	
	Results FindWords(const char* board, unsigned width, unsigned height);

private:
	std::unique_ptr<BoggleImpl> boggleImpl;		
};





