#include "base.hpp"

using namespace std::string_view_literals;


void SyntaxTreeNode::PrintIndent(
	std::ostream& os, std::string_view indent, int depth)
{
	for (int i {0}; i < depth; ++ i) os << indent;
}


void SyntaxTreeNode::PrintMaybe(
	SyntaxTreeNode* node, std::ostream& os, std::string_view indent, int depth)
{
	if (node != nullptr) node->Print(os, indent, depth);
	else
	{
		PrintIndent(os, indent, depth);
		os << "nullptr\n"sv;
	}
}


Type::Type()
{}


Type::Type(std::string type_name)
	: _name{type_name}
{}


void Type::Print(std::ostream& os, std::string_view indent, int depth)
{
	os << _name;
}


void SymbolInfo::SetSymbolInfo(SymbolInfo info)
{
	_row		= info._row;
	_endRow		= info._endRow;
	_col		= info._col;
	_endCol		= info._endCol;
	_offset		= info._offset;
	_endOffset	= info._endOffset;
}
