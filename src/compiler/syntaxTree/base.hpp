#pragma once

#include <string>
#include <string_view>
#include <ostream>
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
	virtual void
		Print(std::ostream& os, std::string_view indent, int depth);
	void PrintIndent(std::ostream& os, std::string_view indent, int depth);
};


// This class is unnecessary now, but will facilitate indirection of defined types.
struct TypeNode : public SyntaxNode
{
	enum class BasicTypes
	{
		Void,
		Int,
		Bool,
		String
	};

	std::string _name;
	BasicTypes _type;

	TypeNode(std::string type_name);
	void Resolve(/* Something goes here that refers to the scope stack. */);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
