#include "literals.hpp"

#include "function.hpp"

#include <iostream>

using namespace std::string_view_literals;


Identifier::Identifier(std::string& value)
	: _value{value}
{}


bool Identifier::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	_def = ss.Lookup(_value);
	// TODO: Ideally, Param and VariableDef would extend a common type.
	// VariableDef's current role is more appropriately called VariableDefInit.
	if (_def != nullptr && dynamic_cast<Function*>(_def) != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown symbol: " << _value << '\n';
		HighlightError(std::cerr, src, *this);
		return false;
	}
	return true;
}


void Identifier::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Identifier(Name = "sv << _value << ")\n"sv;
}


IntLiteral::IntLiteral(int& value)
	: _value{value}
{}


void IntLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IntegerLiteral(Value = "sv << _value << ")\n"sv;
}


BoolLiteral::BoolLiteral(bool& value)
	: _value{value}
{}


void BoolLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BooleanLiteral(Value = "sv << _value << ")\n"sv;
}


StrLiteral::StrLiteral(std::string& value)
	: _value{value}
{}


void StrLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "StringLiteral(Value = "sv << _value << ")\n"sv;
}
