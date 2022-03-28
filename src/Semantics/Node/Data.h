#pragma once
#include "../../Types.h"
#include <utility>

union DataValue
{
	DataValue(int value)
	{
		this->integer = value;
	}

	DataValue(bool value)
	{
		this->boolean = value;
	}

	bool boolean;
	int integer;
};

struct Data
{
	Data(DataType type, int value)
	{
		Type = type;

		switch (type)
		{
		case DataType::Int:
			Value = new DataValue((int)value);
			break;

		case DataType::Bool:
			Value = new DataValue((bool)value);
			break;

		case DataType::Void:
			Value = nullptr;
			break;

		case DataType::Unknown:
			Value = nullptr;
			break;

		default:
			Value = nullptr;
			break;
		}
	}

	Data(DataType type)
	{
		Type = type;

		switch (type)
		{
		case DataType::Int:
			Value = new DataValue((int)0);
			break;

		case DataType::Bool:
			Value = new DataValue((bool)0);
			break;

		case DataType::Void:
			Value = nullptr;
			break;

		case DataType::Unknown:
			Value = nullptr;
			break;

		default:
			Value = nullptr;
			break;
		}
	}

	Data(Data* data)
	{
		this->Type = data->Type;
		this->SetValue(data->GetValueToInt());
	}

	~Data()
	{
		if (Value != nullptr)
			delete Value;
	}

	void SetValue(int value)
	{
		delete Value;
		switch (Type)
		{
		case DataType::Int:
			Value = new DataValue((int)value);
			break;

		case DataType::Bool:
			Value = new DataValue((bool)value);
			break;

		default:
			Value = nullptr;
			break;
		}		
	}

	std::string GetValueToString() const
	{
		switch (Type)
		{
		case DataType::Int:
			return std::to_string(Value->integer);

		case DataType::Bool:
			if (Value->boolean == true)
				return "True";
			else
				return "False";
		default:
			return "null";
		}
	}

	int GetValueToInt() const
	{
		switch (Type)
		{
		case DataType::Int:
			return Value->integer;

		case DataType::Bool:
			if (Value->boolean == true)
				return 1;
			else
				return 0;
		default:
			return NULL;
		}
	}

	static void CastingTypes(Data* recipient, Data* sender)
	{
		switch (sender->Type)
		{
		case DataType::Int:
			recipient->SetValue(sender->Value->integer);
			break;
		case DataType::Bool:
			recipient->SetValue(sender->Value->boolean);
			break;
		default:
			recipient->Value = nullptr;
			break;
		}
	}

	DataValue* Value = nullptr;

	DataType Type;
};