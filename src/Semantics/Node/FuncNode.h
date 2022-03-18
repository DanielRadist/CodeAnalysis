#pragma once
#include <iostream>
#include <utility>

#include "../Types.h"
#include "Node.h"



struct FuncNode : Node
{
	FuncNode(Node* parent, std::string id, DataType type) 
		: Node(std::move(id), parent),
		Value(type) {}

	DataType GetDataType() const override { return Value.Type; }

	SemanticType GetSemanticType() const override { return SemanticType::Func; }

	int ParamsCount = 0;

	Data Value;

protected:
	void Print(std::ostream& out) const override
	{
		out << "Function Node: Type = " << DataTypeToString(Value.Type) << ", Id = " << Identifier << ", Param Count = " << ParamsCount << "\n";
	}
};
