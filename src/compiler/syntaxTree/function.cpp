#include "function.hpp"

#include "../utilities.hpp"

#include <algorithm>
#include <iostream>

using namespace std::string_view_literals;


Parameter::Parameter()
{}


Parameter::Parameter(Type type, std::string name)
	: _type{type}, _name{name}
{}


bool Parameter::Scope(ScopeStack& ss, TUBuffer& src)
{
	return _type.Scope(ss, src);
}


void Parameter::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Parameter(Type = "sv << _type._name
		<< ", Name = "sv << _name << ")\n"sv;
}


Function::Function(
	Type type, std::string name, ParamList params, StmtList body)
	: _type{type}, _name{name}, _params{params}, _body{body}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


bool Function::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	if (Symbol* pre_def = ss.Define(std::string_view(_name), this))
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Symbol collision: "sv << _name
			<< "\n-> The following on line"sv << _row << ":\n";
		HighlightError(std::cerr, src, *this);
		std::cerr << "-> Redefines on line "sv << pre_def->_row << ":\n";
		HighlightError(std::cerr, src, *pre_def);
		success = false;
	}

	ss.Enter();
	for (Parameter param : _params) success = success && param.Scope(ss, src);
	for (Statement* stmt : _body) success = success && stmt->Scope(ss, src);
	ss.Exit();
	return success;
}


void Function::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Function(Name = "sv << _name << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _params.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Parameters[" << i << "](Name = "sv << _params[i]._name
			<< ", Type = "sv << _params[i]._type._name << ")\n"sv;
	}
	for (Statement* node : _body) node->Print(os, indent, depth) ;
}
