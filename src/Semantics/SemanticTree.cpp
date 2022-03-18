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

void SemanticTree::SetVariableInitialized(Node* varNode)
{
	dynamic_cast<DataNode*>(varNode)->IsInitialized = true;
}

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
	dynamic_cast<DataNode*>(varNode)->Value.SetValue(value);
}

void SemanticTree::SetFuncReturn(Node* funcNode, int value)
{
	dynamic_cast<DataNode*>(funcNode)->Value.SetValue(value);
}


DataType SemanticTree::GetResultDataType(DataType leftType, DataType rightType, LexemeType operation)
{
	if (leftType == DataType::Void || rightType == DataType::Void
		|| leftType == DataType::Unknown || rightType == DataType::Unknown)
		return DataType::Unknown;

	if (operation == LexemeType::EQ || operation == LexemeType::NE
		|| operation == LexemeType::LT || operation == LexemeType::RT
		|| operation == LexemeType::LTE || operation == LexemeType::RTE)
		return DataType::Bool;

	if (leftType == DataType::Long || rightType == DataType::Long)
		return DataType::Long;
	return DataType::Int;
}

DataType SemanticTree::GetResultDataType(DataType type, LexemeType operation)
{
	if (type == DataType::Void || type == DataType::Unknown)
		return DataType::Unknown;
	return type;
}

DataType SemanticTree::GetDataTypeOfNum(Lexeme lex)
{
	static std::string MAX_SHORT = "32767";
	static std::string MAX_INT = "2147483647";
	static std::string MAX_LONG = "9223372036854775807";
	if (lex.type == LexemeType::ConstInt)
	{
		if (lex.str.size() < MAX_SHORT.size() || lex.str <= MAX_SHORT)
			return DataType::Short;
		if (lex.str.size() < MAX_INT.size() || lex.str <= MAX_INT)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG.size() || lex.str <= MAX_LONG)
			return DataType::Long;
		return  DataType::Unknown;
	}
	if (lex.type == LexemeType::ConstBool)
	{
		return DataType::Bool;
	}
	return DataType::Unknown;

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
	SetVariableInitialized(node);
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

void SemanticTree::AddScope()
{
	_currNode->RightChild = std::make_unique<EmptyNode>(_currNode);
	SetCurrentNode(_currNode->RightChild.get());
}

void SemanticTree::Print(std::ostream& out) const
{
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
