#pragma once
#include <iomanip>
#include <unordered_map>
#include <map>
#include <string>

#include "Lexeme.h"
#include "SourceText.h"


class Scanner
{
public:
	Scanner(const std::string& sourceFile);
	inline void Scan(std::ostream& out);
	inline Lexeme NextScan();
	inline Lexeme LookForward(int k);
private:
	inline void SkipIgnoreChars();
	inline void SkipComment();

	inline void HandleStringWord();
	inline void HandleDecNum();
	inline void HandleErrWord();
	inline void HandleDoubleChar(LexemeType firstLexeme, char nextChar, LexemeType secondLexeme);

	inline bool NextChar();
	void InputSourceText(const std::string& sourceFile);

	SourceText sourceText;
	SourceText::Iterator curPos;
	Lexeme _lexeme;

	static std::unordered_map<std::string, LexemeType> keywords;
	static const int MAX_LEXEME_SIZE = 100;
};

inline Lexeme Scanner::LookForward(int k)
{
	auto savePos = curPos;
	for(int i = 0; i < k - 1; i++)
		NextScan();
	auto lexeme = NextScan();
	curPos = savePos;
	return lexeme;
}

inline void Scanner::Scan(std::ostream& out)
{
	while (_lexeme.type != LexemeType::End) {
		SourceText::Iterator savePos;
		NextScan();
		out.width(9);
		out.flags(out.left);
		out << _lexeme.str << LexemeTypeToString(_lexeme.type) << " " << _lexeme.pos.row << ' ' << _lexeme.pos.column << std::endl;
	}

}


inline Lexeme Scanner::NextScan()
{
	_lexeme.str.clear();
	_lexeme.str.reserve(MAX_LEXEME_SIZE);

	SkipIgnoreChars();
	_lexeme.pos = curPos;

	if (*curPos == 0) {
		_lexeme.type = LexemeType::End;
	}

	else if (	'a' <= *curPos && *curPos <= 'z' ||
				'A' <= *curPos && *curPos <= 'Z' || 
				'_' == *curPos)
		HandleStringWord();
	else if ('0' <= *curPos && *curPos <= '9')
		HandleDecNum();
	else {
		switch (*curPos)
		{
		case ',':
			NextChar();
			_lexeme.type = LexemeType::Comma;
			break;
		case ';':
			NextChar();
			_lexeme.type = LexemeType::SemCol;
			break;
		case ':':
			NextChar();
			_lexeme.type = LexemeType::Colon;
			break;
		case '(':
			NextChar();
			_lexeme.type = LexemeType::OpenPar;
			break;
		case ')':
			NextChar();
			_lexeme.type = LexemeType::ClosePar;
			break;
		case '{':
			NextChar();
			_lexeme.type = LexemeType::OpenBrace;
			break;
		case '}':
			NextChar();
			_lexeme.type = LexemeType::CloseBrace;
			break;
		case '*':
			HandleDoubleChar(LexemeType::Mul, '=', LexemeType::Assign);
			break;
		case '/':
			HandleDoubleChar(LexemeType::Div, '=', LexemeType::Assign);
			break;
		case '%':
			HandleDoubleChar(LexemeType::Mod, '=', LexemeType::Assign);
			break;
		case '+':
			HandleDoubleChar(LexemeType::Add, '+', LexemeType::Inc);
			if (_lexeme.type == LexemeType::Add)
				HandleDoubleChar(LexemeType::Add, '=', LexemeType::Assign);
			break;
		case '-':
			HandleDoubleChar(LexemeType::Sub, '-', LexemeType::Dec);
			if (_lexeme.type == LexemeType::Sub)
				HandleDoubleChar(LexemeType::Sub, '=', LexemeType::Assign);
			break;
		case '>':
			HandleDoubleChar(LexemeType::RT, '=', LexemeType::RTE);
			if (_lexeme.type == LexemeType::RT)
				HandleDoubleChar(LexemeType::RT, '>', LexemeType::RShift);
			break;
		case '<':
			HandleDoubleChar(LexemeType::LT, '=', LexemeType::LTE);
			if (_lexeme.type == LexemeType::LT)
				HandleDoubleChar(LexemeType::LT, '<', LexemeType::LShift);
			break;
		case '=':
			HandleDoubleChar(LexemeType::Assign, '=', LexemeType::EQ);
			break;
		case '!':
			HandleDoubleChar(LexemeType::Err, '=', LexemeType::NE);
			break;
		case '&':
			HandleDoubleChar(LexemeType::Err, '&', LexemeType::And);
			break;
		case '|':
			HandleDoubleChar(LexemeType::Err, '|', LexemeType::Or);
			break;
		default:
			NextChar();
			_lexeme.type = LexemeType::Err;
		}
	}
	return _lexeme;
}


inline void Scanner::SkipIgnoreChars()
{
	while (curPos != sourceText.end())
	{
		switch (*curPos)
		{
		case '/': {
			auto tmpPos = curPos;
			++tmpPos;
			if (*tmpPos != '/')
				return;
			SkipComment();
			break;
		}
		case '\n': case '\r': case '\t': case ' ':
			++curPos;
			break;
		default:
			return;
		}
	}
}

inline void Scanner::SkipComment()
{
	++curPos;
	while (*curPos != 0 && *curPos != '\n')
		++curPos;
}

inline void Scanner::HandleStringWord()
{
	NextChar();
	while ('a' <= *curPos && *curPos <= 'z'
		|| 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9'
		|| '_' == *curPos)
	{
		if (!NextChar()) {
			HandleErrWord();
			return;
		}
	}
	auto keywordIt = keywords.find(_lexeme.str);
	if (keywordIt != keywords.end())
		_lexeme.type = keywordIt->second;
	else
		_lexeme.type = LexemeType::Id;
}

inline void Scanner::HandleDecNum()
{
	NextChar();
	while ('0' <= *curPos && *curPos <= '9')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.type = LexemeType::ConstInt;
}

inline void Scanner::HandleErrWord()
{
	while ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9' || '_' == *curPos)
	{
		NextChar();
	}
	_lexeme.type = LexemeType::Err;
}

inline void Scanner::HandleDoubleChar(LexemeType firstLexeme, char nextChar, LexemeType secondLexeme)
{
	NextChar();
	if (*curPos == nextChar)
	{
		NextChar();
		_lexeme.type = secondLexeme;
	}
	else
		_lexeme.type = firstLexeme;
}

inline bool Scanner::NextChar()
{
	const bool isLexemeOverflow = _lexeme.str.size() > MAX_LEXEME_SIZE;
	if (!isLexemeOverflow)
		_lexeme.str.push_back(*curPos);
	++curPos;
	return !isLexemeOverflow;
}