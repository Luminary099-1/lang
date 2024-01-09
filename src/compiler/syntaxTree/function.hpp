#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>


// Represents a function definition's parameters.
struct Parameter
	: public SyntaxTreeNode, public TokenInfo
{
	Type* _type {nullptr};	// The parameter's type.
	std::string _name;		// The parameter's name.

	// Default constructor.
	Parameter();

	/**
	 * @brief Construct a new Parameter object.
	 * 
	 * @param type The name of the parameter's type.
	 * @param name The parameter's name.
	 */
	Parameter(Type* type, std::string name);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a function definition.
struct Function
	: public SyntaxTreeNode, public TokenInfo
{
	// Stores the parameters expressed in the function definition.
	using ParamList = std::vector<std::unique_ptr<Parameter>>;

	Type* _type {nullptr};	// The function's return type.
	std::string _name;		// The name of the function.
	ParamList _params;		// The function's parameters.
	StmtList _body;			// The function's body statements.

	/**
	 * @brief Construct a new Function object.
	 * 
	 * @param type The function's return type.
	 * @param name The function's name.
	 * @param params The function's parameters. Assumed to be in reverse order
	 * after being parsed.
	 * @param body The function's body. Assumed to be in reverse order after
	 * being parsed.
	 */
	Function(Type* type, std::string name, ParamList& params, StmtList body);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
