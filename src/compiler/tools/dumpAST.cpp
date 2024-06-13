#include "../parse.h"
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
	carb_set_input(&stack, tu->_buf, tu->_size, 1);

	int status {EXIT_SUCCESS};
	std::vector<SyntaxTreeNode*> out;
	while (true)
	{
		switch (carb_scan(&stack, out))
		{
			case _CARB_FINISH:
				for (SyntaxTreeNode* node : out)
					node->Print(std::cout, "	"sv);
				break;
			case _CARB_FEED_ME:
				tu->ReadNext();
				carb_set_input(&stack, tu->_buf, tu->_size, 1);
				continue;
			case _CARB_END_OF_INPUT:
				break;
			case _CARB_LEXICAL_ERROR:
				std::cerr << "Unexpected token: "sv << carb_text(&stack) << '\n';
				continue;
			case _CARB_SYNTAX_ERROR:
				std::cerr << "Syntax error on ("sv << carb_line(&stack)
					<< ", "sv << carb_column(&stack) << "): "sv
					<< carb_text(&stack) << '\n';
				continue;
			default:
				std::cerr << "Encountered an unrecoverable error, aborting.\n"sv;
				status = EXIT_FAILURE;
		}
		break;
	}

	carb_stack_cleanup(&stack);
	return status;
}
