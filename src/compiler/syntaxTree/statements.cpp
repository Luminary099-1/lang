#include "common.hpp"

#include "globals.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std::string_literals;
using namespace std::string_view_literals;


bool Statement::ValidateAndGetReturn(
	StmtList &stmts, ValidateData& dat, bool& success, bool* has_call)
{
	const size_t stmts_len {stmts.size()};
	if (stmts_len == 0) return false;

	Statement* cur {nullptr};
	Statement* last_ret {stmts[stmts_len - 1].get()};
	bool makes_call {false};
	for (size_t i {0}; i < stmts_len; ++ i)
	{
		cur = stmts[i].get();
		makes_call = makes_call || cur->_hasCall;
		success = cur->Validate(dat) && success;
		if (cur->_hasReturn) last_ret = cur;
	}

	if (last_ret == nullptr) return false;
	if (cur != last_ret)
	{
		std::cerr << '(' << cur->_row << ", "sv << cur->_col
			<< "): Unreachable code after a returning statement.\n"sv;
		dat._src->HighlightError(std::cerr, *cur);
		success = false;
		return false;
	}

	if (has_call != nullptr) *has_call = makes_call;
	return true;
}


VariableDef::VariableDef(Type* type, Identifier* name, Expression* init)
	: Declaration{name}, _type{type}, _init{init}
{}



bool VariableDef::Scope(SymbolTable& symbols, TU& tu)
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


bool VariableDef::Validate(ValidateData& dat)
{
	bool success {_init->Validate(dat)};

	if (_type->IsVoid())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Variable cannot be type void: "sv << _name->_id << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	
	if (*_init->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected initializer of type "sv << _type->_name
			<< ", found: "sv << _init->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}

	return success;
}


void VariableDef::Generate(GenData& dat, std::ostream& os)
{}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(ID = "sv << _name->_id << ", Type = "sv
		<< _type->_name << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


IfStmt::IfStmt(Expression* cond, Statement* body, Statement* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


bool IfStmt::Scope(SymbolTable& symbols, TU& tu)
{
	const bool success {_cond->Scope(symbols, tu) && _body->Scope(symbols, tu)};
	return (_cond != nullptr ? _cond->Scope(symbols, tu) : true) && success;
}


bool IfStmt::Validate(ValidateData& dat)
{
	bool success {_cond->Validate(dat)};
	success = _body->Validate(dat) && success;
	if (_alt != nullptr)
	{
		success = _alt->Validate(dat) && success;
		_hasReturn = _body->_hasReturn && _alt->_hasReturn;
	}
	else _hasReturn = _body->_hasReturn;

	if (!_cond->_type->IsBool())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected if statement condition of type bool, found: "sv
			<< _cond->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}

	return success;
}


void IfStmt::Generate(GenData& dat, std::ostream& os)
{}


void IfStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExpression:\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	_cond->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	_body->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Else =\n"sv;
	PrintMaybe(_alt.get(), os, indent, depth + 1);
}


BreakStmt::BreakStmt(Expression* expr, IntLiteral* count)
	: _expr{expr}, _count{count}
{}


bool BreakStmt::Scope(SymbolTable& symbols, TU& tu)
{
	return _expr != nullptr ? _expr->Scope(symbols, tu) : true;
}


bool BreakStmt::Validate(ValidateData& dat)
{
	const size_t count
		{static_cast<size_t>(_count == nullptr ? 1 : _count->_value)};

	if (dat._bs.size() < count)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Break count exceeds breakable depth: "sv << count << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	_target = dat._bs[count - 1];

	const Type* existing {_target->_type};
	if (_expr != nullptr)
	{
		bool success {_expr->Validate(dat)};
		if (existing->IsVoid()) _target->_type = _expr->_type;
		else if (*existing != *_expr->_type)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected break expression of type "sv << existing->_name
				<< ", found: "sv << existing->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		return success;
	}
	else if (!existing->IsVoid())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected break expression, none provided.\n"sv;
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	else return true;
}


void BreakStmt::Generate(GenData& dat, std::ostream& os)
{}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv
		<< (_count == nullptr ? 0 : _count->_value) << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{
	_hasReturn = true;
}


bool ReturnStmt::Scope(SymbolTable& symbols, TU& tu)
{
	return _expr != nullptr ? _expr->Scope(symbols, tu) : true;
}


bool ReturnStmt::Validate(ValidateData& dat)
{	
	if (dat._curFunc == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Return statement occurs outside of a function body.\n"sv;
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}

	_target = dat._curFunc;

	const Type* expected {dat._curFunc->_type};
	if (_expr == nullptr)
	{
		if (!expected->IsVoid())
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Return from a function of type "sv
				<< expected->_name << " is missing a return expression.\n"sv;
			dat._src->HighlightError(std::cerr, *this);
			return false;
		}
	}
	else
	{
		bool success {_expr->Validate(dat)};
		if (expected->IsVoid())
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Unexpected return expression in a void function.\n"sv;
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		else if (*_expr->_type != *expected)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected return expression of type "sv << expected->_name
				<< ", found: "sv << expected->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		return success;
	}
	return true;
}


void ReturnStmt::Generate(GenData& dat, std::ostream& os)
{}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


CompoundStmt::CompoundStmt(StmtList stmts, Expression* expr)
	: _stmts{std::move(stmts)}, _expr{expr}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Scope(SymbolTable& symbols, TU& tu)
{
	symbols.Enter();
	bool success {true};
	for (size_t i {0}; i < _stmts.size(); ++ i)
		success = _stmts[i]->Scope(symbols, tu) && success;
	success = (_expr != nullptr ? _expr->Scope(symbols, tu) : true) && success;
	symbols.Exit();
	return success;
}


bool CompoundStmt::Validate(ValidateData& dat)
{
	bool success {true};
	if (_expr != nullptr)
	{
		_type = _expr->_type;
		_hasCall = _expr->_hasCall;
		if (_hasReturn)
		{
			std::cerr << '(' << _expr->_row << ", "sv << _expr->_col
				<< "): Evaluation never occurs as it appears after a "sv 
				<< "return statement."sv;
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
	}

	return success;
}


void CompoundStmt::Generate(GenData& dat, std::ostream& os)
{}


void CompoundStmt::Print(
	std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "CompoundStatement:\n"sv;
	++ depth;
	for (size_t i {0}; i < _stmts.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Statement["sv << i << "] ->\n"sv;
		_stmts[i]->Print(os, indent, depth + 1);
	}
	PrintMaybe(_expr.get(), os, indent, depth);
}
