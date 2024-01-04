#include "parse.h"
#include "syntaxTree/base.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
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
	carb_stack* stack, std::vector<SyntaxNode*>& node_list)
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


int readNodes(char* src_path, std::vector<SyntaxNode*>& destTU)
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


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr
			<< "Missing source path. Correct usage: "sv
			<< argv[0] << " <source file path>\n"sv;
		return EXIT_FAILURE;
	}

	std::vector<SyntaxNode*> tu;
	const int exit_code {readNodes(argv[argc - 1], tu)};
	for (SyntaxNode* node : tu) node->Print(std::cout, "  "sv);
	return exit_code;
}