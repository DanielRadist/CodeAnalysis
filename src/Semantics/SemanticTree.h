#pragma once
#include <memory>
#include <vector>

#include "../Lexeme.h"
#include "../Types.h"
#include "Node/Node.h"

class SemanticTree
{
public:
	SemanticTree();

	void SetCurrentNode(Node* node);

	Node* AddData(DataType type, const std::string& id);
	static void SetDataConst(Node* varNode);
	static void SetVariableInitialized(Node* varNode);
	static bool GetVariableInitialized(Node* varNode);
	bool CheckUniqueIdentifier( const std::string& id) const;
	bool CheckDefinedIdentifier( const std::string& id) const;
	static bool CheckCastable(DataType from, DataType to);

	static void SetVariableValue(Node* varNode, int value);
	static void SetFuncReturn(Node* funcNode, int value);

	static DataType GetResultDataType(DataType leftType, DataType rightType, LexemeType operation);
	static DataType GetResultDataType(DataType type,  LexemeType operation);
	DataType GetDataTypeOfNum(Lexeme lex);

	Node* AddFunc(DataType type, const std::string& id);
	void AddParam(Node* funcNode, const std::string& id, DataType type);
	static std::vector<DataType> GetFuncParams(Node* funcNode);

	Node* AddEmpty();

	void AddScope();
	void Print(std::ostream& out) const;

	Node* FindNodeUp(const std::string& id) const;
	Node* FindNodeUpInScope(const std::string& id) const;

private:
	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};



