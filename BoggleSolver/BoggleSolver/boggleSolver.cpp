#include "boggleSolver.h"

// Insert the words into trie
void BoggleSolver::insertToTrie(const std::unique_ptr<Trie> & trie, const std::string & key) {
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

BoggleSolver::BoggleSolver()
{
}

// Load all the words fron dictionary and add them into the trie
void BoggleSolver::LoadDictionary(const char * path) {
	m_dictionaryTree = make_unique<Trie>();

	std::ifstream dictionary(path);
	std::string word;

	while (std::getline(dictionary, word)) {
		insertToTrie(m_dictionaryTree, word);
	}
}

// Delete contents from dictionary tree
void BoggleSolver::FreeDictionary() {
	m_dictionaryTree.reset();
}

BoggleSolver::~BoggleSolver()
{
}

// Delete the words 
void BoggleSolver::FreeWords(Results results) {
	for (unsigned int i = 0; i < results.Count; i++) {
		delete[] results.Words[i];
	}

	delete[] results.Words;
}

// Queue preparation consists of clearing passed queue and initializing the queue with needed elements for propagation
void BoggleSolver::prepareQueue(std::queue<BoardState> & queue, const char * board, unsigned width, unsigned height) {
	
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

// Check if node is within the matrix bounds and not visited
bool BoggleSolver::isEmptyNode(int x, int y, int width, int height, const BoardState & node) {
	int index = y*width + x;

	return (
		x >= 0 &&
		x <= width - 1 &&
		y >= 0 &&
		y <= height - 1 &&
		node.IsVisited[index] == empty
		);
}

bool BoggleSolver::isEmptyNode(int x, int y, int xmove, int ymove, int width, int height, const BoardState & node) {
	return isEmptyNode(x + xmove, y + ymove, width, height, node);
}

// Checks whether current char matches with current Trie node
match_word BoggleSolver::matchChar(char c, BoardState & node) {
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

// traverse the tree until the last symbol of a node and check if its a word
match_word BoggleSolver::matchPath(BoardState & node) {
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

// process neighbour nodes and if they are good candidate then push into a queue for later processing
void BoggleSolver::processNeighbourNodes(int width, int height, const BoardState & node, std::queue<BoardState> & queue) {
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

// clone the passed 'q' node, make it 'u' node and push it to queue
void BoggleSolver::processQnode(const BoardState & node, std::queue<BoardState> & queue) {
	BoardState quState = node;
	quState.IsQNode = true;
	quState.word += 'u';
	quState.IsVisited[node.nextIndex] = used;
	queue.push(quState);
}

unsigned int BoggleSolver::getScore(const std::string & word) {
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

// Merge all the results into Results structure. Update the count and score
void BoggleSolver::mergeResults(Results & results, const std::unordered_set<std::string> & foundWords) 
{
	
	char ** ptrWords = new char *[foundWords.size()];
	int index = 0;

	for (auto word : foundWords) {
		results.Count++;
		results.Score += getScore(word);

		char * copy = new char[word.length() + 1];
		strcpy(copy, word.c_str());

		ptrWords[index++] = copy;
	}
	results.Words = ptrWords;
}

/*  Process the queue until it's empty, each time taking a node then
    checking each neighbour if it's a valid word and not used path and then add it
    to queue for processing. if a letter 'q' is found then a copy of current node is made and 'u' is
    added so to find the 'QUAD' in 'QAD'
*/
Results BoggleSolver::FindWords(const char * board, unsigned width, unsigned height) {
	Results results = {};
	results.Score = 0;
	results.Count = 0;

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

	mergeResults(results, foundWords);

	return results;
}