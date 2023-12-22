#include "expressions.hpp"

using namespace std::string_view_literals;


BinaryExprNode::BinaryExprNode(ExprNode* argl, ExprNode* argr, Ops op)
	: _argl{argl}, _argr{argr}, _op{op}
{}


std::string_view BinaryExprNode::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::LAND:		return "&&"sv;
		case Ops::LOR:		return "||"sv;
		case Ops::AND:		return "&"sv;
		case Ops::XOR:		return "^"sv;
		case Ops::OR:		return "|"sv;
		case Ops::Eq:		return "=="sv;
		case Ops::Neq:		return "!="sv;
		case Ops::Less:		return "<"sv;
		case Ops::LessEq:	return "<="sv;
		case Ops::More:		return ">"sv;
		case Ops::MoreEq:	return ">="sv;
		case Ops::LShift:	return "<<"sv;
		case Ops::RShift:	return ">>"sv;
		case Ops::Add:		return "+"sv;
		case Ops::Sub:		return "-"sv;
		case Ops::Mul:		return "*"sv;
		case Ops::Div:		return "/"sv;
		case Ops::Mod:		return "%"sv;
	}
}


void BinaryExprNode::Validate()
{}


void BinaryExprNode::Scope()
{}


void BinaryExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExprNode(op: "sv << GetOpText(_op) << "):\n"sv;
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


std::string_view PreExprNode::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::Pos: 		return "+"sv;
		case Ops::Neg: 		return "-"sv;
		case Ops::Inc: 		return "++"sv;
		case Ops::Dec: 		return "--"sv;
		case Ops::Deny: 	return "!"sv;
		case Ops::Comp: 	return "~"sv;
	}
}


void PreExprNode::Validate()
{}


void PreExprNode::Scope()
{}


void PreExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExprNode(op: "sv << GetOpText(_op) << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Arg:\n";
	_arg->Print(os, indent, depth + 1);
}


PostExprNode::PostExprNode(ExprNode* arg, Ops op)
	: _arg{arg}, _op{op}
{}


std::string_view PostExprNode::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::Inc: return "++"sv;
		case Ops::Dec: return "--"sv;
	}
}


void PostExprNode::Validate()
{}


void PostExprNode::Scope()
{}


void PostExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExprNode(op: "sv << GetOpText(_op) << "):\n"sv;
	PrintIndent(os, indent, depth);
	os << "Arg:\n";
	_arg->Print(os, indent, depth + 1);
}
