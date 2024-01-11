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


bool Parameter::Scope(ScopeStack& ss, TUBuffer& src)
{
	return _type->Scope(ss, src);
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


bool Function::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	TokenInfo* pre_def {dynamic_cast<TokenInfo*>(ss.Define(_name, this))}; // Trusting this for now.
	if (pre_def != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Symbol collision: "sv << _name
			<< "\n# The following on line"sv << _row << ":\n";
		HighlightError(std::cerr, src, *this);
		std::cerr << "# Redefines on line "sv << pre_def->_row << ":\n";
		HighlightError(std::cerr, src, *pre_def);
		success = false;
	}

	ss.Enter();
	for (size_t i {0}; i < _params.size(); ++ i)
		success = _params[i]->Scope(ss, src) && success;
	for (size_t i {0}; i < _body.size(); ++ i)
		success = _body[i]->Scope(ss, src) && success;
	ss.Exit();
	return success;
}


bool Function::Validate(ValidateData& dat)
{
	dat._curFunc = this;

	if (!_type->IsVoid() && !Statement::DoesListReturn(_body, dat._src))
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Non-void function does not contain a return statement: \n"sv
			<< _name << '\n';
		HighlightError(std::cerr, dat._src, *this);
	}

	dat._curFunc = nullptr;
	return true;
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
	for (size_t i {0}; i < _body.size(); ++ i)
		_body[i]->Print(os, indent, depth);
}
