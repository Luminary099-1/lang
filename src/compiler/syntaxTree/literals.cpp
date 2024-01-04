#include "literals.hpp"

using namespace std::string_view_literals;


Identifier::Identifier(std::string value)
	: _value{value}
{}


void Identifier::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Identifier(Name = "sv << _value << ")\n"sv;
}


IntLiteral::IntLiteral(int value)
	: _value{value}
{}


void IntLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IntegerLiteral(Value = "sv << _value << ")\n"sv;
}


BoolLiteral::BoolLiteral(bool value)
	: _value{value}
{}


void BoolLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BooleanLiteral(Value = "sv << _value << ")\n"sv;
}


StrLiteral::StrLiteral(std::string value)
	: _value{value}
{}


void StrLiteral::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "StringLiteral(Value = "sv << _value << ")\n"sv;
}
