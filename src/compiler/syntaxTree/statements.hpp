#pragma once

#include "base.hpp"

#include <vector>


// Base class to represent statements.
struct StmtNode : public SyntaxNode {};


// Represents compound statements.
struct CompoundStmtNode : public ExprNode
{
	// Stores a list of statements.
	using StmtList = std::vector<StmtNode*>;

	StmtList _kids;	// This node's children statements.
	
	/**
	 * @brief Construct a new CompoundStmtNode object.
	 * 
	 * @param kids This node's children statements. Assumed to be in reverse 
	 * order after being parsed.
	 */
	CompoundStmtNode(StmtList kids);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a variable definition (including initialization).
struct VarDefStmtNode : public StmtNode
{
	TypeNode _type;		// The variable's type.
	std::string _name;	// The variable's name.
	ExprNode* _init;	// The variable's initialization expression.

	/**
	 * @brief Construct a new VarDefStmtNode object.
	 * 
	 * @param type The variable's type.
	 * @param name The variable's name.
	 * @param init The variable's initialization expression.
	 */
	VarDefStmtNode(TypeNode type, std::string name, ExprNode* init);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Base class to represent expressions.
struct ExprNode : public StmtNode
{
	TypeNode _type;	// The expression's type.

	/**
	 * @brief Construct a new ExprNode object.
	 * 
	 * Satisfied the need for a default constructor. The type is only known
	 * after Scope() is called.
	 */
	ExprNode();
};


// Represents an expression statement (does not return its evaluation).
struct ExprStmtNode : public StmtNode
{
	ExprNode* _expr;	// The contained expression.

	/**
	 * @brief Construct a new ExprStmtNode object.
	 * 
	 * @param expr The expression to be contained.
	 */
	ExprStmtNode(ExprNode* expr);
	
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a break statement.
struct BreakStmtNode : public ExprNode
{
	int _levels {1};	// The number of loop levels to break from.
	ExprNode* _expr;	// The expression to be returned (optional).

	/**
	 * @brief Construct a new BreakStmtNode object.
	 * 
	 * @param expr The number of loop levels to break from.
	 * @param levels The expression to be returned.
	 */
	BreakStmtNode(ExprNode* expr, int levels = 1);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
