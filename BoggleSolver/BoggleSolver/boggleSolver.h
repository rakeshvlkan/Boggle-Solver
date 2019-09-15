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
#define MIN_WORD_SIZE 3
#define char_int(c) ((int)c - (int)'a')

// enum for word match
enum match_word { partial_match, full_match, no_match };

// grid node type
enum nodeType { empty, used };

// Trie datastructure indicating if current node is an ending of a word and map to next nodes
struct Trie {
	std::unique_ptr<Trie> childs[26];
	bool leaf;
};

// Structure for storing results
struct Results {
	const char* const* Words;
	unsigned           Count;
	unsigned           Score;
	void*              UserData;
};

// board states are pushed into queue and processed one by one. nextIndex points to the next board char for a word
struct BoardState {
	std::vector<bool> IsVisited;
	std::string word;
	Trie * startingNode;
	bool IsQNode;	// used for 'q' -> 'qu' checking as matchPath checks the last symbol
	int nextIndex;
};

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class BoggleSolver
{  

private: 
	std::unique_ptr<Trie> m_dictionaryTree;

	// Array of Possible neighbours in a grid
	const int m_neighbourIndices[8][2] = {
		{ 1, 0 },
		{ -1, 0 },
		{ 0, 1 },
		{ 0,-1 },
		{ 1, 1 },
		{ 1,-1 },
		{ -1, 1 },
		{ -1,-1 }
	};

private:
	void insertToTrie(const std::unique_ptr<Trie> & trie, const std::string & key);
	void mergeResults(Results & results, const std::unordered_set<std::string> & foundWords);
	void prepareQueue(std::queue<BoardState> & queue, const char * board, unsigned width, unsigned height);
	unsigned int getScore(const std::string & word);
	bool isEmptyNode(int x, int y, int width, int height, const BoardState & node);
	bool isEmptyNode(int x, int y, int xmove, int ymove, int width, int height, const BoardState & node);
	match_word matchChar(char c, BoardState & node);
	match_word matchPath(BoardState & node);
	void processNeighbourNodes(int width, int height, const BoardState & node, std::queue<BoardState> & queue);
	void processQnode(const BoardState & node, std::queue<BoardState> & queue);

public:
	BoggleSolver();
	void LoadDictionary(const char* path);
	Results FindWords(const char* board, unsigned width, unsigned height);
	void FreeWords(Results results);
	void FreeDictionary();
	~BoggleSolver();
};





