#include "expressions.hpp"


using namespace std::string_view_literals;


BinaryExprNode::BinaryExprNode(ExprNode* argl, ExprNode* argr, Ops op)
	: _argl{argl}, _argr{argr}, _op{op}
{}


void BinaryExprNode::Validate()
{}


void BinaryExprNode::Scope()
{}


void BinaryExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExprNode(op: "sv << GetOpName(_op) << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "ArgL:\n";
	_argl->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "ArgR:\n";
	_argr->Print(os, indent, depth + 1);
}


PreExprNode::PreExprNode(ExprNode* arg, Ops op)
	: _arg{arg}, _op{op}
{}


void PreExprNode::Validate()
{}


void PreExprNode::Scope()
{}


void PreExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExprNode(op: "sv << GetOpName(_op) << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Arg:\n";
	_arg->Print(os, indent, depth + 1);
}


PostExprNode::PostExprNode(ExprNode* arg, Ops op)
	: _arg{arg}, _op{op}
{}


void PostExprNode::Validate()
{}


void PostExprNode::Scope()
{}


void PostExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExprNode(op: "sv << GetOpName(_op) << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Arg:\n";
	_arg->Print(os, indent, depth + 1);
}
