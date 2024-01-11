#include "constructs.hpp"

#include <iostream>

using namespace std::string_view_literals;


AssignmentExpr::AssignmentExpr(std::string name, Expression* expr)
	: _name{name}, _expr{expr}
{}


bool AssignmentExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {_expr->Scope(ss, src)};
	_def = dynamic_cast<VariableDef*>(ss.Lookup(_name)); // Trusting this for now.
	if (_def == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown variable: " << _name << '\n';
		HighlightError(std::cerr, src, *this);
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
			<< ", found: " << _expr->_type << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	return success;
}


void AssignmentExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignmentExpression(Name = "sv << _name << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Expression =\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


IfExpr::IfExpr(Expression* cond, Statement* body, Statement* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


bool IfExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	// Kinda ugly to avoid short-circuit evaluation.
	bool success {_cond->Scope(ss, src)};
	success = _body->Scope(ss, src) && success;
	return _alt->Scope(ss, src) && success;
}


bool IfExpr::Validate(ValidateData& dat)
{
	bool success {_cond->Validate(dat)};
	success = _body->Validate(dat) && success;
	success = _alt->Validate(dat) && success;
	_hasReturn = _body->_hasReturn && (_alt != nullptr && _alt->_hasReturn);

	if (!_cond->_type->IsBool())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected if statement condition of type bool, found: "sv
			<< _cond->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	return success;
}


void IfExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExpression("sv;
	_type->Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	PrintMaybe(_cond.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Else =\n"sv;
	PrintMaybe(_alt.get(), os, indent, depth + 1);
}


ForExpr::ForExpr(
	Expression* init, Expression* cond, Expression* inc, Statement* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


bool ForExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {_init->Scope(ss, src)};
	success = _cond->Scope(ss, src) && success;
	success = _inc->Scope(ss, src) && success;
	return _body->Scope(ss, src) && success;
}


bool ForExpr::Validate(ValidateData& dat)
{
	dat._bs.push_front(this);
	bool success {_init->Validate(dat)};
	success = _cond->Validate(dat) && success;
	success = _inc->Validate(dat) && success;
	success = _body->Validate(dat) && success;
	dat._bs.pop_front();
	return success;
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
	PrintMaybe(_init.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	PrintMaybe(_cond.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Increment =\n"sv;
	PrintMaybe(_inc.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body.get(), os, indent, depth + 1);
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
	dat._bs.push_front(this);
	bool success {_body->Validate(dat)};
	dat._bs.pop_front();
	return success;
}


void LoopExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExpression("sv;
	_type->Print(os, indent, depth);
	os << "):\n";
	PrintIndent(os, indent, ++ depth);
	os << "Body =:\n"sv;
	PrintMaybe(_body.get(), os, indent, ++ depth);
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
	dat._bs.push_front(this);
	bool success {_cond->Validate(dat)};
	return _body->Validate(dat) && success;
	dat._bs.pop_front();

	if (!_cond->_type->IsBool())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected if statement condition of type bool, found: "sv
			<< _cond->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	return success;
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
	PrintMaybe(_cond.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body.get(), os, indent, depth + 1);
}
