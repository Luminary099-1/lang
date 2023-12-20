#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <map>
#include <string>
#include <vector>


struct FunctionNode : public SyntaxNode
{
	using Param = std::pair<TypeNode, std::string>;
	using ParamList = std::vector<Param>;

	TypeNode _type;
	std::string _name;
	ParamList _params;
	CompoundStmtNode* _body;

	FunctionNode(TypeNode type, std::string name
		, ParamList params, CompoundStmtNode* body);
};
