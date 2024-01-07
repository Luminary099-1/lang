#include "constructs.hpp"

#include <iostream>

using namespace std::string_view_literals;


AssignmentExpr::AssignmentExpr(std::string name, Expression* expr)
	: _name{name}, _expr{expr}
{}


bool AssignmentExpr::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	SyntaxTreeNode* pre_def {ss.Lookup(_name)};
	if (pre_def == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown function: " << _name << '\n';
		HighlightError(std::cerr, src, *this);
		return false;
	}
	return true;
}


void AssignmentExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignmentExpression(Name = "sv << _name << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Expression =\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}


IfExpr::IfExpr(Expression* cond, Statement* body, Statement* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


void IfExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExpression("sv;
	_type.Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	PrintMaybe(_cond, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Else =\n"sv;
	PrintMaybe(_alt, os, indent, depth + 1);
}


ForExpr::ForExpr(
	Expression* init, Expression* cond, Expression* inc, Statement* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


void ForExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ForExpression("sv;
	_type.Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Initialize =\n"sv;
	PrintMaybe(_init, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	PrintMaybe(_cond, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Increment =\n"sv;
	PrintMaybe(_inc, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body, os, indent, depth + 1);
}


LoopExpr::LoopExpr(Statement* body)
	: _body{body}
{}


void LoopExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExpression("sv;
	_type.Print(os, indent, depth);
	os << "):\n";
	PrintIndent(os, indent, ++ depth);
	os << "Body =:\n"sv;
	PrintMaybe(_body, os, indent, ++ depth);
}


WhileExpr::WhileExpr(Expression* cond, Statement* body)
	: _cond{cond}, _body{body}
{}


void WhileExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "WhileExpression("sv;
	_type.Print(os, indent, depth);
	os << "):\n";
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	PrintMaybe(_cond, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	PrintMaybe(_body, os, indent, depth + 1);
}
