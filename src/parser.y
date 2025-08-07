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

%token MOVE TAKE DROP INV INSPECT USE SAVE LOAD 
%token FROM TO WITH
%token <std::string> DIRECTION
%token <std::string> UNKNOWN
%token ARTICLE
%token END

%type <ObjectDescriptor>        object
%type <std::string>             verb
%type <ObjectDescriptor>        object_without_article
%type <std::string>             adjective
%type <Direction>               direction
%type <std::unique_ptr<Action>> command
%type <std::unique_ptr<Action>> move_command
%type <std::unique_ptr<Action>> take_command
%type <std::unique_ptr<Action>> drop_command
%type <std::unique_ptr<Action>> inspect_command
%type <std::unique_ptr<Action>> interact_command

%start input

%%

input:
    END          { setResult(std::make_unique<Nop>()); YYACCEPT; }
  | command END  { setResult(std::move($1)); YYACCEPT; }

command:
    move_command	{ $$ = std::move($1); }
  | take_command	{ $$ = std::move($1); }
  | drop_command	{ $$ = std::move($1); }
  | inspect_command     { $$ = std::move($1); }
  | inventory_command	{ $$ = std::make_unique<ShowInventory>(); }
  | interact_command    { $$ = std::move($1); }
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

/*
put_command:
    PUT object IN object
;
*/

inventory_command:
    INV
  | INSPECT INV
;

inspect_command:
    INSPECT object { $$ = std::make_unique<Inspect>($2); }
;

interact_command:
    verb                      { $$ = std::make_unique<Interact>($1); }
  | verb object               { $$ = std::make_unique<Interact>($1, $2); }
  | USE object TO verb        { $$ = std::make_unique<Interact>($4, $2); }
  | verb object WITH object   { $$ = std::make_unique<Interact>($1, $2, $4); }
  | USE object TO verb object { $$ = std::make_unique<Interact>($4, $5, $2); }
;

direction:
  DIRECTION { $$ = directionFromString($1); }
;

verb:
    UNKNOWN   { $$ = $1; }
;

object:
    object_without_article         { $$ = $1; }
  | ARTICLE object_without_article { $$ = $2; }
;

object_without_article:
    UNKNOWN { $$.noun = $1; }
  | adjective object_without_article {
      $$ = $2;
      $$.adjectives.push_back($1);
    }
;

adjective:
    UNKNOWN    { $$ = $1; }
;
