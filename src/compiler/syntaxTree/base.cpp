#include "base.hpp"

using namespace std::string_view_literals;


void SyntaxNode::PrintIndent(
	std::ostream& os, std::string_view indent, int depth)
{
	for (int i {0}; i < depth; ++ i) os << indent;
}


TypeNode::TypeNode(std::string type_name)
	: _name{type_name}
{}


void TypeNode::Resolve()
{}


void TypeNode::Validate()
{}


void TypeNode::Scope()
{}


void TypeNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "TypeNode(name: "sv << _name << ')' << '\n';
}
