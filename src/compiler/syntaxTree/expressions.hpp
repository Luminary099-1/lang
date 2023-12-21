#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string_view>


using namespace std::string_view_literals;


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
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
	std::string_view GetOpName(Ops op);
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
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
	std::string_view GetOpName(Ops op);
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
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
	std::string_view GetOpName(Ops op);
};


// FIXME: Function arguments are stored in reverse order.
struct InvokeNode : public ExprNode
{
	using ArgList = std::vector<ExprNode*>;

	std::string _name;
	SyntaxNode* _fn;
	ArgList _args;

	InvokeNode(std::string name, ArgList args);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
