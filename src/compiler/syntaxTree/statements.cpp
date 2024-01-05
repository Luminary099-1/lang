#include "statements.hpp"

using namespace std::string_view_literals;


CompoundStmt::CompoundStmt(StmtList kids)
	: _stmts{kids}
{
	std::reverse(_stmts.begin(), _stmts.end());
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
		os << "Statement["sv << i << "] =\n"sv;
		_stmts[i]->Print(os, indent, depth + 1);
	}
}


VariableDef::VariableDef(Type type, std::string name, Expression* init)
	: _type{type}, _name{name}, _init{init}
{}


Expression::Expression()
	: _type{""}
{}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(_name = "sv << _name << ", Type = "sv;
	_type.Print(os, indent, depth);
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
	PrintMaybe(_expr, os, indent, ++ depth);
}


BreakStmt::BreakStmt(Expression* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv << _levels << "):\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}
