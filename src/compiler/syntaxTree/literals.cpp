#include "literals.hpp"

using namespace std::string_view_literals;


IDNode::IDNode(std::string value)
	: _value{value}
{}


void IDNode::Validate()
{}


void IDNode::Scope()
{}


void IDNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IDNode(name: "sv << _value << ")\n"sv;
}


DecimalNode::DecimalNode(int value)
	: _value{value}
{}


void DecimalNode::Validate()
{}


void DecimalNode::Scope()
{}


void DecimalNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "DecimalNode(value: "sv << _value << ")\n"sv;
}


BoolNode::BoolNode(bool value)
	: _value{value}
{}


void BoolNode::Validate()
{}


void BoolNode::Scope()
{}


void BoolNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BoolNode(value: "sv << _value << ")\n"sv;
}


StrNode::StrNode(std::string value)
	: _value{value}
{}


void StrNode::Validate()
{}


void StrNode::Scope()
{}


void StrNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "StrNode(value: "sv << _value << ")\n"sv;
}
