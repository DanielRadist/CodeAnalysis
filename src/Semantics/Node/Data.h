#pragma once
#include "../../Types.h"
#include <utility>

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
	} *Value;

	DataType Type;
};