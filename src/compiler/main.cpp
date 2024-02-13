#include "parse.h"
#include "syntaxTree/base.hpp"
#include "syntaxTree/common.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std::string_view_literals;


void printErrorLine(char* src, int src_len, carb_stack* stack)
{
	size_t line_start {carb_offset(stack) - carb_column(stack) + 1};
	std::cerr << '\t';
	for (size_t i {line_start}; src[i] != '\n' && i < src_len; ++ i)
		std::cerr << src[i];
	std::cerr << '\n' << '\t';
	for (int i {0}; i < carb_column(stack) - 1; ++ i) std::cerr << ' ';
	for (int i {0}; i < carb_len(stack); ++ i) std::cerr << '^';
	std::cerr << '\n';
}


bool runParser(char* src, int src_len,
	carb_stack* stack, std::vector<SyntaxTreeNode*>& node_list)
{
	bool failed {false};
	int err_code {-1};

	while (err_code != 0)
	{
		err_code = carb_scan(stack, node_list);
		switch (err_code)
		{
			case _CARB_FINISH:
				break;
			case _CARB_LEXICAL_ERROR:
				failed = true;
				std::cerr
					<< '(' << carb_line(stack) << ", "sv << carb_column(stack)
					<< "): Unexpected character: "sv;
				std::cerr.write(carb_text(stack), 1);
				std::cerr << "\n"sv;
				printErrorLine(src, src_len, stack);
				break;
			case _CARB_SYNTAX_ERROR:
				failed = true;
				std::cerr
					<< '(' << carb_line(stack) << ", "sv << carb_column(stack)
					<< "): Unexpected token: "sv;
				std::cerr.write(carb_text(stack), carb_len(stack));
				std::cerr << "\n"sv;
				printErrorLine(src, src_len, stack);
				break;
			case _CARB_NO_MEMORY:
				failed = true;
				std::cerr << "Ran out of memory while parsing.\n"sv;
				break;
			case _CARB_OVERFLOW:
				failed = true;
				std::cerr << "An overflow error occurred while parsing.\n"sv;
				break;
			default:
				failed = true;
				std::cerr << "An unspecified error occurred while parsing.\n"sv;
		}

		if (failed) {
			if (carb_stack_can_recover(stack)) err_code = -1;
			else break;
		}
	}

	return failed;
}


int readNodes(char* src_path, std::vector<SyntaxTreeNode*>& destTU)
{
	std::ifstream src_file;
	src_file.open(src_path, std::ios_base::in);
	if (!src_file)
	{
		std::cerr
			<< "Unable to open source file: "sv
			<< src_path << '\n';
		return EXIT_FAILURE;
	}

	src_file.seekg(0, std::ios_base::end);
	const int tu_size = src_file.tellg();
	src_file.seekg(0, std::ios_base::beg);
	char* tu_buffer {reinterpret_cast<char*>(malloc(tu_size))};
	src_file.read(tu_buffer, tu_size);
	src_file.close();

	carb_stack stack;
	carb_stack_init(&stack);
	carb_set_input(&stack, tu_buffer, tu_size, 1);
	const bool failed = runParser(tu_buffer, tu_size, &stack, destTU);
	carb_stack_cleanup(&stack);
	delete tu_buffer;
	return (failed) ? EXIT_FAILURE : EXIT_SUCCESS;
}


/**
 * @brief Validates the provided translation unit's AST. 
 * 
 * @param tu AST to be validated.
 * @param dat Validation data initialized with the TUBuffer associated with the
 * passed AST.
 * @return true if the validation was successful; false otherwise.
 */
bool validate(std::vector<SyntaxTreeNode*>& tu, ValidateData& dat)
{
	bool success = true;
	ScopeStack ss;
	ss.Enter();

	for (SyntaxTreeNode* node : tu) node->Scope(ss, dat._src) && success;
	if (!success) return false;
	else if (ss.Lookup("main"sv) == nullptr)
	{
		std::cerr << "The function 'main' is undefined.\n"sv;
		return false;
	}

	for (SyntaxTreeNode* node : tu) node->Validate(dat) && success;
	return success;
}


/**
 * @brief Generates assembly output from the provided translation unit's AST.
 * 
 * @param tu AST to be generated.
 */
void generate(std::vector<SyntaxTreeNode*>& tu)
{	
	GenData dat;
	// Output stream for global and static variable initialization assembly.
	std::stringstream staticInit;
	// Output stream for general assembly output.
	std::stringstream primary;
	for (SyntaxTreeNode* node : tu)
	{
		if (dynamic_cast<VariableDef*>(node) != nullptr)
		node->Generate(dat, staticInit);
		else node->Generate(dat, primary);
	}

	for (std::pair<const Type*, IDT> global : dat._globalVars)
	{
		// TODO: Output assembly allocation directives for global variables.
	}

	// TODO: Wrap the next line in the generation for the program entry routine.
	std::cout << staticInit.rdbuf();
	std::cout << primary.rdbuf();
}


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr
			<< "Missing source path. Correct usage: "sv
			<< argv[0] << " <source file path>\n"sv;
		return EXIT_FAILURE;
	}

	std::vector<SyntaxTreeNode*> tu;
	const int exit_code {readNodes(argv[argc - 1], tu)};
	for (SyntaxTreeNode* node : tu) node->Print(std::cout, "  "sv);
	return exit_code;
}
