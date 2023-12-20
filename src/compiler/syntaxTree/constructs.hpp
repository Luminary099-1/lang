#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string>


struct AssignExprNode : public ExprNode
{
	std::string _name;
	ExprNode* _expr;

	AssignExprNode(std::string name, ExprNode* expr);
};


struct IfExprNode : public ExprNode
{
	ExprNode* _cond;
	StmtNode* _body;
	StmtNode* _alt;

	IfExprNode(ExprNode* cond, StmtNode* body, StmtNode* _alt);
};


struct ForExprNode : public ExprNode
{
	ExprNode* _init;
	ExprNode* _cond;
	ExprNode* _inc;
	StmtNode* _body;

	ForExprNode(ExprNode* init, ExprNode* cond, ExprNode* inc, StmtNode* body);
};


struct LoopExprNode : public ExprNode
{
	StmtNode* _body;

	LoopExprNode(StmtNode* body);
};


struct WhileExprNode : public ExprNode
{
	ExprNode* _cond;
	StmtNode* _body;

	WhileExprNode(ExprNode* cond, StmtNode* body);
};
