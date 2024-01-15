#include "common.hpp"

#include <iostream>

using namespace std::string_view_literals;


AssignmentExpr::AssignmentExpr(Identifier* name, Expression* expr)
	: _name{name}, _expr{expr}
{}


bool AssignmentExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {_expr->Scope(ss, src)};
	_def = dynamic_cast<VariableDef*>(ss.Lookup(_name->_id));
	if (_def == nullptr)
	{
		std::cerr << '(' << _name->_row << ", "sv << _name->_col
			<< "): Unkown variable: "sv << _name->_id << '\n';
		HighlightError(std::cerr, src, *_name);
		success = false;
	}
	return success;
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
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	
	return success;
}


void AssignmentExpr::Generate(GenerateData& dat)
{
	// TODO: Implement this.
}


void AssignmentExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignmentExpression(ID) = "sv << _name->_id << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Expression =\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ForExpr::ForExpr(
	Expression* init, Expression* cond, Expression* inc, Statement* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


bool ForExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	if (_init != nullptr) success = _init->Scope(ss, src) && success;
	if (_cond != nullptr) success = _cond->Scope(ss, src) && success;
	if (_inc != nullptr) success = _inc->Scope(ss, src) && success;
	return _body->Scope(ss, src) && success;
}


bool ForExpr::Validate(ValidateData& dat)
{
	dat._bs.push_back(this);
	bool success {true};
	if (_init != nullptr) success = _init->Validate(dat) && success;
	if (_cond != nullptr) success = _cond->Validate(dat) && success;
	if (_inc != nullptr) success = _inc->Validate(dat) && success;
	success = _body->Validate(dat) && success;
	dat._bs.pop_back();
	_hasReturn = _body->_hasReturn;
	return success;
}


void ForExpr::Generate(GenerateData& dat)
{
	// TODO: Implement this.
}


void ForExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ForExpression("sv;
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


LoopExpr::LoopExpr(Statement* body)
	: _body{body}
{}


bool LoopExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	return _body->Scope(ss, src);
}


bool LoopExpr::Validate(ValidateData& dat)
{
	dat._bs.push_back(this);
	bool success {_body->Validate(dat)};
	dat._bs.pop_back();
	_hasReturn = _body->_hasReturn;
	return success;
}


void LoopExpr::Generate(GenerateData& dat)
{
	// TODO: Implement this.
}


void LoopExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExpression("sv;
	_type->Print(os, indent, depth);
	os << "):\n";
	PrintIndent(os, indent, ++ depth);
	os << "Body =:\n"sv;
	_body->Print(os, indent, ++ depth);
}


WhileExpr::WhileExpr(Expression* cond, Statement* body)
	: _cond{cond}, _body{body}
{}


bool WhileExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {_cond->Scope(ss, src)};
	return _body->Scope(ss, src) && success;
}


bool WhileExpr::Validate(ValidateData& dat)
{
	dat._bs.push_back(this);
	bool success {_cond->Validate(dat)};
	return _body->Validate(dat) && success;
	dat._bs.pop_back();
	_hasReturn = _body->_hasReturn;

	if (!_cond->_type->IsBool())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected while loop condition of type bool, found: "sv
			<< _cond->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	return success;
}


void WhileExpr::Generate(GenerateData& dat)
{
	// TODO: Implement this.
}


void WhileExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "WhileExpression("sv;
	_type->Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	_cond->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	_body->Print(os, indent, depth + 1);
}
