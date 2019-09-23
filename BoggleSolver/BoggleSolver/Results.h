#pragma once
#include<vector>
#include<unordered_set>
#include<iostream>
#define MIN_WORD_SIZE 3

class Results
{
public:
	Results();
	~Results();
	void ComputeScore(std::unordered_set<std::string> foundwords);
	unsigned int getWordsCount();
	unsigned int getTotalScore();
	std::vector<std::string> getWords();

private:
	unsigned int getScore(const std::string & word);

private:
	std::vector<std::string> m_words;
	unsigned int m_wordsCount;
	unsigned int m_TotalScore;

};

