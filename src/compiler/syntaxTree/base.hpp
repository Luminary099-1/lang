#pragma once

#include <string>
#include <vector>


struct Unit
{
	std::vector<SyntaxNode*> _defs;

	Unit();
};


struct SyntaxNode
{
	virtual void Validate() = 0;
	virtual void Scope() = 0;
};


// This class is unnecessary now, but will facilitate indirection of defined types.
struct TypeNode
{
	enum class BasicTypes
	{
		Void,
		Int,
		Bool,
		String
	};

	BasicTypes _type;

	TypeNode(std::string type_name);
	static BasicTypes GetType(std::string type_name);
};
