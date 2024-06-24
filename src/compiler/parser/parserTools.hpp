#pragma once

#include "parser.h"

#include <stdexcept>
#include <iostream>
#include <string_view>


// Tools for interacting with the parser.
namespace Parser
{
	using namespace std::string_view_literals;

	// Out of memory error message constant.
	const char* const OOM {"Out of memory."};
	// Fatal internal parser error message constant.
	const char* const FIE {"Fatal interal error."};

	/**
	 * Parses and returns the AST expressed by the provided translation unit
	 * source.
	 * @param stack Parser stack to use.
	 * @param tu Translation unit to read from.
	 * @param ast Destination AST to store the parser results in.
	 * @return true if parsing succeeded without issue; false if a lexical or
	 * syntactic error occurred.
	 */
	bool getAST(carb_stack& stack, TU& tu, AST& ast);
}
