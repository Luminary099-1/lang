#include "statements.hpp"

#include "constructs.hpp"
#include "function.hpp"

#include <iostream>

using namespace std::string_view_literals;


bool Statement::DoesListReturn(StmtList &stmts, TUBuffer& src)
{
	const size_t stmts_len {stmts.size()};
	if (stmts_len == 0) return false;

	Statement* cur {nullptr};
	Statement* last_ret {stmts[stmts_len - 1].get()};
	for (size_t i {0}; i < stmts_len; ++ i)
	{
		cur = stmts[i].get();
		if (cur->_hasReturn) last_ret = cur;
	}

	if (last_ret == nullptr) return false;
	if (cur != last_ret)
	{
		// TODO: Error message here for dead code.
		return false;
	}
	return true;
}


CompoundStmt::CompoundStmt(StmtList kids)
	: _stmts{kids}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	ss.Enter();
	for (size_t i {0}; i < _stmts.size(); ++ i)
		success = _stmts[i]->Scope(ss, src) && success;
	ss.Exit();
	return success;
}


bool CompoundStmt::Validate(ValidateData& dat)
{
	bool success {true};
	const size_t stmts_len {_stmts.size()};
	Statement* last {_stmts[stmts_len - 1].get()};
	for (size_t i {0}; i < stmts_len; ++ i)
	{
		Statement* cur {_stmts[i].get()};
		if (cur != last && dynamic_cast<Expression*>(cur) != nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Statement yields value before the block ends.\n"sv;
			HighlightError(std::cerr, dat._src, *this);
			success = false;
		}
		success = cur->Validate(dat) && success;
	}

	Expression* last_expr {dynamic_cast<Expression*>(last)};
	if (last_expr != nullptr) _type = last_expr->_type;

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
}


VariableDef::VariableDef(Type* type, std::string name, Expression* init)
	: _type{type}, _name{name}, _init{init}
{}


bool VariableDef::Scope(ScopeStack& ss, TUBuffer& src)
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
	return _init->Scope(ss, src) && success;
}


bool VariableDef::Validate(ValidateData& dat)
{
	bool success {_init->Validate(dat)};

	if (*_init->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected initializer of type "sv << _type->_name
			<< ", found: "sv << _init->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}

	return success;
}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(_name = "sv << _name << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


ExprStmt::ExprStmt(Expression* expr)
	: _expr{expr}
{}


bool ExprStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool ExprStmt::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};
	return success;
}


void ExprStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ExpressionStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


BreakStmt::BreakStmt(Expression* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


bool BreakStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool BreakStmt::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};

	Breakable* expr {nullptr};
	int broken {0};
	for (SyntaxTreeNode* node : dat._bs)
	{
		expr = dynamic_cast<Breakable*>(node);
		if (expr != nullptr)
		{
			broken ++;
			if (broken == _levels) break;
		}
	}

	if (broken < _levels)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Break exceeds breakable depth: "sv << _levels << '\n';
		HighlightError(std::cerr, dat._src, *this);
	}
	else if (broken == _levels)
	{
		_target = expr;
		_target->_type = _expr->_type;
	}

	return success;
}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv << _levels << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{
	_hasReturn = true;
}


bool ReturnStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool ReturnStmt::Validate(ValidateData& dat)
{
	bool success {_expr->Validate(dat)};
	
	if (dat._curFunc == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Return statement outside of function body.\n"sv;
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}

	if (*dat._curFunc->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected return value of type "sv
			<< dat._curFunc->_type->_name
			<< ", found: "sv << _type->_name << '\n';
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
