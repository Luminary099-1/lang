#include "constructs.hpp"

using namespace std::string_view_literals;


AssignExprNode::AssignExprNode(std::string name, ExprNode* expr)
	: _name{name}, _expr{expr}
{}


void AssignExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignmentExpression(Name = "sv << _name << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Expression =\n"sv;
	PrintMaybe(_expr, os, indent, ++ depth);
}


IfExprNode::IfExprNode(ExprNode* cond, StmtNode* body, StmtNode* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


void IfExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExpression(Type = "sv;
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


ForExprNode::ForExprNode(
	ExprNode* init, ExprNode* cond, ExprNode* inc, StmtNode* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


void ForExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ForExpression(Type = "sv;
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


LoopExprNode::LoopExprNode(StmtNode* body)
	: _body{body}
{}


void LoopExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExpression(Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n";
	PrintIndent(os, indent, ++ depth);
	os << "Body =:\n"sv;
	PrintMaybe(_body, os, indent, ++ depth);
}


WhileExprNode::WhileExprNode(ExprNode* cond, StmtNode* body)
	: _cond{cond}, _body{body}
{}


void WhileExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "WhileExpression(Type = "sv;
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
