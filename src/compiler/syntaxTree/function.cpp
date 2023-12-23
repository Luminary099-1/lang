#include "function.hpp"

#include <algorithm>

using namespace std::string_view_literals;


FunctionNode::FunctionNode(TypeNode type, std::string name
	, ParamList params, CompoundStmtNode* body)
	: _type{type}, _name{name}, _params{params}, _body{body}
{
	std::reverse(_params.begin(), _params.end());
}


void FunctionNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "FunctionNode(name: "sv << _name << "):\n"sv;
	for (Param p : _params)
	{
		PrintIndent(os, indent, depth);
		os << "Param(name: "sv << p.second << "):\n"sv;
		p.first.Print(os, indent, depth + 1);
	}
}
