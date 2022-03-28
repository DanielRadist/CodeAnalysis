#pragma once
#include <iostream>
#include <utility>

#include "Node.h"
#include "Data.h"
#include "../Types.h"




struct FuncNode : Node
{
	FuncNode(Node* parent, std::string id, DataType type) 
		: Node(std::move(id), parent)
	{
		Value = new Data(type);
	}
	
	~FuncNode()
	{
		if (Value != nullptr)
			delete Value;
	}

	DataType GetDataType() const override { return Value->Type; }
	Data* GetData() override { return Value; }
	SemanticType GetSemanticType() const override { return SemanticType::Func; }

	int ParamsCount = 0;

	Data* Value;

protected:
	void Print(std::ostream& out) const override
	{
		out << "Function Node: Type = " << DataTypeToString(Value->Type) << ", Id = " << Identifier << ", Param Count = " << ParamsCount << "\n";
	}
};
