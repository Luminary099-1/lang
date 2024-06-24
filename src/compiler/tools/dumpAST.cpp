#include "../parser/parser.h"
#include "../parser/parserTools.hpp"
#include "../syntaxTree/base.hpp"
#include "../utilities.hpp"

#include <iostream>
#include <string_view>


using namespace std::string_view_literals;


/**
 * Outputs the AST of a source file to the standard output.
 * @param argc Number of command line arguments (expects 2).
 * @param argv Command line arguments. The second expected argument is the
 * source file to analyze.
 * @return Program exit status code.
 */
int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr
			<< "Missing source path. Correct usage: "sv
			<< argv[0] << " <source file path>\n"sv;
		return EXIT_FAILURE;
	}

	std::unique_ptr<TU> tu;
	try { tu = std::make_unique<TU>(argv[1]); }
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	carb_stack stack;
	carb_stack_init(&stack);
	carb_set_input(&stack, tu->_buf, tu->_size, tu->_end ? 1 : 0);
	int status {EXIT_SUCCESS};
	AST out;
	
	try
	{
		Parser::getAST(stack, *tu, out);
		for (size_t i {0}; i < out.size(); ++ i)
			out[i]->Print(std::cout, "	"sv);
	}
	catch (std::runtime_error& e) { std::cerr << e.what() << '\n'; }

	carb_stack_cleanup(&stack);
	return status;
}
