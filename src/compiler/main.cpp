#include "parse.h"
#include "syntaxTree/base.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std::string_view_literals;


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr
			<< "Missing source path. Correct usage: "sv
			<< argv[0] << " <source file path>\n"sv;
		return EXIT_FAILURE;
	}

	std::ifstream source_file;
	source_file.open(argv[argc - 1], std::ios_base::in);
	if (!source_file)
	{
		std::cerr
			<< "Unable to open source file: "sv
			<< argv[argc - 1] << '\n';
		return EXIT_FAILURE;
	}

	source_file.seekg(0, std::ios_base::end);
	const int tu_size = source_file.tellg();
	source_file.seekg(0);
	char* tu_buffer = reinterpret_cast<char*>(malloc(tu_size));
	source_file.read(tu_buffer, tu_size);
	source_file.close();

	struct carb_stack carb_stack;
	carb_stack_init(&carb_stack);
	carb_set_input(&carb_stack, tu_buffer, tu_size, 1);
	
	SyntaxNode* out_node {nullptr};
	carb_scan(&carb_stack, &out_node);
	if (out_node != nullptr)
	{
		out_node->Print(std::cout, "   "sv);
		delete out_node;
	}
	else std::cerr << "It didn't work...\n"sv;

	carb_stack_cleanup(&carb_stack);
	delete tu_buffer;
	return EXIT_SUCCESS;
}