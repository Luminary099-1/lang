#include "common.hpp"

#include "globals.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std::string_literals;
using namespace std::string_view_literals;


bool Statement::ValidateAndGetReturn(StmtList &stmts, ValidateData& dat,
	bool& success, RegT* eval_weight, bool* has_call)
{
	const size_t stmts_len {stmts.size()};
	if (stmts_len == 0) return false;

	Statement* cur {nullptr};
	Statement* last_ret {stmts[stmts_len - 1].get()};
	RegT max_weight {0};
	bool makes_call {false};
	for (size_t i {0}; i < stmts_len; ++ i)
	{
		cur = stmts[i].get();
		max_weight = std::max(cur->_evalWeight, max_weight);
		makes_call = makes_call || cur->_hasCall;
		success = cur->Validate(dat) && success;
		if (cur->_hasReturn) last_ret = cur;
	}

	if (last_ret == nullptr) return false;
	if (cur != last_ret)
	{
		std::cerr << '(' << cur->_row << ", "sv << cur->_col
			<< "): Unreachable code after a returning statement.\n"sv;
		dat._src->HighlightError(std::cerr, *cur);
		success = false;
		return false;
	}

	if (eval_weight != nullptr) *eval_weight = max_weight;
	if (has_call != nullptr) *has_call = makes_call;
	return true;
}


VariableDef::VariableDef(Type* type, Identifier* name, Expression* init)
	: Declaration{name}, _type{type}, _init{init}
{}


bool VariableDef::Validate(ValidateData& dat)
{
	bool success {_init->Validate(dat)};

	if (_type->IsVoid())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Variable cannot be type void: "sv << _name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	
	if (*_init->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected initializer of type "sv << _type->_name
			<< ", found: "sv << _init->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}

	_evalWeight = _init->_evalWeight;
	return success;
}


void VariableDef::Generate(GenData& dat, std::ostream& os)
{
	Location loc;

	if (dat._isGlobal)
	{
		_init->Generate(dat, os);
		const IDT label {dat.NextLabel()};
		loc = Location::CreateGlobal(_type, label);
		dat._globalVars[this->_type] = label;
	}
	else
	{
		_init->Generate(dat, os);
		dat._frameSize += _type->GetSize();
		loc = Location::CreateLocal(
			_type, dat._isSubFrame ? -dat._frameSize : dat._frameSize);
	}

	_type->GenerateAccess(dat, dat._locations[this], false, os);
}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(ID = "sv << _name->_id << ", Type = "sv
		<< _type->_name << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


IfStmt::IfStmt(Expression* cond, Statement* body, Statement* alt)
	: _cond{cond}, _body{body}, _alt{alt}
{}


bool IfStmt::Validate(ValidateData& dat)
{
	bool success {_cond->Validate(dat)};
	success = _body->Validate(dat) && success;
	if (_alt != nullptr)
	{
		success = _alt->Validate(dat) && success;
		_hasReturn = _body->_hasReturn && _alt->_hasReturn;
	}
	else _hasReturn = _body->_hasReturn;

	if (!_cond->_type->IsBool())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected if statement condition of type bool, found: "sv
			<< _cond->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}

	_evalWeight = std::max(_cond->_evalWeight, _body->_evalWeight);
	_evalWeight = std::max(_alt->_evalWeight, _evalWeight);
	return success;
}


void IfStmt::Generate(GenData& dat, std::ostream& os)
{
	_cond->Generate(dat, os);
	const RegT in_reg {dat._safeRegs.top()};
	const IDT after_body = dat.NextLabel();
	os << "\tcbz\tw"sv << in_reg << ",\tL_"sv << after_body << '\n';
	_body->Generate(dat, os);
	os << "L_"sv << after_body << ":\n"sv;
	_alt->Generate(dat, os);
}


void IfStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "IfExpression:\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Condition =\n"sv;
	_cond->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Body =\n"sv;
	_body->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Else =\n"sv;
	PrintMaybe(_alt.get(), os, indent, depth + 1);
}


BreakStmt::BreakStmt(Expression* expr, IntLiteral* count)
	: _expr{expr}, _count{count}
{}


bool BreakStmt::Validate(ValidateData& dat)
{
	const size_t count
		{static_cast<size_t>((_count == nullptr) ? 1 : _count->_value)};

	if (dat._bs.size() < count)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Break count exceeds breakable depth: "sv << count << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	_target = dat._bs[count - 1];

	const Type* existing {_target->_type};
	if (_expr != nullptr)
	{
		bool success {_expr->Validate(dat)};
		_evalWeight = _expr->_evalWeight;
		if (existing->IsVoid()) _target->_type = _expr->_type;
		else if (*existing != *_expr->_type)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected break expression of type "sv << existing->_name
				<< ", found: "sv << existing->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		return success;
	}
	else if (!existing->IsVoid())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected break expression, none provided.\n"sv;
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	else return true;
}


void BreakStmt::Generate(GenData& dat, std::ostream& os)
{
	if (_expr != nullptr)
	{
		_expr->Generate(dat, os);
		const RegT reg {dat._safeRegs.top()}; // This should work fine.
		const char s {(_expr->_type->GetSize() <= 4) ? 'w' : 'x'};
		os << "\tmov\t"sv << s << reg << ",\t"sv << s << reg << '\n';
	}

	os << "\tb\tL_"sv << dat._breakLabels[_target] << '\n';
}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv
		<< ((_count == nullptr) ? 0 : _count->_value) << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{
	_hasReturn = true;
}


bool ReturnStmt::Validate(ValidateData& dat)
{	
	if (dat._curFunc == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Return statement occurs outside of a function body.\n"sv;
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}

	_target = dat._curFunc;

	const Type* expected {dat._curFunc->_type};
	if (_expr == nullptr)
	{
		if (!expected->IsVoid())
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Return from a function of type "sv
				<< expected->_name << " is missing a return expression.\n"sv;
			dat._src->HighlightError(std::cerr, *this);
			return false;
		}
	}
	else
	{
		bool success {_expr->Validate(dat)};
		_evalWeight = _expr->_evalWeight;
		if (expected->IsVoid())
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Unexpected return expression in a void function.\n"sv;
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		else if (*_expr->_type != *expected)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected return expression of type "sv << expected->_name
				<< ", found: "sv << expected->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		return success;
	}
	return true;
}


void ReturnStmt::Generate(GenData& dat, std::ostream& os)
{
	if (_expr != nullptr)
	{
		_expr->Generate(dat, os);
		if (_expr->_type->GetSize() > 4) os << "\tmov\tx0,\tx"sv;
		else os << "\tmov\tw0,\tw"sv;
		os << dat._safeRegs.top() << '\n';
	}

	os << "\tb\tR_"sv << _target->_name << '\n';
}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


CompoundStmt::CompoundStmt(StmtList stmts, Expression* expr)
	: _stmts{std::move(stmts)}, _expr{expr}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Validate(ValidateData& dat)
{
	bool success {true};
	_hasReturn = Statement::ValidateAndGetReturn(
		_stmts, dat, success, &_evalWeight, &_hasCall);
	if (_expr != nullptr)
	{
		_type = _expr->_type;
		_hasCall = _expr->_hasCall;
		_evalWeight = std::max(_expr->_evalWeight, _evalWeight);
		if (_hasReturn)
		{
			std::cerr << '(' << _expr->_row << ", "sv << _expr->_col
				<< "): Evaluation never occurs as it appears after a "sv 
				<< "return statement."sv;
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
	}

	return success;
}


void CompoundStmt::Generate(GenData& dat, std::ostream& os)
{
	const BytesT prev_frame_size {dat._frameSize};
	bool is_first_sub_frame {false};
	if (!dat._isSubFrame)
	{
		is_first_sub_frame = true;
		dat._isSubFrame = true;
		dat._frameSize = 0;
	}

	std::stringstream dos; // Deferred output for the children.
	for (size_t i {0}; i < _stmts.size(); ++ i) _stmts[i]->Generate(dat, dos);

	const BytesT sub_frame_size {dat._frameSize - prev_frame_size};
	os << "\tsub\tsp,\tsp,\t"sv << sub_frame_size << '\n';
	os << dos.rdbuf();
	os << "\tadd\tsp,\tsp,\t"sv << sub_frame_size << '\n';
	
	if (is_first_sub_frame)
	{
		dat._isSubFrame = false;
		dat._frameSize = prev_frame_size;
	}
}


void CompoundStmt::Print(
	std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "CompoundStatement:\n"sv;
	++ depth;
	for (size_t i {0}; i < _stmts.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Statement["sv << i << "] ->\n"sv;
		_stmts[i]->Print(os, indent, depth + 1);
	}
	PrintMaybe(_expr.get(), os, indent, depth);
}
