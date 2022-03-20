#include "CodeAnalysis.h"

#include <iostream>
#include <stack>
#include "CodeAnalysisException.h"
#include "Semantics/Node/DataNode.h"


void SyntaxAnalyser::Program()
{
	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::End) 
	{
		if (IsType(lex.type))						// либо объ. данных, либо объ. функции
		{
			auto isID = scanner->LookForward(2);
			if (isID.type != LexemeType::Id && isID.type != LexemeType::Main)
				WrongExpected("идентификатор", isID);

			auto isLBr = scanner->LookForward(3);
			if (isLBr.type == LexemeType::OpenPar)	// (
				FuncDecl();
			else
				DataDecl();							// Данные
		}
		else if (lex.type == LexemeType::Const)
		{
			ConstDecl();							// глобальные константы
		}
		else
			WrongType(lex);
		lex = scanner->LookForward(1);
	}
	std::cout << "--- Pre END code ---" << std::endl;
	std::cout << "Semantic tree:\n";
	semTree->Print(std::cout);

	semTree->DeleteAllTree();

	std::cout << "--- Post END code --" << std::endl;
	std::cout << "Semantic tree:\n";
	semTree->Print(std::cout);
}

// Объявление функции
void SyntaxAnalyser::FuncDecl()
{
	auto lexType = scanner->NextScan();				// Тип
	if (!IsType(lexType.type))
		WrongExpected("Тип", lexType);

	auto lex = scanner->NextScan();					// Id, Main

	if (!semTree->CheckUniqueIdentifier(lex.str))	// Проверяем уникальность id
		RedefinitionError(lex);

	const auto funcNode = semTree->AddFunc(
		LexemeStringToDataType(lexType.str), 
		lex.str);									// создаем узел функци в дереве

	if (lex.type == LexemeType::Id)
	{
		lex = scanner->NextScan();					// (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
		Params(funcNode);
	}
	else if (lex.type == LexemeType::Main)
	{
		lex = scanner->NextScan();					// (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
	}
	else
		WrongId(lex);

	lex = scanner->NextScan();						// )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);

	CompStat();

	semTree->SetCurrentNode(funcNode);				// добавляем в дерево узел
}

void SyntaxAnalyser::DataDecl()
{
	auto lex = scanner->NextScan();								// Тип
	if (!IsType(lex.type))
		WrongType(lex);

	const auto leftType = LexemeStringToDataType(lex.str);		// Лексему в тип данных
	do
	{
		lex = scanner->NextScan();								//Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		if (!semTree->CheckUniqueIdentifier(lex.str))			// 
			RedefinitionError(lex);

		const auto varNode = semTree->AddData(leftType, lex.str);//	создаем узел

		lex = scanner->NextScan();								// '=', ',', ';'

		if (lex.type == LexemeType::Assign) 
		{
			const auto rightType = Expr();						// Получаем тип правой части

			if (!semTree->CheckCastable(rightType, leftType))	// проверяем на совместимость типов
				UncastableError(leftType, rightType, lex);

			semTree->SetVariableInitialized(varNode);			// сообщаем о иниализации переменной

			lex = scanner->NextScan();							//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	if (lex.type != LexemeType::SemCol)
		WrongExpected(";", lex);
}


// объявление параметров функции
void SyntaxAnalyser::Params(Node* funcNode) const
{
	auto lex = scanner->LookForward(1);
	if (!IsType(lex.type))
		return;

	while (true) 
	{
		lex = scanner->NextScan();						// Type
		if (!IsType(lex.type))
			WrongType(lex);

		const auto type = LexemeStringToDataType(lex.str);	// dataType

		lex = scanner->NextScan();						// Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		semTree->AddParam(funcNode, lex.str, type);		// Добавить переменную в функцию как параметр

		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::Comma)
			return;

		lex = scanner->NextScan();						// ,
	}
}

// Объявление констант
void SyntaxAnalyser::ConstDecl()
{
	scanner->NextScan();
	auto lex = scanner->LookForward(1);							// тип
	if (!IsType(lex.type))
		WrongType(lex);

	const auto leftType = LexemeStringToDataType(lex.str);		// Лексему в тип данных

	scanner->NextScan();
	lex = scanner->LookForward(1);								// Id
	if (lex.type != LexemeType::Id)
		WrongId(lex);

	if (!semTree->CheckUniqueIdentifier(lex.str))				// проверка уникальности
		RedefinitionError(lex);

	const auto constNode = semTree->AddData(leftType, lex.str);	// создаем узел
	semTree->SetDataConst(constNode);							// ставим флаг узлу что это константа

	scanner->NextScan();

	lex = scanner->LookForward(1);								// =
	if (lex.type != LexemeType::Assign)
		WrongExpected("=", lex);

	scanner->NextScan();
	lex = scanner->LookForward(1);								// ConstInt / ConstBool
	if (lex.type != LexemeType::ConstInt && lex.type != LexemeType::ConstBool)
		WrongExpected("Константа", lex);

	const auto rightType = semTree->GetDataTypeOfNum(lex);
	if (!semTree->CheckCastable(rightType, leftType))			// Проверка приводимости типов
		UncastableError(leftType, rightType, lex);
	semTree->SetVariableInitialized(constNode);

	scanner->NextScan();

	lex = scanner->LookForward(1);								// ;

	if (lex.type != LexemeType::SemCol)
		WrongExpected(";", lex);
	scanner->NextScan();
}

// Оператор
void SyntaxAnalyser::Stat()
{
	auto lex = scanner->LookForward(1);
	if (IsType(lex.type))
		DataDecl();								// объявление данных
	else if (lex.type == LexemeType::OpenBrace)
		CompStat();								// вызов составного оператор
	else if (lex.type == LexemeType::Break)		// break
	{
		scanner->NextScan();
		Stat();
	}
	else if (lex.type == LexemeType::Switch)
		Switch();								// switch
	else if (lex.type == LexemeType::Return)
	{
		scanner->NextScan();
		Expr();
	}
	else {
		if (lex.type != LexemeType::SemCol)
			Expr();
		lex = scanner->NextScan();				// ;
		if (lex.type != LexemeType::SemCol)
			WrongExpected(";", lex);
	}
}

void SyntaxAnalyser::CompStat()
{
	auto lex = scanner->NextScan();				// {
	if (lex.type != LexemeType::OpenBrace)
		WrongExpected("{", lex);

	const auto saveNode = semTree->AddEmpty();	// Создаем пустой узел
	semTree->AddScope();						// идем ниже

	lex = scanner->LookForward(1);
	while (lex.type != LexemeType::CloseBrace)
	{
		if (lex.type == LexemeType::Const)		// объявление константы
			ConstDecl();
		else
			Stat();								// оператор
		lex = scanner->LookForward(1);
	}
	lex = scanner->NextScan();					// }

	semTree->SetCurrentNode(saveNode);			// вышли из составного, переходим на уровень выше
	
	// Удаляем узлы после выхода за состовной оператор
	std::cout << "-----  -Pre-   -----" << std::endl;
	std::cout << "Semantic tree:\n";
	semTree->Print(std::cout);

	semTree->DeleteSubTree(saveNode);

	std::cout << "-----  -Post-  -----" << std::endl;
	std::cout << "Semantic tree:\n";
	semTree->Print(std::cout);
	std::cout << "====================" << std::endl;
}

void SyntaxAnalyser::Switch()
{
	auto lex = scanner->NextScan();				// switch

	auto lexTmp = scanner->LookForward(1);		// (
	if (lexTmp.type != LexemeType::OpenPar)
		WrongExpected("(", lexTmp);
	lex = scanner->NextScan();

	auto nodeSwitch = semTree->AddEmpty();		// пустой узел
	semTree->AddScope();						// на уровень ниже

	Expr();										// выражение

	lexTmp = scanner->LookForward(1);			// )
	if (lexTmp.type != LexemeType::ClosePar)
		WrongExpected(")", lexTmp);
	lex = scanner->NextScan();

	lexTmp = scanner->LookForward(1);			// {
	if (lexTmp.type != LexemeType::OpenBrace)
		WrongExpected("{", lexTmp);
	lex = scanner->NextScan();

	lex = scanner->LookForward(1);				// Case
	scanner->NextScan();
	while (lex.type == LexemeType::Case)
	{
		auto nodeCase = semTree->AddEmpty();	// пустой узел
		semTree->AddScope();					// на уровень ниже

		lex = scanner->LookForward(1);			// ConstInt
		if (lex.type != LexemeType::ConstInt && lex.type != LexemeType::ConstBool)
			WrongExpected("Константа", lex);

		scanner->NextScan();
		lex = scanner->LookForward(1);			// :
		if (lex.type != LexemeType::Colon)
			WrongExpected(":", lex);

		scanner->NextScan();
		lex = scanner->LookForward(1);			// оператор
		while (
			lex.type != LexemeType::CloseBrace &&
			lex.type != LexemeType::Default &&
			lex.type != LexemeType::Case)
		{
			Stat();
			lex = scanner->LookForward(1);
		}
		lex = scanner->NextScan();

		semTree->SetCurrentNode(nodeCase);	// возврат на уровень выше
	}

	if (lex.type == LexemeType::Default)		// defalt
	{
		auto nodeCase = semTree->AddEmpty();	// пустой узел
		semTree->AddScope();					// на уровень ниже

		lex = scanner->LookForward(1);			// :
		if (lex.type != LexemeType::Colon)
			WrongExpected(":", lex);

		scanner->NextScan();

		lex = scanner->LookForward(1);			// оператор
		while (
			lex.type != LexemeType::CloseBrace &&
			lex.type != LexemeType::Default &&
			lex.type != LexemeType::Case)
		{
			Stat();
			lex = scanner->LookForward(1);
		}
		scanner->NextScan();

		semTree->SetCurrentNode(nodeCase);	// возврат на уровень выше
	}
	if (lex.type != LexemeType::CloseBrace)
		WrongExpected("}", lex);

	semTree->SetCurrentNode(nodeSwitch);
}


DataType SyntaxAnalyser::Expr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();									// Scan Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)			// проверка индентификтора данных
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)			// попытка присвоение функции
			AssignToFuncError(lex);


		const auto leftType = node->GetDataType();					// Получить тип данных левой части
		const auto leftTypeData = node->GetSemanticType();

		lex = scanner->NextScan();									// Scan =

		const auto rightType = EqualExpr();							// Получить тип из дерева правого выражения

		if (!semTree->CheckCastable(rightType, leftType))			// если не приводятся или попытка присвоить константе
			UncastableError(leftType, rightType, lex);
		if (leftTypeData == SemanticType::Const)
			UncastableConstError(leftType, rightType, lex);

		semTree->SetVariableInitialized(node);

		return leftType;
	}
	return OrExpr();
}

DataType SyntaxAnalyser::OrExpr()
{
	auto leftType = AndExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Or)
	{
		scanner->NextScan();
		const auto rightType = AndExpr();

		leftType = CheckOperationResult(leftType, rightType, lex);

		lex = scanner->LookForward(1);
	}

	return leftType;
}

DataType SyntaxAnalyser::AndExpr()
{
	auto leftType = EqualExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::And)
	{
		scanner->NextScan();
		const auto rightType = EqualExpr();

		leftType = CheckOperationResult(leftType, rightType, lex);

		lex = scanner->LookForward(1);
	}

	return leftType;
}



DataType SyntaxAnalyser::EqualExpr()
{
	auto leftType = CmpExpr();										
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::EQ || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();									// Scan ==, !=
		const auto rightType = CmpExpr();							

		leftType = CheckOperationResult(leftType, rightType, lex);	

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::CmpExpr()
{
	auto leftType = AddExpr();										
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::LT || lex.type == LexemeType::RT
		|| lex.type == LexemeType::LTE || lex.type == LexemeType::RTE)
	{
		lex = scanner->NextScan();									// Scan >, >=, <, <=
		const auto rightType = AddExpr();							

		leftType = CheckOperationResult(leftType, rightType, lex);	

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::AddExpr()
{
	auto leftType = MultExpr();										
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Add
		|| lex.type == LexemeType::Sub)
	{
		scanner->NextScan();										// Scan +, -
		const auto rightType = MultExpr();							

		leftType = CheckOperationResult(leftType, rightType, lex);	

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::MultExpr()
{
	auto leftType = PrefixExpr();									
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Mod)
	{
		scanner->NextScan();										// Scan *, /, %
		const auto rightType = PrefixExpr();						

		leftType = CheckOperationResult(leftType, rightType, lex);	

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::PrefixExpr()
{
	auto lex = scanner->LookForward(1);
	std::stack<Lexeme> ops;
	while (lex.type == LexemeType::Inc || lex.type == LexemeType::Dec
		|| lex.type == LexemeType::Add || lex.type == LexemeType::Sub)
	{
		lex = scanner->NextScan();								// Scan ++, --, +, -
		ops.push(lex);
		lex = scanner->LookForward(1);
	}
	auto type = PostfixExpr();

	while (!ops.empty())
	{
		const auto resType = semTree->GetResultDataType(type, ops.top().type);
		if (resType == DataType::Unknown)
			OperationArgsError(type, ops.top().str, ops.top());
		ops.pop();
		type = resType;
	}
	return type;
}

DataType SyntaxAnalyser::PostfixExpr()
{
	auto lex = scanner->LookForward(1);
	auto lex2 = scanner->LookForward(2);
	if ((lex.type == LexemeType::Id || lex.type == LexemeType::Main)
		&& lex2.type == LexemeType::OpenPar)							
	{
		lex = scanner->NextScan();										

		auto funcNode = semTree->FindNodeUp(lex.str);
		if (funcNode->GetSemanticType() == SemanticType::Empty)			
			UndefinedError(lex);
		if (funcNode->GetSemanticType() != SemanticType::Func)			
			UseNotFuncError(funcNode->Identifier, lex);

		scanner->NextScan();											// Scan (

		auto paramsTypes = semTree->GetFuncParams(funcNode);
		size_t argsCount = 0;

		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::ClosePar)
		{
			while (true)
			{
				lex = scanner->LookForward(1);						
				auto type = Expr();								

				if (argsCount < paramsTypes.size() && !semTree->CheckCastable(type, paramsTypes[argsCount]))
					WrongArgType(paramsTypes[argsCount], type, argsCount + 1, lex);

				++argsCount;

				lex = scanner->LookForward(1);
				if (lex.type != LexemeType::Comma)
					break;

				lex = scanner->NextScan();								// Scan ,
			}
		}

		if (argsCount != paramsTypes.size())
			WrongArgsCount(paramsTypes.size(), argsCount, funcNode->Identifier, lex);

		scanner->NextScan();											// Scan )
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);

		return DataType::Void;
	}
	
	auto type = PrimExpr();
	lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Inc
		|| lex.type == LexemeType::Dec)
	{
		lex = scanner->NextScan();										// Scan ++, --

		auto resType = semTree->GetResultDataType(type, lex.type);
		if (resType == DataType::Unknown)
			OperationArgsError(type, lex.str, lex);

		type = resType;

		lex = scanner->LookForward(1);
	}
	return type;
}


DataType SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Const, Id, Main, (

	if (lex.type == LexemeType::OpenPar)
	{
		const auto resType = Expr();
		lex = scanner->NextScan();
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);
		return resType;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main) 
	{
		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)
			UseFuncAsVarError(lex);
		if (!semTree->GetVariableInitialized(node))
			VarIsNotInitError(node->Identifier, lex);

		return node->GetDataType();
	}

	if (lex.type == LexemeType::ConstInt || lex.type == LexemeType::ConstBool)
	{
		const auto numType = semTree->GetDataTypeOfNum(lex);
		if (numType == DataType::Unknown)
			WrongNumber(lex);

		return numType;
	}

	ThrowError("Неизвестное выражение: " + lex.str, lex);
}

DataType SyntaxAnalyser::CheckOperationResult(DataType leftType, DataType rightType, const Lexeme& lex) const
{
	const auto resType = semTree->GetResultDataType(leftType, rightType, lex.type);
	if (resType == DataType::Unknown)
		OperationArgsError(leftType, rightType, lex.str, lex);
	return resType;
}


void SyntaxAnalyser::ThrowError(const std::string& mes, const Lexeme& lex)
{
	if (lex.type == LexemeType::End)
		throw CodeAnalysisException("Неожиданное обнаружение конца файла", lex.pos);
	throw CodeAnalysisException(mes, lex.pos);
}

void SyntaxAnalyser::WrongId(const Lexeme& lex)
{
	ThrowError("Недопустимый идентификатор " + lex.str, lex);
}

void SyntaxAnalyser::WrongType(const Lexeme& lex)
{
	ThrowError("Недопустимый тип: " + lex.str, lex);
}

void SyntaxAnalyser::WrongExpected(const std::string& expected, const Lexeme& lex)
{
	ThrowError("Ожидалось " + expected + ", получено " + lex.str, lex);
}
void SyntaxAnalyser::RedefinitionError(const Lexeme& lex)
{
	ThrowError("Идентификатор \"" + lex.str + "\" уже определен", lex);
}

void SyntaxAnalyser::UndefinedError(const Lexeme& lex)
{
	ThrowError("Идентификатор \"" + lex.str + "\" не определен", lex);
}

void SyntaxAnalyser::UncastableError(DataType from, DataType to, const Lexeme& lex)
{
	ThrowError("Невозможно привести тип " + DataTypeToString(from) + " к типу " + DataTypeToString(to), lex);
}

void SyntaxAnalyser::UncastableConstError(DataType from, DataType to, const Lexeme& lex)
{
	ThrowError("Невозможно изменить значение константы с типом " + DataTypeToString(from), lex);
}

void SyntaxAnalyser::OperationArgsError(DataType leftType, DataType rightType, const std::string& op,
	const Lexeme& lex)
{
	ThrowError("Невозможно выполнить операцию \"" + op + "\" над типами " + DataTypeToString(leftType) + " и " + DataTypeToString(rightType), lex);
}

void SyntaxAnalyser::OperationArgsError(DataType type, const std::string& op, const Lexeme& lex)
{
	ThrowError("Невозможно выполнить операцию \"" + op + "\" над типом " + DataTypeToString(type), lex);
}

void SyntaxAnalyser::WrongNumber(const Lexeme& lex)
{
	ThrowError("Не удалось определить тип константы", lex);
}

void SyntaxAnalyser::WrongArgsCount(size_t reqCount, size_t givenCount, const std::string& funcId, const Lexeme& lex)
{
	ThrowError("Несоответствие количества параметров и аргументов функции " + funcId
		+ ": требуется " + std::to_string(reqCount) + ", дано " + std::to_string(givenCount), lex);
}

void SyntaxAnalyser::WrongArgType(DataType reqType, DataType givenType, size_t argPos, const Lexeme& lex)
{
	ThrowError("Невозможно привести тип аргумента к параметру функции " + lex.str + " на позиции " + std::to_string(argPos) +
		+": требуется " + DataTypeToString(reqType) + ", дано " + DataTypeToString(givenType), lex);
}

void SyntaxAnalyser::AssignToFuncError(const Lexeme& lex)
{
	ThrowError("Невозможно присвоить значение функции", lex);
}

void SyntaxAnalyser::VarIsNotInitError(const std::string& id, const Lexeme& lex)
{
	ThrowError("Переменная " + id + " не инициализирована перед использованием", lex);
}

void SyntaxAnalyser::UseNotFuncError(const std::string& id, const Lexeme& lex)
{
	ThrowError("Переменная " + id + " не является функцией", lex);
}

void SyntaxAnalyser::UseFuncAsVarError(const Lexeme& lex)
{
	ThrowError("Использование функции в качестве переменной невозможно", lex);
}

bool SyntaxAnalyser::IsType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Short
		|| code == LexemeType::Long || code == LexemeType::Bool;
}
