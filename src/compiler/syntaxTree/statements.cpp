#include "statements.hpp"

using namespace std::string_view_literals;


CompoundStmtNode::CompoundStmtNode(StmtList kids)
	: _stmts{kids}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


void CompoundStmtNode::Print(
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


VarDefStmtNode::VarDefStmtNode(TypeNode type, std::string name, ExprNode* init)
	: _type{type}, _name{name}, _init{init}
{}


ExprNode::ExprNode()
	: _type{""}
{}


void VarDefStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(_name = "sv << _name << ", Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


ExprStmtNode::ExprStmtNode(ExprNode* expr)
	: _expr{expr}
{}


void ExprStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ExpressionStatement:\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}


BreakStmtNode::BreakStmtNode(ExprNode* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


void BreakStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv << _levels << "):\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}


ReturnStmtNode::ReturnStmtNode(ExprNode* expr)
	: _expr{expr}
{}


void ReturnStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}
