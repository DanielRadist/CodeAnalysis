#pragma once
#include <map>
#include <string>

enum class LexemeType : unsigned int
{
	Id = 1,
	Main = 2,
	Return = 3,
	Switch = 4,
	Case = 5,
	If = 6,
	Else = 7,
	Const = 8,
	Break = 9,
	Default = 10,

	ConstInt = 11,
	ConstBool = 12,

	Short = 20,
	Int = 21,
	Long = 22,
	Bool = 23,
	True = 24,
	False = 25,

	Add = 30,		// +
	Sub = 31,		// -
	Mul = 32,		// *
	Div = 33,		// /
	Mod = 34,		// %
	Assign = 35,	// =
	Inc = 36,		// ++
	Dec = 37,		// --
	NE = 38,		// !-
	LT = 39,		// <
	RT = 40,		// >
	LTE = 41,		// <=
	RTE = 42,		// >=
	EQ = 43,		// ==

	OpenPar = 44,
	ClosePar = 45,
	OpenBrace = 46,
	CloseBrace = 47,

	Comma = 48,		// ,
	SemCol = 49,	// ;
	LShift = 50,	// <<
	RShift = 51,	// >>
	Colon = 52,		// :

	And = 53,		// &&
	Or = 54,		// ||

	End = 100,		// \0
	Err = 999		// error
};

enum class DataType
{
	Int, Short, Long, Bool, Void, Unknown
};

enum class SemanticType
{
	Func, Data, Empty, Const
};

inline std::string LexemeTypeToString(LexemeType code) {
	std::map<LexemeType, std::string> lexicalStrings = {
		{LexemeType::Id, "Id"},
		{LexemeType::Main, "Main"},
		{LexemeType::Return, "Return"},
		{LexemeType::Switch, "Switch"},
		{LexemeType::Case, "Case"},
		{LexemeType::Break, "Break"},
		{LexemeType::Default, "Default"},
		{LexemeType::If, "If"},
		{LexemeType::Else, "Else"},
		{LexemeType::Const, "Const"},

		{LexemeType::ConstInt, "ConstInt"},
		{LexemeType::ConstBool, "ConstBool"},

		{LexemeType::Short, "Short"},
		{LexemeType::Int, "Int"},
		{LexemeType::Long, "Long"},
		{LexemeType::Bool, "Bool"},
		{LexemeType::True, "True"},
		{LexemeType::False, "False"},

		{LexemeType::Add, "Add"},
		{LexemeType::Sub, "Sub"},
		{LexemeType::Mul, "Mul"},
		{LexemeType::Div, "Div"},
		{LexemeType::Mod, "Mod"},
		{LexemeType::Assign, "Assign"},
		{LexemeType::Inc, "Inc"},
		{LexemeType::Dec, "Dec"},
		{LexemeType::NE, "NE"},
		{LexemeType::LT, "LT"},
		{LexemeType::RT, "RT"},
		{LexemeType::LTE, "LTE"},
		{LexemeType::RTE, "RTE"},
		{LexemeType::EQ, "EQ"},
		{LexemeType::OpenPar, "OpenPar"},
		{LexemeType::ClosePar, "ClosePar"},
		{LexemeType::OpenBrace, "OpenBrace"},
		{LexemeType::CloseBrace, "CloseBrace"},
		{LexemeType::Comma, "Comma"},
		{LexemeType::SemCol, "SemCol"},
		{LexemeType::LShift, "LShift"},
		{LexemeType::RShift, "RShift"},
		{LexemeType::Colon, "Colon"},

		{LexemeType::And, "And"},
		{LexemeType::Or, "Or"},

		{LexemeType::End, "End"},
	};
	
	return lexicalStrings.at(code);
}


inline std::string DataTypeToString(DataType type) {
	std::map<DataType, std::string> semStrings = {
		{DataType::Bool, "Bool"},
		{DataType::Unknown, "Unknown"},
		{DataType::Int, "Int"},
		{DataType::Long, "Long"},
		{DataType::Short, "Short"},
		{DataType::Void, "Void"},
	};
	
	return semStrings.at(type);
}

inline DataType LexemeStringToDataType(const std::string& lexStr)
{
	std::map<std::string, DataType> semStrings = {
		{"bool", DataType::Bool},
		{"int", DataType::Int},
		{"long", DataType::Long},
		{"short", DataType::Short},
		{"void", DataType::Void},
	};
	return semStrings.at(lexStr);
}