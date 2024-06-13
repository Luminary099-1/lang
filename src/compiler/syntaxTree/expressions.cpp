#include "common.hpp"

#include <iostream>

using namespace std::string_view_literals;


AssignmentExpr::AssignmentExpr(Identifier* name, Expression* expr)
	: _name{name}, _expr{expr}
{}


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
	_evalWeight = _expr->_evalWeight;
	// An additional register is required to store the destination address.
	if (dat._curFunc == nullptr) ++ _evalWeight;
	return success;
}


void AssignmentExpr::Generate(GenData& dat, std::ostream& os)
{
	_expr->Generate(dat, os);
	const RegT in_reg {dat._safeRegs.top()};
	Location loc {dat._locations[_def]};
	_type->GenerateAccess(dat, dat._locations[_def], false, os);
}


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
	_evalWeight = std::max(_init->_evalWeight, _cond->_evalWeight);
	_evalWeight = std::max(_inc->_evalWeight, _evalWeight);
	_evalWeight = std::max(_body->_evalWeight, _evalWeight);
	return success;
}


void LoopExpr::Generate(GenData& dat, std::ostream& os)
{
	const IDT before_loop = dat.NextLabel();
	const IDT after_loop = dat.NextLabel();
	dat._breakLabels[this] = after_loop;

	if (_init != nullptr) _init->Generate(dat, os);
	os << "L_"sv << before_loop << ":\n";

	if (_cond != nullptr)
	{
		_cond->Generate(dat, os);
		os << "\tcbz\tw"sv << dat._safeRegs.top()
			<< ",\tL_"sv << after_loop << '\n';
	}

	_body->Generate(dat, os);

	if (_inc != nullptr) _inc->Generate(dat, os);
	os << "\tb\tL_"sv << before_loop << '\n';
	os << "L_"sv << after_loop << ":\n"sv;
}


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
