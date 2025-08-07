// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parserbase.h"


// Unqualified %code blocks.
#line 17 "parser.y"

  yy::ParserBase::symbol_type yylex();
  void setResult(std::unique_ptr<Action>&&);

#line 51 "parserbase.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 124 "parserbase.cc"

  /// Build a parser object.
  ParserBase::ParserBase ()
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr)
#else

#endif
  {}

  ParserBase::~ParserBase ()
  {}

  ParserBase::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  ParserBase::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  ParserBase::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  ParserBase::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  ParserBase::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  ParserBase::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  ParserBase::symbol_kind_type
  ParserBase::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  ParserBase::stack_symbol_type::stack_symbol_type ()
  {}

  ParserBase::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_direction: // direction
        value.YY_MOVE_OR_COPY< Direction > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_object: // object
      case symbol_kind::S_object_without_article: // object_without_article
        value.YY_MOVE_OR_COPY< ItemDescriptor > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_DIRECTION: // DIRECTION
      case symbol_kind::S_VERB: // VERB
      case symbol_kind::S_NOUN: // NOUN
      case symbol_kind::S_ADJECTIVE: // ADJECTIVE
      case symbol_kind::S_UNKNOWN: // UNKNOWN
      case symbol_kind::S_verb: // verb
      case symbol_kind::S_modifier: // modifier
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_command: // command
      case symbol_kind::S_move_command: // move_command
      case symbol_kind::S_take_command: // take_command
      case symbol_kind::S_drop_command: // drop_command
      case symbol_kind::S_inspect_command: // inspect_command
      case symbol_kind::S_interact_command: // interact_command
        value.YY_MOVE_OR_COPY< std::unique_ptr<Action> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  ParserBase::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_direction: // direction
        value.move< Direction > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_object: // object
      case symbol_kind::S_object_without_article: // object_without_article
        value.move< ItemDescriptor > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_DIRECTION: // DIRECTION
      case symbol_kind::S_VERB: // VERB
      case symbol_kind::S_NOUN: // NOUN
      case symbol_kind::S_ADJECTIVE: // ADJECTIVE
      case symbol_kind::S_UNKNOWN: // UNKNOWN
      case symbol_kind::S_verb: // verb
      case symbol_kind::S_modifier: // modifier
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_command: // command
      case symbol_kind::S_move_command: // move_command
      case symbol_kind::S_take_command: // take_command
      case symbol_kind::S_drop_command: // drop_command
      case symbol_kind::S_inspect_command: // inspect_command
      case symbol_kind::S_interact_command: // interact_command
        value.move< std::unique_ptr<Action> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  ParserBase::stack_symbol_type&
  ParserBase::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_direction: // direction
        value.copy< Direction > (that.value);
        break;

      case symbol_kind::S_object: // object
      case symbol_kind::S_object_without_article: // object_without_article
        value.copy< ItemDescriptor > (that.value);
        break;

      case symbol_kind::S_DIRECTION: // DIRECTION
      case symbol_kind::S_VERB: // VERB
      case symbol_kind::S_NOUN: // NOUN
      case symbol_kind::S_ADJECTIVE: // ADJECTIVE
      case symbol_kind::S_UNKNOWN: // UNKNOWN
      case symbol_kind::S_verb: // verb
      case symbol_kind::S_modifier: // modifier
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_command: // command
      case symbol_kind::S_move_command: // move_command
      case symbol_kind::S_take_command: // take_command
      case symbol_kind::S_drop_command: // drop_command
      case symbol_kind::S_inspect_command: // inspect_command
      case symbol_kind::S_interact_command: // interact_command
        value.copy< std::unique_ptr<Action> > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  ParserBase::stack_symbol_type&
  ParserBase::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_direction: // direction
        value.move< Direction > (that.value);
        break;

      case symbol_kind::S_object: // object
      case symbol_kind::S_object_without_article: // object_without_article
        value.move< ItemDescriptor > (that.value);
        break;

      case symbol_kind::S_DIRECTION: // DIRECTION
      case symbol_kind::S_VERB: // VERB
      case symbol_kind::S_NOUN: // NOUN
      case symbol_kind::S_ADJECTIVE: // ADJECTIVE
      case symbol_kind::S_UNKNOWN: // UNKNOWN
      case symbol_kind::S_verb: // verb
      case symbol_kind::S_modifier: // modifier
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_command: // command
      case symbol_kind::S_move_command: // move_command
      case symbol_kind::S_take_command: // take_command
      case symbol_kind::S_drop_command: // drop_command
      case symbol_kind::S_inspect_command: // inspect_command
      case symbol_kind::S_interact_command: // interact_command
        value.move< std::unique_ptr<Action> > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  ParserBase::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  ParserBase::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  ParserBase::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  ParserBase::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  ParserBase::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  ParserBase::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  ParserBase::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  ParserBase::debug_level_type
  ParserBase::debug_level () const
  {
    return yydebug_;
  }

  void
  ParserBase::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  ParserBase::state_type
  ParserBase::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  ParserBase::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  ParserBase::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  ParserBase::operator() ()
  {
    return parse ();
  }

  int
  ParserBase::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_direction: // direction
        yylhs.value.emplace< Direction > ();
        break;

      case symbol_kind::S_object: // object
      case symbol_kind::S_object_without_article: // object_without_article
        yylhs.value.emplace< ItemDescriptor > ();
        break;

      case symbol_kind::S_DIRECTION: // DIRECTION
      case symbol_kind::S_VERB: // VERB
      case symbol_kind::S_NOUN: // NOUN
      case symbol_kind::S_ADJECTIVE: // ADJECTIVE
      case symbol_kind::S_UNKNOWN: // UNKNOWN
      case symbol_kind::S_verb: // verb
      case symbol_kind::S_modifier: // modifier
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_command: // command
      case symbol_kind::S_move_command: // move_command
      case symbol_kind::S_take_command: // take_command
      case symbol_kind::S_drop_command: // drop_command
      case symbol_kind::S_inspect_command: // inspect_command
      case symbol_kind::S_interact_command: // interact_command
        yylhs.value.emplace< std::unique_ptr<Action> > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // input: END
#line 46 "parser.y"
                 { setResult(std::make_unique<Nop>()); YYACCEPT; }
#line 645 "parserbase.cc"
    break;

  case 3: // input: command END
#line 47 "parser.y"
                 { setResult(std::move(yystack_[1].value.as < std::unique_ptr<Action> > ())); YYACCEPT; }
#line 651 "parserbase.cc"
    break;

  case 4: // command: move_command
#line 50 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::move(yystack_[0].value.as < std::unique_ptr<Action> > ()); }
#line 657 "parserbase.cc"
    break;

  case 5: // command: take_command
#line 51 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::move(yystack_[0].value.as < std::unique_ptr<Action> > ()); }
#line 663 "parserbase.cc"
    break;

  case 6: // command: drop_command
#line 52 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::move(yystack_[0].value.as < std::unique_ptr<Action> > ()); }
#line 669 "parserbase.cc"
    break;

  case 7: // command: inspect_command
#line 53 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::move(yystack_[0].value.as < std::unique_ptr<Action> > ()); }
#line 675 "parserbase.cc"
    break;

  case 8: // command: inventory_command
#line 54 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<ShowInventory>(); }
#line 681 "parserbase.cc"
    break;

  case 9: // command: interact_command
#line 55 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::move(yystack_[0].value.as < std::unique_ptr<Action> > ()); }
#line 687 "parserbase.cc"
    break;

  case 10: // command: SAVE
#line 56 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Save>(); }
#line 693 "parserbase.cc"
    break;

  case 11: // command: LOAD
#line 57 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Load>(); }
#line 699 "parserbase.cc"
    break;

  case 12: // move_command: direction
#line 61 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Move>(yystack_[0].value.as < Direction > ()); }
#line 705 "parserbase.cc"
    break;

  case 13: // move_command: MOVE direction
#line 62 "parser.y"
                        { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Move>(yystack_[0].value.as < Direction > ()); }
#line 711 "parserbase.cc"
    break;

  case 14: // take_command: TAKE object
#line 66 "parser.y"
                            { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Take>(yystack_[0].value.as < ItemDescriptor > ()); }
#line 717 "parserbase.cc"
    break;

  case 15: // take_command: TAKE object FROM object
#line 67 "parser.y"
                            { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Take>(yystack_[2].value.as < ItemDescriptor > (), yystack_[0].value.as < ItemDescriptor > ()); }
#line 723 "parserbase.cc"
    break;

  case 16: // drop_command: DROP object
#line 71 "parser.y"
                { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Drop>(yystack_[0].value.as < ItemDescriptor > ()); }
#line 729 "parserbase.cc"
    break;

  case 19: // inspect_command: INSPECT object
#line 86 "parser.y"
                   { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Inspect>(yystack_[0].value.as < ItemDescriptor > ()); }
#line 735 "parserbase.cc"
    break;

  case 20: // interact_command: verb
#line 90 "parser.y"
                              { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Interact>(yystack_[0].value.as < std::string > ()); }
#line 741 "parserbase.cc"
    break;

  case 21: // interact_command: verb object
#line 91 "parser.y"
                              { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Interact>(yystack_[1].value.as < std::string > (), yystack_[0].value.as < ItemDescriptor > ()); }
#line 747 "parserbase.cc"
    break;

  case 22: // interact_command: USE object TO verb
#line 92 "parser.y"
                              { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Interact>(yystack_[0].value.as < std::string > (), yystack_[2].value.as < ItemDescriptor > ()); }
#line 753 "parserbase.cc"
    break;

  case 23: // interact_command: verb object WITH object
#line 93 "parser.y"
                              { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Interact>(yystack_[3].value.as < std::string > (), yystack_[2].value.as < ItemDescriptor > (), yystack_[0].value.as < ItemDescriptor > ()); }
#line 759 "parserbase.cc"
    break;

  case 24: // interact_command: USE object TO verb object
#line 94 "parser.y"
                              { yylhs.value.as < std::unique_ptr<Action> > () = std::make_unique<Interact>(yystack_[1].value.as < std::string > (), yystack_[0].value.as < ItemDescriptor > (), yystack_[3].value.as < ItemDescriptor > ()); }
#line 765 "parserbase.cc"
    break;

  case 25: // direction: DIRECTION
#line 98 "parser.y"
            { yylhs.value.as < Direction > () = directionFromString(yystack_[0].value.as < std::string > ()); }
#line 771 "parserbase.cc"
    break;

  case 26: // verb: VERB
#line 102 "parser.y"
              { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 777 "parserbase.cc"
    break;

  case 27: // verb: UNKNOWN
#line 103 "parser.y"
              { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 783 "parserbase.cc"
    break;

  case 28: // object: object_without_article
#line 107 "parser.y"
                                   { yylhs.value.as < ItemDescriptor > () = yystack_[0].value.as < ItemDescriptor > (); }
#line 789 "parserbase.cc"
    break;

  case 29: // object: ARTICLE object_without_article
#line 108 "parser.y"
                                   { yylhs.value.as < ItemDescriptor > () = yystack_[0].value.as < ItemDescriptor > (); }
#line 795 "parserbase.cc"
    break;

  case 30: // object_without_article: NOUN
#line 112 "parser.y"
                        { yylhs.value.as < ItemDescriptor > ().noun = yystack_[0].value.as < std::string > (); }
#line 801 "parserbase.cc"
    break;

  case 31: // object_without_article: UNKNOWN
#line 113 "parser.y"
                        { yylhs.value.as < ItemDescriptor > ().noun = yystack_[0].value.as < std::string > (); }
#line 807 "parserbase.cc"
    break;

  case 32: // object_without_article: modifier object
#line 114 "parser.y"
                    {
      yylhs.value.as < ItemDescriptor > () = yystack_[0].value.as < ItemDescriptor > ();
      yylhs.value.as < ItemDescriptor > ().adjectives.push_back(yystack_[1].value.as < std::string > ());
    }
#line 816 "parserbase.cc"
    break;

  case 33: // modifier: ADJECTIVE
#line 121 "parser.y"
               { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 822 "parserbase.cc"
    break;

  case 34: // modifier: UNKNOWN
#line 122 "parser.y"
               { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 828 "parserbase.cc"
    break;


#line 832 "parserbase.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  ParserBase::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  ParserBase::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char ParserBase::yypact_ninf_ = -15;

  const signed char ParserBase::yytable_ninf_ = -35;

  const signed char
  ParserBase::yypact_[] =
  {
       2,     0,    12,    12,   -15,     7,    12,   -15,   -15,   -15,
     -15,   -15,   -15,    15,    -2,   -15,   -15,   -15,   -15,   -15,
     -15,   -15,    12,   -15,   -15,   -15,    16,    26,    10,   -15,
      12,   -15,   -15,   -15,    25,   -15,   -15,    27,   -15,    12,
     -15,   -14,    12,   -15,    12,   -15,   -15
  };

  const signed char
  ParserBase::yydefact_[] =
  {
       0,     0,     0,     0,    17,     0,     0,    10,    11,    25,
      26,    27,     2,     0,     0,     4,     5,     6,     8,     7,
       9,    12,    20,    13,    30,    33,    31,     0,    14,    28,
       0,    16,    18,    19,     0,     1,     3,    21,    29,     0,
      32,     0,     0,    15,    22,    23,    24
  };

  const signed char
  ParserBase::yypgoto_[] =
  {
     -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,   -15,    37,
       4,    -3,    19,   -15
  };

  const signed char
  ParserBase::yydefgoto_[] =
  {
       0,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    28,    29,    30
  };

  const signed char
  ParserBase::yytable_[] =
  {
      31,    10,    33,    34,    11,     1,     2,     3,     4,     5,
       6,     7,     8,    32,     9,    35,     9,    10,    36,    37,
      11,    39,    12,    24,    25,    26,    27,    40,    24,    25,
      26,    27,   -34,   -34,   -34,   -34,    43,    41,    23,    45,
      42,    46,    24,    25,    26,    44,    38
  };

  const signed char
  ParserBase::yycheck_[] =
  {
       3,    15,     5,     6,    18,     3,     4,     5,     6,     7,
       8,     9,    10,     6,    14,     0,    14,    15,    20,    22,
      18,    11,    20,    16,    17,    18,    19,    30,    16,    17,
      18,    19,    16,    17,    18,    19,    39,    12,     1,    42,
      13,    44,    16,    17,    18,    41,    27
  };

  const signed char
  ParserBase::yystos_[] =
  {
       0,     3,     4,     5,     6,     7,     8,     9,    10,    14,
      15,    18,    20,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    30,    16,    17,    18,    19,    32,    33,
      34,    32,     6,    32,    32,     0,    20,    32,    33,    11,
      32,    12,    13,    32,    31,    32,    32
  };

  const signed char
  ParserBase::yyr1_[] =
  {
       0,    21,    22,    22,    23,    23,    23,    23,    23,    23,
      23,    23,    24,    24,    25,    25,    26,    27,    27,    28,
      29,    29,    29,    29,    29,    30,    31,    31,    32,    32,
      33,    33,    33,    34,    34
  };

  const signed char
  ParserBase::yyr2_[] =
  {
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     2,     1,     2,     2,
       1,     2,     4,     4,     5,     1,     1,     1,     1,     2,
       1,     1,     2,     1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const ParserBase::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "MOVE", "TAKE", "DROP",
  "INV", "INSPECT", "USE", "SAVE", "LOAD", "FROM", "TO", "WITH",
  "DIRECTION", "VERB", "NOUN", "ADJECTIVE", "UNKNOWN", "ARTICLE", "END",
  "$accept", "input", "command", "move_command", "take_command",
  "drop_command", "inventory_command", "inspect_command",
  "interact_command", "direction", "verb", "object",
  "object_without_article", "modifier", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const signed char
  ParserBase::yyrline_[] =
  {
       0,    46,    46,    47,    50,    51,    52,    53,    54,    55,
      56,    57,    61,    62,    66,    67,    71,    81,    82,    86,
      90,    91,    92,    93,    94,    98,   102,   103,   107,   108,
     112,   113,   114,   121,   122
  };

  void
  ParserBase::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  ParserBase::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1164 "parserbase.cc"

