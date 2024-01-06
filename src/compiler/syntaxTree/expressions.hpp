#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string_view>

using namespace std::string_view_literals;


// Represents a binary expression.
struct BinaryExpr : public Expression
{
	// Enumerates all valid binary operators.
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

	Expression* _argl;	// The expression's left operand.
	Expression* _argr;	// The expression's right operand.
	Ops _op;			// The operator being applied.

	/**
	 * @brief Construct a new BinaryExpr object.
	 * 
	 * @param argl A left operand expression.
	 * @param argr A right operand expression.
	 * @param op The operator to be applied.
	 */
	BinaryExpr(Expression* argl, Expression* argr, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a prefix expression.
struct PreExpr : public Expression
{
	// Enumerates all valid prefix operators.
	enum class Ops
	{
		Pos,
		Neg,
		Inc,
		Dec,
		Deny,
		Comp
	};

	Expression* _arg;		// The expression's operand.
	Ops _op;			// The operator being applied.

	/**
	 * @brief Construct a new PreExpr object.
	 * 
	 * @param arg An operand expression.
	 * @param op The operator to be applied.
	 */
	PreExpr(Expression* arg, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a postfix expression.
struct PostExpr : public Expression
{
	// Enumeration of all valid postfix operators.
	enum class Ops
	{
		Inc,
		Dec
	};

	Expression* _arg;	// The expression's operand.
	Ops _op;		// The operator being applied.

	/**
	 * @brief Construct a new PostExpr object.
	 * 
	 * @param arg An operand expression.
	 * @param op The operator to be applied.
	 */
	PostExpr(Expression* arg, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represnts a function invocation.
struct Invocation : public Expression
{
	// Stores the arguments passed to a function.
	using ArgList = std::vector<Expression*>;

	std::string _name;		// The name of the function being called.
	SyntaxTreeNode* _fn;	// The AST node of the function being called.
	ArgList _args;			// The arguments specified by the function call.

	/**
	 * @brief Construct a new Invocation object.
	 * 
	 * @param name The name pf tje function being called.
	 * @param args The arguments specified by the function call. Assumed to be
	 * in reverse order after being parsed.
	 */
	Invocation(std::string name, ArgList args);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
