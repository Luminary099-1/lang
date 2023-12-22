#include "statements.hpp"

using namespace std::string_view_literals;


CompoundStmtNode::CompoundStmtNode(StmtList kids)
	: _kids{kids}
{}


void CompoundStmtNode::Validate()
{}


void CompoundStmtNode::Scope()
{}


void CompoundStmtNode::Print(
	std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "CompoundStmtNode:\n"sv;
	++ depth;
	for (StmtNode* stmt : _kids) stmt->Print(os, indent, depth);
}


VarDefStmtNode::VarDefStmtNode(TypeNode type, std::string name, ExprNode* init)
	: _type{type}, _name{name}, _init{init}
{}


void VarDefStmtNode::Validate()
{}


void VarDefStmtNode::Scope()
{}


ExprNode::ExprNode()
	: _type{""}
{}


void VarDefStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VarDefStmtNode(name: "sv << _name << "):\n"sv;
	++ depth;
	_type.Print(os, indent, depth);
	_init->Print(os, indent, depth);
}


ExprStmtNode::ExprStmtNode(ExprNode* expr)
	: _expr{expr}
{}


void ExprStmtNode::Validate()
{}


void ExprStmtNode::Scope()
{}


void ExprStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ExprStmtNode:"sv;
	PrintIndent(os, indent, depth);
	os << "Expr:\n";
	_expr->Print(os, indent, ++ depth);
}


BreakStmtNode::BreakStmtNode(ExprNode* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


void BreakStmtNode::Validate()
{}


void BreakStmtNode::Scope()
{}


void BreakStmtNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStmtNode(levels: "sv << _levels << "):\n"sv;
	if (_expr == nullptr) return;
	PrintIndent(os, indent, depth);
	os << "Expr:\n";
	_expr->Print(os, indent, ++ depth);
}
