#include "Results.h"

Results::Results() : m_wordsCount(0),m_TotalScore(0)
{

}

Results::~Results()
{

}

void Results::ComputeScore(std::unordered_set<std::string> foundWords)
{		
	for (auto word : foundWords) {
		m_wordsCount++;
		m_TotalScore += getScore(word);
		m_words.push_back(word);		
	}	
}

unsigned int Results::getWordsCount()
{
	return m_wordsCount;
}

unsigned int Results::getTotalScore()
{
	return m_TotalScore;
}

std::vector<std::string> Results::getWords()
{
	return m_words;
}

unsigned int Results::getScore(const std::string & word)
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