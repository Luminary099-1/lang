#include "common.hpp"

#include <iostream>

using namespace std::string_view_literals;


AssignmentExpr::AssignmentExpr(Identifier* name, Expression* expr)
	: _name{name}, _expr{expr}
{}


bool AssignmentExpr::Scope(SymTab& symbols, TU& tu)
{
	bool success {true};
	_def = dynamic_cast<VariableDef*>(symbols.Lookup(_name->_id));
	if (_def == nullptr)
	{
		std::cerr << '(' << _name->_row << ", "sv << _name->_col
			<< "): Unkown variable: "sv << _name->_id << '\n';
		tu.HighlightError(std::cerr, *_name);
		success = false;
	}
	
	return _expr->Scope(symbols, tu) && success;
}


bool AssignmentExpr::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};

	_type = _def->_type;
	if (*_type != *_expr->_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected assignment expression of type "sv << _type->_name
			<< ", found: "sv << _expr->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}
	
	_hasCall = _expr->_hasCall;
	return success;
}


void AssignmentExpr::Generate(GenData& dat, std::ostream& os)
{}


void AssignmentExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignmentExpression(ID) = "sv << _name->_id << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Expression =\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


LoopExpr::LoopExpr(
	Expression* init, Expression* cond, Expression* inc, Statement* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


bool LoopExpr::Scope(SymTab& symbols, TU& tu)
{
	bool success {_init != nullptr ? _init->Scope(symbols, tu) : true};
	success = (_cond != nullptr ? _init->Scope(symbols, tu) : true) && success;
	success = (_inc != nullptr ? _init->Scope(symbols, tu) : true) && success;
	return (_body != nullptr ? _init->Scope(symbols, tu) : true) && success;
}


bool LoopExpr::Validate(ValidateData& dat)
{
	dat._bs.push_back(this);
	bool success {true};
	if (_init != nullptr) success = _init->Validate(dat) && success;
	if (_cond != nullptr) success = _cond->Validate(dat) && success;
	if (_inc != nullptr) success = _inc->Validate(dat) && success;
	success = _body->Validate(dat) && success;
	dat._bs.pop_back();
	_hasReturn = _body->_hasReturn;
	_hasCall = _init->_hasCall || _cond->_hasCall
		|| _body->_hasCall || _inc->_hasCall;
	return success;
}


void LoopExpr::Generate(GenData& dat, std::ostream& os)
{}


void LoopExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExpression("sv;
	_type->Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Initialize =\n"sv;
	_init->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	_cond->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Increment =\n"sv;
	_inc->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	_body->Print(os, indent, depth + 1);
}
