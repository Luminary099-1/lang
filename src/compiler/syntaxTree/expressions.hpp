#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string_view>

using namespace std::string_view_literals;


// Represents a binary expression.
struct BinaryExprNode : public ExprNode
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

	ExprNode* _argl;	// The expression's left operand.
	ExprNode* _argr;	// The expression's right operand.
	Ops _op;			// The operator being applied.

	/**
	 * @brief Construct a new BinaryExprNode object.
	 * 
	 * @param argl A left operand expression.
	 * @param argr A right operand expression.
	 * @param op The operator to be applied.
	 */
	BinaryExprNode(ExprNode* argl, ExprNode* argr, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a prefix expression.
struct PreExprNode : public ExprNode
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

	ExprNode* _arg;		// The expression's operand.
	Ops _op;			// The operator being applied.

	/**
	 * @brief Construct a new PreExprNode object.
	 * 
	 * @param arg An operand expression.
	 * @param op The operator to be applied.
	 */
	PreExprNode(ExprNode* arg, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a postfix expression.
struct PostExprNode : public ExprNode
{
	// Enumeration of all valid postfix operators.
	enum class Ops
	{
		Inc,
		Dec
	};

	ExprNode* _arg;	// The expression's operand.
	Ops _op;		// The operator being applied.

	/**
	 * @brief Construct a new PostExprNode object.
	 * 
	 * @param arg An operand expression.
	 * @param op The operator to be applied.
	 */
	PostExprNode(ExprNode* arg, Ops op);

	/**
	 * @brief Returns a view to a textual representation of the operator.
	 * 
	 * @param op The operator to be named.
	 * @return std::string_view A view to the operator's name.
	 */
	std::string_view GetOpText(Ops op);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represnts a function invocation.
struct InvokeNode : public ExprNode
// FIXME: Function arguments are stored in reverse order.
{
	// Stores the arguments passed to a function.
	using ArgList = std::vector<ExprNode*>;

	std::string _name;	// The name of the function being called.
	SyntaxNode* _fn;	// The AST node of the function being called.
	ArgList _args;		// The arguments specified by the function call.

	/**
	 * @brief Construct a new InvokeNode object.
	 * 
	 * @param name The name pf tje function being called.
	 * @param args The arguments specified by the function call.
	 */
	InvokeNode(std::string name, ArgList args);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
