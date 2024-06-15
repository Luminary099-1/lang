#pragma once

#include "base.hpp"
#include "common.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>


// Represents a function definition's parameters.
struct Parameter
	: public Declaration
{
	Type* _type {nullptr}; // Parameter's type.

	/**
	 * Construct a new Parameter object.
	 * @param type Name of the parameter's type.
	 * @param name Parameter's name.
	 */
	Parameter(Type* type, Identifier* name);

	bool Scope(SymTab& symbols, TU& tu) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the span of the parameter's type and name.
};


// Represents a function definition.
struct Function
	: public Declaration
{
	// Stores the parameters expressed in the function definition.
	using ParamList = std::vector<std::unique_ptr<Parameter>>;

	Type* _type {nullptr};	// Function's return type.
	ParamList _params;		// Function's parameters.
	StmtList _body;			// Function's body statements.
	bool _hasCall;			// Function's body contains a function call.

	/**
	 * Construct a new function.
	 * @param name Function's name.
	 * @param params Function's parameters. Assumed to be in reverse order after
	 * being parsed bottom-up.
	 * @param type Function's return type.
	 * @param body Function's body. Assumed to be in reverse order after being
	 * parsed bottom-up.
	 */
	Function(Identifier* name, ParamList params, Type* type, StmtList body);

	bool Scope(SymTab& symbols, TU& tu) override;
	bool Validate(ValidateData& dat) override;
	void Generate(GenData& dat, std::ostream& os) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the span from the name to return type.
};
