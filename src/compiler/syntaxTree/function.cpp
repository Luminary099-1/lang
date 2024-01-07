#include "function.hpp"

#include "../utilities.hpp"

#include <algorithm>
#include <iostream>

using namespace std::string_view_literals;


Parameter::Parameter()
{}


Parameter::Parameter(Type* type, std::string name)
	: _type{type}, _name{name}
{}


bool Parameter::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	return _type->Scope(ss, src, first_pass);
}


void Parameter::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Parameter("sv;
	_type->Print(os, indent, depth);
	os << ", Name = "sv << _name << ")\n"sv;
}


Function::Function(
	Type* type, std::string name, ParamList& params, StmtList body)
	: _type{type}, _name{name}, _params{std::move(params)}, _body{body}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


bool Function::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	if (first_pass)
	{
		TokenInfo* pre_def {dynamic_cast<TokenInfo*>(ss.Define(_name, this))}; // Trusting this for now.
		if (pre_def != nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Symbol collision: "sv << _name
				<< "\n-> The following on line"sv << _row << ":\n";
			HighlightError(std::cerr, src, *this);
			std::cerr << "-> Redefines on line "sv << pre_def->_row << ":\n";
			HighlightError(std::cerr, src, *pre_def);
			return false;
		}
		return true;
	}

	bool success {true};
	ss.Enter();
	for (size_t i {0}; i < _params.size(); ++ i)
		success = success && _params[i]->Scope(ss, src, first_pass);
	for (Statement* stmt : _body)
		success = success && stmt->Scope(ss, src, first_pass);
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
		os << "Parameters[" << i << "](Name = "sv << _params[i]->_name << ", "sv;
		_type->Print(os, indent, depth);
		os << ")\n"sv;
	}
	for (Statement* node : _body) node->Print(os, indent, depth) ;
}
