#include "literals.hpp"

using namespace std::string_view_literals;


IDNode::IDNode(std::string value)
	: _value{value}
{}


void IDNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Identifier(Name = "sv << _value << ")\n"sv;
}


DecimalNode::DecimalNode(int value)
	: _value{value}
{}


void DecimalNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IntegerLiteral(Value = "sv << _value << ")\n"sv;
}


BoolNode::BoolNode(bool value)
	: _value{value}
{}


void BoolNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BooleanLiteral(Value = "sv << _value << ")\n"sv;
}


StrNode::StrNode(std::string value)
	: _value{value}
{}


void StrNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "StringLiteral(Value = "sv << _value << ")\n"sv;
}
