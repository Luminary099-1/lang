#include "function.hpp"

#include <algorithm>

using namespace std::string_view_literals;


Function::Function(Type type, std::string name
	, ParamList params, StmtList body)
	: _type{type}, _name{name}, _params{params}, _body{body}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


void Function::Print(std::ostream& os, std::string_view indent, int depth)
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
	for (Statement* node : _body) node->Print(os, indent, depth) ;
}
