#include "parser.h"
#include "util.h"

namespace Impl {
  Parser *parser = nullptr;
}

ParserBase::symbol_type yylex() {
  return Impl::parser->yylex();
}

void setResult(std::unique_ptr<Action> &&result) {
  Impl::parser->setResult(std::move(result));
}

void ParserBase::error(std::string const &) {}

Parser::Parser(Dictionary const &dict):
  _dict(dict)
{
  assert(Impl::parser == nullptr && "Multiple instances of Parser");
  Impl::parser = this;
}

void Parser::setResult(std::unique_ptr<Action> &&result) {
  _result = std::move(result);
}

std::unique_ptr<Action> Parser::parseAction(std::string const &input, size_t ngramSize) {
  _tokens = _dict.tokenize(input, ngramSize);
  _pos = 0;

  if (ParserBase::parse() != 0) {
    std::cout << "I don't know what that means!\n"; // TODO: factor out narration
    return nullptr;
  }
  return std::move(_result);
}

ParserBase::symbol_type Parser::yylex() {
  if (_pos >= _tokens.size()) {
    return Parser::make_END();
  }
  
  Dictionary::Entry const &token = _tokens[_pos++];
  switch (token.type) {
  case WordType::Unknown: return ParserBase::make_UNKNOWN(token.str);
  case WordType::Direction: return ParserBase::make_DIRECTION(token.str);
  case WordType::Number: assert(false && "todo: numbers"); 
  case WordType::Verb: return ParserBase::make_VERB(token.str);
  case WordType::Noun: return ParserBase::make_NOUN(token.str);
  case WordType::Preposition: {
    if (token.str == "from") return ParserBase::make_FROM();
    UNREACHABLE("unimplemented preposition");
  }
  case WordType::Adjective: return ParserBase::make_ADJECTIVE(token.str);
  case WordType::Ignored: UNREACHABLE("ignored words survived");
  case WordType::BuiltinCommand: {
    if (token.str == "move") return ParserBase::make_MOVE();
    if (token.str == "take") return ParserBase::make_TAKE();
    if (token.str == "drop") return ParserBase::make_DROP();
    UNREACHABLE("unimplemented command");
  }
  default: UNREACHABLE("default");
  }

  UNREACHABLE();
}

