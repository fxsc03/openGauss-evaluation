/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_PLPGSQL_YY_SRC_PLTSQL_PL_GRAM_HPP_INCLUDED
# define YY_PLPGSQL_YY_SRC_PLTSQL_PL_GRAM_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int plpgsql_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENT = 258,
    FCONST = 259,
    SCONST = 260,
    BCONST = 261,
    VCONST = 262,
    XCONST = 263,
    Op = 264,
    CmpOp = 265,
    CmpNullOp = 266,
    COMMENTSTRING = 267,
    SET_USER_IDENT = 268,
    SET_IDENT = 269,
    UNDERSCORE_CHARSET = 270,
    FCONST_F = 271,
    FCONST_D = 272,
    ICONST = 273,
    PARAM = 274,
    TYPECAST = 275,
    ORA_JOINOP = 276,
    DOT_DOT = 277,
    COLON_EQUALS = 278,
    PARA_EQUALS = 279,
    SET_IDENT_SESSION = 280,
    SET_IDENT_GLOBAL = 281,
    DIALECT_TSQL = 282,
    T_WORD = 283,
    T_CWORD = 284,
    T_DATUM = 285,
    T_PLACEHOLDER = 286,
    T_LABELLOOP = 287,
    T_LABELWHILE = 288,
    T_LABELREPEAT = 289,
    T_VARRAY = 290,
    T_ARRAY_FIRST = 291,
    T_ARRAY_LAST = 292,
    T_ARRAY_COUNT = 293,
    T_ARRAY_EXISTS = 294,
    T_ARRAY_PRIOR = 295,
    T_ARRAY_NEXT = 296,
    T_ARRAY_DELETE = 297,
    T_ARRAY_EXTEND = 298,
    T_ARRAY_TRIM = 299,
    T_VARRAY_VAR = 300,
    T_RECORD = 301,
    T_TABLE = 302,
    T_TABLE_VAR = 303,
    T_PACKAGE_VARIABLE = 304,
    T_OBJECT_TYPE_VAR_METHOD = 305,
    T_PACKAGE_CURSOR_ISOPEN = 306,
    T_PACKAGE_CURSOR_FOUND = 307,
    T_PACKAGE_CURSOR_NOTFOUND = 308,
    T_PACKAGE_CURSOR_ROWCOUNT = 309,
    LESS_LESS = 310,
    GREATER_GREATER = 311,
    T_REFCURSOR = 312,
    T_SQL_ISOPEN = 313,
    T_SQL_FOUND = 314,
    T_SQL_NOTFOUND = 315,
    T_SQL_ROWCOUNT = 316,
    T_SQL_BULK_EXCEPTIONS = 317,
    T_CURSOR_ISOPEN = 318,
    T_CURSOR_FOUND = 319,
    T_CURSOR_NOTFOUND = 320,
    T_CURSOR_ROWCOUNT = 321,
    T_DECLARE_CURSOR = 322,
    T_DECLARE_CONDITION = 323,
    T_DECLARE_HANDLER = 324,
    K_ABSOLUTE = 325,
    K_ALIAS = 326,
    K_ALL = 327,
    K_ALTER = 328,
    K_ARRAY = 329,
    K_AS = 330,
    K_BACKWARD = 331,
    K_BEGIN = 332,
    K_BULK = 333,
    K_BY = 334,
    K_CALL = 335,
    K_CASE = 336,
    K_CATALOG_NAME = 337,
    K_CLASS_ORIGIN = 338,
    K_CLOSE = 339,
    K_COLLATE = 340,
    K_COLLECT = 341,
    K_COLUMN_NAME = 342,
    K_COMMIT = 343,
    K_CONDITION = 344,
    K_CONSTANT = 345,
    K_CONSTRAINT_CATALOG = 346,
    K_CONSTRAINT_NAME = 347,
    K_CONSTRAINT_SCHEMA = 348,
    K_CONTINUE = 349,
    K_CURRENT = 350,
    K_CURSOR = 351,
    K_CURSOR_NAME = 352,
    K_DEBUG = 353,
    K_DECLARE = 354,
    K_DEFAULT = 355,
    K_DELETE = 356,
    K_DETAIL = 357,
    K_DETERMINISTIC = 358,
    K_DIAGNOSTICS = 359,
    K_DISTINCT = 360,
    K_DO = 361,
    K_DUMP = 362,
    K_ELSE = 363,
    K_ELSIF = 364,
    K_END = 365,
    K_ERRCODE = 366,
    K_ERROR = 367,
    K_EXCEPT = 368,
    K_EXCEPTION = 369,
    K_EXCEPTIONS = 370,
    K_EXECUTE = 371,
    K_EXIT = 372,
    K_FALSE = 373,
    K_FETCH = 374,
    K_FIRST = 375,
    K_FOR = 376,
    K_FORALL = 377,
    K_FOREACH = 378,
    K_FORWARD = 379,
    K_FOUND = 380,
    K_FROM = 381,
    K_FUNCTION = 382,
    K_GET = 383,
    K_GOTO = 384,
    K_HANDLER = 385,
    K_HINT = 386,
    K_IF = 387,
    K_IMMEDIATE = 388,
    K_INSTANTIATION = 389,
    K_IN = 390,
    K_INDEX = 391,
    K_INFO = 392,
    K_INSERT = 393,
    K_INTERSECT = 394,
    K_INTO = 395,
    K_IS = 396,
    K_ITERATE = 397,
    K_LAST = 398,
    K_LEAVE = 399,
    K_LIMIT = 400,
    K_LOG = 401,
    K_LOOP = 402,
    K_MERGE = 403,
    K_MESSAGE = 404,
    K_MESSAGE_TEXT = 405,
    K_MOVE = 406,
    K_MULTISET = 407,
    K_MULTISETS = 408,
    K_MYSQL_ERRNO = 409,
    K_NUMBER = 410,
    K_NEXT = 411,
    K_NO = 412,
    K_NOT = 413,
    K_NOTICE = 414,
    K_NULL = 415,
    K_OF = 416,
    K_OPEN = 417,
    K_OPTION = 418,
    K_OR = 419,
    K_OUT = 420,
    K_PACKAGE = 421,
    K_PERFORM = 422,
    K_PIPE = 423,
    K_PG_EXCEPTION_CONTEXT = 424,
    K_PG_EXCEPTION_DETAIL = 425,
    K_PG_EXCEPTION_HINT = 426,
    K_PRAGMA = 427,
    K_PRIOR = 428,
    K_PROCEDURE = 429,
    K_QUERY = 430,
    K_RAISE = 431,
    K_RECORD = 432,
    K_REF = 433,
    K_RELATIVE = 434,
    K_RELEASE = 435,
    K_REPEAT = 436,
    K_REPLACE = 437,
    K_RESULT_OID = 438,
    K_RESIGNAL = 439,
    K_RETURN = 440,
    K_RETURNED_SQLSTATE = 441,
    K_REVERSE = 442,
    K_ROLLBACK = 443,
    K_ROW = 444,
    K_ROWTYPE = 445,
    K_ROW_COUNT = 446,
    K_SAVE = 447,
    K_SAVEPOINT = 448,
    K_SCHEMA_NAME = 449,
    K_SELECT = 450,
    K_SCROLL = 451,
    K_SIGNAL = 452,
    K_SLICE = 453,
    K_SQLEXCEPTION = 454,
    K_SQLSTATE = 455,
    K_SQLWARNING = 456,
    K_STACKED = 457,
    K_STRICT = 458,
    K_SUBCLASS_ORIGIN = 459,
    K_SUBTYPE = 460,
    K_SYS_REFCURSOR = 461,
    K_TABLE = 462,
    K_TABLE_NAME = 463,
    K_THEN = 464,
    K_TO = 465,
    K_TRUE = 466,
    K_TYPE = 467,
    K_UNION = 468,
    K_UNTIL = 469,
    K_UPDATE = 470,
    K_USE_COLUMN = 471,
    K_USE_VARIABLE = 472,
    K_USING = 473,
    K_VARIABLE_CONFLICT = 474,
    K_VARRAY = 475,
    K_WARNING = 476,
    K_WHEN = 477,
    K_WHILE = 478,
    K_WITH = 479
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 300 "src/pltsql/gram.y" /* yacc.c:1909  */

        core_YYSTYPE			core_yystype;
        /* these fields must match core_YYSTYPE: */
        int						ival;
        char					*str;
        const char				*keyword;

        PLword					word;
        PLcword					cword;
        PLwdatum				wdatum;
        bool					boolean;
        Oid						oid;
        VarName					*varname;
        struct
        {
            char *name;
            int  lineno;
            PLpgSQL_datum   *scalar;
            PLpgSQL_rec		*rec;
            PLpgSQL_row		*row;
            int  dno;
        }						forvariable;
        struct
        {
            char *label;
            int  n_initvars;
            int  *initvarnos;
            bool isAutonomous;
        }						declhdr;
        struct
        {
            List *stmts;
            char *end_label;
            int   end_label_location;
        }						loop_body;
        struct
        {
            List *stmts;
            char *end_label;
            int   end_label_location;
        }                                               while_body;
        struct
        {
            PLpgSQL_expr                    *expr;
            char *end_label;
            int   end_label_location;
        }                                               repeat_condition;
        struct
        {
            PLpgSQL_expr  *expr;
            int            endtoken;
        }                                               expr_until_while_loop;
        List					*list;
        PLpgSQL_type			*dtype;
        PLpgSQL_datum			*datum;
        PLpgSQL_var				*var;
        PLpgSQL_expr			*expr;
        PLpgSQL_stmt			*stmt;
        PLpgSQL_condition		*condition;
        PLpgSQL_exception		*exception;
        PLpgSQL_exception_block	*exception_block;
        PLpgSQL_nsitem			*nsitem;
        PLpgSQL_diag_item		*diagitem;
        PLpgSQL_stmt_fetch		*fetch;
        PLpgSQL_case_when		*casewhen;
        PLpgSQL_declare_handler declare_handler_type;
        PLpgSQL_rec_attr	*recattr;
        Node                            *plnode;
        DefElem             *def;

#line 350 "src/pltsql/pl_gram.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern THR_LOCAL YYSTYPE plpgsql_yylval;
extern THR_LOCAL YYLTYPE plpgsql_yylloc;
int pltsql_yyparse (void);

#endif /* !YY_PLPGSQL_YY_SRC_PLTSQL_PL_GRAM_HPP_INCLUDED  */
