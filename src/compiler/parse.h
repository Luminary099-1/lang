#ifndef CARB_CARB_CLANGSRCCOMPILERPARSE_H_INCLUDED
#define CARB_CARB_CLANGSRCCOMPILERPARSE_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
  #endif
  
  #define _CARB_FINISH 0
  #define _CARB_MATCH 1
  #define _CARB_OVERFLOW 2
  #define _CARB_NO_MEMORY 3
  #define _CARB_FEED_ME 4
  #define _CARB_END_OF_INPUT 5
  #define _CARB_SYNTAX_ERROR 6
  #define _CARB_LEXICAL_ERROR 7
  #define _CARB_INTERNAL_ERROR 8
  
  #define CARB_ASSIGN 3
  #define CARB_SEMICOL 4
  #define CARB_COMMA 5
  #define CARB_PLUS 6
  #define CARB_MINUS 7
  #define CARB_ASTERISK 8
  #define CARB_SLASH 9
  #define CARB_PERCENT 10
  #define CARB_INC 11
  #define CARB_DEC 12
  #define CARB_AND 13
  #define CARB_OR 14
  #define CARB_XOR 15
  #define CARB_COMP 16
  #define CARB_SHR 17
  #define CARB_SHL 18
  #define CARB_LAND 19
  #define CARB_LOR 20
  #define CARB_DENY 21
  #define CARB_EQUAL 22
  #define CARB_NEQUAL 23
  #define CARB_GREATER 24
  #define CARB_LESSER 25
  #define CARB_GREATEQ 26
  #define CARB_LESSEQ 27
  #define CARB_LPAREN 28
  #define CARB_RPAREN 29
  #define CARB_LCURLY 30
  #define CARB_RCURLY 31
  #define CARB_DECIMAL 32
  #define CARB_ID 33
  #define CARB_STRING 34
  #define CARB_IF 35
  #define CARB_ELSE 36
  #define CARB_MATCH 37
  #define CARB_FOR 38
  #define CARB_LOOP 39
  #define CARB_WHILE 40
  #define CARB_TRUE 41
  #define CARB_FALSE 42
  #define CARB_BREAK 43
  #define CARB_ERROR 44
  #define CARB_INPUT_END 45
  
  #define CARB_GLOBALS 46
  #define CARB_GLOBAL 47
  #define CARB_TYPE_EXPR 48
  #define CARB_FUNCTION_DEF 49
  #define CARB_PARAM_LIST 50
  #define CARB_PARAM_LIST_TAIL 51
  #define CARB_PARAM 52
  #define CARB_STATEMENT 53
  #define CARB_COMPOUND_STMT 54
  #define CARB_STATEMENTS 55
  #define CARB_VAR_DEF_STMT 56
  #define CARB_EXPRESSION 57
  #define CARB_EXPRESSION_STMT 58
  #define CARB_ASSIGNMENT_EXPR 59
  #define CARB_IF_EXPR 60
  #define CARB_FOR_EXPR 61
  #define CARB_LOOP_EXPR 62
  #define CARB_WHILE_EXPR 63
  #define CARB_BREAK_STMT 64
  #define CARB_EXPR_MAYBE 65
  #define CARB_PRIMARY_EXPR 66
  #define CARB_ARG_LIST 67
  #define CARB_ARG_LIST_TAIL 68
  #define CARB_PRE_EXPR 69
  #define CARB_MULTIPLICATIVE_EXPR 70
  #define CARB_ADDITIVE_EXPR 71
  #define CARB_SHIFT_EXPR 72
  #define CARB_RELATIVE_EXPR 73
  #define CARB_ABSOLUTE_EXPR 74
  #define CARB_BIT_AND_EXPR 75
  #define CARB_BIT_XOR_EXPR 76
  #define CARB_BIT_OR_EXPR 77
  #define CARB_LOGIC_AND_EXPR 78
  #define CARB_LOGIC_OR_EXPR 79
  #define CARB_LITERAL 80
  #define CARB_BOOLEAN 81
  
  #define M_CARB_DEFAULT 1
  
  struct carb_stack {
    int error_recovery_:1;
    int pending_reset_:1;
    int discard_remaining_actions_:1;
    int slot_1_has_sym_data_:1;
    int slot_1_has_common_data_:1;
    int top_of_stack_has_sym_data_:1;
    int top_of_stack_has_common_data_:1;
    int need_sym_:1;
    int is_final_input_:1;
    int slot_0_has_current_sym_data_:1;
    int slot_0_has_common_data_:1;
    int current_sym_;
    size_t input_size_;
    const char *input_;
    int newbuf_pos_has_common_data_:1;
    int newbuf_pos_has_sym_data_:1;
    int stack_newbuf_pos_has_common_data_:1;
    int stack_newbuf_pos_has_sym_data_:1;
    int slot_1_sym_;
    int continue_at_;
    int mute_error_turns_;
    size_t pos_, num_stack_allocated_;
    struct carb_sym_data *stack_;
    struct carb_sym_data *sym_data_;
    struct carb_sym_data *new_buf_;
    size_t new_buf_num_allocated_;
    size_t new_buf_sym_partial_pos_;
    size_t current_production_length_;
    int current_production_nonterminal_;
    size_t sym_idx_;
    size_t scan_state_;
    size_t current_mode_start_state_;
    size_t match_index_;
    size_t best_match_action_;
    size_t best_match_size_;
    size_t input_index_;
    size_t input_offset_;
    size_t match_buffer_size_;
    size_t match_buffer_size_allocated_;
    /* offset, line and column at the start of match_buffer_ */
    size_t match_offset_;
    int match_line_;
    int match_col_;
    /* offset, line and column at the zero-termination (best_match_size_) of a match 
    * in match_buffer_ (the actual buffer may well be longer.) */
    size_t best_match_offset_;
    int best_match_line_;
    int best_match_col_;
    size_t token_size_;
    char *match_buffer_;
    char terminator_repair_;
    int input_line_;
    int input_col_;
    int sym_grp_;
    char codepoint_[4];
    char *cp_;
  };
  
  void carb_stack_init(struct carb_stack *stack);
  void carb_stack_cleanup(struct carb_stack *stack);
  int carb_stack_reset(struct carb_stack *stack);
  int carb_stack_can_recover(struct carb_stack *stack);
  int carb_stack_accepts(struct carb_stack *stack, int sym);
  void carb_set_mode(struct carb_stack *stack, int mode);
  int carb_mode(struct carb_stack *stack);
  void carb_set_input(struct carb_stack *stack, const char *input, size_t input_size, int is_final_input);
  int carb_scan(struct carb_stack *stack);
  void carb_set_location(struct carb_stack *stack, int line, int col, size_t offset);
  const char *carb_text(struct carb_stack *stack);
  size_t carb_len(struct carb_stack *stack);
  int carb_line(struct carb_stack *stack);
  int carb_column(struct carb_stack *stack);
  size_t carb_offset(struct carb_stack *stack);
  int carb_endline(struct carb_stack *stack);
  int carb_endcolumn(struct carb_stack *stack);
  size_t carb_endoffset(struct carb_stack *stack);
  void *carb_token_common_data(struct carb_stack *stack);
  int carb_lex(struct carb_stack *stack);
  int carb_parse(struct carb_stack *stack, int sym);
  
  #ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARB_CARB_CLANGSRCCOMPILERPARSE_H_INCLUDED */
