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
	os << "Function(Name = "sv << _name << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _params.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Parameters[" << i << "](Name = "sv << _params[i].second
			<< ", Type = "sv;
		_params[i].first.Print(os, indent, depth);
		os << ")\n"sv;
	}
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body, os, indent, depth + 1);
}
