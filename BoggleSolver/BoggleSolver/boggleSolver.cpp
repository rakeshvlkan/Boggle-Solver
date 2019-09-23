#include "boggleSolver.h"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class BoggleImpl
{

public:
	BoggleImpl(const char* dictPath) : m_dictionaryPath(dictPath)
	{
	}

	~BoggleImpl()
	{
		m_dictionaryTree.reset();
	}

	Results FindWords(const char* board, unsigned width, unsigned height);

private:

	// enum for word match
	enum match_word { partial_match, full_match, no_match };

	// grid node type
	enum nodeType { empty, used };

	// Trie datastructure indicating if current node is an ending of a word and map to next nodes
	struct Trie {
		std::unique_ptr<Trie> childs[26];
		bool leaf;
	};

	// board states are pushed into queue and processed one by one. nextIndex points to the next board char for a word
	struct BoardState {
		std::vector<bool> IsVisited;
		std::string word;
		Trie * startingNode;
		bool IsQNode;	// used for 'q' -> 'qu' checking as matchPath checks the last symbol
		int nextIndex;
	};

	const char* m_dictionaryPath;
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
	void prepareQueue(std::queue<BoardState> & queue, const char * board, unsigned width, unsigned height);
	unsigned int getScore(const std::string & word);
	bool isEmptyNode(int x, int y, int width, int height, const BoardState & node);
	bool isEmptyNode(int x, int y, int xmove, int ymove, int width, int height, const BoardState & node);
	match_word matchChar(char c, BoardState & node);
	match_word matchPath(BoardState & node);
	void processNeighbourNodes(int width, int height, const BoardState & node, std::queue<BoardState> & queue);
	void processQnode(const BoardState & node, std::queue<BoardState> & queue);
	void LoadDictionary();		
};


void BoggleImpl::insertToTrie(const std::unique_ptr<BoggleImpl::Trie>& trie, const std::string & key)
{
	Trie * root = trie.get();

	for (const auto symbol : key) {
		if (symbol < 'a' || symbol > 'z') continue;

		// if key exists, we change the current root, elsewise construct a new record
		auto c = char_int(symbol);

		if (root->childs[c]) {
			root = root->childs[c].get();
		}
		else {
			root->childs[c] = make_unique<Trie>();
			root = root->childs[c].get();
		}
	}

	root->leaf = true;
}

void BoggleImpl::prepareQueue(std::queue<BoardState>& queue, const char * board, unsigned width, unsigned height)
{
	// Clear the queue
	std::queue<BoardState> empty;
	std::swap(queue, empty);

	for (unsigned i = 0; i < width*height; i++) {
		BoardState state;
		state.IsVisited.resize(height*width);
		state.nextIndex = i;
		state.IsQNode = false;
		state.startingNode = m_dictionaryTree.get();
		queue.push(state);
	}
}

unsigned int BoggleImpl::getScore(const std::string & word)
{
	std::size_t len = word.length();

	if (len < MIN_WORD_SIZE) return 0;

	switch (len) {
	case 3: return 1; break;
	case 4: return 1; break;
	case 5: return 2; break;
	case 6: return 3; break;
	case 7: return 5; break;

	default: return 11;
	}
}

bool BoggleImpl::isEmptyNode(int x, int y, int width, int height, const BoardState & node)
{
	int index = y*width + x;

	return (
		x >= 0 &&
		x <= width - 1 &&
		y >= 0 &&
		y <= height - 1 &&
		node.IsVisited[index] == empty
		);
}

bool BoggleImpl::isEmptyNode(int x, int y, int xmove, int ymove, int width, int height, const BoardState & node)
{
	return isEmptyNode(x + xmove, y + ymove, width, height, node);
}

BoggleImpl::match_word BoggleImpl::matchChar(char c, BoardState & node)
{
	Trie * root = node.startingNode;

	if (root->childs[c]) {
		root = root->childs[c].get();
	}
	else {
		return no_match;
	}

	node.startingNode = root;
	if (root->leaf) {
		return full_match;
	}

	return partial_match;
}

BoggleImpl::match_word BoggleImpl::matchPath(BoardState & node)
{
	Trie * root = node.startingNode;

	// check 'q' node here and 'u' later on
	if (node.IsQNode) {
		node.IsQNode = false;
		auto c = char_int(node.word[node.word.length() - 2]);

		auto _match = matchChar(c, node);
		if (_match == full_match || _match == no_match) return _match;
		root = node.startingNode;
	}

	// just check the last symbol
	auto c = char_int(node.word.back());
	return matchChar(c, node);
}

void BoggleImpl::processNeighbourNodes(int width, int height, const BoardState & node, std::queue<BoardState>& queue)
{
	for (unsigned i = 0; i < 8; i++) {
		int x = node.nextIndex % width;
		int y = node.nextIndex / width;

		int xmove = m_neighbourIndices[i][0];
		int ymove = m_neighbourIndices[i][1];

		if (isEmptyNode(x, y, xmove, ymove, width, height, node)) {
			BoardState state = node;
			state.nextIndex = (y + ymove)*width + (x + xmove);
			queue.push(state);
		}
	}
}

void BoggleImpl::processQnode(const BoardState & node, std::queue<BoardState>& queue)
{
	BoardState quState = node;
	quState.IsQNode = true;
	quState.word += 'u';
	quState.IsVisited[node.nextIndex] = used;
	queue.push(quState);
}

// Load all the words fron dictionary and add them into the trie
void BoggleImpl::LoadDictionary() {
	m_dictionaryTree = make_unique<Trie>();

	std::ifstream dictionary(m_dictionaryPath);
	std::string word;

	while (std::getline(dictionary, word)) {
		insertToTrie(m_dictionaryTree, word);
	}
}

/*  Process the queue until it's empty, each time taking a node then
checking each neighbour if it's a valid word and not used path and then add it
to queue for processing. if a letter 'q' is found then a copy of current node is made and 'u' is
added so to find the 'QUAD' in 'QAD'
*/
Results BoggleImpl::FindWords(const char * board, unsigned width, unsigned height) {
	Results results;
	LoadDictionary();
	std::queue<BoardState> queue;
	std::unordered_set<std::string> foundWords;
	prepareQueue(queue, board, width, height);

	// process the queue
	while (!queue.empty()) {
		BoardState node = queue.front();
		queue.pop();

		char symbol = board[node.nextIndex];

		// only append the character if that path is correct
		if (node.IsVisited[node.nextIndex] == empty) {
			node.word += symbol;
		}

		// handle the 'q'-> 'qu' case by making a copy of current 'q' node, marking it and pushing it to the queue
		if (symbol == 'q' && node.IsVisited[node.nextIndex] == empty) {
			processQnode(node, queue);
		}

		node.IsVisited[node.nextIndex] = used;

		// if we matched the word, we just add it to the results, but it may be a part of a longer word so continue
		match_word match = matchPath(node);
		if (node.word.length() >= MIN_WORD_SIZE && match == full_match) {
			foundWords.insert(node.word);
		}

		if (match == full_match || match == partial_match) {
			processNeighbourNodes(width, height, node, queue);
		}
	}

	results.ComputeScore(foundWords);

	return results;
}

BoggleSolver::BoggleSolver(const char * dictionaryPath) : boggleImpl(new BoggleImpl(dictionaryPath))
{
}

BoggleSolver::BoggleSolver(BoggleSolver &&) noexcept = default;
BoggleSolver& BoggleSolver::operator=(BoggleSolver &&) noexcept = default;

Results BoggleSolver::FindWords(const char * board, unsigned width, unsigned height)
{  
	return boggleImpl->FindWords(board, width, height);
}

BoggleSolver::~BoggleSolver()
{
}