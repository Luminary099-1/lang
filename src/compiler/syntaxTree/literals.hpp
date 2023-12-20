#pragma once

#include "statements.hpp"

#include <string>


struct LiteralNode : public ExprNode
{
	
};


struct IDNode : public LiteralNode
{
	std::string _value;

	IDNode(std::string value);
};


struct DecimalNode : public LiteralNode
{
	int _value;

	DecimalNode(int value);
};


struct BoolNode : public LiteralNode
{
	bool _value;

	BoolNode(bool value);
};


struct StrNode : public LiteralNode
{
	std::string _value;

	static StrNode* build(std::string value);
};
