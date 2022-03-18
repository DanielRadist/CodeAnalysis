#include <fstream>
#include <sstream>

#include "Scanner.h"

 std::unordered_map<std::string, LexemeType> Scanner::keywords = {
	{"switch", LexemeType::Switch},
	{"default", LexemeType::Default},
	{"case", LexemeType::Case},
	{"break", LexemeType::Break},
	{"int", LexemeType::Int},
	{"short", LexemeType::Short},
	{"long", LexemeType::Long},
	{"const", LexemeType::Const},
	{"main", LexemeType::Main},
	{"bool", LexemeType::Bool},
	{"return", LexemeType::Return},
	{"if", LexemeType::If},
	{"else", LexemeType::Else},
	{"true", LexemeType::ConstBool},
	{"false", LexemeType::ConstBool}
 };

Scanner::Scanner(const std::string& sourceFile)
{
	InputSourceText(sourceFile);
	curPos = sourceText.begin();
}



void Scanner::InputSourceText(const std::string& sourceFile)
{
	std::ifstream fin(sourceFile);
	std::stringstream sb;
	sb << fin.rdbuf();
	auto source = sb.str();
	source.push_back(0);
	sourceText = source;
}
