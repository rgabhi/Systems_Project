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
     INTEGER = 258,
     IDENTIFIER = 259,
     VAR = 260,
     IF = 261,
     ELSE = 262,
     WHILE = 263,
     FOR = 264,
     EQ = 265,
     NEQ = 266,
     LT = 267,
     GT = 268,
     LE = 269,
     GE = 270,
     PLUS = 271,
     MINUS = 272,
     MULT = 273,
     DIV = 274,
     ASSIGN = 275,
     SEMI = 276,
     LPAREN = 277,
     RPAREN = 278,
     LBRACE = 279,
     RBRACE = 280,
     LOWER_THAN_ELSE = 281
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define IDENTIFIER 259
#define VAR 260
#define IF 261
#define ELSE 262
#define WHILE 263
#define FOR 264
#define EQ 265
#define NEQ 266
#define LT 267
#define GT 268
#define LE 269
#define GE 270
#define PLUS 271
#define MINUS 272
#define MULT 273
#define DIV 274
#define ASSIGN 275
#define SEMI 276
#define LPAREN 277
#define RPAREN 278
#define LBRACE 279
#define RBRACE 280
#define LOWER_THAN_ELSE 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 56 "src/parser/parser.y"
{
    int intValue;
    char* idName;
    struct ASTNode* node;
}
/* Line 1529 of yacc.c.  */
#line 107 "src/parser/parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

