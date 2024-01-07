#include "statements.hpp"

#include <iostream>

using namespace std::string_view_literals;


CompoundStmt::CompoundStmt(StmtList kids)
	: _stmts{kids}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	bool success {true};
	ss.Enter();
	for (Statement* stmt : _stmts)
		success = success && stmt->Scope(ss, src, first_pass);
	ss.Exit();
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


bool VariableDef::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	TokenInfo* pre_def {dynamic_cast<TokenInfo*>(ss.Define(_name, this))}; // Trusting this for now.
	if (pre_def != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Symbol collision: "sv << _name
			<< "\n-> The following on line"sv << _row << ":\n";
		HighlightError(std::cerr, src, *this);
		std::cerr << "-> Redefines on line "sv << pre_def->_row << ":\n";
		HighlightError(std::cerr, src, *pre_def);
		return false;
	}
	return true;
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


void ExprStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ExpressionStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


BreakStmt::BreakStmt(Expression* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv << _levels << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}
