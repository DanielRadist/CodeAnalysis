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
	//static void SetVariableInitialized(Node* varNode);
	static bool GetVariableInitialized(Node* varNode);
	bool CheckUniqueIdentifier( const std::string& id) const;
	bool CheckDefinedIdentifier( const std::string& id) const;
	static bool CheckCastable(DataType from, DataType to);

	static void SetVariableValue(Node* varNode, int value);
	static void SetVariableValue(Node* varNode, Data* data);
	static void SetFuncReturn(Node* funcNode, int value);

	static Data* GetResultData(Data* leftType, Data* rightType, LexemeType operation);
	static Data* GetResultData(Data* type, LexemeType operation);
	Data* GetDataOfNum(Lexeme lex);

	Node* AddFunc(DataType type, const std::string& id);
	void AddParam(Node* funcNode, const std::string& id, DataType type);
	static std::vector<DataType> GetFuncParams(Node* funcNode);
	static Data* GetFuncReturn(Node* funcNode);

	Node* AddEmpty();

	void AddScope();
	static void DeleteSubTree(Node* node);
	void DeleteAllTree();
	void Print(std::ostream& out) const;

	Node* FindNodeUp(const std::string& id) const;
	Node* FindNodeUpInScope(const std::string& id) const;

private:
	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};



