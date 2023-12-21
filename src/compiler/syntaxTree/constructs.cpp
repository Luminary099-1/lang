#include "constructs.hpp"


using namespace std::string_view_literals;


AssignExprNode::AssignExprNode(std::string name, ExprNode* expr)
	: _name{name}, _expr{expr}
{}


void AssignExprNode::Validate()
{}


void AssignExprNode::Scope()
{}


void AssignExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "AssignExprNode(name: "sv << _name << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Expr:\n";
	_expr->Print(os, indent, ++ depth);
}


IfExprNode::IfExprNode(ExprNode* cond, StmtNode* body, StmtNode* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


void IfExprNode::Validate()
{}


void IfExprNode::Scope()
{}


void IfExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExprNode:\n"sv;
	PrintIndent(os, indent, depth);
	os << "Cond:\n";
	_cond->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body:\n";
	_body->Print(os, indent, depth + 1);
	if (_alt == nullptr) return;
	PrintIndent(os, indent, depth);
	os << "Else:\n";
	_alt->Print(os, indent, depth + 1);
}


ForExprNode::ForExprNode(
	ExprNode* init, ExprNode* cond, ExprNode* inc, StmtNode* body)
	: _init{init}, _cond{cond}, _inc{inc}, _body{body}
{}


void ForExprNode::Validate()
{}


void ForExprNode::Scope()
{}


void ForExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ForExprNode:\n"sv;
	if (_init != nullptr)
	{
		PrintIndent(os, indent, depth);
		os << "Init:\n";
		_init->Print(os, indent, depth + 1);
	}
	if (_cond != nullptr)
	{
		PrintIndent(os, indent, depth);
		os << "Cond:\n";
		_cond->Print(os, indent, depth + 1);
	}
	if (_inc != nullptr)
	{
		PrintIndent(os, indent, depth);
		os << "Inc:\n";
		_inc->Print(os, indent, depth + 1);
	}
	PrintIndent(os, indent, depth);
	os << "Body:\n";
	_body->Print(os, indent, depth + 1);
}


LoopExprNode::LoopExprNode(StmtNode* body)
	: _body{body}
{}


void LoopExprNode::Validate()
{}


void LoopExprNode::Scope()
{}


void LoopExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "LoopExprNode:\n"sv;
	PrintIndent(os, indent, depth);
	os << "Body:\n";
	_body->Print(os, indent, depth + 1);
}


WhileExprNode::WhileExprNode(ExprNode* cond, StmtNode* body)
	: _cond{cond}, _body{body}
{}


void WhileExprNode::Validate()
{}


void WhileExprNode::Scope()
{}


void WhileExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	if (_cond != nullptr)
	{
		PrintIndent(os, indent, depth);
		os << "Cond:\n";
		_cond->Print(os, indent, depth + 1);
	}
	os << "WhileExprNode(op: "sv << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Body:\n";
	_body->Print(os, indent, depth + 1);
}
