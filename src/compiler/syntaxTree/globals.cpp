#include "globals.hpp"

#include "../utilities.hpp"

#include <algorithm>
#include <iostream>

using namespace std::string_view_literals;


Parameter::Parameter(Type* type, Identifier* name)
	: Declaration{name}, _type{type}
{}


bool Parameter::Scope(SymbolTable& symbols, TU& tu)
{
	Declaration* pre {symbols.Define(_name->_id, this)};
	if (pre != nullptr)
	{
		std::cerr << '(' << _name->_row << ", "sv << _name->_col
			<< "): Symbol collision: "sv << _name->_id
			<< "\n# The following on line "sv << _name->_row << ":\n"sv;
		tu.HighlightError(std::cerr, *_name);
		std::cerr << "# Redefines on line "sv << pre->_name->_row << ":\n"sv;
		tu.HighlightError(std::cerr, *pre->_name);
		return false;
	}
	return true;
}


void Parameter::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Parameter(ID = "sv << _name->_id
		<< ", Type = "sv << _type->_name << ")\n"sv;
}


Function::Function(
	Identifier* name, ParamList params, Type* type, StmtList body)
	: Declaration{name}, _params{std::move(params)}
		, _type{type}, _body{std::move(body)}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


bool Function::Scope(SymbolTable& symbols, TU& tu)
{
	symbols.Enter();
	bool success {true};
	for (size_t i {0}; i < _params.size(); ++ i)
		success = _params[i]->Scope(symbols, tu) && success;
	for (size_t i {0}; i < _body.size(); ++ i)
		success = _body[i]->Scope(symbols, tu) && success;
	symbols.Exit();
	return success;
}


bool Function::Validate(ValidateData& dat)
{
	bool success {true};
	dat._curFunc = this;
	const bool returns {Statement::ValidateAndGetReturn(
		_body, dat, success, &_hasCall)};
	dat._curFunc = nullptr;

	if (!_type->IsVoid() && !returns)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Non-void function does not return in all control paths: \n"sv
			<< _name->_id << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}

	dat._curFunc = nullptr;
	return success;
}


void Function::Generate(GenData& dat, std::ostream& os)
{}


void Function::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Function(ID = "sv << _name->_id
		<< ", Type = "sv << _type->_name << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _params.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Parameters["sv << i << "](ID = "sv << _params[i]->_name->_id
			<< ", Type = "sv << _params[i]->_type->_name << ")\n"sv;
	}
	for (size_t i {0}; i < _body.size(); ++ i)
		_body[i]->Print(os, indent, depth);
}
