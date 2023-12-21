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
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct DecimalNode : public LiteralNode
{
	int _value;

	DecimalNode(int value);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct BoolNode : public LiteralNode
{
	bool _value;

	BoolNode(bool value);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct StrNode : public LiteralNode
{
	std::string _value;

	StrNode(std::string value);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
