#pragma once
#include "../../Types.h"
#include "Node.h"
#include "Data.h"
#include <iostream>
#include <utility>

	


struct DataNode : Node
{
	DataNode(Node* parent, std::string id, DataType type, bool isInitialized, bool isConst)
		: Node(std::move(id), parent),
		IsInitialized(isInitialized),
		IsConst(isConst)
	{
		Value = new Data(type);
	}
	
	~DataNode()
	{
		if (Value != nullptr)
			delete Value;
	}

	DataType GetDataType() const override { return Value->Type; }
	Data* GetData() override { return new Data(Value); }
	SemanticType GetSemanticType() const override 
	{ 
		if (IsConst)
			return SemanticType::Const;
		else
			return SemanticType::Data; 
	}

	bool IsInitialized;
	bool IsConst;

	Data* Value;

protected:
	void Print(std::ostream& out) const override
	{
		//out << "Data Node: Type = " << DataTypeToString(Value.Type) << ", Id = " << Identifier << ", Is Initialized = " << IsInitialized << ", Is Const = " << IsConst << "\n";
		out << "Data Node: Type = " << DataTypeToString(Value->Type) << ", Id = " << Identifier << ", Is Const = " << IsConst << ", Value = " << Value->GetValueToString() << std::endl;
	}
};
