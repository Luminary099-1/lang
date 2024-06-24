#include "parserTools.hpp"


bool Parser::getAST(carb_stack& stack, TU& tu, AST& ast)
{
	bool success {true};
	while (true)
	{
		switch (carb_scan(&stack, ast))
		{
			case _CARB_FINISH:
			case _CARB_END_OF_INPUT:
				break;
			case _CARB_FEED_ME:
				tu.ReadNext();
				carb_set_input(&stack, tu.GetBuf(), tu.GetSize(), tu.IsFinal());
				continue;
			case _CARB_LEXICAL_ERROR:
				std::cerr << "Unexpected token: "sv
					<< carb_text(&stack) << '\n';
				success = false;
				continue;
			case _CARB_SYNTAX_ERROR:
				std::cerr << "Syntax error on ("sv << carb_line(&stack)
					<< ", "sv << carb_column(&stack) << "): "sv
					<< carb_text(&stack) << '\n';
				success = false;
				continue;
			case _CARB_NO_MEMORY:
				throw std::runtime_error(OOM);
				break;
			case _CARB_INTERNAL_ERROR:
				throw std::runtime_error(FIE);
				break;
		}
		break;
	}

	return success;
}
