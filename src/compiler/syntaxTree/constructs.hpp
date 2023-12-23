#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string>


// Represents an assignment expression.
struct AssignExprNode : public ExprNode
{
	std::string _name;	// The name of the variable being assigned.
	ExprNode* _expr;	// The expression being assigned to the variable.

	/**
	 * @brief Construct a new AssignExprNode object.
	 * 
	 * @param name The name of the variable being assigned.
	 * @param expr The expression being assigned to the variable.
	 */
	AssignExprNode(std::string name, ExprNode* expr);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an if expression (if statement that can return values).
struct IfExprNode : public ExprNode
{
	ExprNode* _cond;	// The condition expression. Must be a Boolean.
	StmtNode* _body;	// A statement executed if the condition is true.
	StmtNode* _alt;		// The else case if the condition is false (optional).

	/**
	 * @brief Construct a new IfExprNode object;
	 * 
	 * @param cond A condition expression.
	 * @param body A statement executed on true.
	 * @param alt The else case if the condition is false.
	 */
	IfExprNode(ExprNode* cond, StmtNode* body, StmtNode* alt);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a counting for expression (a for loop that can return values).
struct ForExprNode : public ExprNode
{
	ExprNode* _init;	// Initialization expression; runs before the loop.
	ExprNode* _cond;	// Conditional expression; runs between iterations.
	ExprNode* _inc;		// Increment expression; runs after the condition.
	StmtNode* _body;	// The statement to be executed each iteration.

	/**
	 * @brief Construct a new ForExprNode object.
	 * 
	 * @param init An initialization expression.
	 * @param cond A condition expression.
	 * @param inc An increment expression.
	 * @param body The statement executed each iteration.
	 */
	ForExprNode(ExprNode* init, ExprNode* cond, ExprNode* inc, StmtNode* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an infinite loop expression (a loop that can return values).
struct LoopExprNode : public ExprNode
{
	StmtNode* _body;	// The statement to be executed each iteration.

	/**
	 * @brief Construct a new LoopExprNode object;
	 * 
	 * @param body The statement executed each iteration.
	 */
	LoopExprNode(StmtNode* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a while loop expression (a while loop that can return a value).
struct WhileExprNode : public ExprNode
{
	ExprNode* _cond;	// Conditional expression; runs between iterations.
	StmtNode* _body;	// // The statement to be executed each iteration.

	/**
	 * @brief Construct a new While Expr Node object
	 * 
	 * @param cond A condition expression.
	 * @param body The statement executed each iteration.
	 */
	WhileExprNode(ExprNode* cond, StmtNode* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
