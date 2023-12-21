#pragma once

#include "base.hpp"

#include <vector>


struct StmtNode : public SyntaxNode
{

};


struct CompoundStmtNode : public StmtNode
{
	using StmtList = std::vector<StmtNode*>;
	StmtList _kids;
	
	CompoundStmtNode(StmtList kids);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct VarDefStmtNode : public StmtNode
{
	TypeNode _type;
	std::string _name;
	ExprNode* _init;

	VarDefStmtNode(TypeNode type, std::string name, ExprNode* init);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct ExprNode : public StmtNode
{
	TypeNode _type;

	ExprNode();
};


struct ExprStmtNode : public StmtNode
{
	ExprNode* _expr;

	ExprStmtNode(ExprNode* expr);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct BreakStmtNode : public StmtNode
{
	int _levels {1};
	ExprNode* _expr;

	BreakStmtNode(ExprNode* expr, int levels = 1);
	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
