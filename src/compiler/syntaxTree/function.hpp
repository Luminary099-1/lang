#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <map>
#include <string>
#include <vector>


// Represents a function definition.
struct FunctionNode : public SyntaxNode
{	
	// Stores a parameter expressed in the function definition.
	using Param = std::pair<TypeNode, std::string>;
	// Stores the parameters expressed in the function definition.
	using ParamList = std::vector<Param>;

	TypeNode _type;				// The function's return type.
	std::string _name;			// The name of the function.
	ParamList _params;			// The function's parameters.
	CompoundStmtNode* _body;	// The function's body statements.

	/**
	 * @brief Construct a new FunctionNode object.
	 * 
	 * @param type The function's return type.
	 * @param name The function's name.
	 * @param params The function's parameters. Assumed to be in reverse order
	 * after being parsed.
	 * @param body The function's body.
	 */
	FunctionNode(TypeNode type, std::string name 
		, ParamList params, CompoundStmtNode* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
