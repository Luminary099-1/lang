#include "statements.hpp"

#include "constructs.hpp"
#include "function.hpp"

#include <iostream>

using namespace std::string_literals;
using namespace std::string_view_literals;


bool Statement::ValidateAndGetReturn(
	StmtList &stmts, ValidateData& dat, bool& success)
{
	const size_t stmts_len {stmts.size()};
	if (stmts_len == 0) return false;

	Statement* cur {nullptr};
	Statement* last_ret {stmts[stmts_len - 1].get()};
	for (size_t i {0}; i < stmts_len; ++ i)
	{
		cur = stmts[i].get();
		success = cur->Validate(dat) && success;
		if (cur->_hasReturn) last_ret = cur;
	}

	if (last_ret == nullptr) return false;
	if (cur != last_ret)
	{
		std::cerr << '(' << cur->_row << ", "sv << cur->_col
			<< "): Unreachable code after a returning statement.\n"sv;
		HighlightError(std::cerr, dat._src, *cur);
		success = false;
		return false;
	}
	return true;
}


CompoundStmt::CompoundStmt(StmtList stmts, Expression* expr)
	: _stmts{std::move(stmts)}, _expr{expr}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	ss.Enter();
	for (size_t i {0}; i < _stmts.size(); ++ i)
		success = _stmts[i]->Scope(ss, src) && success;
	if (_expr != nullptr) success = _expr->Scope(ss, src) && success;
	ss.Exit();
	return success;
}


bool CompoundStmt::Validate(ValidateData& dat)
{
	bool success {true};
	_hasReturn = Statement::ValidateAndGetReturn(_stmts, dat, success);
	if (_expr != nullptr)
	{
		_type = _expr->_type;
		if (_hasReturn)
		{
			std::cerr << '(' << _expr->_row << ", "sv << _expr->_col
				<< "): Evaluation never occurs as it appears after a "sv 
				<< "return statement."sv;
			HighlightError(std::cerr, dat._src, *_expr);
			success = false;
		}
	}

	return success;
}


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


VariableDef::VariableDef(Type* type, Identifier* name, Expression* init)
	: Declaration{name}, _type{type}, _init{init}
{}


bool VariableDef::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {Declaration::Scope(ss, src)};
	return _init->Scope(ss, src) && success;
}


bool VariableDef::Validate(ValidateData& dat)
{
	bool success {_init->Validate(dat)};

	if (_type->IsVoid())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Variable cannot be type void: "sv << _name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}
	
	if (*_init->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected initializer of type "sv << _type->_name
			<< ", found: "sv << _init->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}

	return success;
}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(ID = "sv << _name->_id << ", Type = "sv
		<< _type->_name << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


BreakStmt::BreakStmt(Expression* expr, IntLiteral* count)
	: _expr{expr}, _count{count}
{}


bool BreakStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	if (_expr != nullptr) return _expr->Scope(ss, src);
	else return true;
}


bool BreakStmt::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};
	const int count {(_count == nullptr) ? 0 : _count->_value};

	if (dat._bs.size() < count)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Break count exceeds breakable depth: "sv << count << '\n';
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}
	_target = dat._bs[count - 1];

	const Type* existing {_target->_type};
	if (!existing->IsVoid())
	{
		if (_expr == nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected break expression, none provided.\n"sv;
			HighlightError(std::cerr, dat._src, *this);
			return false;
		}
		else if (*_expr->_type != *existing)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected break expression of type "sv << existing->_name
				<< ", found: "sv << existing->_name << '\n';
			HighlightError(std::cerr, dat._src, *this);
			return false;
		}
	}
	else if (_expr != nullptr) _target->_type = _expr->_type;

	return success;
}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv
		<< ((_count == nullptr) ? 0 : _count->_value) << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{}


bool ReturnStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	if (_expr != nullptr) return _expr->Scope(ss, src);
	else return true;
}


bool ReturnStmt::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};
	_hasReturn = true;
	
	if (dat._curFunc == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Return statement occurs outside of a function body.\n"sv;
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}

	const Type* expected {dat._curFunc->_type};
	if (!expected->IsVoid())
	{
		if (_expr == nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Return from a function of type "sv
				<< expected->_name << " is missing a return expression.\n"sv;
			HighlightError(std::cerr, dat._src, *this);
			return false;
		}
		else if (*_expr->_type != *expected)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected return expression of type "sv << expected->_name
				<< ", found: "sv << expected->_name << '\n';
			HighlightError(std::cerr, dat._src, *this);
			return false;
		}
	}
	else if (_expr != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unexpected return expression from a void function.\n"sv;
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}

	return success;
}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}
