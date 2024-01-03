#include "base.hpp"

using namespace std::string_view_literals;


void SyntaxNode::PrintIndent(
	std::ostream& os, std::string_view indent, int depth)
{
	for (int i {0}; i < depth; ++ i) os << indent;
}


void SyntaxNode::PrintMaybe(
	SyntaxNode* node, std::ostream& os, std::string_view indent, int depth)
{
	if (node != nullptr) node->Print(os, indent, depth);
	else
	{
		PrintIndent(os, indent, depth);
		os << "nullptr\n"sv;
	}
}


TypeNode::TypeNode()
{}


TypeNode::TypeNode(std::string type_name)
	: _name{type_name}
{}


void TypeNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	os << _name;
}
