#pragma once

#include "base.hpp"
#include "statements.hpp"


struct BinaryExprNode : public ExprNode
{
	enum class Ops
	{
		LAND,
		LOR,
		AND,
		XOR,
		OR,
		Eq,
		Neq,
		Less,
		LessEq,
		More,
		MoreEq,
		LShift,
		RShift,
		Add,
		Sub,
		Mul,
		Div,
		Mod
	};

	ExprNode* _argl;
	ExprNode* _argr;
	Ops _op;

	BinaryExprNode(ExprNode* argl, ExprNode* argr, Ops op);
};


struct PreExprNode : public ExprNode
{
	enum class Ops
	{
		Pos,
		Neg,
		Inc,
		Dec,
		Deny,
		Comp
	};

	ExprNode* _arg;
	Ops _op;

	PreExprNode(ExprNode* arg, Ops op);
};


struct PostExprNode : public ExprNode
{
	enum class Ops
	{
		Inc,
		Dec
	};

	ExprNode* _arg;
	Ops _op;

	PostExprNode(ExprNode* arg, Ops op);
};


struct InvokeNode : public ExprNode
{
	using ArgList = std::vector<ExprNode*>;

	std::string _name;
	SyntaxNode* _fn;
	ArgList _args;

	InvokeNode(std::string name, ArgList args);
};
