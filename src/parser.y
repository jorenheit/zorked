// File: parser.y

%language "c++"
%skeleton "lalr1.cc"
%define api.parser.class {ParserBase}
%define api.value.type variant
%define api.token.constructor

%code requires {
  #include <string>
  #include <memory>
  #include "action.h"
  #include "direction.h"
  #include "parserbase.h"
}

%code {
  yy::ParserBase::symbol_type yylex();
  void setResult(std::unique_ptr<Action>&&);
}

%token MOVE TAKE DROP INV INSPECT SAVE LOAD 
%token FROM
%token <std::string> DIRECTION
%token <std::string> VERB NOUN ADJECTIVE UNKNOWN
%token ARTICLE
%token END

%type <ItemDescriptor>          object
%type <ItemDescriptor>          object_without_article
%type <std::string>             modifier
%type <Direction>               direction
%type <std::unique_ptr<Action>> command
%type <std::unique_ptr<Action>> move_command
%type <std::unique_ptr<Action>> take_command
%type <std::unique_ptr<Action>> drop_command
%type <std::unique_ptr<Action>> inspect_command

%start input

%%

input:
    END          { setResult(nullptr); YYACCEPT; }
  | command END  { setResult(std::move($1)); YYACCEPT; }

command:
    move_command	{ $$ = std::move($1); }
  | take_command	{ $$ = std::move($1); }
  | drop_command	{ $$ = std::move($1); }
  | inspect_command     { $$ = std::move($1); }
  | inventory_command	{ $$ = std::make_unique<ShowInventory>(); }
  | SAVE                { $$ = std::make_unique<Save>(); }
  | LOAD                { $$ = std::make_unique<Load>(); }
;

move_command:
    direction		{ $$ = std::make_unique<Move>($1); }	
  | MOVE direction	{ $$ = std::make_unique<Move>($2); }
;

take_command:
    TAKE object		    { $$ = std::make_unique<Take>($2); }
  | TAKE object FROM object { $$ = std::make_unique<Take>($2, $4); }
;

drop_command:
    DROP object { $$ = std::make_unique<Drop>($2); }
;

inventory_command:
    INV
  | INSPECT INV
;

inspect_command:
    INSPECT object { $$ = std::make_unique<Inspect>($2); }
;

direction:
  DIRECTION { $$ = directionFromString($1); }
;

object:
    object_without_article { $$ = $1; }
  | ARTICLE object { $$ = $2; }
;

object_without_article:
    NOUN		{ $$.noun = $1; }
  | UNKNOWN		{ $$.noun = $1; }
  | modifier object {
      $$ = $2;
      $$.adjectives.push_back($1);
    }
;

modifier:
    ADJECTIVE  { $$ = $1; }
  | UNKNOWN    { $$ = $1; }
;