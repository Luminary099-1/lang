#pragma once

#include "base.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;


// The following is implemented in statements.cpp ==============================

struct Statement;	// Forward declaration.

// Stores a list of statements.
using StmtList = std::vector<std::unique_ptr<Statement>>;

// Base class to represent statements.
struct Statement
	: public virtual SyntaxTreeNode
{
	bool _hasReturn {false};	// Indicates whether all control paths return.

	/**
	 * @brief Determines whether the passed list of statements contains a
	 * return statement in a valid place (at the end of all control paths).
	 * 
	 * @param stmts The list of statements to consider.
	 * @param dat An instance of ValidateData to store the state of the
	 * validation.
	 * @param success Reference to a Boolean that will be updated with the
	 * success of the validations performed. A false value will not be set true.
	 * @return true if the statements return in a valid way; false otherwise.
	 */
	static bool
		ValidateAndGetReturn(StmtList& stmts, ValidateData& dat, bool& success);

	// TokenInfo should refer to the entire statement in extending classes.
};


// Abstract expression.
struct Expression
	: public Statement
{
	Type* _type {Type::Create("void")};	// The expression's type.
};


// Abstract representation of expressions that can be broken.
struct Breakable
	: public Expression
{};


// Represents a variable definition (including initialization).
struct VariableDef
	: public virtual Statement, public Declaration
{
	Type* _type;						// The variable's type.
	std::unique_ptr<Expression> _init;	// The variable's initializer.

	/**
	 * @brief Construct a new VariableDef object.
	 * 
	 * @param type The variable's type.
	 * @param name The variable's identifier.
	 * @param init The variable's initialization expression.
	 */
	VariableDef(Type* type, Identifier* name, Expression* init);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the equal symbol.
};


// Represents a conditional statement.
struct IfStmt
	: public Statement
{
	std::unique_ptr<Expression> _cond;	// A conditional expression.
	std::unique_ptr<Statement> _body;	// The statement executed on true.
	std::unique_ptr<Statement> _alt;	// The else case. Optional.

	/**
	 * @brief Construct a new IfStmt object;
	 * 
	 * @param cond A condition expression.
	 * @param body A statement executed on true.
	 * @param alt The else case if the condition is false.
	 */
	IfStmt(Expression* cond, Statement* body, Statement* alt);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
	
	// TokenInfo refers to the if keyword.
};


struct IntLiteral;	// Forward declaration.


// Represents a break statement.
struct BreakStmt
	: public Statement
{
	Breakable* _target {nullptr};		// The targetted breakable expression.
	std::unique_ptr<Expression> _expr;	// Yield expression. Optional.
	std::unique_ptr<IntLiteral> _count;	// The break count literal. Optional.

	/**
	 * @brief Construct a new BreakStmt object.
	 * 
	 * @param expr Yield expression. Optional.
	 * @param cout The number of expressions to break. Optional.
	 */
	BreakStmt(Expression* expr, IntLiteral* count);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the break keyword.
};


struct ReturnStmt
	: public Statement
{
	std::unique_ptr<Expression> _expr;	// An optional return expression.

	/**
	 * @brief Construct a new ReturnStmt object.
	 * 
	 * @param expr An optional return expression.
	 */
	ReturnStmt(Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the return keyword.
};


// Represents compound statements.
struct CompoundStmt
	: public Expression
{
	StmtList _stmts;					// This node's children statements.
	std::unique_ptr<Expression> _expr;	// An evaluation expression. Optional.
	
	/**
	 * @brief Construct a new CompoundStmt object.
	 * 
	 * @param stmts This node's children statements. Assumed to be in reverse 
	 * order after being parsed.
	 * @param expr An optional return expression, declared as the last statement
	 * without a semicolon.
	 */
	CompoundStmt(StmtList stmts, Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the inclusive span between curly braces.
};


// The following is implemented in operators.cpp ===============================

// Represents a binary expression.
struct BinaryExpr
	: public Expression
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
		NE,
		LT,
		LE,
		GT,
		GE,
		LShift,
		RShift,
		Add,
		Sub,
		Mul,
		Div,
		Mod
	};

	std::unique_ptr<Expression> _argl;	// The expression's left operand.
	std::unique_ptr<Expression> _argr;	// The expression's right operand.
	Ops _op;							// The operator being applied.

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
	
	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the operator symbol.
};


// Represents a prefix expression.
struct PreExpr
	: public Expression
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

	std::unique_ptr<Expression> _arg;	// The expression's operand.
	Ops _op;							// The operator being applied.

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

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the operator symbol.
};


// Represents a postfix expression.
struct PostExpr
	: public Expression
{
	// Enumeration of all valid postfix operators.
	enum class Ops
	{
		Inc,
		Dec
	};

	std::unique_ptr<Expression> _arg;	// The expression's operand.
	Ops _op;							// The operator being applied.

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

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the operator symbol.
};


// Represnts a function invocation.
struct Invocation
	: public Expression
{
	// Stores the arguments passed to a function.
	using ArgList = std::vector<std::unique_ptr<Expression>>;

	std::unique_ptr<Identifier> _name;	// The called function's identifier.
	Function* _def;						// The node defining the call.
	ArgList _args;						// The arguments specified by the call.

	/**
	 * @brief Construct a new Invocation object.
	 * 
	 * @param name The name of the function being called.
	 * @param args The arguments specified by the function call. Assumed to be
	 * in reverse order after being parsed.
	 */
	Invocation(Identifier* name, ArgList& args);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the span of the ID to the closing parentheses.
};


// The following is implemented in expressions.cpp =============================

// Represents an assignment expression.
struct AssignmentExpr
	: public Expression
{
	std::unique_ptr<Identifier> _name;	// The variable's identifier.
	VariableDef* _def;					// The AST node declaring the variable.
	std::unique_ptr<Expression> _expr;	// The expression being assigned.

	/**
	 * @brief Construct a new AssignmentExpr object.
	 * 
	 * @param name The variable's identifier.
	 * @param expr The expression being assigned to the variable.
	 */
	AssignmentExpr(Identifier* name, Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the equal symbol.
};


// Represents a counting for expression (a for loop that can return values).
struct ForExpr
	: public Breakable
{
	std::unique_ptr<Expression> _init;	// Runs before the loop. Optional.
	std::unique_ptr<Expression> _cond;	// Tested before iterations. Optional.
	std::unique_ptr<Expression> _inc;	// Runs after the condition. Optional.
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new ForExpr object.
	 * 
	 * @param init An initialization expression.
	 * @param cond A conditional expression.
	 * @param inc An increment expression.
	 * @param body The statement executed each iteration.
	 */
	ForExpr(Expression* init, Expression* cond, Expression* inc, Statement* body);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the for keyword.
};


// Represents an infinite loop expression (a loop that can return values).
struct LoopExpr
	: public Breakable
{
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new LoopExpr object;
	 * 
	 * @param body The statement executed each iteration.
	 */
	LoopExpr(Statement* body);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the loop keyword.
};


// Represents a while loop expression (a while loop that can return a value).
struct WhileExpr
	: public Breakable
{
	std::unique_ptr<Expression> _cond;	// Tested before iterations.
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new WhileExpr object.
	 * 
	 * @param cond A conditional expression.
	 * @param body The statement executed each iteration.
	 */
	WhileExpr(Expression* cond, Statement* body);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the while keyword.
};


// The following is implemented in literals.cpp ================================

// Base class to represent literals.
struct Literal
	: public Expression
{
	std::string_view _literalName;	// The name of the literal's node.
	std::string _rawValue;			// The raw value obtained from the source.

	/**
	 * @brief Construct a new Literal object.
	 * 
	 * @param literal_name
	 * @param raw_value The value described by the literal sans type descriptors.
	 */
	Literal(std::string_view literal_name, std::string& raw_value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo should refer to the entire literal.
};


// Represents identifiers.
struct Variable
	: public Literal
{
	SyntaxTreeNode* _def;	// The AST node that defines this variable.

	/**
	 * @brief Construct a new Variable object.
	 * 
	 * @param name The variable's identifier.
	 */
	Variable(std::string& name);

	void Generate(GenerateData& dat) override;
	bool Scope(ScopeStack& ss, TUBuffer& src) override;

	// TokenInfo refers to the identifier itself.
};


// Represents integer literals.
struct IntLiteral
	: public Literal
{
	int32_t _value {0};	// Concrete literal value.

	/**
	 * @brief Construct a new IntLiteral object.
	 * 
	 * @param raw_val The literal's integer value.
	 */
	IntLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;

	// TokenInfo refers to the value itself.
};


// Represents Boolean literals.
struct BoolLiteral
	: public Literal
{
	bool _value {false};	// Concrete literal value.

	/**
	 * @brief Construct a new BoolLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	BoolLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;

	// TokenInfo refers to the value itself.
};


// Represents string literals.
struct StrLiteral
	: public Literal
{
	std::string _value;	// Concrete literal value.

	/**
	 * @brief Construct a new StrLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	StrLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;
	void Generate(GenerateData& dat) override;

	// TokenInfo refers to the value itself.
};
