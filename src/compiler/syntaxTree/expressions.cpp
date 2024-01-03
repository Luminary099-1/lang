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

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


void BinaryExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExpression(Op = "sv << GetOpText(_op) << ", Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Left Operand =\n"sv;
	PrintMaybe(_argl, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Right Operand =\n"sv;
	PrintMaybe(_argr, os, indent, depth + 1);
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

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


void PreExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExpression(Op = "sv << GetOpText(_op) << ", Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg, os, indent, ++ depth);
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

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


void PostExprNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExpression(Op = "sv << GetOpText(_op) << ", Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg, os, indent, ++ depth);
}


InvokeNode::InvokeNode(std::string name, ArgList args)
	: _name{name}, _args{args}
{
	std::reverse(_args.begin(), _args.end());
}


void InvokeNode::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "InvokeExpression(Function = "sv << _name << ", Type = "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _args.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Arg["sv << i << "] =\n"sv;
		_args[i]->Print(os, indent, depth + 1);
	}
}
