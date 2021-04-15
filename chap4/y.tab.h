/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     COMMA = 258,
     COLON = 259,
     SEMICOLON = 260,
     LPAREN = 261,
     RPAREN = 262,
     LBRACK = 263,
     RBRACK = 264,
     LBRACE = 265,
     RBRACE = 266,
     DOT = 267,
     ASSIGN = 268,
     ARRAY = 269,
     IF = 270,
     THEN = 271,
     ELSE = 272,
     WHILE = 273,
     FOR = 274,
     TO = 275,
     DO = 276,
     LET = 277,
     IN = 278,
     END = 279,
     OF = 280,
     BREAK = 281,
     NIL = 282,
     FUNCTION = 283,
     VAR = 284,
     TYPE = 285,
     ID = 286,
     STRING = 287,
     INT = 288,
     MINUS = 289,
     DIVIDE = 290,
     TIMES = 291,
     EQ = 292,
     NEQ = 293,
     GT = 294,
     LT = 295,
     GE = 296,
     LE = 297,
     AND = 298,
     OR = 299,
     PLUS = 300
   };
#endif
/* Tokens.  */
#define COMMA 258
#define COLON 259
#define SEMICOLON 260
#define LPAREN 261
#define RPAREN 262
#define LBRACK 263
#define RBRACK 264
#define LBRACE 265
#define RBRACE 266
#define DOT 267
#define ASSIGN 268
#define ARRAY 269
#define IF 270
#define THEN 271
#define ELSE 272
#define WHILE 273
#define FOR 274
#define TO 275
#define DO 276
#define LET 277
#define IN 278
#define END 279
#define OF 280
#define BREAK 281
#define NIL 282
#define FUNCTION 283
#define VAR 284
#define TYPE 285
#define ID 286
#define STRING 287
#define INT 288
#define MINUS 289
#define DIVIDE 290
#define TIMES 291
#define EQ 292
#define NEQ 293
#define GT 294
#define LT 295
#define GE 296
#define LE 297
#define AND 298
#define OR 299
#define PLUS 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 19 "tiger.grm"
{
	int pos;
	int ival;
	string sval;
  A_oper op;
	A_var var;
	A_exp exp;
  A_expList expList;
  A_dec dec;
  A_decList decList;
  A_field field;
  A_fieldList fields;
  A_efield efield;
  A_efieldList efields;
  A_ty type;
	/* et cetera */
	}
/* Line 1529 of yacc.c.  */
#line 157 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

