#include "globals.hpp"

#include "../utilities.hpp"

#include <algorithm>
#include <iostream>

using namespace std::string_view_literals;


Parameter::Parameter(Type* type, Identifier* name)
	: Declaration{name}, _type{type}
{}


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
