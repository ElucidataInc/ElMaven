/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   mgflex

/* First part of user declarations.  */

/* Line 283 of lalr1.cc  */
#line 40 "Parser.cpp"


#include "Parser.h"

/* User implementation prologue.  */
/* Line 289 of lalr1.cc  */
#line 128 "Parser.ypp"

#include "Driver.h"
#include "Scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

/* Line 289 of lalr1.cc  */
#line 59 "Parser.cpp"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace mgf {
/* Line 357 of lalr1.cc  */
#line 154 "Parser.cpp"

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  Parser::Parser (Driver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {
  }

  Parser::~Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    YYUSE (yytype);
  }


  void
  Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
    {
      case 8: /* "string" */
/* Line 452 of lalr1.cc  */
#line 125 "Parser.ypp"
        { delete ((*yyvaluep).stringVal); };
/* Line 452 of lalr1.cc  */
#line 257 "Parser.cpp"
        break;
      case 64: /* ion */
/* Line 452 of lalr1.cc  */
#line 126 "Parser.ypp"
        { delete ((*yyvaluep).doublepairnode); };
/* Line 452 of lalr1.cc  */
#line 264 "Parser.cpp"
        break;
      case 65: /* ions */
/* Line 452 of lalr1.cc  */
#line 126 "Parser.ypp"
        { delete ((*yyvaluep).doublepairlistnode); };
/* Line 452 of lalr1.cc  */
#line 271 "Parser.cpp"
        break;
      case 66: /* charge */
/* Line 452 of lalr1.cc  */
#line 126 "Parser.ypp"
        { delete ((*yyvaluep).integernode); };
/* Line 452 of lalr1.cc  */
#line 278 "Parser.cpp"
        break;
      case 67: /* charges */
/* Line 452 of lalr1.cc  */
#line 126 "Parser.ypp"
        { delete ((*yyvaluep).integerlistnode); };
/* Line 452 of lalr1.cc  */
#line 285 "Parser.cpp"
        break;
      case 68: /* csintegerlist */
/* Line 452 of lalr1.cc  */
#line 126 "Parser.ypp"
        { delete ((*yyvaluep).integerlistnode); };
/* Line 452 of lalr1.cc  */
#line 292 "Parser.cpp"
        break;

      default:
        break;
    }
  }

  void
  Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


/* User initialization code.  */
/* Line 543 of lalr1.cc  */
#line 38 "Parser.ypp"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &driver.streamname;
}
/* Line 543 of lalr1.cc  */
#line 393 "Parser.cpp"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_.clear ();
    yysemantic_stack_.clear ();
    yylocation_stack_.clear ();
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 2:
/* Line 664 of lalr1.cc  */
#line 141 "Parser.ypp"
    { (yyval.doublepairnode) = new DoublePairNode((yysemantic_stack_[(3) - (1)].doubleVal), (yysemantic_stack_[(3) - (2)].doubleVal)); }
    break;

  case 3:
/* Line 664 of lalr1.cc  */
#line 142 "Parser.ypp"
    { (yyval.doublepairnode) = new DoublePairNode((yysemantic_stack_[(3) - (1)].integerVal), (yysemantic_stack_[(3) - (2)].doubleVal)); }
    break;

  case 4:
/* Line 664 of lalr1.cc  */
#line 143 "Parser.ypp"
    { (yyval.doublepairnode) = new DoublePairNode((yysemantic_stack_[(3) - (1)].doubleVal), (yysemantic_stack_[(3) - (2)].integerVal)); }
    break;

  case 5:
/* Line 664 of lalr1.cc  */
#line 144 "Parser.ypp"
    { (yyval.doublepairnode) = new DoublePairNode((yysemantic_stack_[(3) - (1)].integerVal), (yysemantic_stack_[(3) - (2)].integerVal)); }
    break;

  case 6:
/* Line 664 of lalr1.cc  */
#line 146 "Parser.ypp"
    { (yyval.doublepairlistnode) = new DoublePairListNode((yysemantic_stack_[(2) - (1)].doublepairlistnode), (yysemantic_stack_[(2) - (2)].doublepairnode)); }
    break;

  case 7:
/* Line 664 of lalr1.cc  */
#line 147 "Parser.ypp"
    { (yyval.doublepairlistnode) = new DoublePairListNode((yysemantic_stack_[(1) - (1)].doublepairnode)); }
    break;

  case 8:
/* Line 664 of lalr1.cc  */
#line 149 "Parser.ypp"
    { (yyval.integernode) = new IntegerNode((yysemantic_stack_[(2) - (1)].integerVal)); }
    break;

  case 9:
/* Line 664 of lalr1.cc  */
#line 150 "Parser.ypp"
    { (yyval.integernode) = new IntegerNode(-(yysemantic_stack_[(2) - (1)].integerVal)); }
    break;

  case 10:
/* Line 664 of lalr1.cc  */
#line 151 "Parser.ypp"
    { (yyval.integernode) = new IntegerNode((yysemantic_stack_[(1) - (1)].integerVal)); }
    break;

  case 11:
/* Line 664 of lalr1.cc  */
#line 153 "Parser.ypp"
    { (yyval.integerlistnode) = (yysemantic_stack_[(3) - (2)].integerlistnode); }
    break;

  case 12:
/* Line 664 of lalr1.cc  */
#line 154 "Parser.ypp"
    { (yyval.integerlistnode) = new IntegerListNode((yysemantic_stack_[(3) - (1)].integerlistnode), (yysemantic_stack_[(3) - (3)].integernode)); }
    break;

  case 13:
/* Line 664 of lalr1.cc  */
#line 155 "Parser.ypp"
    { (yyval.integerlistnode) = new IntegerListNode((yysemantic_stack_[(3) - (1)].integerlistnode), (yysemantic_stack_[(3) - (3)].integernode)); }
    break;

  case 14:
/* Line 664 of lalr1.cc  */
#line 156 "Parser.ypp"
    { (yyval.integerlistnode) = new IntegerListNode((yysemantic_stack_[(1) - (1)].integernode)); }
    break;

  case 15:
/* Line 664 of lalr1.cc  */
#line 158 "Parser.ypp"
    { (yyval.integerlistnode) = new IntegerListNode((yysemantic_stack_[(3) - (1)].integerlistnode), (yysemantic_stack_[(3) - (3)].integerVal)); }
    break;

  case 16:
/* Line 664 of lalr1.cc  */
#line 159 "Parser.ypp"
    { (yyval.integerlistnode) = new IntegerListNode((yysemantic_stack_[(1) - (1)].integerVal)); }
    break;

  case 18:
/* Line 664 of lalr1.cc  */
#line 163 "Parser.ypp"
    {
            driver.context.mgfFile.push_back(driver.context.currentSpectrum);
            driver.context.currentSpectrum.clear();
        }
    break;

  case 19:
/* Line 664 of lalr1.cc  */
#line 169 "Parser.ypp"
    {
            driver.context.currentSpectrum.assign(((yysemantic_stack_[(6) - (4)].doublepairlistnode)->pairlist_)->begin(), ((yysemantic_stack_[(6) - (4)].doublepairlistnode)->pairlist_)->end());
            delete (yysemantic_stack_[(6) - (4)].doublepairlistnode); 
        }
    break;

  case 23:
/* Line 664 of lalr1.cc  */
#line 178 "Parser.ypp"
    { driver.context.header.setCLE(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 24:
/* Line 664 of lalr1.cc  */
#line 179 "Parser.ypp"
    { driver.context.header.setCOM(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 25:
/* Line 664 of lalr1.cc  */
#line 180 "Parser.ypp"
    { driver.context.header.setDB(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 26:
/* Line 664 of lalr1.cc  */
#line 181 "Parser.ypp"
    { driver.context.header.setFORMAT(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 27:
/* Line 664 of lalr1.cc  */
#line 182 "Parser.ypp"
    { driver.context.header.setINSTRUMENT(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 28:
/* Line 664 of lalr1.cc  */
#line 183 "Parser.ypp"
    { driver.context.header.setIT_MODS(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 29:
/* Line 664 of lalr1.cc  */
#line 184 "Parser.ypp"
    { driver.context.header.setITOLU(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 30:
/* Line 664 of lalr1.cc  */
#line 185 "Parser.ypp"
    { driver.context.header.setMASS(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 31:
/* Line 664 of lalr1.cc  */
#line 186 "Parser.ypp"
    { driver.context.header.setMODS(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 32:
/* Line 664 of lalr1.cc  */
#line 187 "Parser.ypp"
    { driver.context.header.setQUANTITATION(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 33:
/* Line 664 of lalr1.cc  */
#line 188 "Parser.ypp"
    { driver.context.header.setREPORT(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 34:
/* Line 664 of lalr1.cc  */
#line 189 "Parser.ypp"
    { driver.context.header.setREPTYPE(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 35:
/* Line 664 of lalr1.cc  */
#line 190 "Parser.ypp"
    { driver.context.header.setSEARCH(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 36:
/* Line 664 of lalr1.cc  */
#line 191 "Parser.ypp"
    { driver.context.header.setTAXONOMY(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 37:
/* Line 664 of lalr1.cc  */
#line 192 "Parser.ypp"
    { driver.context.header.setTOLU(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 38:
/* Line 664 of lalr1.cc  */
#line 193 "Parser.ypp"
    { /*driver.context.header.setUSER(*$3);*/ delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 39:
/* Line 664 of lalr1.cc  */
#line 194 "Parser.ypp"
    { driver.context.header.setUSEREMAIL(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 40:
/* Line 664 of lalr1.cc  */
#line 195 "Parser.ypp"
    { driver.context.header.setUSERNAME(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 41:
/* Line 664 of lalr1.cc  */
#line 196 "Parser.ypp"
    { driver.context.header.setDECOY(((yysemantic_stack_[(4) - (3)].integerVal))!=0); }
    break;

  case 42:
/* Line 664 of lalr1.cc  */
#line 197 "Parser.ypp"
    { driver.context.header.setERRORTOLERANT(((yysemantic_stack_[(4) - (3)].integerVal))!=0); }
    break;

  case 43:
/* Line 664 of lalr1.cc  */
#line 198 "Parser.ypp"
    { driver.context.header.setPFA((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 44:
/* Line 664 of lalr1.cc  */
#line 199 "Parser.ypp"
    { driver.context.header.setITOL((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 45:
/* Line 664 of lalr1.cc  */
#line 200 "Parser.ypp"
    { driver.context.header.setITOL((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 46:
/* Line 664 of lalr1.cc  */
#line 201 "Parser.ypp"
    { driver.context.header.setPEP_ISOTOPE_ERROR((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 47:
/* Line 664 of lalr1.cc  */
#line 202 "Parser.ypp"
    { driver.context.header.setPRECURSOR((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 48:
/* Line 664 of lalr1.cc  */
#line 203 "Parser.ypp"
    { driver.context.header.setPRECURSOR((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 49:
/* Line 664 of lalr1.cc  */
#line 204 "Parser.ypp"
    { driver.context.header.setSEG((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 50:
/* Line 664 of lalr1.cc  */
#line 205 "Parser.ypp"
    { driver.context.header.setSEG((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 51:
/* Line 664 of lalr1.cc  */
#line 206 "Parser.ypp"
    { driver.context.header.setTOL((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 52:
/* Line 664 of lalr1.cc  */
#line 207 "Parser.ypp"
    { driver.context.header.setTOL((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 53:
/* Line 664 of lalr1.cc  */
#line 208 "Parser.ypp"
    { driver.context.header.setCHARGE(*((yysemantic_stack_[(4) - (3)].integerlistnode)->integers_)); delete (yysemantic_stack_[(4) - (3)].integerlistnode); }
    break;

  case 54:
/* Line 664 of lalr1.cc  */
#line 209 "Parser.ypp"
    { driver.context.header.setFRAMES(*((yysemantic_stack_[(4) - (3)].integerlistnode)->integers_)); delete (yysemantic_stack_[(4) - (3)].integerlistnode); }
    break;

  case 58:
/* Line 664 of lalr1.cc  */
#line 215 "Parser.ypp"
    { driver.context.currentSpectrum.setTITLE(*(yysemantic_stack_[(2) - (1)].stringVal)); delete (yysemantic_stack_[(2) - (1)].stringVal); }
    break;

  case 59:
/* Line 664 of lalr1.cc  */
#line 216 "Parser.ypp"
    { driver.context.currentSpectrum.setCOMP(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 60:
/* Line 664 of lalr1.cc  */
#line 217 "Parser.ypp"
    { driver.context.currentSpectrum.setINSTRUMENT(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 61:
/* Line 664 of lalr1.cc  */
#line 218 "Parser.ypp"
    { driver.context.currentSpectrum.setIT_MODS(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 62:
/* Line 664 of lalr1.cc  */
#line 219 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 63:
/* Line 664 of lalr1.cc  */
#line 220 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 64:
/* Line 664 of lalr1.cc  */
#line 221 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS(std::make_pair((yysemantic_stack_[(6) - (3)].doubleVal), (yysemantic_stack_[(6) - (5)].doubleVal))); }
    break;

  case 65:
/* Line 664 of lalr1.cc  */
#line 222 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS(std::make_pair((yysemantic_stack_[(6) - (3)].doubleVal), (yysemantic_stack_[(6) - (5)].integerVal))); }
    break;

  case 66:
/* Line 664 of lalr1.cc  */
#line 223 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS(std::make_pair((yysemantic_stack_[(6) - (3)].integerVal), (yysemantic_stack_[(6) - (5)].doubleVal))); }
    break;

  case 67:
/* Line 664 of lalr1.cc  */
#line 224 "Parser.ypp"
    { driver.context.currentSpectrum.setRTINSECONDS(std::make_pair((yysemantic_stack_[(6) - (3)].integerVal), (yysemantic_stack_[(6) - (5)].integerVal))); }
    break;

  case 68:
/* Line 664 of lalr1.cc  */
#line 225 "Parser.ypp"
    { driver.context.currentSpectrum.setSCANS(std::make_pair((yysemantic_stack_[(4) - (3)].integerVal), -1)); }
    break;

  case 69:
/* Line 664 of lalr1.cc  */
#line 226 "Parser.ypp"
    { driver.context.currentSpectrum.setSCANS(std::make_pair((yysemantic_stack_[(6) - (3)].integerVal), (yysemantic_stack_[(6) - (5)].integerVal))); }
    break;

  case 70:
/* Line 664 of lalr1.cc  */
#line 227 "Parser.ypp"
    { driver.context.currentSpectrum.setTOLU(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 71:
/* Line 664 of lalr1.cc  */
#line 228 "Parser.ypp"
    { driver.context.currentSpectrum.setSEQ(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 72:
/* Line 664 of lalr1.cc  */
#line 229 "Parser.ypp"
    { driver.context.currentSpectrum.setTAG(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 73:
/* Line 664 of lalr1.cc  */
#line 230 "Parser.ypp"
    { driver.context.currentSpectrum.setETAG(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 74:
/* Line 664 of lalr1.cc  */
#line 231 "Parser.ypp"
    { driver.context.currentSpectrum.setTOL((yysemantic_stack_[(4) - (3)].doubleVal)); }
    break;

  case 75:
/* Line 664 of lalr1.cc  */
#line 232 "Parser.ypp"
    { driver.context.currentSpectrum.setTOL((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 76:
/* Line 664 of lalr1.cc  */
#line 233 "Parser.ypp"
    { driver.context.currentSpectrum.setCHARGE(*((yysemantic_stack_[(4) - (3)].integerlistnode)->integers_)); delete (yysemantic_stack_[(4) - (3)].integerlistnode); }
    break;

  case 77:
/* Line 664 of lalr1.cc  */
#line 234 "Parser.ypp"
    { driver.context.currentSpectrum.setMSLEVEL((yysemantic_stack_[(4) - (3)].integerVal)); }
    break;

  case 78:
/* Line 664 of lalr1.cc  */
#line 235 "Parser.ypp"
    { driver.context.currentSpectrum.setFILENAME(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 79:
/* Line 664 of lalr1.cc  */
#line 236 "Parser.ypp"
    { driver.context.currentSpectrum.setIONMODE(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 80:
/* Line 664 of lalr1.cc  */
#line 237 "Parser.ypp"
    { driver.context.currentSpectrum.setORGANISM(*(yysemantic_stack_[(4) - (3)].stringVal)); delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 81:
/* Line 664 of lalr1.cc  */
#line 238 "Parser.ypp"
    { /*driver.context.header.setPI(*$3);*/ delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 82:
/* Line 664 of lalr1.cc  */
#line 239 "Parser.ypp"
    { /*driver.context.header.setDATACOLLECTOR(*$3);*/ delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 83:
/* Line 664 of lalr1.cc  */
#line 240 "Parser.ypp"
    { /*driver.context.header.setSUBMITUSER(*$3);*/ delete (yysemantic_stack_[(4) - (3)].stringVal); }
    break;

  case 84:
/* Line 664 of lalr1.cc  */
#line 241 "Parser.ypp"
    { driver.context.currentSpectrum.setSMILES(*(yysemantic_stack_[(2) - (1)].stringVal)); delete (yysemantic_stack_[(2) - (1)].stringVal); }
    break;

  case 85:
/* Line 664 of lalr1.cc  */
#line 242 "Parser.ypp"
    { driver.context.currentSpectrum.setINCHI(*(yysemantic_stack_[(2) - (1)].stringVal)); delete (yysemantic_stack_[(2) - (1)].stringVal); }
    break;

  case 86:
/* Line 664 of lalr1.cc  */
#line 243 "Parser.ypp"
    { /*driver.context.header.setINCHIAUX(*$1);*/ delete (yysemantic_stack_[(2) - (1)].stringVal); }
    break;

  case 87:
/* Line 664 of lalr1.cc  */
#line 244 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(4) - (3)].doubleVal), 0.0)); }
    break;

  case 88:
/* Line 664 of lalr1.cc  */
#line 245 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(5) - (3)].doubleVal), (yysemantic_stack_[(5) - (4)].doubleVal))); }
    break;

  case 89:
/* Line 664 of lalr1.cc  */
#line 246 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(5) - (3)].doubleVal), (yysemantic_stack_[(5) - (4)].integerVal))); }
    break;

  case 90:
/* Line 664 of lalr1.cc  */
#line 247 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(4) - (3)].integerVal), 0.0)); }
    break;

  case 91:
/* Line 664 of lalr1.cc  */
#line 248 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(5) - (3)].integerVal), (yysemantic_stack_[(5) - (4)].doubleVal))); }
    break;

  case 92:
/* Line 664 of lalr1.cc  */
#line 249 "Parser.ypp"
    { driver.context.currentSpectrum.setPEPMASS(std::make_pair((yysemantic_stack_[(5) - (3)].integerVal), (yysemantic_stack_[(5) - (4)].integerVal))); }
    break;

  case 94:
/* Line 664 of lalr1.cc  */
#line 254 "Parser.ypp"
    {
             driver.context.mgfFile.setHeader(driver.context.header);
             driver.context.header.clear();
         }
    break;


/* Line 664 of lalr1.cc  */
#line 1032 "Parser.cpp"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (int yystate, int yytoken)
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yychar.
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            /* Stay within bounds of both yycheck and yytname.  */
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULL;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char Parser::yypact_ninf_ = -97;
  const short int
  Parser::yypact_[] =
  {
       -97,    93,   -97,    18,    16,   -37,    -4,     5,    27,    28,
      29,    30,    31,    32,    33,    34,    36,    37,    38,    39,
      43,    47,    49,    51,    78,    80,    82,    83,    84,    85,
      86,    87,    88,    12,   -97,   -97,   -97,     9,    94,    96,
      40,    98,   102,   104,   133,   135,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   156,   157,    42,
     158,   160,    44,    46,   161,   -97,   165,   -97,   -50,     9,
     -97,    -2,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   -97,     2,   -97,   -97,   -97,     1,   -97,   196,
     196,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   197,    22,   -97,   -97,   -97,   -97,   198,    48,
      50,   137,   138,   201,   154,   155,   159,   162,   163,   164,
     199,   200,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   215,   216,   -97,    10,   -97,   -97,   217,
     219,   220,   224,     9,   221,   -97,   222,   223,   225,    73,
      75,   226,   227,   228,   -97,    77,   229,   232,   231,   233,
     234,   235,   236,   237,   -97,   -97,   -97,   243,   -97,   -97,
     -97,   -97,   -97,    -1,   244,   245,   246,   247,    17,    24,
       6,     7,     8,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   260,   261,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   271,   272,   -97,   273,   274,   -97,    79,   -97,
      81,   -97,   275,   -97,   -97,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97,   276,
     277,   279,   280,   281,   -97,   -97,   -97,   -97,   -97
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  Parser::yydefact_[] =
  {
        21,    17,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,     1,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    94,     0,    18,    10,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,    56,     8,     9,     0,    53,     0,
       0,    27,    28,    52,    51,    37,    23,    24,    25,    26,
      29,    30,    31,    32,    33,    34,    35,    36,    38,    39,
      40,    41,    42,    45,    44,    46,    43,    48,    47,    50,
      49,    54,     0,     0,    11,    13,    12,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,    57,    93,     0,
       0,     0,     0,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    84,    85,    86,     0,     6,     5,
       3,     4,     2,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    76,    59,    73,    60,
      61,    90,     0,     0,    87,     0,     0,    63,     0,    62,
       0,    68,     0,    71,    72,    75,    74,    70,    77,    78,
      79,    80,    81,    82,    83,    92,    91,    89,    88,     0,
       0,     0,     0,     0,    67,    66,    65,    64,    69
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  Parser::yypgoto_[] =
  {
       -97,   -12,   -97,   -96,   -69,   -97,   -97,   -97,   -97,   -97,
     -97,   -97,   -97,   -97
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  Parser::yydefgoto_[] =
  {
        -1,   175,   176,    70,    71,   103,    33,    67,     1,    34,
     143,   177,     2,     3
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char Parser::yytable_ninf_ = -1;
  const unsigned short int
  Parser::yytable_[] =
  {
       107,   108,   236,   109,   109,   141,   109,   105,   106,   247,
     249,   251,    65,   145,   146,    68,   149,   150,    35,    36,
     241,    66,   207,   242,   243,    37,   148,   244,   149,   150,
     245,   246,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,    74,    75,    94,    95,
      98,    99,   100,   101,   179,   180,   181,   182,    38,   110,
     110,   144,   110,   142,   248,   250,   252,    39,    69,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   218,
     219,   220,   221,   225,   226,   269,   270,   271,   272,    40,
      41,    42,    43,    44,    45,    46,    47,     4,    48,    49,
      50,    51,    72,     5,    73,    52,    76,     6,     7,    53,
      77,    54,    78,    55,   213,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      56,    79,    57,    80,    58,    59,    60,    61,    62,    63,
      64,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,   208,    96,    97,   102,   104,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   183,
     184,   178,    68,   147,   185,   194,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   204,   186,   187,   205,   206,
     209,   188,   210,   211,   189,   190,   191,   212,     0,   214,
     215,   216,   222,   217,     0,   223,   224,   227,   228,   229,
       0,   230,   231,   232,   233,   234,   235,   237,   238,   239,
     240,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   192,   193,   263,   264,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   265,   266,   267,   268,     0,   274,
     275,   273,   276,   277,   278
  };

  /* YYCHECK.  */
  const short int
  Parser::yycheck_[] =
  {
        69,     3,     3,     5,     5,     3,     5,    57,    58,     3,
       3,     3,     0,   109,   110,     6,     6,     7,     0,     3,
       3,     9,    12,     6,     7,    62,     4,     3,     6,     7,
       6,     7,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,     6,     7,     6,     7,
       6,     7,     6,     7,     6,     7,     6,     7,    62,    61,
      61,    60,    61,    61,    58,    58,    58,    62,    59,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,     6,
       7,     6,     7,     6,     7,     6,     7,     6,     7,    62,
      62,    62,    62,    62,    62,    62,    62,     4,    62,    62,
      62,    62,     8,    10,     8,    62,     8,    14,    15,    62,
       8,    62,     8,    62,   183,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      62,     8,    62,     8,    62,    62,    62,    62,    62,    62,
      62,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     6,     6,   176,     7,     6,     6,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,    62,
      62,     3,     6,     6,     3,     3,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    62,    62,     3,     3,
       3,    62,     3,     3,    62,    62,    62,     3,    -1,     8,
       8,     8,     6,     8,    -1,     8,     8,     8,     6,     8,
      -1,     8,     8,     8,     8,     8,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,    62,    62,     3,     3,    62,    62,    62,    62,    62,
      62,    62,    62,    62,     3,     3,     3,     3,    -1,     3,
       3,     6,     3,     3,     3
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  Parser::yystos_[] =
  {
         0,    71,    75,    76,     4,    10,    14,    15,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    69,    72,     0,     3,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,     0,     9,    70,     6,    59,
      66,    67,     8,     8,     6,     7,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     6,     6,     6,     7,     7,     6,     6,     7,
       6,     7,     6,    68,     3,    57,    58,    67,     3,     5,
      61,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,    61,    73,    60,    66,    66,     6,     4,     6,
       7,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    64,    65,    74,     3,     6,
       7,     6,     7,    62,    62,     3,    62,    62,    62,    62,
      62,    62,    62,    62,     3,    62,    62,    62,    62,    62,
      62,    62,    62,    62,     3,     3,     3,    12,    64,     3,
       3,     3,     3,    67,     8,     8,     8,     8,     6,     7,
       6,     7,     6,     8,     8,     6,     7,     8,     6,     8,
       8,     8,     8,     8,     8,     3,     3,     3,     3,     3,
       3,     3,     6,     7,     3,     6,     7,     3,    58,     3,
      58,     3,    58,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     6,
       7,     6,     7,     6,     3,     3,     3,     3,     3
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    43,    45,    40,
      41,    44,    61
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  Parser::yyr1_[] =
  {
         0,    63,    64,    64,    64,    64,    65,    65,    66,    66,
      66,    67,    67,    67,    67,    68,    68,    69,    69,    70,
      70,    71,    71,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    73,    73,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    75,    76
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  Parser::yyr2_[] =
  {
         0,     2,     3,     3,     3,     3,     2,     1,     2,     2,
       1,     3,     3,     3,     1,     3,     1,     0,     2,     6,
       5,     0,     2,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     2,     0,     2,     2,     4,
       4,     4,     4,     4,     6,     6,     6,     6,     4,     6,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     2,     2,     2,     4,     5,     5,
       4,     5,     5,     2,     3,     1
  };


  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"end of line\"",
  "\"comment\"", "\"and keyword\"", "\"integer\"", "\"double\"",
  "\"string\"", "\"begin_ions keyword\"", "\"charge set keyword\"",
  "\"amino acid composition keyword\"", "\"end_ions keyword\"",
  "\"error tolerant sequence keyword\"", "\"MS/MS ion series keyword\"",
  "\"variable modifications keyword\"", "\"precursor mass keyword\"",
  "\"retention time or range keyword\"",
  "\"scan number of range keyword\"", "\"amino acid sequence keyword\"",
  "\"sequence tag keyword\"", "\"title keyword and full title string\"",
  "\"peptide mass tolerance keyword\"", "\"tolerance units keyword\"",
  "\"enzyme keyword\"", "\"search title keyword\"", "\"database keyword\"",
  "\"MS/MS datafile format keyword\"", "\"units for ITOL keyword\"",
  "\"mass type (mono or avg) keyword\"", "\"fixed modifications keyword\"",
  "\"quantitation method keyword\"", "\"maximum hits keyword\"",
  "\"type of report keyword\"", "\"type of search keyword\"",
  "\"taxonomy keyword\"", "\"user keyword\"", "\"user email keyword\"",
  "\"username keyword\"", "\"perform decoy search keyword\"",
  "\"error tolerance keyword\"", "\"fragment ion tolerance keyword\"",
  "\"misassigned 13C keyword\"", "\"partials keyword\"",
  "\"precursor m/z keyword\"", "\"protein mass (kDa) keyword\"",
  "\"NA translation keyword\"", "\"MS level keyword\"",
  "\"filename keyword\"", "\"ionisation mode keyword\"",
  "\"organism keyword\"", "\"ivestigator keyword\"",
  "\"data collector keyword\"", "\"submit user keyword\"",
  "\"smile string keyword\"",
  "\"international chemical identifier keyword\"",
  "\"auxiliary INCHI information keyword\"", "'+'", "'-'", "'('", "')'",
  "','", "'='", "$accept", "ion", "ions", "charge", "charges",
  "csintegerlist", "blocks", "block", "globalparams", "globalparam",
  "localparams", "localparam", "contents", "start", YY_NULL
  };

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const Parser::rhs_number_type
  Parser::yyrhs_[] =
  {
        76,     0,    -1,     7,     7,     3,    -1,     6,     7,     3,
      -1,     7,     6,     3,    -1,     6,     6,     3,    -1,    65,
      64,    -1,    64,    -1,     6,    57,    -1,     6,    58,    -1,
       6,    -1,    59,    67,    60,    -1,    67,    61,    66,    -1,
      67,     5,    66,    -1,    66,    -1,    68,    61,     6,    -1,
       6,    -1,    -1,    69,    70,    -1,     9,     3,    73,    65,
      12,     3,    -1,     9,     3,    73,    12,     3,    -1,    -1,
      71,    72,    -1,    24,    62,     8,     3,    -1,    25,    62,
       8,     3,    -1,    26,    62,     8,     3,    -1,    27,    62,
       8,     3,    -1,    14,    62,     8,     3,    -1,    15,    62,
       8,     3,    -1,    28,    62,     8,     3,    -1,    29,    62,
       8,     3,    -1,    30,    62,     8,     3,    -1,    31,    62,
       8,     3,    -1,    32,    62,     8,     3,    -1,    33,    62,
       8,     3,    -1,    34,    62,     8,     3,    -1,    35,    62,
       8,     3,    -1,    23,    62,     8,     3,    -1,    36,    62,
       8,     3,    -1,    37,    62,     8,     3,    -1,    38,    62,
       8,     3,    -1,    39,    62,     6,     3,    -1,    40,    62,
       6,     3,    -1,    43,    62,     6,     3,    -1,    41,    62,
       7,     3,    -1,    41,    62,     6,     3,    -1,    42,    62,
       7,     3,    -1,    44,    62,     7,     3,    -1,    44,    62,
       6,     3,    -1,    45,    62,     7,     3,    -1,    45,    62,
       6,     3,    -1,    22,    62,     7,     3,    -1,    22,    62,
       6,     3,    -1,    10,    62,    67,     3,    -1,    46,    62,
      68,     3,    -1,     4,     3,    -1,    -1,    73,    74,    -1,
      21,     3,    -1,    11,    62,     8,     3,    -1,    14,    62,
       8,     3,    -1,    15,    62,     8,     3,    -1,    17,    62,
       7,     3,    -1,    17,    62,     6,     3,    -1,    17,    62,
       7,    58,     7,     3,    -1,    17,    62,     7,    58,     6,
       3,    -1,    17,    62,     6,    58,     7,     3,    -1,    17,
      62,     6,    58,     6,     3,    -1,    18,    62,     6,     3,
      -1,    18,    62,     6,    58,     6,     3,    -1,    23,    62,
       8,     3,    -1,    19,    62,     8,     3,    -1,    20,    62,
       8,     3,    -1,    13,    62,     8,     3,    -1,    22,    62,
       7,     3,    -1,    22,    62,     6,     3,    -1,    10,    62,
      67,     3,    -1,    47,    62,     6,     3,    -1,    48,    62,
       8,     3,    -1,    49,    62,     8,     3,    -1,    50,    62,
       8,     3,    -1,    51,    62,     8,     3,    -1,    52,    62,
       8,     3,    -1,    53,    62,     8,     3,    -1,    54,     3,
      -1,    55,     3,    -1,    56,     3,    -1,    16,    62,     7,
       3,    -1,    16,    62,     7,     7,     3,    -1,    16,    62,
       7,     6,     3,    -1,    16,    62,     6,     3,    -1,    16,
      62,     6,     7,     3,    -1,    16,    62,     6,     6,     3,
      -1,     4,     3,    -1,    71,    69,     0,    -1,    75,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  Parser::yyprhs_[] =
  {
         0,     0,     3,     7,    11,    15,    19,    22,    24,    27,
      30,    32,    36,    40,    44,    46,    50,    52,    53,    56,
      63,    69,    70,    73,    78,    83,    88,    93,    98,   103,
     108,   113,   118,   123,   128,   133,   138,   143,   148,   153,
     158,   163,   168,   173,   178,   183,   188,   193,   198,   203,
     208,   213,   218,   223,   228,   233,   236,   237,   240,   243,
     248,   253,   258,   263,   268,   275,   282,   289,   296,   301,
     308,   313,   318,   323,   328,   333,   338,   343,   348,   353,
     358,   363,   368,   373,   378,   381,   384,   387,   392,   398,
     404,   409,   415,   421,   424,   428
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  Parser::yyrline_[] =
  {
         0,   141,   141,   142,   143,   144,   146,   147,   149,   150,
     151,   153,   154,   155,   156,   158,   159,   161,   162,   168,
     173,   175,   176,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   212,   213,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   253,   259
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      59,    60,     2,    57,    61,    58,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    62,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int Parser::yyeof_ = 0;
  const int Parser::yylast_ = 284;
  const int Parser::yynnts_ = 14;
  const int Parser::yyempty_ = -2;
  const int Parser::yyfinal_ = 35;
  const int Parser::yyterror_ = 1;
  const int Parser::yyerrcode_ = 256;
  const int Parser::yyntokens_ = 63;

  const unsigned int Parser::yyuser_token_number_max_ = 311;
  const Parser::token_number_type Parser::yyundef_token_ = 2;


} // mgf
/* Line 1135 of lalr1.cc  */
#line 1786 "Parser.cpp"
/* Line 1136 of lalr1.cc  */
#line 261 "Parser.ypp"
 /*** Additional Code ***/

void mgf::Parser::error(const Parser::location_type& l, const std::string& m)
{
    driver.error(l, m);
}
