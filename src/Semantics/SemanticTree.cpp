#include "SemanticTree.h"

#include "Node/EmptyNode.h"
#include "Node/DataNode.h"
#include "Node/FuncNode.h"
#include "../Types.h"
#include "../Lexeme.h"

SemanticTree::SemanticTree()
	:_rootNode(std::make_unique<EmptyNode>(nullptr)),
	_currNode(_rootNode.get())
{}

void SemanticTree::SetCurrentNode(Node* node)
{
	_currNode = node;
}



Node* SemanticTree::AddData(DataType type, const std::string& id)
{
	_currNode->LeftChild = std::make_unique<DataNode>(_currNode, id, type, false, false);
	SetCurrentNode(_currNode->LeftChild.get());
	return _currNode;
}

void SemanticTree::SetDataConst(Node* varNode)
{
	dynamic_cast<DataNode*>(varNode)->IsConst = true;
}

/*
void SemanticTree::SetVariableInitialized(Node* varNode)
{
	dynamic_cast<DataNode*>(varNode)->IsInitialized = true;
}*/

bool SemanticTree::GetVariableInitialized(Node* varNode)
{
	return dynamic_cast<DataNode*>(varNode)->IsInitialized;
}

bool SemanticTree::CheckUniqueIdentifier(const std::string& id) const
{
	auto node = FindNodeUpInScope(id);
	return node->GetSemanticType() == SemanticType::Empty;
}

void SemanticTree::SetVariableValue(Node* varNode, int value)
{
	dynamic_cast<DataNode*>(varNode)->IsInitialized = true;
	dynamic_cast<DataNode*>(varNode)->Value->SetValue(value);
}

void SemanticTree::SetVariableValue(Node* varNode, Data* data)
{
	dynamic_cast<DataNode*>(varNode)->IsInitialized = true;
	Data::CastingTypes(dynamic_cast<DataNode*>(varNode)->Value, data);
	delete data;
}

void SemanticTree::SetFuncReturn(Node* funcNode, int value)
{
	dynamic_cast<DataNode*>(funcNode)->Value->SetValue(value);
}


Data* SemanticTree::GetResultData(Data* leftData, Data* rightData, LexemeType operation)
{
	if (leftData->Type == DataType::Void || rightData->Type == DataType::Void
		|| leftData->Type == DataType::Unknown || rightData->Type == DataType::Unknown)
		return nullptr;

	if (operation == LexemeType::EQ || operation == LexemeType::NE || 
		operation == LexemeType::And || operation == LexemeType::Or
		|| operation == LexemeType::LT || operation == LexemeType::RT
		|| operation == LexemeType::LTE || operation == LexemeType::RTE)
	{
		bool value;
		switch (operation)
		{
		case LexemeType::NE:	// !=
			value = leftData->GetValueToInt() != rightData->GetValueToInt();
			break;
		case LexemeType::LT:	// <
			value = leftData->GetValueToInt() < rightData->GetValueToInt();
			break;
		case LexemeType::RT:	// >
			value = leftData->GetValueToInt() > rightData->GetValueToInt();
			break;
		case LexemeType::LTE:	// <=
			value = leftData->GetValueToInt() <= rightData->GetValueToInt();
			break;
		case LexemeType::RTE:	// >=
			value = leftData->GetValueToInt() >= rightData->GetValueToInt();
			break;
		case LexemeType::EQ:	// ==
			value = leftData->GetValueToInt() == rightData->GetValueToInt();
			break;
		case LexemeType::And:	// &&
			value = leftData->GetValueToInt() && rightData->GetValueToInt();
			break;
		case LexemeType::Or:	// ||
			value = leftData->GetValueToInt() || rightData->GetValueToInt();
			break;
		default:
			break;
		}

		leftData->SetValue(value);
		return leftData;
	}

	if (leftData->Type == DataType::Int || rightData->Type == DataType::Int)
	{
		int value;
		switch (operation)
		{
		case LexemeType::Add:
			value = leftData->GetValueToInt() + rightData->GetValueToInt();
			break;
		case LexemeType::Sub:
			value = leftData->GetValueToInt() - rightData->GetValueToInt();
			break;
		case LexemeType::Mul:
			value = leftData->GetValueToInt() * rightData->GetValueToInt();
			break;
		case LexemeType::Div:
			value = leftData->GetValueToInt() / rightData->GetValueToInt();
			break;
		case LexemeType::Mod:
			value = leftData->GetValueToInt() % rightData->GetValueToInt();
			break;
		default:
			break;
		}

		leftData->SetValue(value);
		return leftData;
	}

	return nullptr;
}

Data* SemanticTree::GetResultData(Data* data, LexemeType operation)
{
	if (data->Type == DataType::Void || data->Type == DataType::Unknown)
		return nullptr;

	switch (operation)
	{
	case LexemeType::Inc:	// ++
		data->SetValue(data->GetValueToInt() + 1);
		return data;
	case LexemeType::Dec:	// --
		data->SetValue(data->GetValueToInt() - 1);
		return data;
	default:
		return data;
	}
}

Data* SemanticTree::GetDataOfNum(Lexeme lex)
{
	static std::string TRUE = "true";
	static std::string FALSE = "false";

	static std::string MAX_INT = "2147483647";

	if (lex.type == LexemeType::ConstBool)
	{
		if (lex.str == TRUE)
			return new Data(DataType::Bool, 1);
		else if (lex.str == FALSE)
			return new Data(DataType::Bool, 0);
	}

	else if (lex.type == LexemeType::ConstInt && lex.str.size() < MAX_INT.size())
	{
		size_t offset = 0;
		if (lex.str[offset] == '-')
			offset++;

		if (lex.str.find_first_not_of("0123456789", offset) == std::string::npos)
			return new Data(DataType::Int, stoi((lex.str)));
	}
	return nullptr;
}

bool SemanticTree::CheckDefinedIdentifier(const std::string& id) const
{
	auto node = FindNodeUp(id);
	return node->GetSemanticType() != SemanticType::Empty;
}

bool SemanticTree::CheckCastable(DataType from, DataType to)
{
	if (from == DataType::Unknown || to == DataType::Unknown)
		return false;
	return true;
}

Node* SemanticTree::AddFunc(DataType type, const std::string& id)
{
	_currNode->LeftChild = std::make_unique<FuncNode>(_currNode, id, type);
	const auto funcNode = _currNode->LeftChild.get();
	SetCurrentNode(funcNode);
	AddScope();
	return funcNode;
}

Node* SemanticTree::AddEmpty()
{
	_currNode->LeftChild = std::make_unique<EmptyNode>(_currNode);
	SetCurrentNode(_currNode->LeftChild.get());
	return _currNode;
}

void SemanticTree::AddParam(Node* funcNode, const std::string& id, DataType type)
{
	auto func = dynamic_cast<FuncNode*>(funcNode);
	func->ParamsCount++;
	auto node = AddData(type, id);
	SetVariableValue(node, 0);
}

std::vector<DataType> SemanticTree::GetFuncParams(Node* funcNode)
{
	const auto funcNodeD = dynamic_cast<FuncNode*>(funcNode);
	auto paramNode = funcNodeD->RightChild->LeftChild.get();
	std::vector<DataType> paramsTypes(funcNodeD->ParamsCount);
	for (int i = 0; i < funcNodeD->ParamsCount; i++)
	{
		paramsTypes[i] = paramNode->GetDataType();
		paramNode = paramNode->LeftChild.get();
	}
	return paramsTypes;
}

Data* SemanticTree::GetFuncReturn(Node* funcNode)
{
	auto func = dynamic_cast<FuncNode*>(funcNode);
	return func->Value;
}

void SemanticTree::AddScope()
{
	_currNode->RightChild = std::make_unique<EmptyNode>(_currNode);
	SetCurrentNode(_currNode->RightChild.get());
}

void SemanticTree::DeleteSubTree(Node* node)
{
	node->RightChild.reset();
}

void SemanticTree::DeleteAllTree()
{
	_rootNode.reset();
}

void SemanticTree::Print(std::ostream& out) const
{
	if (_rootNode == nullptr)
	{
		out << "null\n";
		return;
	}
	_rootNode->RecPrint(out);
}

Node* SemanticTree::FindNodeUp(const std::string& id) const
{
	auto node = _currNode;
	while (node->Parent && node->Identifier != id) {
		node = node->Parent;
	}
	return node;
}

Node* SemanticTree::FindNodeUpInScope(const std::string& id) const
{
	auto node = _currNode;
	auto par = _currNode->Parent;
	while (par && par->RightChild.get() != node && node->Identifier != id) {
		node = par;
		par = node->Parent;

	}
	return node;
}
