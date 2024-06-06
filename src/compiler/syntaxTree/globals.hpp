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
	Type* _type {nullptr}; // The parameter's type.

	/**
	 * @brief Construct a new Parameter object.
	 * 
	 * @param type The name of the parameter's type.
	 * @param name The parameter's name.
	 */
	Parameter(Type* type, Identifier* name);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the span of the parameter's type and name.
};


// Represents a function definition.
struct Function
	: public Declaration
{
	// Stores the parameters expressed in the function definition.
	using ParamList = std::vector<std::unique_ptr<Parameter>>;

	Type* _type {nullptr};	// This function's return type.
	ParamList _params;		// This function's parameters.
	StmtList _body;			// This function's body statements.
	bool _hasCall;			// This function's body contains a function call.

	/**
	 * @brief Construct a new Function object.
	 * 
	 * @param name The function's name.
	 * @param params The function's parameters. Assumed to be in reverse order
	 * after being parsed.
	 * @param type The function's return type.
	 * @param body The function's body. Assumed to be in reverse order after
	 * being parsed.
	 */
	Function(Identifier* name, ParamList params, Type* type, StmtList body);

	/**
	 * @brief Determine the storage allocations for arguments for this
	 * function's parameters.
	 * 
	 * Implements the algorithm for te marshalling of parameters described in
	 * the ARM64 Procedure Call Standard (aapcs64), a part of the ARM ABI.
	 * 
	 * @param dat Instance of GenData with the appropriate context for this
	 * opperation.
	 */
	BytesT MarshalParams(GenData& dat);

	bool Validate(ValidateData& dat) override;
	void Generate(GenData& dat, std::ostream& os) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the span from the name to the closing parentheses.
};
