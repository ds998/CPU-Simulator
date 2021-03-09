#ifndef __LEXERS_H__
#define __LEXERS_H__

#include <regex>
#include <string.h>
#include "Enums.h"
using namespace std;

string symbol="([a-zA-Z_][a-zA-Z0-9_]*)";
string dec_literal="([0-9]+)";
string hex_literal="(0x[0-9abcdef]{1,4})";
string register_op="r([0-7f])";
/*
unordered_map<int, regex> TokenParsers = {
  {ACCESSOR,regex("^(\\.global|\\.extern)$")},
  {SECTION,regex("^\\.(section) ([a-zA-Z_][a-zA-Z0-9]*)?$")},
  {DIRECTIVE,regex("^\\.(word|byte|skip|equ)$")},
  {LABEL,regex("^" + symbol + ":$")},
  {OPCODE,regex("^(halt|iret|ret|int|call|jmp|jeq|jne|jgt|push|pop|xchg|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr)$")},
  {REG_IMM_DEC_OPERAND,regex("^*" + dec_literal + "$")},
  {REG_IMM_HEX_OPERAND,regex("^*" + hex_literal + "$")},
  {REG_SYM_OPERAND,regex("^*" + symbol + "$")},
  {REG_REG_OPERAND,regex("^%" + register_op + "$")},
  {REG_REG_IND_OPERAND,regex("^\\(%" + register_op + "\\)$")},
  {REG_REGINDIMM_DEC_OPERAND,regex("^" + dec_literal + "\\(%" + register_op + "\\)$")},
  {REG_REGINDIMM_HEX_OPERAND,regex("^" + hex_literal + "\\(%" + register_op + "\\)$")},
  {REG_REGINDSYM_OPERAND,regex("^" + symbol + "\\(%((" + register_op + ")|pc)\\)$")},
  {REG_MEMDIR_DEC_OPERAND,regex("^" + dec_literal + "$")},
  {REG_MEMDIR_HEX_OPERAND,regex("^" + hex_literal + "$")},
  {REG_MEMDIR_SYM_OPERAND,regex("^" + symbol + "$")},
  {JMP_IMM_DEC_OPERAND,regex("^" + dec_literal + "$")},
  {JMP_IMM_HEX_OPERAND,regex("^" + hex_literal + "$")},
  {JMP_SYM_OPERAND,regex("^" + symbol + "$")},
  {JMP_REG_OPERAND,regex("^*%" + register_op + "$")},
  {JMP_REG_IND_OPERAND,regex("^*\\(%" + register_op + "\\)$")},
  {JMP_REGINDIMM_DEC_OPERAND,regex("^*" + dec_literal + "\\(%" + register_op + "\\)$")},
  {JMP_REGINDIMM_HEX_OPERAND,regex("^*" + hex_literal + "\\(%" + register_op + "\\)$")},
  {JMP_REGINDSYM_OPERAND,regex("^*" + symbol + "\\(%((" + register_op + ")|pc)\\)$")},
  {JMP_MEMDIR_DEC_OPERAND,regex("^*" + dec_literal + "$")},
  {JMP_MEMDIR_HEX_OPERAND,regex("^*" + hex_literal + "$")},
  {JMP_MEMDIR_SYM_OPERAND,regex("^*" + symbol + "$")}

};
*/





#endif
