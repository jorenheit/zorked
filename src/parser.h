#ifndef PARSER_H
#define PARSER_H

#include "parserbase.h"
#include "dictionary.h"
#include "action.h"

using yy::ParserBase;

class Parser: public ParserBase {
  Dictionary const &_dict;
  std::vector<Dictionary::Entry> _tokens;
  size_t _pos;
  std::unique_ptr<Action> _result;

public:
  Parser(Dictionary const &dict);
  std::unique_ptr<Action> parseAction(std::string const &input, size_t ngramSize = 4);

private:
  friend void setResult(std::unique_ptr<Action> &&action);
  void setResult(std::unique_ptr<Action> &&action);

  friend ParserBase::symbol_type yylex();
  ParserBase::symbol_type yylex();
};
#endif // PARSER_H
