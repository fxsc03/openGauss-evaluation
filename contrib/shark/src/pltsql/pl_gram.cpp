/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         pltsql_yyparse
#define yylex           pltsql_yylex
#define yyerror         plpgsql_yyerror
#define yydebug         plpgsql_yydebug
#define yynerrs         plpgsql_yynerrs

#define yylval          plpgsql_yylval
#define yychar          plpgsql_yychar
#define yylloc          plpgsql_yylloc

/* Copy the first part of user declarations.  */
#line 1 "src/pltsql/gram.y" /* yacc.c:339  */

/* -------------------------------------------------------------------------
 *
 * gram.y				- Parser for the PL/pgSQL procedural language
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/pl/plpgsql/src/gram.y
 *
 * -------------------------------------------------------------------------
 */

#include "utils/plpgsql_domain.h"
#include "pltsql.h"

#include "access/transam.h"
#include "access/xact.h"
#include "access/tupconvert.h"
#include "catalog/dependency.h"
#include "catalog/gs_package.h"
#include "catalog/gs_dependencies_fn.h"
#include "catalog/namespace.h"
#include "catalog/pg_object.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_synonym.h"
#include "catalog/pg_type.h"
#include "catalog/pg_type_fn.h"
#include "catalog/pg_object_type.h"
#include "commands/typecmds.h"
#include "funcapi.h"
#include "nodes/makefuncs.h"
#include "parser/analyze.h"
#include "parser/keywords.h"
#include "parser/parser.h"
#include "parser/parse_coerce.h"
#include "parser/parse_expr.h"
#include "parser/parse_func.h"
#include "parser/parse_type.h"
#include "parser/scanner.h"
#include "parser/scansup.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/guc.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/pl_package.h"
#include "utils/syscache.h"
#include "utils/typcache.h"
#include "knl/knl_session.h"
#include "utils/varbit.h"
#include "catalog/indexing.h"

#include <limits.h>

#define MAX_LAYER 256
#define LENGTH_OF_BRACKETS_AND_DOT 4
#define LENGTH_OF_QUOTATION_MARKS 2
#define IS_ANONYMOUS_BLOCK \
    (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL && \
        strcmp(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_signature, "inline_code_block") ==0)
#define IS_PACKAGE \
    (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL && \
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL)
/* Location tracking support --- simpler than bison's default */
#define YYLLOC_DEFAULT(Current, Rhs, N) \
    do { \
        if (N) \
            (Current) = (Rhs)[1]; \
        else \
            (Current) = (Rhs)[0]; \
    } while (0)

/*
 * Helper function to record statements label so that GOTO statements could reach
 * there.
 */
static inline void
record_stmt_label(char * label, PLpgSQL_stmt *stmt)
{
    /*
     * Note, we do plpgsql parsing under "PL/pgSQL Function" memory context,
     * so palloc memory for goto-lable element and the global array under it.
     */
    PLpgSQL_gotoLabel *gl =
            (PLpgSQL_gotoLabel *)palloc0(sizeof(PLpgSQL_gotoLabel));
    gl->label = label;
    gl->stmt = stmt;
    u_sess->plsql_cxt.curr_compile_context->goto_labels = lappend(u_sess->plsql_cxt.curr_compile_context->goto_labels, (void *)gl);
}

/*
 * Bison doesn't allocate anything that needs to live across parser calls,
 * so we can easily have it use palloc instead of malloc.  This prevents
 * memory leaks if we error out during parsing.  Note this only works with
 * bison >= 2.0.  However, in bison 1.875 the default is to use alloca()
 * if possible, so there's not really much problem anyhow, at least if
 * you're building with gcc.
 */
#define YYMALLOC palloc
#define YYFREE   pfree


typedef struct
{
    int			location;
    int			leaderlen;
} sql_error_callback_arg;

#define parser_errposition(pos)  plpgsql_scanner_errposition(pos)

union YYSTYPE;					/* need forward reference for tok_is_keyword */


static	bool			tok_is_keyword(int token, union YYSTYPE *lval,
                                       int kw_token, const char *kw_str);
static	void			word_is_not_variable(PLword *word, int location);
static	void			cword_is_not_variable(PLcword *cword, int location);
static	void			current_token_is_not_variable(int tok);
static void yylex_inparam(StringInfoData* func_inparam,
                                            int *params,
                                            int * tok,
                                            int *tableof_func_dno,
                                            int *tableof_var_dno);
static int   	yylex_outparam(char** fieldnames,
                               int *varnos,
                               int nfields,
                               PLpgSQL_row **row,
                               PLpgSQL_rec **rec,
                               int *token,
                               int *varno,
                               bool is_push_back,
                               bool overload = false);

static bool is_function(const char *name, bool is_assign, bool no_parenthesis, List* funcNameList = NULL);
static bool is_unreservedkeywordfunction(int kwnum, bool no_parenthesis, const char *name);
static bool is_paren_friendly_datatype(TypeName *name);
static void 	plpgsql_parser_funcname(const char *s, char **output,
                                        int numidents);
static PLpgSQL_stmt 	*make_callfunc_stmt(const char *sqlstart,
                                int location, bool is_assign, bool eaten_first_token, List* funcNameList = NULL, int arrayFuncDno = -1, bool isCallFunc = false, PLwdatum *objtypwdatum = NULL);
static PLpgSQL_stmt 	*make_callfunc_stmt_no_arg(const char *sqlstart, int location, bool withsemicolon = false, List* funcNameList = NULL);
static PLpgSQL_expr 	*read_sql_construct6(int until,
                                             int until2,
                                             int until3,
                                             int until4,
                                             int until5,
                                             int until6,
                                             const char *expected,
                                             const char *sqlstart,
                                             bool isexpression,
                                             bool valid_sql,
                                             bool trim,
                                             int *startloc,
                                             int *endtoken,
                                             DList* tokenstack = NULL,
                                             bool issaveexpr = false);
                                             
static	PLpgSQL_expr	*read_sql_construct(int until,
                                            int until2,
                                            int until3,
                                            const char *expected,
                                            const char *sqlstart,
                                            bool isexpression,
                                            bool valid_sql,
                                            bool trim,
                                            int *startloc,
                                            int *endtoken);
static	PLpgSQL_expr	*read_sql_expression(int until,
                                             const char *expected);
static	PLpgSQL_expr	*read_sql_expression2(int until, int until2,
                                              const char *expected,
                                              int *endtoken);
static	PLpgSQL_expr	*read_sql_stmt(const char *sqlstart);
static	PLpgSQL_type	*read_datatype(int tok);
static  PLpgSQL_stmt	*parse_lob_open_close(int location);
static	PLpgSQL_stmt	*make_execsql_stmt(int firsttoken, int location);
static	PLpgSQL_stmt_fetch *read_fetch_direction(void);
static	void			 complete_direction(PLpgSQL_stmt_fetch *fetch,
                                            bool *check_FROM);
static	PLpgSQL_stmt	*make_return_stmt(int location);
static	PLpgSQL_stmt	*make_return_next_stmt(int location);
static	PLpgSQL_stmt	*make_return_query_stmt(int location);
static  PLpgSQL_stmt	*make_case(int location, PLpgSQL_expr *t_expr,
                                   List *case_when_list, List *else_stmts);
static	char			*NameOfDatum(PLwdatum *wdatum);
static  char                    *CopyNameOfDatum(PLwdatum *wdatum);
static	void			 check_assignable(PLpgSQL_datum *datum, int location);
static	bool			 read_into_target(PLpgSQL_rec **rec, PLpgSQL_row **row,
                                          bool *strict, int firsttoken, bool bulk_collect);
static	PLpgSQL_row		*read_into_scalar_list(char *initial_name,
                                               PLpgSQL_datum *initial_datum,
                                               int initial_dno,
                                               int initial_location);
static	PLpgSQL_row		*read_into_array_table_scalar_list(char *initial_name,
                                               PLpgSQL_datum *initial_datum,
                                               int initial_dno,
                                               int initial_location,
                                               bool bulk_collect);
static void             read_using_target(List **in_expr, PLpgSQL_row **out_row);

static PLpgSQL_row  *read_into_placeholder_scalar_list(char *initial_name,
                      int initial_location);

static	PLpgSQL_row		*make_scalar_list1(char *initial_name,
                                           PLpgSQL_datum *initial_datum,
                                           int initial_dno,
                                           int lineno, int location);
static	void			 check_sql_expr(const char *stmt, int location,
                                        int leaderlen);
static	void			 plpgsql_sql_error_callback(void *arg);
static	PLpgSQL_type	*parse_datatype(const char *string, int location);
static	void			 check_labels(const char *start_label,
                                      const char *end_label,
                                      int end_location);
static	PLpgSQL_expr	*read_cursor_args(PLpgSQL_var *cursor,
                                          int until, const char *expected);
static	List			*read_raise_options(void);
static void read_signal_sqlstate(PLpgSQL_stmt_signal *newp, int tok);
static void read_signal_condname(PLpgSQL_stmt_signal *newp, int tok);
static void read_signal_set(PLpgSQL_stmt_signal *newp, int tok);
static List *read_signal_items(void);
static  int             errstate = ERROR;
static DefElem* get_proc_str(int tok);
static char* get_init_proc(int tok);
static char* get_attrname(int tok);
static AttrNumber get_assign_attrno(PLpgSQL_datum* target,  char* attrname);
static void raw_parse_package_function(char* proc_str, int location, int leaderlen);
static void checkFuncName(List* funcname);
static void checkTypeName(List* nest_typnames, List* target_typnames);
static List* get_current_type_nest_type(List* old_nest_typenames, char* typname, bool add_current_type, bool is_varray = false);
static void IsInPublicNamespace(char* varname);
static void CheckDuplicateCondition (char* name);
static void SetErrorState();
static bool oid_is_function(Oid funcid, bool* isSystemObj);
static void AddNamespaceIfNeed(int dno, char* ident);
static void AddNamespaceIfPkgVar(const char* ident, IdentifierLookup save_IdentifierLookup);
bool pltsql_is_token_keyword(int tok);
static void check_bulk_into_type(PLpgSQL_row* row);
static void check_table_index(PLpgSQL_datum* datum, char* funcName);
static PLpgSQL_type* build_type_from_record_var(int dno, int location, bool for_proc = false);
static PLpgSQL_type * build_array_type_from_elemtype(PLpgSQL_type *elem_type);
static PLpgSQL_var* plpgsql_build_nested_variable(PLpgSQL_var *nest_table, bool isconst, char* name, int lineno);
static void read_multiset(StringInfoData* ds, char* tableName1, Oid typeOid1);
static void CastTypeVariableNameToString(StringInfoData* ds, List* idents, bool needDot = true);
static Oid get_table_type(PLpgSQL_datum* datum);
static Node* make_columnDef_from_attr(PLpgSQL_rec_attr* attr);
static TypeName* make_typename_from_datatype(PLpgSQL_type* datatype);
static Oid plpgsql_build_package_record_type(const char* typname, List* list, bool add2namespace);
static void  plpgsql_build_package_array_type(const char* typname, Oid elemtypoid, char arraytype, TypeDependExtend* dependExtend = NULL);
static void plpgsql_build_func_array_type(const char* typname,Oid elemtypoid, char arraytype, int32 atttypmod, TypeDependExtend* dependExtend = NULL);
static void plpgsql_build_package_refcursor_type(const char* typname);
static Oid plpgsql_build_anonymous_subtype(char* typname, PLpgSQL_type* newp, const List* RangeList, bool isNotNull);
static Oid plpgsql_build_function_package_subtype(char* typname, PLpgSQL_type* newp, const List* RangeList, bool isNotNull);
int pltsql_yylex_single(void);
static void get_datum_tok_type(PLpgSQL_datum* target, int* tok_flag);
static bool copy_table_var_indents(char* tableName, char* idents, int tablevar_namelen);
static int push_back_token_stack(DList* tokenstack);
static int read_assignlist(bool is_push_back, int* token);
static void plpgsql_cast_reference_list(List* idents, StringInfoData* ds, bool isPkgVar);
static bool PkgVarNeedCast(List* idents);
static void CastArrayNameToArrayFunc(StringInfoData* ds, List* idents, bool needDot = true);
static Oid get_table_index_type(PLpgSQL_datum* datum, int *tableof_func_dno);
static int get_nest_tableof_layer(PLpgSQL_var *var, const char *typname, int errstate);
static void SetErrorState();
static void CheckDuplicateFunctionName(List* funcNameList);
static void check_autonomous_nest_tablevar(PLpgSQL_var* var);
static bool is_unreserved_keyword_func(const char *name);
static PLpgSQL_stmt *funcname_is_call(const char* name, int location);
#ifndef ENABLE_MULTIPLE_NODES
static bool checkAllAttrName(TupleDesc tupleDesc);
static void BuildForQueryVariable(PLpgSQL_expr* expr, PLpgSQL_row **row, PLpgSQL_rec **rec,
    const char* refname, int lineno);
#endif
static Oid createCompositeTypeForCursor(PLpgSQL_var* var, PLpgSQL_expr* expr);
static void check_record_type(PLpgSQL_rec_type * var_type, int location, bool check_nested = true);
static void check_record_nest_tableof_index(PLpgSQL_datum* datum);
static void check_tableofindex_args(int tableof_var_dno, Oid argtype);
static bool need_build_row_for_func_arg(PLpgSQL_rec **rec, PLpgSQL_row **row, int out_arg_num, int all_arg, int *varnos, char *p_argmodes);
static void processFunctionRecordOutParam(int varno, Oid funcoid, int* outparam);
static void yylex_object_type_selfparam(char** fieldnames,
                int *varnos,
                int nfields,
                PLpgSQL_row **row,
                PLpgSQL_rec **rec,
                int *retvarno,
                PLwdatum *objtypwdatum,
                bool overload = false);
static void CheckParallelCursorOpr(PLpgSQL_stmt_fetch* fetch);
static void HandleSubprogram();
static void HandleBlockLevel();

#line 370 "src/pltsql/pl_gram.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "pl_gram.hpp".  */
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
#line 300 "src/pltsql/gram.y" /* yacc.c:355  */

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

#line 706 "src/pltsql/pl_gram.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */

#line 737 "src/pltsql/pl_gram.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1705

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  235
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  155
/* YYNRULES -- Number of rules.  */
#define YYNRULES  506
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  742

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   479

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   225,     2,     2,     2,     2,
     228,   229,     2,     2,   227,   230,   232,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   234,   226,
       2,   231,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   233,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   649,   649,   650,   651,   653,   653,   676,   676,   698,
     698,   704,   705,   708,   712,   716,   720,   726,   727,   731,
     730,   780,   785,   800,   807,   818,   817,   859,   893,   912,
     914,   917,   926,   932,   937,   972,   977,   982,   987,  1021,
    1029,  1037,  1046,  1055,  1064,  1073,  1085,  1099,  1134,  1170,
    1179,  1187,  1187,  1198,  1211,  1212,  1215,  1216,  1220,  1235,
    1236,  1239,  1368,  1384,  1383,  1418,  1430,  1454,  1480,  1529,
    1572,  1592,  1604,  1616,  1657,  1711,  1730,  1783,  1795,  1807,
    1819,  1831,  1902,  1928,  1998,  2040,  2075,  2105,  2105,  2119,
    2119,  2133,  2156,  2177,  2213,  2243,  2265,  2276,  2283,  2287,
    2294,  2298,  2304,  2310,  2318,  2326,  2336,  2385,  2446,  2457,
    2510,  2567,  2570,  2574,  2581,  2594,  2597,  2635,  2639,  2645,
    2677,  2678,  2678,  2683,  2686,  2704,  2719,  2720,  2722,  2740,
    2779,  2795,  2811,  2824,  2837,  2850,  2863,  2879,  2897,  2898,
    2910,  2911,  2916,  2928,  2929,  2934,  2941,  2942,  2946,  2948,
    2955,  2958,  2968,  2969,  2972,  2973,  2977,  2978,  2982,  2989,
    3002,  3004,  3006,  3008,  3010,  3012,  3014,  3016,  3018,  3020,
    3022,  3028,  3031,  3039,  3049,  3051,  3053,  3055,  3057,  3059,
    3061,  3063,  3065,  3067,  3069,  3071,  3073,  3075,  3077,  3079,
    3081,  3085,  3085,  3106,  3129,  3135,  3213,  3235,  3260,  3263,
    3267,  3273,  3277,  3283,  3294,  3311,  3330,  3334,  3338,  3342,
    3346,  3350,  3354,  3361,  3368,  3377,  3392,  3397,  3405,  3422,
    3423,  3424,  3425,  3430,  3431,  3432,  3433,  3437,  3441,  3445,
    3452,  3463,  3477,  3478,  3479,  3480,  3481,  3482,  3483,  3484,
    3485,  3486,  3487,  3488,  3489,  3492,  3499,  3506,  3514,  3545,
    3576,  3608,  3655,  3672,  3673,  3675,  3679,  3686,  3696,  3701,
    3714,  3720,  3738,  3741,  3755,  3758,  3764,  3771,  3785,  3789,
    3795,  3808,  3811,  3826,  3845,  3881,  3882,  3884,  3915,  3958,
    3998,  4041,  4070,  4109,  4110,  4113,  4114,  4117,  4159,  4192,
    4526,  4634,  4648,  4669,  4712,  4748,  4787,  4802,  4809,  4862,
    4865,  4870,  4872,  4876,  4892,  4896,  4900,  4912,  4926,  4954,
    5024,  5202,  5244,  5298,  5306,  5314,  5323,  5336,  5340,  5344,
    5348,  5357,  5365,  5373,  5377,  5381,  5455,  5506,  5557,  5618,
    5660,  5694,  5762,  5830,  6016,  6098,  6202,  6239,  6277,  6294,
    6299,  6303,  6310,  6325,  6339,  6353,  6370,  6377,  6381,  6388,
    6391,  6397,  6427,  6442,  6459,  6494,  6540,  6545,  6553,  6555,
    6554,  6576,  6580,  6586,  6599,  6608,  6614,  6684,  6800,  6804,
    6808,  6812,  6816,  6829,  6833,  6841,  6844,  6850,  6852,  6859,
    6863,  6877,  6878,  6879,  6880,  6881,  6882,  6883,  6884,  6885,
    6886,  6887,  6888,  6889,  6890,  6891,  6892,  6893,  6894,  6895,
    6896,  6897,  6898,  6899,  6900,  6901,  6902,  6903,  6904,  6905,
    6906,  6907,  6908,  6909,  6910,  6911,  6912,  6913,  6914,  6915,
    6916,  6917,  6918,  6919,  6920,  6921,  6922,  6925,  6926,  6927,
    6928,  6929,  6930,  6931,  6932,  6933,  6934,  6935,  6936,  6937,
    6938,  6939,  6940,  6941,  6942,  6943,  6944,  6945,  6946,  6947,
    6948,  6949,  6950,  6951,  6952,  6953,  6954,  6955,  6956,  6957,
    6958,  6959,  6960,  6961,  6962,  6963,  6964,  6965,  6966,  6967,
    6968,  6969,  6970,  6971,  6972,  6973,  6974,  6975,  6976,  6977,
    6978,  6979,  6980,  6981,  6982,  6983,  6984,  6985,  6986,  6987,
    6988,  6989,  6990,  6991,  6992,  6993,  6994,  6995,  6996,  6997,
    6998,  6999,  7000,  7001,  7002,  7003,  7004
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "FCONST", "SCONST", "BCONST",
  "VCONST", "XCONST", "Op", "CmpOp", "CmpNullOp", "COMMENTSTRING",
  "SET_USER_IDENT", "SET_IDENT", "UNDERSCORE_CHARSET", "FCONST_F",
  "FCONST_D", "ICONST", "PARAM", "TYPECAST", "ORA_JOINOP", "DOT_DOT",
  "COLON_EQUALS", "PARA_EQUALS", "SET_IDENT_SESSION", "SET_IDENT_GLOBAL",
  "DIALECT_TSQL", "T_WORD", "T_CWORD", "T_DATUM", "T_PLACEHOLDER",
  "T_LABELLOOP", "T_LABELWHILE", "T_LABELREPEAT", "T_VARRAY",
  "T_ARRAY_FIRST", "T_ARRAY_LAST", "T_ARRAY_COUNT", "T_ARRAY_EXISTS",
  "T_ARRAY_PRIOR", "T_ARRAY_NEXT", "T_ARRAY_DELETE", "T_ARRAY_EXTEND",
  "T_ARRAY_TRIM", "T_VARRAY_VAR", "T_RECORD", "T_TABLE", "T_TABLE_VAR",
  "T_PACKAGE_VARIABLE", "T_OBJECT_TYPE_VAR_METHOD",
  "T_PACKAGE_CURSOR_ISOPEN", "T_PACKAGE_CURSOR_FOUND",
  "T_PACKAGE_CURSOR_NOTFOUND", "T_PACKAGE_CURSOR_ROWCOUNT", "LESS_LESS",
  "GREATER_GREATER", "T_REFCURSOR", "T_SQL_ISOPEN", "T_SQL_FOUND",
  "T_SQL_NOTFOUND", "T_SQL_ROWCOUNT", "T_SQL_BULK_EXCEPTIONS",
  "T_CURSOR_ISOPEN", "T_CURSOR_FOUND", "T_CURSOR_NOTFOUND",
  "T_CURSOR_ROWCOUNT", "T_DECLARE_CURSOR", "T_DECLARE_CONDITION",
  "T_DECLARE_HANDLER", "K_ABSOLUTE", "K_ALIAS", "K_ALL", "K_ALTER",
  "K_ARRAY", "K_AS", "K_BACKWARD", "K_BEGIN", "K_BULK", "K_BY", "K_CALL",
  "K_CASE", "K_CATALOG_NAME", "K_CLASS_ORIGIN", "K_CLOSE", "K_COLLATE",
  "K_COLLECT", "K_COLUMN_NAME", "K_COMMIT", "K_CONDITION", "K_CONSTANT",
  "K_CONSTRAINT_CATALOG", "K_CONSTRAINT_NAME", "K_CONSTRAINT_SCHEMA",
  "K_CONTINUE", "K_CURRENT", "K_CURSOR", "K_CURSOR_NAME", "K_DEBUG",
  "K_DECLARE", "K_DEFAULT", "K_DELETE", "K_DETAIL", "K_DETERMINISTIC",
  "K_DIAGNOSTICS", "K_DISTINCT", "K_DO", "K_DUMP", "K_ELSE", "K_ELSIF",
  "K_END", "K_ERRCODE", "K_ERROR", "K_EXCEPT", "K_EXCEPTION",
  "K_EXCEPTIONS", "K_EXECUTE", "K_EXIT", "K_FALSE", "K_FETCH", "K_FIRST",
  "K_FOR", "K_FORALL", "K_FOREACH", "K_FORWARD", "K_FOUND", "K_FROM",
  "K_FUNCTION", "K_GET", "K_GOTO", "K_HANDLER", "K_HINT", "K_IF",
  "K_IMMEDIATE", "K_INSTANTIATION", "K_IN", "K_INDEX", "K_INFO",
  "K_INSERT", "K_INTERSECT", "K_INTO", "K_IS", "K_ITERATE", "K_LAST",
  "K_LEAVE", "K_LIMIT", "K_LOG", "K_LOOP", "K_MERGE", "K_MESSAGE",
  "K_MESSAGE_TEXT", "K_MOVE", "K_MULTISET", "K_MULTISETS", "K_MYSQL_ERRNO",
  "K_NUMBER", "K_NEXT", "K_NO", "K_NOT", "K_NOTICE", "K_NULL", "K_OF",
  "K_OPEN", "K_OPTION", "K_OR", "K_OUT", "K_PACKAGE", "K_PERFORM",
  "K_PIPE", "K_PG_EXCEPTION_CONTEXT", "K_PG_EXCEPTION_DETAIL",
  "K_PG_EXCEPTION_HINT", "K_PRAGMA", "K_PRIOR", "K_PROCEDURE", "K_QUERY",
  "K_RAISE", "K_RECORD", "K_REF", "K_RELATIVE", "K_RELEASE", "K_REPEAT",
  "K_REPLACE", "K_RESULT_OID", "K_RESIGNAL", "K_RETURN",
  "K_RETURNED_SQLSTATE", "K_REVERSE", "K_ROLLBACK", "K_ROW", "K_ROWTYPE",
  "K_ROW_COUNT", "K_SAVE", "K_SAVEPOINT", "K_SCHEMA_NAME", "K_SELECT",
  "K_SCROLL", "K_SIGNAL", "K_SLICE", "K_SQLEXCEPTION", "K_SQLSTATE",
  "K_SQLWARNING", "K_STACKED", "K_STRICT", "K_SUBCLASS_ORIGIN",
  "K_SUBTYPE", "K_SYS_REFCURSOR", "K_TABLE", "K_TABLE_NAME", "K_THEN",
  "K_TO", "K_TRUE", "K_TYPE", "K_UNION", "K_UNTIL", "K_UPDATE",
  "K_USE_COLUMN", "K_USE_VARIABLE", "K_USING", "K_VARIABLE_CONFLICT",
  "K_VARRAY", "K_WARNING", "K_WHEN", "K_WHILE", "K_WITH", "'#'", "';'",
  "','", "'('", "')'", "'-'", "'='", "'.'", "'['", "':'", "$accept",
  "pl_body", "pl_package_spec", "$@1", "pl_package_init", "$@2",
  "pl_function", "$@3", "comp_options", "comp_option", "opt_semi",
  "pl_block", "$@4", "declare_sect_b", "declare_stmts", "declare_stmt",
  "$@5", "handler_type", "cond_list", "cond_element", "declare_condname",
  "condition_value", "decl_sect", "$@6", "decl_start", "decl_stmts",
  "decl_stmt", "as_is", "decl_statement", "$@7", "$@8", "$@9",
  "subtype_base_type", "subtype_base_collect_type", "error_code",
  "spec_proc", "init_proc", "record_attr_list", "record_attr",
  "opt_scrollable", "decl_cursor_query", "decl_cursor_args",
  "decl_cursor_arglist", "decl_cursor_arg", "cursor_in_out_option",
  "opt_cursor_returntype", "decl_is_for", "decl_aliasitem", "decl_varname",
  "decl_varname_list", "decl_const", "decl_datatype", "decl_collate",
  "decl_notnull", "decl_defval", "decl_rec_defval", "decl_defkey",
  "assign_operator", "proc_sect", "proc_stmts", "proc_stmt",
  "goto_block_label", "label_stmts", "label_stmt", "stmt_perform", "$@10",
  "stmt_assign", "stmt_getdiag", "getdiag_area_opt", "getdiag_list",
  "getdiag_list_item", "getdiag_item", "statement_information_item_name",
  "getdiag_target", "condition_number_item", "condition_number",
  "condition_information", "condition_information_item",
  "condition_information_item_name", "table_var", "varray_var",
  "record_var", "assign_var", "assign_list", "assign_el", "attr_name",
  "stmt_goto", "label_name", "stmt_if", "stmt_elsifs", "stmt_else",
  "stmt_case", "opt_expr_until_when", "case_when_list", "case_when",
  "opt_case_else", "stmt_loop", "label_loop", "stmt_while", "label_while",
  "label_repeat", "stmt_for", "for_control", "forall_control",
  "opt_save_exceptions", "for_variable", "stmt_foreach_a", "foreach_slice",
  "forall_body", "stmt_exit", "exit_type", "stmt_return", "stmt_pipe_row",
  "stmt_raise", "stmt_signal", "stmt_resignal", "loop_body", "while_body",
  "repeat_condition", "repeat_condition_expr", "stmt_execsql",
  "stmt_dynexecute", "stmt_open", "stmt_fetch", "stmt_move",
  "opt_fetch_direction", "fetch_limit_expr", "fetch_into_target",
  "stmt_close", "stmt_null", "stmt_commit", "savepoint_name",
  "opt_savepoint_name", "opt_rollback_to", "stmt_rollback",
  "stmt_savepoint", "cursor_variable", "exception_sect", "@11",
  "proc_exceptions", "proc_exception", "proc_conditions", "proc_condition",
  "expr_until_semi", "expr_until_rightbracket", "expr_until_parenthesis",
  "expr_until_then", "expr_until_loop", "expr_until_while_loop",
  "opt_block_label", "opt_label", "opt_exitcond", "any_identifier",
  "unreserved_keyword_func", "unreserved_keyword", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,    35,    59,    44,    40,    41,
      45,    61,    46,    91,    58
};
# endif

#define YYPACT_NINF -455

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-455)))

#define YYTABLE_NINF -375

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -62,  -455,  -455,    62,  -455,  -455,  -455,  -171,  -455,    18,
    -455,   -95,    18,  -455,  -455,    51,   -21,    -4,     6,   -80,
     -66,    88,  -455,  -455,   124,  -455,  -455,   -16,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,   757,   192,  -455,  -455,
    -455,  -455,  -455,  -455,    51,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,   940,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,    51,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,   940,  -455,  -455,  -455,   940,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,   757,  -455,  -455,  -455,   -27,  -455,   940,
    1123,   -11,   504,   192,  -455,   159,   -91,  -455,  -112,  -455,
      95,   -23,  -455,  -455,    13,   141,  -455,    38,   940,   152,
     -10,   179,  -455,  -455,  -455,  -455,  -455,  -455,  -455,   198,
    -455,  -455,  -455,   168,  -455,  -455,  -455,    51,  -455,  -455,
    -101,    64,   189,   274,  -455,  1320,  -455,  -455,   535,  -455,
    -455,   587,  -455,  -455,  -455,  -455,  -455,  -455,  -455,    86,
    -455,  -455,   108,  -455,  -455,  -455,  -455,    51,  -455,   126,
    -455,  -455,   -29,  -455,   161,  -455,  -455,  -455,  -455,   227,
     -10,   -10,  -455,  -455,  -455,  -455,  -455,  -455,  -455,   -91,
     193,   194,   260,  -455,  -455,  -455,  -455,  -455,   207,  -455,
    -455,    -9,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,    91,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,   -59,   296,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,   100,    91,  -455,  -455,  -455,   136,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,    -1,  -455,
    -455,  -455,  -455,    51,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,   218,  -455,   535,   123,   115,
     265,   265,   265,   535,     3,   587,   133,    51,  -455,  -455,
     119,   121,  -455,   191,   127,  -455,  -455,  -455,   128,   129,
     256,   195,   130,  -455,  -455,   134,   166,   191,  -455,  -455,
    -455,     2,    82,   -30,   137,    51,  -455,  -455,  -455,   -76,
     -76,   -76,   -76,   -76,   139,  -455,  -455,  -455,  -455,   138,
      91,  -455,  -455,   251,   140,  -455,   382,    91,  -455,  -455,
    -455,   135,  -455,   -67,  -455,   221,   255,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,   535,   234,   178,   236,
     174,  -455,    51,  -455,   149,  -455,   162,     3,   151,   153,
     535,   940,   -39,    -7,   225,   155,  -455,  -455,  -455,   201,
     206,   360,  -455,  -455,  -455,   -10,   119,  -455,  -455,   163,
    -455,   262,  -455,  -455,  -455,   555,  -455,  -455,    51,   137,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,   -84,  -455,
    -455,   -25,    46,  -455,  -455,   164,  -455,  -455,  -455,  -455,
    -455,  -455,    51,   -82,  -455,   210,  -455,  -455,   277,  1481,
    -455,   375,   259,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,   -43,  -455,   -58,  -455,  -455,  -455,  -455,   380,   170,
    -455,  -455,    16,  -455,   245,   354,   176,   191,   191,   191,
     191,   191,   172,  -455,   -39,  -455,  -455,    82,  -455,  -108,
    -455,  -455,  -455,  -455,  -455,  -455,   418,   535,  -455,   294,
     322,  -455,   180,  -455,  -455,  -455,    21,    28,  -455,     0,
     160,  -455,  -455,   183,    51,    51,  -455,    -8,  -455,  -455,
    -455,  -455,   336,  -455,   940,  -455,  -455,  -455,  -455,  -455,
    -455,   186,  -455,   187,   191,   191,   191,   191,   191,  -455,
    -455,  -455,   -99,   -98,   -96,   -94,   -93,   257,  -455,  -455,
      51,    54,  -455,  -455,   338,   280,   -88,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,   116,   116,  -455,
     175,   268,    87,   535,  -455,   314,  -455,   199,   200,  -455,
    -455,    -2,  -455,  -455,  -455,    -2,    -2,    -2,    -2,    -2,
     348,  -455,   349,  -455,   350,  -455,   351,  -455,   352,  -455,
     254,  -455,  -455,  -455,   208,  -455,  -455,  -455,  -455,   202,
     204,    45,  -455,    58,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,   382,   300,  -455,  -455,   535,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,   212,   214,   215,   216,   217,  -455,  -100,   253,
     253,  -455,   116,  -455,  -455,   219,  -455,   230,   237,   238,
     239,   240,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,  -455
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      11,     7,     5,     0,     2,     4,     3,     9,   103,   373,
       1,     0,   373,    12,     8,     0,     0,    49,     0,     0,
      17,     0,   379,   380,     0,     6,    53,    51,    13,    14,
      16,    15,    18,    10,    19,   374,     0,    21,   130,   132,
     137,   133,   134,   136,     0,   135,   427,   428,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,     0,   444,   445,    57,   446,   447,   448,
     449,   450,   451,   452,   453,   454,    87,   455,   472,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,   466,
     467,   468,   469,   470,   471,   476,   473,   474,   475,     0,
     477,    89,   478,   479,   480,   482,   481,   483,   484,   485,
     486,   488,   487,   489,   490,   491,   492,   493,   494,   495,
     496,     0,   497,   498,   499,     0,   500,   501,   502,   503,
     504,   505,   506,    52,    55,    56,   138,   140,   131,     0,
       0,     0,   171,    22,    24,     0,   111,   102,     0,   102,
       0,     0,    54,   247,     0,     0,   141,     0,     0,   142,
       0,     0,    39,    41,    46,    42,    43,    45,    44,     0,
      40,    30,    29,     0,   275,   283,   285,     0,   312,   311,
       0,     0,   358,   171,   159,     0,   167,   161,   171,   162,
     372,   171,   163,   164,   169,   170,   165,   166,   168,    49,
      23,    58,     0,   113,    63,    88,    91,     0,    90,   142,
      60,    59,     0,    66,     0,    67,   139,   246,   245,   143,
       0,     0,   155,   153,   148,   154,    86,   149,   152,   111,
       0,     0,     0,   276,   286,   284,   160,   359,     0,   158,
     326,     0,   253,   333,   332,   331,   253,   253,   253,   253,
     328,   381,   382,   317,   383,   325,   267,     0,   384,   305,
     385,   386,   322,   387,   388,   389,   390,   391,   334,   304,
     339,   392,   393,   198,     0,   394,   370,   407,   395,   396,
     318,   307,   397,   306,   398,   324,   399,   400,   339,   401,
     402,   403,   404,     0,     0,   405,   406,   191,     0,   408,
     409,   410,   411,   310,   412,   413,   319,   414,   308,   415,
     416,   417,   320,   418,   419,   420,   421,   321,   422,   423,
     424,   425,   426,   323,   173,   182,   174,   183,     0,   184,
     175,   176,   177,   375,   178,   190,   179,   180,   181,   185,
     186,   187,   188,   189,   327,     0,   274,   171,     0,     0,
       0,     0,     0,   171,     0,   171,   349,     0,   372,   112,
     115,     0,    96,   146,     0,    97,    99,    98,     0,     0,
       0,     0,     0,   128,   129,     0,     0,   146,    84,    73,
      25,     0,     0,   172,     0,   375,   330,   329,   367,   251,
     249,   248,   250,   252,     0,   356,   357,   354,   355,     0,
       0,   199,   200,     0,     0,   259,   171,     0,   344,   335,
     367,     0,   367,     0,   376,     0,     0,   278,   280,   316,
     345,   296,   297,   293,   294,   295,   171,     0,   292,     0,
     299,   273,     0,   348,     0,   346,     0,     0,     0,     0,
     171,     0,     0,     0,     0,     0,    95,    94,   258,   123,
     142,     0,    62,   144,   145,     0,   115,    48,    47,     0,
      38,     0,    37,    34,    35,   171,    32,    33,     0,   360,
     362,    20,   194,   369,   258,   368,   254,   370,   271,   269,
     343,     0,     0,   260,   262,     0,   192,   309,   193,   367,
     377,   303,   375,     0,   282,     0,   287,   289,     0,     0,
     290,     0,     0,   347,   353,   316,   350,   351,   352,   277,
     279,     0,   117,   122,   127,   126,   114,   100,     0,     0,
     147,    93,     0,   104,   142,   142,     0,   146,   146,   146,
     146,   146,     0,    61,     0,    27,    36,     0,    28,     0,
     365,   366,   361,   256,   255,   257,   171,   171,   268,     0,
       0,   336,     0,   216,   217,   215,     0,     0,   202,     0,
     264,   338,   378,     0,   375,   375,   291,     0,   288,   302,
     301,   300,     0,   281,     0,   116,   120,   121,   142,    64,
     101,     0,   258,     0,   146,   146,   146,   146,   146,   124,
     125,    65,     0,     0,     0,     0,     0,     0,   114,    31,
       0,   171,   270,   272,     0,     0,     0,   220,   221,   222,
     226,   219,   227,   218,   224,   225,   223,     0,     0,   195,
       0,   154,     0,   171,   370,     0,   313,     0,     0,   371,
     118,   150,    92,   105,    85,   150,   150,   150,   150,   150,
       0,    78,     0,    74,     0,    75,     0,    77,     0,    76,
     142,    26,   364,   363,     0,   342,   213,   214,   205,     0,
       0,     0,   229,     0,   201,   204,   211,   209,   208,   210,
     207,   212,   206,   203,   265,   171,     0,   314,   315,   171,
     119,   151,   108,   106,   110,   109,   107,   142,   142,   142,
     142,   142,     0,     0,     0,     0,     0,   266,     0,     0,
       0,   196,     0,   197,   263,     0,   298,     0,     0,     0,
       0,     0,    72,    68,    71,    70,    69,   340,   341,   337,
     237,   232,   240,   234,   236,   235,   241,   242,   243,   244,
     238,   233,   239,   231,   230,   228,   261,    80,    81,    82,
      79,    83
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -455,   424,  -455,  -455,  -455,   325,  -455,  -455,  -455,   -68,
    -455,  -455,   463,  -455,  -455,  -455,   341,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,   326,  -455,  -455,  -106,   248,
    -120,    23,  -455,   -85,  -455,  -455,   -54,  -455,   -61,  -455,
    -455,  -204,  -455,  -360,  -205,  -305,  -150,  -232,  -141,  -455,
    -179,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,  -455,
    -127,  -455,  -455,  -454,  -455,  -455,  -124,  -203,  -200,  -202,
    -201,  -206,  -455,   -36,  -455,    24,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,    29,  -455,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,   -55,  -455,  -455,  -455,  -455,
    -455,  -455,  -455,  -455,  -455,  -455,  -341,    61,    -3,  -455,
      10,    11,  -455,  -455,  -455,   223,  -455,  -455,  -455,  -455,
    -455,  -309,    75,  -455,  -455,  -455,  -253,  -455,  -455,  -455,
      47,  -455,   -86,  -355,  -455,  -455,  -442,  -455,   157,    99,
    -367,  -455,   -15,  -455,   378
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     9,     5,     8,     6,    12,     7,    13,
      33,   181,    37,   142,   143,   144,   456,   173,   465,   466,
     169,   459,    21,    36,    27,   133,   134,   212,   135,   360,
     147,   149,   363,   364,   519,   205,    14,   522,   523,   204,
     579,   442,   511,   512,   578,   526,   516,   375,   136,   137,
     159,   219,   377,   445,   226,   680,   681,   228,   345,   183,
     184,   185,   186,   324,   325,   410,   326,   327,   403,   557,
     558,   673,   658,   660,   617,   618,   661,   662,   733,   220,
     221,   160,   328,   389,   476,   524,   329,   405,   330,   560,
     625,   331,   394,   478,   479,   549,   187,   188,   189,   190,
     191,   192,   426,   428,   499,   427,   193,   502,   568,   332,
     333,   334,   335,   336,   194,   195,   346,   418,   494,   495,
     337,   338,   339,   340,   341,   400,   719,   698,   342,   343,
     196,   433,   434,   438,   197,   198,   399,   238,   384,   469,
     470,   539,   540,   472,   545,   543,   406,   679,   347,   199,
     413,   491,   414,   344,   138
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      24,   182,   146,   368,   239,   365,   417,   366,   367,   388,
     227,   517,   431,   222,   222,   378,   379,   455,   471,   153,
     457,   222,   222,   222,   547,   607,   608,   609,   559,   145,
     154,    22,    29,    23,   610,   546,   401,   640,   642,   611,
     644,   409,   646,   648,   155,   717,   233,  -374,   439,   612,
     348,   613,   210,   550,    11,   486,   600,   488,  -374,   552,
     150,   -50,    10,   156,   151,   492,   202,   656,    18,  -374,
     227,   227,     1,    15,   553,   554,   555,   576,   161,    22,
     234,    23,   514,   171,   148,   496,   174,   175,   176,    25,
     223,  -374,  -374,  -374,   -50,    26,   412,   216,   223,   509,
     460,   601,   515,   657,     2,   203,   172,   577,    17,   177,
      22,    17,    23,    28,   206,   551,   207,  -374,   211,   395,
     396,   397,   235,   503,    19,   563,   718,   641,   643,   659,
     645,  -373,   647,   649,   562,   556,    30,    31,   477,   614,
     398,   564,  -373,   402,   553,   554,   555,   481,   369,   370,
    -374,  -374,   473,  -373,   485,   489,   474,   475,  -374,   490,
      32,   217,   232,  -374,  -156,    34,   559,   592,   593,   594,
     595,   596,   153,   218,   349,  -373,  -373,  -373,   371,   157,
      35,   615,   675,   362,   574,    26,   575,   217,   552,   373,
     374,   372,   361,  -374,   453,   454,   432,   627,   628,   218,
     158,  -373,   458,   553,   554,   555,   416,   350,   351,   352,
     390,   391,   392,   393,   436,   201,   224,   386,   386,   387,
     387,   225,   225,   518,   635,   636,   637,   638,   639,   225,
     225,   621,   616,   353,  -373,  -373,   209,   666,   178,   213,
     461,   217,  -373,   582,   531,   583,   528,  -373,   529,   530,
     533,   179,   153,   218,   619,   620,   667,   668,   669,   139,
     140,   141,   214,   527,   215,   484,   354,   355,   623,   624,
     670,   701,   702,   671,   356,   229,  -156,  -373,   672,   357,
     217,   462,   463,   464,   703,   702,   538,   230,   180,   217,
     236,   153,   218,   421,   422,   423,   429,   430,   231,   416,
     153,   218,   584,   237,   359,   227,   174,   175,   176,   358,
     424,   692,   376,   425,   381,   382,   383,   385,   588,   590,
     585,   589,   586,   587,   404,   411,   408,   622,   415,   177,
     682,   683,   684,   685,   686,   720,   721,   419,   706,   435,
     722,   420,   435,   437,   723,   724,   725,   441,   443,   444,
     726,  -373,   449,   446,   447,   482,   450,   448,   451,   468,
     452,   477,  -373,   487,   480,   493,   483,   467,   492,   497,
     498,   500,   501,  -373,   631,   504,   505,   507,   532,   508,
     513,   521,  -157,  -157,  -157,   520,   525,   536,  -157,   535,
     561,   565,   566,   571,   572,  -373,  -373,  -373,   580,   581,
     153,   597,   591,   727,   604,   602,   603,   728,   605,   626,
     629,   606,   632,   634,   174,   175,   176,   435,   650,   654,
     655,  -373,   435,   665,   676,   677,   678,   687,   688,   689,
     690,   691,   705,   699,   697,   700,    20,   177,   712,   729,
     713,   714,   715,   716,   696,   736,   693,   730,   694,   695,
     174,   175,   176,   541,  -373,  -373,   737,   731,   178,  -373,
     653,   732,  -373,   738,   739,   740,   741,  -373,   200,   599,
    -373,   179,    16,   177,   152,   208,   633,   380,   651,   534,
     598,  -373,   674,   707,   708,   709,   710,   711,  -157,   630,
    -156,  -156,  -156,   664,   663,  -373,  -157,  -373,   544,   735,
     734,   510,   573,  -373,  -373,  -373,  -373,   548,   180,   569,
     570,   407,   506,   513,   652,   440,   542,  -373,   170,     0,
       0,     0,   467,     0,     0,     0,  -156,     0,  -156,  -373,
       0,     0,     0,     0,   704,     0,   174,   175,   176,  -373,
    -373,  -373,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
       0,     0,  -373,  -373,     0,  -373,   178,   174,   175,   176,
    -373,     0,     0,     0,     0,  -373,     0,     0,     0,   179,
       0,  -373,     0,     0,     0,   541,     0,   174,   175,   176,
     177,     0,  -373,     0,     0,     0,     0,     0,  -373,  -373,
       0,     0,   178,  -373,     0,  -373,  -373,     0,     0,     0,
     177,  -373,  -373,     0,  -156,   179,   180,     0,  -156,   174,
     175,   176,     0,  -373,     0,  -373,  -373,  -373,     0,     0,
       0,     0,  -373,     0,  -373,     0,     0,     0,     0,     0,
    -156,  -373,   177,  -373,     0,  -156,     0,     0,     0,     0,
       0,  -373,   180,     0,  -373,     0,  -373,  -373,  -373,     0,
       0,     0,     0,     0,  -373,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -373,  -373,  -373,  -373,     0,
       0,     0,  -373,     0,  -373,  -373,  -373,     0,   178,     0,
       0,     0,  -373,     0,     0,     0,     0,  -373,     0,     0,
       0,   179,  -373,     0,     0,     0,     0,     0,  -373,  -373,
    -373,     0,     0,     0,     0,  -373,  -373,     0,     0,   178,
       0,     0,     0,  -373,     0,     0,     0,  -373,  -373,     0,
       0,     0,   179,     0,  -373,  -373,  -373,     0,   180,   178,
       0,     0,     0,  -373,     0,     0,     0,     0,  -373,     0,
       0,     0,   179,     0,     0,     0,     0,     0,  -373,     0,
       0,     0,     0,     0,     0,     0,     0,  -373,  -373,   180,
       0,   178,     0,     0,     0,  -373,     0,     0,  -373,     0,
    -373,     0,   537,     0,   179,    38,     0,     0,     0,   180,
       0,     0,    39,     0,     0,     0,     0,     0,     0,     0,
       0,  -156,    40,    41,    42,    43,     0,     0,     0,     0,
    -373,     0,    44,     0,    45,     0,     0,     0,     0,     0,
       0,   180,     0,     0,     0,     0,     0,    46,    47,     0,
      48,    49,     0,    50,     0,     0,     0,    51,     0,    52,
      53,     0,     0,     0,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,     0,     0,    67,
       0,     0,    68,     0,    69,     0,     0,     0,    70,    71,
      72,     0,    73,     0,     0,     0,     0,    74,     0,     0,
       0,    75,     0,     0,    76,     0,     0,     0,    77,     0,
       0,    78,     0,    79,    80,     0,    81,     0,    82,     0,
      83,     0,     0,    84,     0,    85,    86,    87,     0,    88,
       0,    89,     0,    90,    91,     0,    92,     0,     0,     0,
      93,     0,     0,    94,     0,    95,    96,    97,    98,    99,
     100,   101,   102,     0,   103,     0,   104,     0,     0,     0,
     105,   106,     0,   107,   108,   109,   110,   111,   112,   113,
       0,   114,     0,   115,   116,   117,     0,   118,     0,   119,
       0,   120,   121,   122,   123,   124,     0,     0,    38,   125,
     126,     0,     0,   127,   128,    39,   129,   130,   131,     0,
       0,   132,     0,     0,     0,    40,    41,    42,    43,     0,
       0,     0,     0,     0,     0,     0,     0,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    47,     0,    48,    49,     0,    50,     0,     0,     0,
      51,     0,    52,    53,     0,     0,     0,    54,    55,    56,
      57,    58,    59,    60,    61,    62,     0,    64,    65,     0,
       0,     0,    67,     0,     0,    68,     0,    69,     0,     0,
       0,    70,    71,    72,     0,    73,     0,     0,     0,     0,
      74,     0,     0,     0,    75,     0,     0,     0,     0,     0,
       0,    77,     0,     0,    78,     0,    79,    80,     0,    81,
       0,    82,     0,    83,     0,     0,    84,     0,    85,    86,
      87,     0,    88,     0,    89,     0,    90,    91,     0,    92,
       0,     0,     0,    93,     0,     0,    94,     0,    95,    96,
      97,    98,     0,   100,     0,   102,     0,   103,     0,   104,
       0,     0,     0,   105,   106,     0,   107,   108,   109,   110,
     111,   112,   113,     0,   114,     0,   115,   116,   117,     0,
     118,     0,   119,     0,   120,     0,   122,   123,   124,     0,
       0,   162,     0,   126,     0,     0,   127,   128,   163,   129,
     130,   131,     0,     0,   132,     0,     0,     0,   164,   165,
     166,   167,     0,     0,     0,     0,     0,     0,     0,     0,
     168,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,     0,    48,    49,     0,    50,
       0,     0,     0,    51,     0,    52,    53,     0,     0,     0,
      54,    55,    56,    57,    58,    59,    60,    61,    62,     0,
      64,    65,     0,     0,     0,    67,     0,     0,    68,     0,
      69,     0,     0,     0,    70,    71,    72,     0,    73,     0,
       0,     0,     0,    74,     0,     0,     0,    75,     0,     0,
       0,     0,     0,     0,    77,     0,     0,    78,     0,    79,
      80,     0,    81,     0,    82,     0,    83,     0,     0,    84,
       0,    85,    86,    87,     0,    88,     0,    89,     0,    90,
      91,     0,    92,     0,     0,     0,    93,     0,     0,    94,
       0,    95,    96,    97,    98,     0,   100,     0,   102,     0,
     103,     0,   104,     0,     0,     0,   105,   106,     0,   107,
     108,   109,   110,   111,   112,   113,     0,   114,     0,   115,
     116,   117,     0,   118,     0,   119,     0,   120,     0,   122,
     123,   124,     0,     0,     0,     0,   126,     0,     0,   127,
     128,     0,   129,   130,   131,     0,     0,   132,   240,   241,
     242,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   243,   244,   245,   246,   247,     0,   248,   249,
     250,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     251,   252,     0,   253,     0,     0,   254,     0,     0,     0,
     255,   256,     0,     0,   257,     0,     0,     0,     0,     0,
     258,     0,     0,     0,   259,   260,     0,     0,   261,     0,
       0,   262,   263,     0,     0,     0,     0,   264,     0,     0,
       0,   265,   266,     0,     0,   267,   268,   269,     0,   270,
     271,     0,     0,     0,   272,     0,     0,     0,   273,   274,
       0,   275,   276,     0,   277,     0,   278,   279,   280,     0,
       0,     0,   281,   282,   283,     0,   284,     0,   285,   286,
     287,   288,   289,     0,     0,     0,   290,   291,     0,   292,
     293,     0,   294,   295,     0,     0,   296,   297,   298,   299,
     300,   301,     0,     0,     0,   302,   303,   304,     0,   305,
       0,     0,   306,   307,     0,   308,   309,   310,     0,   240,
     567,   311,     0,     0,     0,   312,   313,     0,   314,     0,
       0,     0,   315,   243,   244,   245,   316,     0,     0,     0,
       0,   250,     0,     0,     0,   317,   318,   319,     0,   320,
     321,   322,     0,     0,   323,     0,     0,     0,     0,     0,
       0,   251,   252,     0,   253,     0,     0,   254,     0,     0,
       0,   255,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   258,     0,     0,     0,     0,   260,     0,     0,   261,
       0,     0,   262,   263,     0,     0,     0,     0,   264,     0,
       0,     0,   265,   266,     0,     0,   267,   268,     0,     0,
       0,   271,     0,     0,     0,   272,     0,     0,     0,     0,
       0,     0,   275,     0,     0,   277,     0,   278,   279,   280,
       0,     0,     0,     0,   282,     0,     0,   284,     0,   285,
     286,   287,     0,   289,     0,     0,     0,   290,   291,     0,
     292,     0,     0,     0,   295,     0,     0,   296,     0,     0,
     299,   300,   301,     0,     0,     0,   302,     0,   304,     0,
     305,     0,     0,   306,   307,     0,     0,   309,   310,     0,
       0,     0,   311,     0,     0,     0,   312,   313,     0,   314,
       0,     0,     0,   315,     0,     0,     0,   316,     0,     0,
       0,     0,     0,     0,     0,     0,   317,   318,   319,     0,
     320,   321,   322,     0,     0,   323
};

static const yytype_int16 yycheck[] =
{
      15,   142,    63,   209,   183,   209,   347,   209,   209,   241,
     160,    18,   353,    23,    23,   220,   221,   377,   385,    46,
      18,    23,    23,    23,   108,     4,     5,     6,   482,    44,
      57,    28,   112,    30,    13,   477,    95,   136,   136,    18,
     136,   294,   136,   136,    71,   145,   147,    77,   357,    28,
     191,    30,    75,    78,   225,   410,   164,   412,    88,    13,
     121,    77,     0,    90,   125,   147,   157,   155,   163,    99,
     220,   221,   134,    55,    28,    29,    30,   135,   139,    28,
     181,    30,   121,    94,    99,   426,    32,    33,    34,   110,
     100,   121,   122,   123,   110,    99,   328,   158,   100,   440,
      18,   209,   141,   191,   166,   196,   117,   165,     9,    55,
      28,    12,    30,   107,   226,   140,   228,   147,   141,    28,
      29,    30,   223,   432,   219,   492,   226,   226,   226,    13,
     226,    77,   226,   226,   489,    89,   216,   217,   222,   118,
      49,   223,    88,   202,    28,    29,    30,   400,   177,   178,
     180,   181,   228,    99,   407,   222,   232,   233,   188,   226,
     226,    35,   177,   193,   110,    77,   620,   527,   528,   529,
     530,   531,    46,    47,    88,   121,   122,   123,   207,   206,
      56,   160,   624,    57,   227,    99,   229,    35,    13,    28,
      29,   220,   207,   223,    28,    29,   193,   564,   565,    47,
     227,   147,   200,    28,    29,    30,   347,   121,   122,   123,
     246,   247,   248,   249,   355,    56,   226,   226,   226,   228,
     228,   231,   231,   230,   584,   585,   586,   587,   588,   231,
     231,   231,   211,   147,   180,   181,   141,   150,   184,   226,
     158,    35,   188,   227,   450,   229,   450,   193,   450,   450,
     455,   197,    46,    47,   226,   227,   169,   170,   171,    67,
      68,    69,   121,    57,   226,   406,   180,   181,   108,   109,
     183,   226,   227,   186,   188,    96,   222,   223,   191,   193,
      35,   199,   200,   201,   226,   227,   465,    89,   234,    35,
     226,    46,    47,    28,    29,    30,   351,   352,   130,   440,
      46,    47,    57,   114,   196,   455,    32,    33,    34,   223,
      45,    57,    85,    48,   121,   121,    56,   110,   524,   525,
     524,   525,   524,   524,    28,   189,   226,   559,   110,    55,
     635,   636,   637,   638,   639,    82,    83,   214,   679,   354,
      87,   226,   357,   210,    91,    92,    93,   228,   227,   158,
      97,    77,    96,   226,   226,   104,   161,   228,   228,   222,
     226,   222,    88,   228,   226,   110,   226,   382,   147,   135,
     192,   135,   198,    99,   578,   226,   214,   226,    18,   226,
     441,   226,   108,   109,   110,   160,   185,   125,   114,   226,
     226,   181,   115,    18,   135,   121,   122,   123,    18,   229,
      46,   229,   226,   150,   110,   546,   547,   154,    86,   226,
      74,   231,   226,   226,    32,    33,    34,   432,   161,    81,
     140,   147,   437,   155,   110,   226,   226,    79,    79,    79,
      79,    79,   132,   231,   226,   231,    12,    55,   226,   186,
     226,   226,   226,   226,   650,   226,   650,   194,   650,   650,
      32,    33,    34,   468,   180,   181,   226,   204,   184,    77,
     601,   208,   188,   226,   226,   226,   226,   193,   143,   537,
      88,   197,     9,    55,   133,   149,   582,   229,   598,   456,
     534,    99,   623,   687,   688,   689,   690,   691,   214,   574,
     108,   109,   110,   620,   618,    77,   222,   223,   474,   702,
     700,   440,   505,   121,   122,   123,    88,   478,   234,   499,
     499,   288,   437,   574,   600,   358,   469,    99,   140,    -1,
      -1,    -1,   537,    -1,    -1,    -1,   108,    -1,   110,   147,
      -1,    -1,    -1,    -1,   675,    -1,    32,    33,    34,   121,
     122,   123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,   180,   181,    -1,   147,   184,    32,    33,    34,
     188,    -1,    -1,    -1,    -1,   193,    -1,    -1,    -1,   197,
      -1,    77,    -1,    -1,    -1,   600,    -1,    32,    33,    34,
      55,    -1,    88,    -1,    -1,    -1,    -1,    -1,   180,   181,
      -1,    -1,   184,    99,    -1,   223,   188,    -1,    -1,    -1,
      55,   193,    77,    -1,   110,   197,   234,    -1,   114,    32,
      33,    34,    -1,    88,    -1,   121,   122,   123,    -1,    -1,
      -1,    -1,    77,    -1,    99,    -1,    -1,    -1,    -1,    -1,
     222,   223,    55,    88,    -1,   110,    -1,    -1,    -1,    -1,
      -1,   147,   234,    -1,    99,    -1,   121,   122,   123,    -1,
      -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    88,   121,   122,   123,    -1,
      -1,    -1,   147,    -1,   180,   181,    99,    -1,   184,    -1,
      -1,    -1,   188,    -1,    -1,    -1,    -1,   193,    -1,    -1,
      -1,   197,   147,    -1,    -1,    -1,    -1,    -1,   121,   122,
     123,    -1,    -1,    -1,    -1,   180,   181,    -1,    -1,   184,
      -1,    -1,    -1,   188,    -1,    -1,    -1,   223,   193,    -1,
      -1,    -1,   197,    -1,   147,   180,   181,    -1,   234,   184,
      -1,    -1,    -1,   188,    -1,    -1,    -1,    -1,   193,    -1,
      -1,    -1,   197,    -1,    -1,    -1,    -1,    -1,   223,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   180,   181,   234,
      -1,   184,    -1,    -1,    -1,   188,    -1,    -1,   223,    -1,
     193,    -1,   227,    -1,   197,    28,    -1,    -1,    -1,   234,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   214,    45,    46,    47,    48,    -1,    -1,    -1,    -1,
     223,    -1,    55,    -1,    57,    -1,    -1,    -1,    -1,    -1,
      -1,   234,    -1,    -1,    -1,    -1,    -1,    70,    71,    -1,
      73,    74,    -1,    76,    -1,    -1,    -1,    80,    -1,    82,
      83,    -1,    -1,    -1,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,   102,
      -1,    -1,   105,    -1,   107,    -1,    -1,    -1,   111,   112,
     113,    -1,   115,    -1,    -1,    -1,    -1,   120,    -1,    -1,
      -1,   124,    -1,    -1,   127,    -1,    -1,    -1,   131,    -1,
      -1,   134,    -1,   136,   137,    -1,   139,    -1,   141,    -1,
     143,    -1,    -1,   146,    -1,   148,   149,   150,    -1,   152,
      -1,   154,    -1,   156,   157,    -1,   159,    -1,    -1,    -1,
     163,    -1,    -1,   166,    -1,   168,   169,   170,   171,   172,
     173,   174,   175,    -1,   177,    -1,   179,    -1,    -1,    -1,
     183,   184,    -1,   186,   187,   188,   189,   190,   191,   192,
      -1,   194,    -1,   196,   197,   198,    -1,   200,    -1,   202,
      -1,   204,   205,   206,   207,   208,    -1,    -1,    28,   212,
     213,    -1,    -1,   216,   217,    35,   219,   220,   221,    -1,
      -1,   224,    -1,    -1,    -1,    45,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    -1,    73,    74,    -1,    76,    -1,    -1,    -1,
      80,    -1,    82,    83,    -1,    -1,    -1,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    -1,    97,    98,    -1,
      -1,    -1,   102,    -1,    -1,   105,    -1,   107,    -1,    -1,
      -1,   111,   112,   113,    -1,   115,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,
      -1,   131,    -1,    -1,   134,    -1,   136,   137,    -1,   139,
      -1,   141,    -1,   143,    -1,    -1,   146,    -1,   148,   149,
     150,    -1,   152,    -1,   154,    -1,   156,   157,    -1,   159,
      -1,    -1,    -1,   163,    -1,    -1,   166,    -1,   168,   169,
     170,   171,    -1,   173,    -1,   175,    -1,   177,    -1,   179,
      -1,    -1,    -1,   183,   184,    -1,   186,   187,   188,   189,
     190,   191,   192,    -1,   194,    -1,   196,   197,   198,    -1,
     200,    -1,   202,    -1,   204,    -1,   206,   207,   208,    -1,
      -1,    28,    -1,   213,    -1,    -1,   216,   217,    35,   219,
     220,   221,    -1,    -1,   224,    -1,    -1,    -1,    45,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    -1,    73,    74,    -1,    76,
      -1,    -1,    -1,    80,    -1,    82,    83,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    -1,
      97,    98,    -1,    -1,    -1,   102,    -1,    -1,   105,    -1,
     107,    -1,    -1,    -1,   111,   112,   113,    -1,   115,    -1,
      -1,    -1,    -1,   120,    -1,    -1,    -1,   124,    -1,    -1,
      -1,    -1,    -1,    -1,   131,    -1,    -1,   134,    -1,   136,
     137,    -1,   139,    -1,   141,    -1,   143,    -1,    -1,   146,
      -1,   148,   149,   150,    -1,   152,    -1,   154,    -1,   156,
     157,    -1,   159,    -1,    -1,    -1,   163,    -1,    -1,   166,
      -1,   168,   169,   170,   171,    -1,   173,    -1,   175,    -1,
     177,    -1,   179,    -1,    -1,    -1,   183,   184,    -1,   186,
     187,   188,   189,   190,   191,   192,    -1,   194,    -1,   196,
     197,   198,    -1,   200,    -1,   202,    -1,   204,    -1,   206,
     207,   208,    -1,    -1,    -1,    -1,   213,    -1,    -1,   216,
     217,    -1,   219,   220,   221,    -1,    -1,   224,    28,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    -1,    48,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    -1,    73,    -1,    -1,    76,    -1,    -1,    -1,
      80,    81,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      90,    -1,    -1,    -1,    94,    95,    -1,    -1,    98,    -1,
      -1,   101,   102,    -1,    -1,    -1,    -1,   107,    -1,    -1,
      -1,   111,   112,    -1,    -1,   115,   116,   117,    -1,   119,
     120,    -1,    -1,    -1,   124,    -1,    -1,    -1,   128,   129,
      -1,   131,   132,    -1,   134,    -1,   136,   137,   138,    -1,
      -1,    -1,   142,   143,   144,    -1,   146,    -1,   148,   149,
     150,   151,   152,    -1,    -1,    -1,   156,   157,    -1,   159,
     160,    -1,   162,   163,    -1,    -1,   166,   167,   168,   169,
     170,   171,    -1,    -1,    -1,   175,   176,   177,    -1,   179,
      -1,    -1,   182,   183,    -1,   185,   186,   187,    -1,    28,
      29,   191,    -1,    -1,    -1,   195,   196,    -1,   198,    -1,
      -1,    -1,   202,    42,    43,    44,   206,    -1,    -1,    -1,
      -1,    50,    -1,    -1,    -1,   215,   216,   217,    -1,   219,
     220,   221,    -1,    -1,   224,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    -1,    73,    -1,    -1,    76,    -1,    -1,
      -1,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    -1,    -1,    -1,    -1,    95,    -1,    -1,    98,
      -1,    -1,   101,   102,    -1,    -1,    -1,    -1,   107,    -1,
      -1,    -1,   111,   112,    -1,    -1,   115,   116,    -1,    -1,
      -1,   120,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,
      -1,    -1,   131,    -1,    -1,   134,    -1,   136,   137,   138,
      -1,    -1,    -1,    -1,   143,    -1,    -1,   146,    -1,   148,
     149,   150,    -1,   152,    -1,    -1,    -1,   156,   157,    -1,
     159,    -1,    -1,    -1,   163,    -1,    -1,   166,    -1,    -1,
     169,   170,   171,    -1,    -1,    -1,   175,    -1,   177,    -1,
     179,    -1,    -1,   182,   183,    -1,    -1,   186,   187,    -1,
      -1,    -1,   191,    -1,    -1,    -1,   195,   196,    -1,   198,
      -1,    -1,    -1,   202,    -1,    -1,    -1,   206,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   215,   216,   217,    -1,
     219,   220,   221,    -1,    -1,   224
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   134,   166,   236,   237,   239,   241,   243,   240,   238,
       0,   225,   242,   244,   271,    55,   257,   384,   163,   219,
     246,   257,    28,    30,   387,   110,    99,   259,   107,   112,
     216,   217,   226,   245,    77,    56,   258,   247,    28,    35,
      45,    46,    47,    48,    55,    57,    70,    71,    73,    74,
      76,    80,    82,    83,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   102,   105,   107,
     111,   112,   113,   115,   120,   124,   127,   131,   134,   136,
     137,   139,   141,   143,   146,   148,   149,   150,   152,   154,
     156,   157,   159,   163,   166,   168,   169,   170,   171,   172,
     173,   174,   175,   177,   179,   183,   184,   186,   187,   188,
     189,   190,   191,   192,   194,   196,   197,   198,   200,   202,
     204,   205,   206,   207,   208,   212,   213,   216,   217,   219,
     220,   221,   224,   260,   261,   263,   283,   284,   389,    67,
      68,    69,   248,   249,   250,   387,   283,   265,   387,   266,
     283,   283,   261,    46,    57,    71,    90,   206,   227,   285,
     316,   283,    28,    35,    45,    46,    47,    48,    57,   255,
     389,    94,   117,   252,    32,    33,    34,    55,   184,   197,
     234,   246,   293,   294,   295,   296,   297,   331,   332,   333,
     334,   335,   336,   341,   349,   350,   365,   369,   370,   384,
     250,    56,   157,   196,   274,   270,   226,   228,   270,   141,
      75,   141,   262,   226,   121,   226,   283,    35,    47,   286,
     314,   315,    23,   100,   226,   231,   289,   291,   292,    96,
      89,   130,   387,   147,   181,   223,   226,   114,   372,   295,
      28,    29,    30,    42,    43,    44,    45,    46,    48,    49,
      50,    70,    71,    73,    76,    80,    81,    84,    90,    94,
      95,    98,   101,   102,   107,   111,   112,   115,   116,   117,
     119,   120,   124,   128,   129,   131,   132,   134,   136,   137,
     138,   142,   143,   144,   146,   148,   149,   150,   151,   152,
     156,   157,   159,   160,   162,   163,   166,   167,   168,   169,
     170,   171,   175,   176,   177,   179,   182,   183,   185,   186,
     187,   191,   195,   196,   198,   202,   206,   215,   216,   217,
     219,   220,   221,   224,   298,   299,   301,   302,   317,   321,
     323,   326,   344,   345,   346,   347,   348,   355,   356,   357,
     358,   359,   363,   364,   388,   293,   351,   383,   293,    88,
     121,   122,   123,   147,   180,   181,   188,   193,   223,   196,
     264,   387,    57,   267,   268,   286,   314,   315,   316,   177,
     178,   207,   220,    28,    29,   282,    85,   287,   289,   289,
     274,   121,   121,    56,   373,   110,   226,   228,   292,   318,
     318,   318,   318,   318,   327,    28,    29,    30,    49,   371,
     360,    95,   202,   303,    28,   322,   381,   360,   226,   371,
     300,   189,   292,   385,   387,   110,   293,   351,   352,   214,
     226,    28,    29,    30,    45,    48,   337,   340,   338,   340,
     340,   351,   193,   366,   367,   387,   293,   210,   368,   366,
     383,   228,   276,   227,   158,   288,   226,   226,   228,    96,
     161,   228,   226,    28,    29,   288,   251,    18,   200,   256,
      18,   158,   199,   200,   201,   253,   254,   387,   222,   374,
     375,   385,   378,   228,   232,   233,   319,   222,   328,   329,
     226,   371,   104,   226,   293,   371,   378,   228,   378,   222,
     226,   386,   147,   110,   353,   354,   351,   135,   192,   339,
     135,   198,   342,   366,   226,   214,   367,   226,   226,   351,
     352,   277,   278,   283,   121,   141,   281,    18,   230,   269,
     160,   226,   272,   273,   320,   185,   280,    57,   286,   314,
     315,   316,    18,   289,   276,   226,   125,   227,   295,   376,
     377,   387,   375,   380,   320,   379,   381,   108,   329,   330,
      78,   140,    13,    28,    29,    30,    89,   304,   305,   308,
     324,   226,   378,   385,   223,   181,   115,    29,   343,   355,
     356,    18,   135,   353,   227,   229,   135,   165,   279,   275,
      18,   229,   227,   229,    57,   286,   314,   315,   316,   286,
     316,   226,   288,   288,   288,   288,   288,   229,   281,   254,
     164,   209,   293,   293,   110,    86,   231,     4,     5,     6,
      13,    18,    28,    30,   118,   160,   211,   309,   310,   226,
     227,   231,   292,   108,   109,   325,   226,   385,   385,    74,
     278,   286,   226,   273,   226,   288,   288,   288,   288,   288,
     136,   226,   136,   226,   136,   226,   136,   226,   136,   226,
     161,   275,   377,   293,    81,   140,   155,   191,   307,    13,
     308,   311,   312,   311,   305,   155,   150,   169,   170,   171,
     183,   186,   191,   306,   293,   381,   110,   226,   226,   382,
     290,   291,   290,   290,   290,   290,   290,    79,    79,    79,
      79,    79,    57,   286,   314,   315,   316,   226,   362,   231,
     231,   226,   227,   226,   293,   132,   351,   286,   286,   286,
     286,   286,   226,   226,   226,   226,   226,   145,   226,   361,
      82,    83,    87,    91,    92,    93,    97,   150,   154,   186,
     194,   204,   208,   313,   313,   312,   226,   226,   226,   226,
     226,   226
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   235,   236,   236,   236,   238,   237,   240,   239,   242,
     241,   243,   243,   244,   244,   244,   244,   245,   245,   247,
     246,   248,   248,   249,   249,   251,   250,   250,   250,   252,
     252,   253,   253,   254,   254,   254,   254,   254,   254,   255,
     255,   255,   255,   255,   255,   255,   255,   256,   256,   257,
     257,   258,   257,   259,   260,   260,   261,   261,   261,   262,
     262,   263,   263,   264,   263,   263,   263,   263,   263,   263,
     263,   263,   263,   263,   263,   263,   263,   263,   263,   263,
     263,   263,   263,   263,   263,   263,   263,   265,   263,   266,
     263,   263,   263,   263,   263,   263,   267,   267,   268,   268,
     269,   269,   270,   271,   272,   272,   273,   273,   273,   273,
     273,   274,   274,   274,   275,   276,   276,   277,   277,   278,
     279,   279,   279,   280,   280,   280,   281,   281,   282,   282,
     283,   283,   283,   283,   283,   283,   283,   283,   284,   284,
     285,   285,   286,   287,   287,   287,   288,   288,   289,   289,
     290,   290,   291,   291,   292,   292,   293,   293,   294,   294,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   296,   296,   297,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   300,   299,   301,   301,   302,   302,   302,   303,   303,
     303,   304,   304,   305,   305,   305,   306,   306,   306,   306,
     306,   306,   306,   307,   307,   308,   308,   308,   309,   310,
     310,   310,   310,   310,   310,   310,   310,   310,   311,   311,
     312,   312,   313,   313,   313,   313,   313,   313,   313,   313,
     313,   313,   313,   313,   313,   314,   315,   316,   317,   317,
     317,   317,   317,   318,   318,   319,   319,   319,   320,   321,
     322,   323,   324,   324,   325,   325,   326,   327,   328,   328,
     329,   330,   330,   331,   331,   332,   332,   333,   333,   333,
     333,   333,   333,   334,   334,   335,   335,   336,   336,   337,
     338,   339,   339,   340,   340,   340,   340,   340,   341,   342,
     342,   343,   343,   344,   345,   345,   345,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   354,   355,   355,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   355,   355,   355,   356,   357,   358,   358,   359,   360,
     361,   361,   362,   363,   364,   365,   366,   367,   367,   368,
     368,   369,   370,   370,   371,   371,   371,   371,   372,   373,
     372,   374,   374,   375,   376,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   384,   385,   385,   386,   386,   387,
     387,   388,   388,   388,   388,   388,   388,   388,   388,   388,
     388,   388,   388,   388,   388,   388,   388,   388,   388,   388,
     388,   388,   388,   388,   388,   388,   388,   388,   388,   388,
     388,   388,   388,   388,   388,   388,   388,   388,   388,   388,
     388,   388,   388,   388,   388,   388,   388,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     0,     4,     0,     3,     0,
       4,     0,     2,     3,     3,     3,     3,     0,     1,     0,
       8,     0,     1,     2,     1,     0,     8,     6,     6,     1,
       1,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     0,     4,     1,     2,     1,     1,     1,     3,     1,
       1,     6,     5,     0,     7,     7,     3,     3,    10,    10,
      10,    10,    10,     4,     8,     8,     8,     8,     8,    11,
      11,    11,    11,    11,     4,     8,     3,     0,     3,     0,
       3,     3,     8,     6,     5,     5,     1,     1,     1,     1,
       1,     2,     0,     0,     1,     3,     4,     4,     4,     4,
       4,     0,     2,     1,     0,     0,     3,     1,     3,     4,
       1,     1,     0,     0,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       0,     1,     0,     0,     2,     2,     0,     2,     1,     1,
       0,     1,     1,     1,     1,     1,     0,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     3,     3,     5,     7,     7,     0,     1,
       1,     3,     1,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     0,     2,     2,     2,     2,     0,     2,
       2,     8,     0,     4,     0,     2,     7,     0,     2,     1,
       3,     0,     2,     3,     2,     1,     2,     4,     3,     4,
       3,     5,     4,     1,     2,     1,     2,     4,     5,     2,
       2,     2,     0,     1,     1,     1,     1,     1,     8,     0,
       2,     1,     1,     3,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     5,     5,     4,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     1,     2,     4,     8,     4,     0,
       1,     1,     0,     3,     2,     3,     1,     2,     1,     0,
       2,     4,     4,     4,     1,     1,     1,     1,     0,     0,
       3,     2,     1,     4,     3,     1,     1,     0,     0,     0,
       0,     0,     0,     0,     3,     0,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
THR_LOCAL int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 1000
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
THR_LOCAL int yychar;

/* The semantic value of the lookahead symbol.  */
THR_LOCAL YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
THR_LOCAL YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
THR_LOCAL int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 653 "src/pltsql/gram.y" /* yacc.c:1646  */
    { SetErrorState(); }
#line 2769 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 654 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int nDatums = 0;
                        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile!=NULL) {
                            nDatums = u_sess->plsql_cxt.curr_compile_context->plpgsql_nDatums;
                        } else {
                            nDatums = u_sess->plsql_cxt.curr_compile_context->plpgsql_pkg_nDatums;
                        }

                        for (int i = 0; i < nDatums; i++) {
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[i]->ispkg = true;
                        }
                        PLpgSQL_nsitem* ns_cur = plpgsql_ns_top();
                        while (ns_cur != NULL) {
                            if (ns_cur != NULL && ns_cur->pkgname == NULL) {
                                ns_cur->pkgname =  u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_signature;
                            }
                            ns_cur = ns_cur->prev;
                        }
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->n_initvars = (yyvsp[-1].declhdr).n_initvars;
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->initvarnos = (yyvsp[-1].declhdr).initvarnos;
                    }
#line 2795 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 676 "src/pltsql/gram.y" /* yacc.c:1646  */
    { SetErrorState(); }
#line 2801 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 677 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL)
                        {
                            List* raw_parsetree_list = NULL;
                            u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                            raw_parsetree_list = raw_parser((yyvsp[0].str));
                            DoStmt* stmt;
                            stmt = (DoStmt *)linitial(raw_parsetree_list);
                            stmt->isExecuted = false;
                            if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling) {
                                stmt->isSpec = true;
                            } else {
                                stmt->isSpec = false;
                            }
                            List *proc_list = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->proc_list;
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->proc_list = lappend(proc_list,stmt);
                        } else {
                            yyerror("instantiation only use in package compile");
                        }
                    }
#line 2826 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 698 "src/pltsql/gram.y" /* yacc.c:1646  */
    { SetErrorState(); }
#line 2832 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 699 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_parse_result = (PLpgSQL_stmt_block *) (yyvsp[-1].stmt);
                    }
#line 2840 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 709 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_DumpExecTree = true;
                    }
#line 2848 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 713 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_ERROR;
                    }
#line 2856 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 717 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_VARIABLE;
                    }
#line 2864 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 721 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->resolve_option = PLPGSQL_RESOLVE_COLUMN;
                    }
#line 2872 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 731 "src/pltsql/gram.y" /* yacc.c:1646  */
    { 
						if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
							HandleBlockLevel();
							HandleSubprogram();
							//u_sess->plsql_cxt.curr_compile_context->in_begin = true;
						}
					}
#line 2884 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 739 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_block *newp;
                        //u_sess->plsql_cxt.curr_compile_context->in_begin = false;

                        newp = (PLpgSQL_stmt_block *)palloc0(sizeof(PLpgSQL_stmt_block));

                        newp->cmd_type	= PLPGSQL_STMT_BLOCK;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[-6]));
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->label		= (yyvsp[-7].declhdr).label;
                        newp->isAutonomous = (yyvsp[-7].declhdr).isAutonomous;
                        newp->n_initvars = (yyvsp[-7].declhdr).n_initvars;
                        newp->initvarnos = (yyvsp[-7].declhdr).initvarnos;
                        newp->body		= (yyvsp[-3].list);
                        if ((yyvsp[-4].exception_block) != NULL) {
                            if ((yyvsp[-2].exception_block) != NULL) {
                                const char* message = "declare handler and exception cannot be used at the same time";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_UNDEFINED_OBJECT),
                                        errmsg("declare handler and exception cannot be used at the same time")));
                            } else {
                                newp->exceptions	= (yyvsp[-4].exception_block);
                                newp->isDeclareHandlerStmt  = true;
                            }
                        } else {
                            newp->exceptions	= (yyvsp[-2].exception_block);
                            newp->isDeclareHandlerStmt  = false;
                        }

                        check_labels((yyvsp[-7].declhdr).label, (yyvsp[0].str), (yylsp[0]));
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-7].declhdr).label, (PLpgSQL_stmt *)newp);
                    }
#line 2927 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 780 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* done with decls, so resume identifier lookup */
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.exception_block) = NULL; 
                    }
#line 2937 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 786 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* done with decls, so resume identifier lookup */
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        if ((yyvsp[0].list) == NULL) {
                            (yyval.exception_block) = NULL;
                        } else {
                            PLpgSQL_exception_block *newp = (PLpgSQL_exception_block *)palloc(sizeof(PLpgSQL_exception_block));
                            newp->exc_list = (yyvsp[0].list);

                            (yyval.exception_block) = newp;
                        }
                    }
#line 2954 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 801 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].exception) == NULL)
                            (yyval.list) = (yyvsp[-1].list);
                        else
                            (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].exception));
                    }
#line 2965 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 808 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].exception) == NULL) {
                            (yyval.list) = NULL;
                        } else {
                            (yyval.list) = list_make1((yyvsp[0].exception));
                        }
                    }
#line 2977 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 818 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-2].varname)->name);
                        plpgsql_ns_push((yyvsp[-2].varname)->name); 
                    }
#line 2986 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 823 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok = -1;
                        plpgsql_peek(&tok);
                        if (tok != K_DECLARE) {
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        }
                        PLpgSQL_var *newp;

                        /* pop local namespace for cursor args */
                        plpgsql_ns_pop();

                        newp = (PLpgSQL_var *)
                                plpgsql_build_variable((yyvsp[-6].varname)->name, (yyvsp[-6].varname)->lineno,
                                                    plpgsql_build_datatype(REFCURSOROID,
                                                                            -1,
                                                                            InvalidOid),
                                                                            true);

                        newp->cursor_explicit_expr = (yyvsp[0].expr);
                        if ((yyvsp[-2].datum) == NULL) {
                            newp->cursor_explicit_argrow = -1;
                            newp->cursor_implicit_argrow = -1;
                        }
                        else {
                            newp->cursor_explicit_argrow = (yyvsp[-2].datum)->dno;
                            newp->cursor_implicit_argrow = ((PLpgSQL_row*)(yyvsp[-2].datum))->needValDno;
                        }
                        newp->cursor_options = CURSOR_OPT_FAST_PLAN | (yyvsp[-4].ival);
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        newp->datatype->cursorCompositeOid = IS_ANONYMOUS_BLOCK ? 
                            InvalidOid : createCompositeTypeForCursor(newp, (yyvsp[0].expr));
                        newp->datatype->cursorExpr = (yyvsp[0].expr);
                        pfree_ext((yyvsp[-6].varname)->name);
                        pfree((yyvsp[-6].varname));
                        (yyval.exception) = NULL;
                    }
#line 3027 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 860 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok = -1;
                        plpgsql_peek(&tok);
                        if (tok != K_DECLARE) {
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        }
                        if ((yyvsp[-4].varname)->name != NULL) {
                            if (pg_strcasecmp((yyvsp[-4].varname)->name, "set") == 0) {
                                yyerror("syntax error, set is keyword.");
                            }
                            if (pg_strcasecmp((yyvsp[-4].varname)->name, "sqlwarning") == 0) {
                                yyerror("syntax error, sqlwarning is keyword.");
                            }
                            if (pg_strcasecmp((yyvsp[-4].varname)->name, "sqlexception") == 0) {
                                yyerror("syntax error, sqlexception is keyword.");
                            }
                        }
                        
                        CheckDuplicateCondition((yyvsp[-4].varname)->name);
                        PLpgSQL_condition* cond = (yyvsp[-1].condition);
                        cond->condname = pstrdup((yyvsp[-4].varname)->name);
                        PLpgSQL_condition* old = u_sess->plsql_cxt.curr_compile_context->plpgsql_conditions;
                        if (old != NULL) {
                            cond->next = old;
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_conditions = cond;
                        } else {
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_conditions = cond;
                        }

                        pfree_ext((yyvsp[-4].varname)->name);
                        pfree((yyvsp[-4].varname));
                        (yyval.exception) = NULL;
                    }
#line 3065 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 894 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok = -1;
                        plpgsql_peek(&tok);
                        if (tok != K_DECLARE) {
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        }
                        PLpgSQL_exception *newp;

                        newp = (PLpgSQL_exception *)palloc0(sizeof(PLpgSQL_exception));
                        newp->lineno     = plpgsql_location_to_lineno((yylsp[-5]));
                        newp->handler_type = (yyvsp[-4].declare_handler_type);
                        newp->conditions = (yyvsp[-1].condition);
                        newp->action	 = list_make1((yyvsp[0].stmt));

                        (yyval.exception) = newp;
                    }
#line 3086 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 913 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.declare_handler_type) = PLpgSQL_declare_handler::DECLARE_HANDLER_EXIT; }
#line 3092 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 915 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.declare_handler_type) = PLpgSQL_declare_handler::DECLARE_HANDLER_CONTINUE; }
#line 3098 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 918 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_condition	*old;

                        for (old = (yyvsp[-2].condition); old->next != NULL; old = old->next)
                                /* skip */ ;
                        old->next = (yyvsp[0].condition);
                        (yyval.condition) = (yyvsp[-2].condition);
                    }
#line 3111 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 927 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.condition) = (yyvsp[0].condition);
                    }
#line 3119 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 933 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.condition) = plpgsql_parse_err_condition_b((yyvsp[0].str));
                    }
#line 3128 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 938 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_condition *newp;
                        newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                        /* next token should be a string literal */
                        char   *sqlstatestr;
                        yylex();
                        if (strcmp(yylval.str, "value") ==0) {
                            yylex();
                        }
                        sqlstatestr = yylval.str;
                        
                        if (strlen(sqlstatestr) != 5)
                            yyerror("invalid SQLSTATE code");
                        if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                            yyerror("invalid SQLSTATE code");
                        if (strncmp(sqlstatestr, "00", 2) == 0) {
                            const char* message = "bad SQLSTATE";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                                        errmsg("bad SQLSTATE '%s'",sqlstatestr)));
                        }

                        newp->sqlerrstate = MAKE_SQLSTATE(sqlstatestr[0],
                                                          sqlstatestr[1],
                                                          sqlstatestr[2],
                                                          sqlstatestr[3],
                                                          sqlstatestr[4]);
                        newp->condname = NULL;
                        newp->sqlstate = pstrdup(sqlstatestr);
                        newp->next = NULL;
                        newp->isSqlvalue = false;
                        (yyval.condition) = newp;
                    }
#line 3167 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 973 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.condition) = plpgsql_parse_err_condition_b("k_sqlwarning");
                    }
#line 3176 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 978 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.condition) = plpgsql_parse_err_condition_b("not_found");
                    }
#line 3185 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 983 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.condition) = plpgsql_parse_err_condition_b("k_sqlexception");
                    }
#line 3194 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 988 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].ival) == 0) {
                            const char* message = "Incorrect CONDITION value: '0'";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                                        errmsg("Incorrect CONDITION value: '0'")));
                        }
                        int num = (yyvsp[0].ival);
                        char *sqlstatestr = (char *)palloc0(sizeof(char) * 6);
                        for (int i= 4; i >= 0; i--) {
                            sqlstatestr[i] = num % 10 + '0';
                            num /= 10;
                        }
                        sqlstatestr[5] = '\0';
                        if (num != 0) {
                            (yyval.condition) = NULL;
                        }
                        
                        PLpgSQL_condition *newp = (PLpgSQL_condition *)palloc0(sizeof(PLpgSQL_condition));
                        newp->sqlerrstate = MAKE_SQLSTATE(sqlstatestr[0],
                                                          sqlstatestr[1],
                                              	          sqlstatestr[2],
                                              	          sqlstatestr[3],
                                              	          sqlstatestr[4]);
                        newp->condname = NULL;
                        newp->next = NULL;
                        newp->sqlstate = pstrdup(sqlstatestr);
                        newp->isSqlvalue = true;
                        (yyval.condition) = newp;
                    }
#line 3230 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 1022 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = (yyvsp[0].word).ident;
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;
                    }
#line 3242 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 1030 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].keyword));
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;
                    }
#line 3254 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 1038 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3267 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 1047 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3280 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 1056 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3293 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 1065 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3306 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 1074 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        if ((yyvsp[0].wdatum).idents != NIL) {
                            yyerror("syntax error");
                        }
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3322 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 1086 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        if ((yyvsp[0].wdatum).idents != NIL || strcmp((yyvsp[0].wdatum).ident, "bulk_exceptions") == 0) {
                            yyerror("syntax error");
                        }
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.varname) = varname;

                    }
#line 3338 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 1100 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_condition *newp;
                        newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                        /* next token should be a string literal */
                        char   *sqlstatestr;
                        yylex();
                        if (strcmp(yylval.str, "value") == 0) {
                            yylex();
                        }
                        sqlstatestr = yylval.str;
                        
                        if (strlen(sqlstatestr) != 5)
                            yyerror("invalid SQLSTATE code");
                        if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                            yyerror("invalid SQLSTATE code");
                        if (strncmp(sqlstatestr, "00", 2) == 0) {
                            const char* message = "bad SQLSTATE";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                                        errmsg("bad SQLSTATE '%s'",sqlstatestr)));
                        }

                        newp->sqlerrstate = MAKE_SQLSTATE(sqlstatestr[0],
                                                          sqlstatestr[1],
                                                          sqlstatestr[2],
                                                          sqlstatestr[3],
                                                          sqlstatestr[4]);
                        newp->condname = NULL;
                        newp->sqlstate = sqlstatestr;
                        newp->next = NULL;
                        newp->isSqlvalue = false;
                        (yyval.condition) = newp;
                    }
#line 3377 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 1135 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_condition *newp;
                        newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                        if ((yyvsp[0].ival) == 0) {
                            const char* message = "Incorrect CONDITION value: '0'";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                                        errmsg("Incorrect CONDITION value: '0'")));
                        }

                        int num = (yyvsp[0].ival);
                        char *sqlstatestr = (char *)palloc0(sizeof(char) * 6);
                        for (int i = 4; i >= 0; i--) {
                            sqlstatestr[i] = num % 10 + '0';
                            num /= 10;
                        }
                        sqlstatestr[5] = '\0';
                        if (num != 0) {
                            (yyval.condition) = NULL;
                        }

                        newp->sqlerrstate = MAKE_SQLSTATE(sqlstatestr[0],
                                                          sqlstatestr[1],
                                                          sqlstatestr[2],
                                                          sqlstatestr[3],
                                                          sqlstatestr[4]);
                        newp->condname = NULL;
                        newp->sqlstate = pstrdup(sqlstatestr);
                        newp->next = NULL;
                        newp->isSqlvalue = true;
                        (yyval.condition) = newp;
                    }
#line 3415 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 1171 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* done with decls, so resume identifier lookup */
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[0].str);
                        (yyval.declhdr).n_initvars = 0;
                        (yyval.declhdr).initvarnos = NULL;
                        (yyval.declhdr).isAutonomous = false;
                    }
#line 3428 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 1180 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[-1].str);
                        (yyval.declhdr).n_initvars = 0;
                        (yyval.declhdr).initvarnos = NULL;
                        (yyval.declhdr).isAutonomous = false;
                    }
#line 3440 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 1187 "src/pltsql/gram.y" /* yacc.c:1646  */
    { SetErrorState(); }
#line 3446 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 1188 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
                        (yyval.declhdr).label	  = (yyvsp[-3].str);
                        /* Remember variables declared in decl_stmts */
                        (yyval.declhdr).n_initvars = plpgsql_add_initdatums(&((yyval.declhdr).initvarnos));
                        (yyval.declhdr).isAutonomous = u_sess->plsql_cxt.pragma_autonomous;
                        u_sess->plsql_cxt.pragma_autonomous = false;
                    }
#line 3459 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 1199 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* Forget any variables created before block */
                        plpgsql_add_initdatums(NULL);
                        u_sess->plsql_cxt.pragma_autonomous = false;
                        /*
                         * Disable scanner lookup of identifiers while
                         * we process the decl_stmts
                         */
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_DECLARE;
                    }
#line 3474 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 1217 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* We allow useless extra DECLAREs */
                    }
#line 3482 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 1221 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * Throw a helpful error if user tries to put block
                         * label just before BEGIN, instead of before DECLARE.
                         */
                        const char* message = "block label must be placed before DECLARE, not after";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                 errmsg("block label must be placed before DECLARE, not after"),
                                 parser_errposition((yylsp[-2]))));
                    }
#line 3499 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 1240 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ListCell *lc = NULL;
                        PLpgSQL_row* row = NULL;
                        if ((list_length((yyvsp[-5].list)) > 1) && ((yyvsp[-3].dtype) && (yyvsp[-3].dtype)->typoid == REFCURSOROID))
						    ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("not support declare multiple variable"),
                                 parser_errposition((yylsp[-5]))));

                        /* build declare variable list*/
                        foreach(lc, (yyvsp[-5].list))
                        {
                            VarName* varname = (VarName*) lfirst(lc);
                            PLpgSQL_variable	*var;

                            IsInPublicNamespace(varname->name);

                            /*
                             * If a collation is supplied, insert it into the
                             * datatype.  We assume decl_datatype always returns
                             * a freshly built struct not shared with other
                             * variables.
                             */

                            if ((yyvsp[-3].dtype) == NULL) {
                                // not allowed going on when plsql_show_all_error is on
#ifndef ENABLE_MULTIPLE_NODES
                                if (!u_sess->attr.attr_common.plsql_show_all_error) {
                                    yyerror("missing data type declaration");
                                } else {
                                    const char* message = "missing data type declaration";
                                    InsertErrorMessage(message, (yylsp[-2]));
                                    yyerror("missing data type declaration");
                                    ereport(ERROR,
                                            (errcode(ERRCODE_DATATYPE_MISMATCH),
                                             errmsg("missing data type declaration"),
                                             parser_errposition((yylsp[-2]))));
                                }
#else
                                yyerror("missing data type declaration");
#endif
                            } else {

                            if ((yyvsp[-3].dtype) && (yyvsp[-3].dtype)->typoid == REFCURSOROID && IsOnlyCompilePackage())
                            {
                                yyerror("not allow use ref cursor in package");
                            }

                            if (OidIsValid((yyvsp[-2].oid)))
                            {
                                if (!OidIsValid((yyvsp[-3].dtype)->collation)) {
                                    const char* message = "collations are not supported by type";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                             (errcode(ERRCODE_DATATYPE_MISMATCH),
                                             errmsg("collations are not supported by type %s",
                                                    format_type_be((yyvsp[-3].dtype)->typoid)),
                                             parser_errposition((yylsp[-2]))));
                                }
                                (yyvsp[-3].dtype)->collation = (yyvsp[-2].oid);
                            }

                            var = plpgsql_build_variable(varname->name, varname->lineno,
                                                         (yyvsp[-3].dtype), true);
                            if (var->dtype == PLPGSQL_DTYPE_ROW || var->dtype == PLPGSQL_DTYPE_RECORD)
                            {
                                row = (PLpgSQL_row*)var;
                                PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                                ntype->index = 1;
                                List* nest_typnames = NIL;
                                row->nest_typnames = search_external_nest_type((yyvsp[-3].dtype)->typname, (yyvsp[-3].dtype)->typoid, -1, nest_typnames, ntype);
                            }
                            if ((yyvsp[-4].boolean))
                            {
                                if (var->dtype == PLPGSQL_DTYPE_VAR)
                                    ((PLpgSQL_var *) var)->isconst = (yyvsp[-4].boolean);
                                else {
                                    const char* message = "row or record variable cannot be CONSTANT";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                             errmsg("row or record variable cannot be CONSTANT"),
                                             parser_errposition((yylsp[-4]))));
                                }
                            }
                            if ((yyvsp[-1].boolean))
                            {
                                if (var->dtype == PLPGSQL_DTYPE_VAR)
                                    ((PLpgSQL_var *) var)->notnull = (yyvsp[-1].boolean);
                                else {
                                    const char* message = "row or record variable cannot be NOT NULL";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                             (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                             errmsg("row or record variable cannot be NOT NULL"),
                                             parser_errposition((yylsp[-2]))));
                                }
                            }
                            if ((yyvsp[0].expr) != NULL)
                            {
                                if (var->dtype == PLPGSQL_DTYPE_VAR)
                                    ((PLpgSQL_var *) var)->default_val = (yyvsp[0].expr);
                                else if (var->dtype == PLPGSQL_DTYPE_ROW || var->dtype == PLPGSQL_DTYPE_RECORD) {
                                    PLpgSQL_row * row = (PLpgSQL_row *) var;
                                    if (!(row->atomically_null_object && pg_strcasecmp((yyvsp[0].expr)->query, "select null") == 0)) {
                                        row->default_val = (yyvsp[0].expr);
                                        checkTypeName((yyvsp[0].expr)->nest_typnames, row->nest_typnames);
                                    }
                                } else if (var->dtype == PLPGSQL_DTYPE_CURSORROW) {
                                     ((PLpgSQL_rec *) var)->default_val = (yyvsp[0].expr);
                                }
                                else {
                                    const char* message = "default value for rec variable is not supported";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                            errmsg("default value for rec variable is not supported"),
                                            parser_errposition((yylsp[-1]))));
                                }
                            }
                            if (enable_plpgsql_gsdependency()) {
                                gsplsql_build_gs_type_in_body_dependency((yyvsp[-3].dtype));
                            }
                            }
                            pfree_ext(varname->name);
                        }
                        list_free_deep((yyvsp[-5].list));
                    }
#line 3632 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 1369 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (list_length((yyvsp[-4].list)) != 1)
                            ereport(errstate,
                                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                            errmsg("alias not support declare multiple variable"),
                                            parser_errposition((yylsp[-4]))));

                        VarName *varname = (VarName*) lfirst(list_head((yyvsp[-4].list)));
                        IsInPublicNamespace(varname->name);
                        plpgsql_ns_additem((yyvsp[-1].nsitem)->itemtype,
                                                (yyvsp[-1].nsitem)->itemno, varname->name);
                        pfree_ext(varname->name);
                        list_free_deep((yyvsp[-4].list));
                    }
#line 3651 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 1384 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-1].varname)->name);
                        plpgsql_ns_push((yyvsp[-1].varname)->name); 
                    }
#line 3660 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 1389 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_var *newp;

                        /* pop local namespace for cursor args */
                        plpgsql_ns_pop();

                        newp = (PLpgSQL_var *)
                                plpgsql_build_variable((yyvsp[-5].varname)->name, (yyvsp[-5].varname)->lineno,
                                                    plpgsql_build_datatype(REFCURSOROID,
                                                                            -1,
                                                                            InvalidOid),
                                                                            true);

                        newp->cursor_explicit_expr = (yyvsp[0].expr);
                        if ((yyvsp[-2].datum) == NULL) {
                            newp->cursor_explicit_argrow = -1;
                            newp->cursor_implicit_argrow = -1;
                        } else {
                            newp->cursor_explicit_argrow = (yyvsp[-2].datum)->dno;
                            newp->cursor_implicit_argrow = ((PLpgSQL_row*)(yyvsp[-2].datum))->needValDno;
                        }
                        newp->cursor_options = CURSOR_OPT_FAST_PLAN | (yyvsp[-4].ival);
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        newp->datatype->cursorCompositeOid = IS_ANONYMOUS_BLOCK ? 
                            InvalidOid : createCompositeTypeForCursor(newp, (yyvsp[0].expr));
                        newp->datatype->cursorExpr = (yyvsp[0].expr);
                        pfree_ext((yyvsp[-5].varname)->name);
						pfree((yyvsp[-5].varname));
                    }
#line 3694 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 1419 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-5].varname)->name);
                        /* add name of cursor type to PLPGSQL_NSTYPE_REFCURSOR */
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_REFCURSOR,0,(yyvsp[-5].varname)->name);
                        if (IS_PACKAGE) {
                            plpgsql_build_package_refcursor_type((yyvsp[-5].varname)->name);
                        }
                        pfree_ext((yyvsp[-5].varname)->name);
						pfree((yyvsp[-5].varname));

                    }
#line 3710 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 1431 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (list_length((yyvsp[-2].list)) != 1)
                            ereport(errstate,
                                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                            errmsg("refcursor not support declare multiple variable"),
                                            parser_errposition((yylsp[-2]))));

                        VarName *varname = (VarName*) lfirst(list_head((yyvsp[-2].list)));

                        IsInPublicNamespace(varname->name);
                        if (IsOnlyCompilePackage()) {
                            yyerror("not allow use ref cursor in package");
                        }
                        AddNamespaceIfNeed(-1, (yyvsp[-1].wdatum).ident);

                        plpgsql_build_variable(
                                varname->name, 
                                varname->lineno,
                                plpgsql_build_datatype(REFCURSOROID,-1,InvalidOid),
                                true);
                        pfree_ext(varname->name);
                        list_free_deep((yyvsp[-2].list));
                    }
#line 3738 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 1455 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (list_length((yyvsp[-2].list)) != 1)
                            ereport(errstate,
                                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                            errmsg("refcursor not support declare multiple variable"),
                                            parser_errposition((yylsp[-2]))));

                        VarName *varname = (VarName*)lfirst(list_head((yyvsp[-2].list)));
                        IsInPublicNamespace(varname->name);
                        if (IsOnlyCompilePackage()) {
                            yyerror("not allow use ref cursor in package");
                        }
                        plpgsql_build_variable(
                                varname->name, 
                                varname->lineno,
                                plpgsql_build_datatype(REFCURSOROID,-1,InvalidOid),
                                true);
                        pfree_ext(varname->name);
                        list_free_deep((yyvsp[-2].list));
                    }
#line 3763 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 1481 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-8].varname)->name);

                        (yyvsp[-1].dtype)->collectionType = PLPGSQL_COLLECTION_ARRAY;
                        (yyvsp[-1].dtype)->tableOfIndexType = InvalidOid;

                        if((yyvsp[-1].dtype)->typinput.fn_oid == F_ARRAY_IN) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or table type nested by array type is not supported yet."),
                                    errdetail("Define array type \"%s\" of array or table type is not supported yet.", (yyvsp[-8].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of array type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        if((yyvsp[-1].dtype)->typoid == REFCURSOROID) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("ref cursor type nested by array is not supported yet."),
                                    errdetail("Define array type \"%s\" of ref cursor type is not supported yet.", (yyvsp[-8].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of array type")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_varrayType((yyvsp[-8].varname)->name, (yyvsp[-8].varname)->lineno, (yyvsp[-1].dtype), true);

                        PLpgSQL_nest_type *first_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));     
                        PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        first_ntype->typname = pstrdup(var->refname);
                        first_ntype->layer = 0;
                        first_ntype->index = -1;
                        var->nest_typnames = lappend(var->nest_typnames, first_ntype);   
                        ntype->index = -1;
                        var->nest_typnames = search_external_nest_type((yyvsp[-1].dtype)->typname, (yyvsp[-1].dtype)->typoid, 0, var->nest_typnames, ntype);

                        if (IS_PACKAGE) {
                            plpgsql_build_package_array_type((yyvsp[-8].varname)->name, (yyvsp[-1].dtype)->typoid, TYPCATEGORY_ARRAY, (yyvsp[-1].dtype)->dependExtend);
                        } else if (enable_plpgsql_gsdependency()) {
                            gsplsql_build_gs_type_in_body_dependency((yyvsp[-1].dtype));
                        } else {
                             plpgsql_build_func_array_type((yyvsp[-8].varname)->name, (yyvsp[-1].dtype)->typoid, TYPCATEGORY_ARRAY, (yyvsp[-1].dtype)->atttypmod, (yyvsp[-1].dtype)->dependExtend);
                        }
                        pfree_ext((yyvsp[-8].varname)->name);
						pfree((yyvsp[-8].varname));
                    }
#line 3816 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 1530 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array or record type nesting is not supported in distributed database yet."),
                                errdetail("Define array type \"%s\" of record is not supported in distributed database yet.", (yyvsp[-8].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of array type")));
                        u_sess->plsql_cxt.have_error = true;
#endif
                        if (IS_ANONYMOUS_BLOCK) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or record type nesting is not supported in anonymous block yet."),
                                    errdetail("Define array type \"%s\" of record is not supported yet.", (yyvsp[-8].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of array type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        IsInPublicNamespace((yyvsp[-8].varname)->name);

                        PLpgSQL_type *newp = NULL;
                        newp = build_type_from_record_var((yyvsp[-1].ival), (yylsp[-1]));
                        newp->collectionType = PLPGSQL_COLLECTION_ARRAY;
                        newp->tableOfIndexType = InvalidOid;
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_varrayType((yyvsp[-8].varname)->name, (yyvsp[-8].varname)->lineno, newp, true);
                        PLpgSQL_rec_type* rec_var = (PLpgSQL_rec_type*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                        var->nest_typnames = get_current_type_nest_type(rec_var->nest_typnames, var->refname, true);
                        if (IS_PACKAGE) {
                            plpgsql_build_package_array_type((yyvsp[-8].varname)->name, newp->typoid, TYPCATEGORY_ARRAY);
                        } else if (enable_plpgsql_gsdependency()) {
                            int i;
                            for (i = 0; i < rec_var->attrnum; i++) {
                                gsplsql_build_gs_type_in_body_dependency(rec_var->types[i]);
                            }
                        }
                        pfree_ext((yyvsp[-8].varname)->name);
						pfree((yyvsp[-8].varname));
                    }
#line 3862 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 1573 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-8].varname)->name);
                        PLpgSQL_var *check_var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                        /* get and check nest tableof's depth */
                        int depth = get_nest_tableof_layer(check_var, (yyvsp[-8].varname)->name, errstate);
                        PLpgSQL_type *nest_type = plpgsql_build_nested_datatype();
                        nest_type->tableOfIndexType = INT4OID;
                        nest_type->collectionType = PLPGSQL_COLLECTION_ARRAY;
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_varrayType((yyvsp[-8].varname)->name, (yyvsp[-8].varname)->lineno, nest_type, true);
                        /* nested table type */
                        var->nest_table = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                        List *old_nest_typenames = var->nest_table && var->nest_table->nest_typnames ? var->nest_table->nest_typnames : NULL;
                        var->nest_typnames = get_current_type_nest_type(old_nest_typenames, var->refname, true);
                        var->nest_layers = depth;
                        var->isIndexByTblOf = false;
                        pfree_ext((yyvsp[-8].varname)->name);
                        pfree((yyvsp[-8].varname));
                    }
#line 3885 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 1593 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("table type nested by array is not supported yet."),
                                errdetail("Define array type \"%s\" of table type is not supported yet.", (yyvsp[-8].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of array type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 3900 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 1605 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("ref cursor type nested by array is not supported yet."),
                                errdetail("Define array type \"%s\" of ref cursor type is not supported yet.", (yyvsp[-8].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of array type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 3915 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 1617 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ListCell* lc = NULL;
                        /* build declare variable list */
                        foreach(lc, (yyvsp[-3].list))
                        {
                            VarName *varname = (VarName*) lfirst(lc);
                            IsInPublicNamespace(varname->name);

                            PLpgSQL_type *var_type = ((PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)])->datatype;
                            PLpgSQL_var *varray_type = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                            PLpgSQL_var *newp;
                            PLpgSQL_type *new_var_type;

                            new_var_type = build_array_type_from_elemtype(var_type);
                            new_var_type->collectionType = var_type->collectionType;
                            new_var_type->tableOfIndexType = var_type->tableOfIndexType;
                            newp = (PLpgSQL_var *)plpgsql_build_variable(varname->name, varname->lineno, new_var_type, true);
                            newp->isconst = (yyvsp[-2].boolean);
                            newp->default_val = (yyvsp[0].expr);
                            newp->nest_typnames = varray_type->nest_typnames;
                            if (newp->default_val != NULL) {
                                checkTypeName(newp->default_val->nest_typnames, varray_type->nest_typnames);
                            }

                            if (NULL == newp) {
                                const char* message = "build variable failed";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                    (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                        errmsg("build variable failed")));
                                u_sess->plsql_cxt.have_error = true;
                            }
                            if (varray_type->nest_table != NULL) {
                                newp->nest_table = plpgsql_build_nested_variable(varray_type->nest_table, (yyvsp[-2].boolean), varname->name, varname->lineno);
                                newp->nest_layers = varray_type->nest_layers;
                            }
                            pfree_ext(varname->name);
                        }
                        list_free_deep((yyvsp[-3].list));
                    }
#line 3960 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 1658 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(ERROR,
                            (errmodule(MOD_PLSQL),
                                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("table of is not support in distribute database"),
                                errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                erraction("Modify SQL statement according to the manual.")));
#endif
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                            ereport(ERROR,
                                (errmodule(MOD_PLSQL),
                                    errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("table of is only support in A-format database."),
                                    errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                    erraction("Modify SQL statement according to the manual.")));
                        }
                        IsInPublicNamespace((yyvsp[-6].varname)->name);

                        (yyvsp[-2].dtype)->collectionType = PLPGSQL_COLLECTION_TABLE;
                        (yyvsp[-2].dtype)->tableOfIndexType = InvalidOid;
                        if((yyvsp[-2].dtype)->typinput.fn_oid == F_ARRAY_IN) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or table type nested by table type is not supported yet."),
                                    errdetail("Define table type \"%s\" of array or table type is not supported yet.", (yyvsp[-6].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        if((yyvsp[-2].dtype)->typoid == REFCURSOROID) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("ref cursor type nested by table type is not supported yet."),
                                    errdetail("Define table type \"%s\" of ref cursor type is not supported yet.", (yyvsp[-6].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-6].varname)->name, (yyvsp[-6].varname)->lineno, (yyvsp[-2].dtype), true);
                        var->nest_typnames = get_current_type_nest_type(var->nest_typnames, var->refname, true);
                        if (IS_PACKAGE) {
                            plpgsql_build_package_array_type((yyvsp[-6].varname)->name, (yyvsp[-2].dtype)->typoid, TYPCATEGORY_TABLEOF, (yyvsp[-2].dtype)->dependExtend);
                        } else if (enable_plpgsql_gsdependency()) {
                            gsplsql_build_gs_type_in_body_dependency((yyvsp[-2].dtype));
                        } else {
                            plpgsql_build_func_array_type((yyvsp[-6].varname)->name, (yyvsp[-2].dtype)->typoid, TYPCATEGORY_TABLEOF, (yyvsp[-2].dtype)->atttypmod, (yyvsp[-2].dtype)->dependExtend);
                        }
                        pfree_ext((yyvsp[-6].varname)->name);
						pfree((yyvsp[-6].varname));
                    }
#line 4018 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 1712 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-6].varname)->name);
                        PLpgSQL_var *check_var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];
                        /* get and check nest tableof's depth */
                        int depth = get_nest_tableof_layer(check_var, (yyvsp[-6].varname)->name, errstate);
                        PLpgSQL_type *nest_type = plpgsql_build_nested_datatype();
                        nest_type->tableOfIndexType = INT4OID;
                        nest_type->collectionType = PLPGSQL_COLLECTION_TABLE;
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-6].varname)->name, (yyvsp[-6].varname)->lineno, nest_type, true);
                        /* nested table type */
                        var->nest_table = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];
                        List *old_nest_typenames = var->nest_table && var->nest_table->nest_typnames ? var->nest_table->nest_typnames : NULL;
                        var->nest_typnames = get_current_type_nest_type(old_nest_typenames, var->refname, true);
                        var->nest_layers = depth;
                        var->isIndexByTblOf = false;
                        pfree_ext((yyvsp[-6].varname)->name);
						pfree((yyvsp[-6].varname));
                    }
#line 4041 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 1731 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array or record type nesting is not supported in distributed database yet."),
                                errdetail("Define table type \"%s\" of record is not supported in distributed database yet.", (yyvsp[-6].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
#endif
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                            ereport(ERROR,
                                (errmodule(MOD_PLSQL),
                                    errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("table of is only support in A-format database."),
                                    errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                    erraction("Modify SQL statement according to the manual.")));
                        }

                        if (IS_ANONYMOUS_BLOCK) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or record type nesting is not supported in anonymous block yet."),
                                    errdetail("Define table type \"%s\" of record is not supported yet.", (yyvsp[-6].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        IsInPublicNamespace((yyvsp[-6].varname)->name);

                        PLpgSQL_type *newp = NULL;
                        newp = build_type_from_record_var((yyvsp[-2].ival), (yylsp[-2]));
                        newp->collectionType = PLPGSQL_COLLECTION_TABLE;
                        newp->tableOfIndexType = InvalidOid;
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-6].varname)->name, (yyvsp[-6].varname)->lineno, newp, true);
                        PLpgSQL_rec_type* rec_var = (PLpgSQL_rec_type*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];
                        var->nest_typnames = get_current_type_nest_type(rec_var->nest_typnames, var->refname, true);
                        if (IS_PACKAGE) {
                            plpgsql_build_package_array_type((yyvsp[-6].varname)->name, newp->typoid, TYPCATEGORY_TABLEOF);
                        } else if (enable_plpgsql_gsdependency()) {
                            int i;
                            for (i = 0; i < rec_var->attrnum; i++) {
                                gsplsql_build_gs_type_in_body_dependency(rec_var->types[i]);
                            }
                        }
                        pfree_ext((yyvsp[-6].varname)->name);
						pfree((yyvsp[-6].varname));
                    }
#line 4097 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 1784 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array type nested by table type is not supported yet."),
                                errdetail("Define table type \"%s\" of array is not supported yet.", (yyvsp[-6].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 4112 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 1796 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("ref cursor type nested by table type is not supported yet."),
                                errdetail("Define table type \"%s\" of ref cursor is not supported yet.", (yyvsp[-6].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 4127 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 1808 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array type nested by table type is not supported yet."),
                                errdetail("Define table type \"%s\" of array is not supported yet.", (yyvsp[-9].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 4142 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 1820 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("ref cursor type nested by table type is not supported yet."),
                                errdetail("Define table type \"%s\" of ref cursor type is not supported yet.", (yyvsp[-9].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
                    }
#line 4157 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 1832 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(ERROR,
                            (errmodule(MOD_PLSQL),
                                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("table of is not support in distribute database"),
                                errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                erraction("Modify SQL statement according to the manual.")));
#endif
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                            ereport(ERROR,
                                (errmodule(MOD_PLSQL),
                                    errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("table of is only support in A-format database."),
                                    errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                    erraction("Modify SQL statement according to the manual.")));
                        }
                        IsInPublicNamespace((yyvsp[-9].varname)->name);

                        (yyvsp[-5].dtype)->collectionType = PLPGSQL_COLLECTION_TABLE;
                        if ((yyvsp[-1].dtype)->typoid != VARCHAROID && (yyvsp[-1].dtype)->typoid != INT4OID) {
                            const char* message = "unsupported table index type";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("unsupported table index type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        (yyvsp[-5].dtype)->tableOfIndexType = (yyvsp[-1].dtype)->typoid;
                        if((yyvsp[-5].dtype)->typinput.fn_oid == F_ARRAY_IN) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or table type nested by table type is not supported yet."),
                                    errdetail("Define table type \"%s\" of array or table type is not supported yet.", (yyvsp[-9].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        if((yyvsp[-5].dtype)->typoid == REFCURSOROID) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("ref cursor type nested by table type is not supported yet."),
                                    errdetail("Define table type \"%s\" of ref cursor type is not supported yet.", (yyvsp[-9].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-9].varname)->name, (yyvsp[-9].varname)->lineno, (yyvsp[-5].dtype), true);
                        var->isIndexByTblOf = true;

                        if (IS_PACKAGE) {
                            if ((yyvsp[-1].dtype)->typoid == VARCHAROID) {
                                plpgsql_build_package_array_type((yyvsp[-9].varname)->name, (yyvsp[-5].dtype)->typoid, TYPCATEGORY_TABLEOF_VARCHAR, (yyvsp[-5].dtype)->dependExtend);
                            } else {
                                plpgsql_build_package_array_type((yyvsp[-9].varname)->name, (yyvsp[-5].dtype)->typoid, TYPCATEGORY_TABLEOF_INTEGER, (yyvsp[-5].dtype)->dependExtend);
                            }
                        } else if (enable_plpgsql_gsdependency()) {
                            gsplsql_build_gs_type_in_body_dependency((yyvsp[-5].dtype));
                        } else {
                            if ((yyvsp[-1].dtype)->typoid == VARCHAROID) {
                                plpgsql_build_func_array_type((yyvsp[-9].varname)->name, (yyvsp[-5].dtype)->typoid, TYPCATEGORY_TABLEOF_VARCHAR, (yyvsp[-5].dtype)->atttypmod, (yyvsp[-5].dtype)->dependExtend);
                            } else {
                                plpgsql_build_func_array_type((yyvsp[-9].varname)->name, (yyvsp[-5].dtype)->typoid, TYPCATEGORY_TABLEOF_INTEGER, (yyvsp[-5].dtype)->atttypmod, (yyvsp[-5].dtype)->dependExtend);
                            }
                        }
                        pfree_ext((yyvsp[-9].varname)->name);
						pfree((yyvsp[-9].varname));
                    }
#line 4232 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 1903 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-9].varname)->name);

                        if ((yyvsp[-1].dtype)->typoid != VARCHAROID && (yyvsp[-1].dtype)->typoid != INT4OID) {
                            const char* message = "unsupported table index type";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("unsupported table index type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_var *check_var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-5].ival)];
                        /* get and check nest tableof's depth */
                        int depth = get_nest_tableof_layer(check_var, (yyvsp[-9].varname)->name, errstate);
                        PLpgSQL_type *nest_type = plpgsql_build_nested_datatype();
                        nest_type->tableOfIndexType = (yyvsp[-1].dtype)->typoid;
                        nest_type->collectionType = PLPGSQL_COLLECTION_TABLE;
                        PLpgSQL_var* var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-9].varname)->name, (yyvsp[-9].varname)->lineno, nest_type, true);
                        /* nested table type */
                        var->nest_table = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-5].ival)];
                        var->nest_layers = depth;
                        var->isIndexByTblOf = true;
                        pfree_ext((yyvsp[-9].varname)->name);
						pfree((yyvsp[-9].varname));
                    }
#line 4262 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 1929 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array or record type nesting is not supported in distributed database yet."),
                                errdetail("Define table type \"%s\" of record is not supported in distributed database yet.", (yyvsp[-9].varname)->name),
                                errcause("feature not supported"),
                                erraction("check define of table type")));
                        u_sess->plsql_cxt.have_error = true;
#endif
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                            ereport(ERROR,
                                (errmodule(MOD_PLSQL),
                                    errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmsg("table of is only support in A-format database."),
                                    errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                    erraction("Modify SQL statement according to the manual.")));
                        }
                        if (IS_ANONYMOUS_BLOCK) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or record type nesting is not supported in anonymous block yet."),
                                    errdetail("Define table type \"%s\" of record is not supported yet.", (yyvsp[-9].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        IsInPublicNamespace((yyvsp[-9].varname)->name);

                        PLpgSQL_type *newp = NULL;
                        newp = build_type_from_record_var((yyvsp[-5].ival), (yylsp[-5]));
                        newp->collectionType = PLPGSQL_COLLECTION_TABLE;

                        if ((yyvsp[-1].dtype)->typoid != VARCHAROID && (yyvsp[-1].dtype)->typoid != INT4OID) {
                            ereport(errstate,
                                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("unsupported table index type")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        newp->tableOfIndexType = (yyvsp[-1].dtype)->typoid;
                        PLpgSQL_var *var = (PLpgSQL_var*)plpgsql_build_tableType((yyvsp[-9].varname)->name, (yyvsp[-9].varname)->lineno, newp, true);
                        var->isIndexByTblOf = true;

                        if (IS_PACKAGE) {
                            if ((yyvsp[-1].dtype)->typoid == VARCHAROID) {
                                plpgsql_build_package_array_type((yyvsp[-9].varname)->name, newp->typoid, TYPCATEGORY_TABLEOF_VARCHAR);
                            } else {
                                plpgsql_build_package_array_type((yyvsp[-9].varname)->name, newp->typoid, TYPCATEGORY_TABLEOF_INTEGER);
                            }
                        } else if (enable_plpgsql_gsdependency()) {
                            int i;
                            PLpgSQL_rec_type* rec_var = (PLpgSQL_rec_type*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-5].ival)];
                            for (i = 0; i < rec_var->attrnum; i++) {
                                gsplsql_build_gs_type_in_body_dependency(rec_var->types[i]);
                            }
                        } else {
                            if ((yyvsp[-1].dtype)->typoid == VARCHAROID) {
                                plpgsql_build_func_array_type((yyvsp[-9].varname)->name, newp->typoid, TYPCATEGORY_TABLEOF_VARCHAR, (yyvsp[-1].dtype)->atttypmod, (yyvsp[-1].dtype)->dependExtend);
                            } else {
                                plpgsql_build_func_array_type((yyvsp[-9].varname)->name, newp->typoid, TYPCATEGORY_TABLEOF_INTEGER, (yyvsp[-1].dtype)->atttypmod, (yyvsp[-1].dtype)->dependExtend);
                            }
                        }
                        pfree_ext((yyvsp[-9].varname)->name);
						pfree((yyvsp[-9].varname));
                    }
#line 4335 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 1999 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ListCell* lc = NULL;
                        /* build variable list*/
                        foreach(lc, (yyvsp[-3].list))
                        {
                            VarName* varname = (VarName*) lfirst(lc);
                            IsInPublicNamespace(varname->name);

                            PLpgSQL_type *var_type = ((PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)])->datatype;
                            PLpgSQL_var *table_type = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];

                            PLpgSQL_var *newp;
                            PLpgSQL_type *new_var_type;
                            new_var_type = build_array_type_from_elemtype(var_type);

                            new_var_type->collectionType = var_type->collectionType;
                            new_var_type->tableOfIndexType = var_type->tableOfIndexType;

                            newp = (PLpgSQL_var *)plpgsql_build_variable(varname->name, varname->lineno, new_var_type, true);
                            if (NULL == newp) {
                                const char* message = "build variable failed";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                         errmsg("build variable failed")));
                                u_sess->plsql_cxt.have_error = true;
                            }
                            newp->isconst = (yyvsp[-2].boolean);
                            newp->default_val = (yyvsp[0].expr);
                            newp->nest_typnames = table_type->nest_typnames;
                            if (newp->default_val != NULL) {
                                checkTypeName(newp->default_val->nest_typnames, table_type->nest_typnames);
                            }
                            if (table_type->nest_table != NULL) {
                                newp->nest_table = plpgsql_build_nested_variable(table_type->nest_table, (yyvsp[-2].boolean), varname->name, varname->lineno);
                                newp->nest_layers = table_type->nest_layers;
                            }
                            pfree_ext(varname->name);
                        }
                        list_free_deep((yyvsp[-3].list));
                    }
#line 4381 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 2041 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        IsInPublicNamespace((yyvsp[-6].varname)->name);
                        PLpgSQL_rec_type	*newp = NULL;

                        newp = plpgsql_build_rec_type((yyvsp[-6].varname)->name, (yyvsp[-6].varname)->lineno, (yyvsp[-2].list), true);

                        PLpgSQL_nest_type *new_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        new_ntype->typname = pstrdup(newp->typname);
                        new_ntype->layer = 0;
                        new_ntype->index = 1;
                        newp->nest_typnames = lappend(newp->nest_typnames, new_ntype);

                        if (NULL == newp) {
                            const char* message = "build variable failed";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                     errmsg("build variable failed")));
                            u_sess->plsql_cxt.have_error = true;
                        }
                        if (IS_PACKAGE) {
                            newp->typoid = plpgsql_build_package_record_type((yyvsp[-6].varname)->name, (yyvsp[-2].list), true);
                        } else if (enable_plpgsql_gsdependency()) {
                            ListCell* cell =  NULL;
                            foreach(cell, (yyvsp[-2].list)) {
                                gsplsql_build_gs_type_in_body_dependency(((PLpgSQL_rec_attr*)lfirst(cell))->type);
                            }
                        } else {
                            PLpgSQL_type* rec_comptype = build_type_from_record_var(newp->dno, (yylsp[-2]), true);
                            newp->typoid = rec_comptype->typoid;
                        }
                        pfree_ext((yyvsp[-6].varname)->name);
						pfree((yyvsp[-6].varname));
                    }
#line 4420 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 2076 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ListCell* lc = NULL;
                        foreach(lc, (yyvsp[-2].list))
                        {
                            VarName *varname = (VarName*) lfirst(lc);
                            IsInPublicNamespace(varname->name);

                            PLpgSQL_var *newp = NULL;
                            PLpgSQL_type * var_type = (PLpgSQL_type *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];

                            newp = (PLpgSQL_var *)plpgsql_build_variable(varname->name,varname->lineno,
                                                                                var_type,true);
                            if (NULL == newp) {
                                const char* message = "build variable failed";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                                         errmsg("build variable failed")));
                                u_sess->plsql_cxt.have_error = true;
                            }
                            if ((yyvsp[0].expr) != NULL) {
                                PLpgSQL_row* row = (PLpgSQL_row*)newp;
                                row->default_val = (yyvsp[0].expr);
                                checkTypeName((yyvsp[0].expr)->nest_typnames, row->nest_typnames);
                            }
                            pfree_ext(varname->name);
                        }
                        list_free_deep((yyvsp[-2].list));
                    }
#line 4454 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 2105 "src/pltsql/gram.y" /* yacc.c:1646  */
    {u_sess->parser_cxt.is_procedure=false;}
#line 4460 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 2106 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        DefElem *def = (yyvsp[0].def);
                        bool oldisCreateFunction = u_sess->plsql_cxt.isCreateFunction;
                        u_sess->plsql_cxt.procedure_start_line = GetLineNumber(u_sess->plsql_cxt.curr_compile_context->core_yy->scanbuf, (yylsp[-2]));
                        u_sess->plsql_cxt.plpgsql_yylloc = (yylsp[-2]);
                        u_sess->plsql_cxt.isCreateFunction = true;
                        set_is_create_pkg_function(true);
                        raw_parse_package_function(def->defname, def->location, def->begin_location);
                        set_is_create_pkg_function(false);
                        u_sess->plsql_cxt.isCreateFunction = oldisCreateFunction;
                        u_sess->plsql_cxt.procedure_start_line = 0;
                        u_sess->plsql_cxt.plpgsql_yylloc = 0;
                    }
#line 4478 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 2119 "src/pltsql/gram.y" /* yacc.c:1646  */
    {u_sess->parser_cxt.is_procedure=true;}
#line 4484 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 2120 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        DefElem *def = (yyvsp[0].def);
                        bool oldisCreateFunction = u_sess->plsql_cxt.isCreateFunction;
                        u_sess->plsql_cxt.procedure_start_line = GetLineNumber(u_sess->plsql_cxt.curr_compile_context->core_yy->scanbuf, (yylsp[-2]));
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        u_sess->plsql_cxt.isCreateFunction = true;
                        set_is_create_pkg_function(true);
                        raw_parse_package_function(def->defname, def->location, def->begin_location);
                        set_is_create_pkg_function(false);
                        u_sess->plsql_cxt.isCreateFunction = oldisCreateFunction;
                        u_sess->plsql_cxt.procedure_start_line = 0;
                        u_sess->plsql_cxt.plpgsql_yylloc = 0;
                    }
#line 4502 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 2134 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_func != NULL &&
                            OidIsValid(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_oid)) {
                                ereport(ERROR,
                                    (errmodule(MOD_PLSQL),
                                        errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("Subprogram autonomous transaction is not supported."),
                                        errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                        erraction("Modify SQL statement according to the manual.")));
                        }
                        if (pg_strcasecmp((yyvsp[-1].str), "autonomous_transaction") == 0) {
                            u_sess->plsql_cxt.pragma_autonomous = true;
                            if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile !=NULL) {
                                u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_autonomous = true;
                            }
                        }
                        else {
                            elog(errstate, "invalid pragma");
                            u_sess->plsql_cxt.have_error = true;
                        }
                    }
#line 4529 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 2157 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_func != NULL &&
                            OidIsValid(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_oid)) {
                                ereport(ERROR,
                                    (errmodule(MOD_PLSQL),
                                        errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                        errmsg("Subprogram autonomous transaction is not supported."),
                                        errdetail("N/A"), errcause("PL/SQL uses unsupported feature."),
                                        erraction("Modify SQL statement according to the manual.")));
                        }
                        if (pg_strcasecmp((yyvsp[-6].str), "exception_init") == 0) {
                            if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
                                plpgsql_set_variable((yyvsp[-4].str), (yyvsp[-2].ival));
                            }
                        } else {
                            elog(errstate, "invalid pragma.");
                            u_sess->plsql_cxt.have_error = true;
                        }
                    }
#line 4554 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 2178 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("SUBTYPE is only supported in database which dbcompatibility='A'")));
                        }
                        IsInPublicNamespace((yyvsp[-4].varname)->name);
                        PLpgSQL_type *newp = (yyvsp[-2].dtype);
                        Oid newtypeoid = InvalidOid;

                        if (PLPGSQL_TTYPE_ROW == newp->ttype) {
                            /* handle when basetype is ROW */
                            plpgsql_build_synonym((yyvsp[-4].varname)->name, newp->typname);
                        } else if (F_ARRAY_IN == newp->typinput.fn_oid) {
                            /* collection handle in next project */
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or table type nested by table type is not supported yet."),
                                    errdetail("Define table type \"%s\" of array or table type is not supported yet.", (yyvsp[-4].varname)->name),
                                    errcause("feature not supported"),
                                    erraction("check define of table type")));
                            u_sess->plsql_cxt.have_error = true;
                        } else {
                            /* basstype is not a row, build subtype, save constraint in plsql cache */
                            if (IS_ANONYMOUS_BLOCK) {
                                newtypeoid = plpgsql_build_anonymous_subtype((yyvsp[-4].varname)->name, newp, NULL, (yyvsp[-1].boolean));
                            } else {
                                newtypeoid = plpgsql_build_function_package_subtype((yyvsp[-4].varname)->name, newp, NULL, (yyvsp[-1].boolean));
                            }

                            pfree_ext((yyvsp[-4].varname)->name);
                            pfree_ext((yyvsp[-4].varname));
                        }
                    }
#line 4594 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 2214 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* copy all params if basetype is record, build a new record */
                        List* attr_list = NIL;
                        PLpgSQL_type* newp = (PLpgSQL_type*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                        PLpgSQL_rec_type* old = (PLpgSQL_rec_type*)newp;
                        for (int i = 0; i < old->attrnum; i++) {
                            PLpgSQL_rec_attr* attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));
                            attr->attrname = pstrdup(old->attrnames[i]);
                            attr->type = old->types[i];
                            attr->notnull = old->notnulls[i];
                            attr->defaultvalue = old->defaultvalues[i];
                            attr_list = lappend(attr_list, attr);
                        }
                        if (attr_list == NIL) {
                            elog(errstate, "Execption when defining subtype");
                            u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_rec_type* rec = NULL;
                        rec = plpgsql_build_rec_type((yyvsp[-3].varname)->name, (yyvsp[-3].varname)->lineno, attr_list, true);
                        if (IS_PACKAGE) {
                            rec->typoid = plpgsql_build_package_record_type((yyvsp[-3].varname)->name, attr_list, true);
                        } else if (enable_plpgsql_gsdependency()){
                            ListCell* cell =  NULL;
                            foreach(cell, attr_list) {
                                gsplsql_build_gs_type_in_body_dependency(((PLpgSQL_rec_attr*)lfirst(cell))->type);
                            }
                        }
                        list_free_deep(attr_list);
                    }
#line 4628 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 2244 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* SUBTYPE basse type is table/varray type */
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT){
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("SUBTYPE is only supported in database which dbcompatibility='A'")));
                        } 
                        IsInPublicNamespace((yyvsp[-3].varname)->name);
                        PLpgSQL_var *arrarytype = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].ival)];
                        PLpgSQL_type *newp = arrarytype->datatype;
                        plpgsql_build_varrayType((yyvsp[-3].varname)->name, (yyvsp[-3].varname)->lineno, newp, true);
                        if (IS_PACKAGE) {
                            plpgsql_build_package_array_type((yyvsp[-3].varname)->name, newp->typoid, TYPCATEGORY_ARRAY, newp->dependExtend);
                        } else if (enable_plpgsql_gsdependency()) {
                            gsplsql_build_gs_type_in_body_dependency(newp);
                        }
                        pfree_ext((yyvsp[-3].varname)->name);
                        pfree_ext((yyvsp[-3].varname));
                    }
#line 4652 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 2266 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    ereport(ERROR,
                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmodule(MOD_PLSQL),
                            errmsg("ref cursor type nested by record is not supported yet."),
                            errdetail("Define record type of ref cursor type is not supported yet."),
                            errcause("feature not supported"),
                            erraction("check define of record type")));
                    u_sess->plsql_cxt.have_error = true;
                }
#line 4667 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 2277 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    (yyval.dtype) = (yyvsp[0].dtype);
                }
#line 4675 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 2284 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    (yyval.ival) = (yyvsp[0].ival);
                }
#line 4683 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 2288 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    (yyval.ival) = (yyvsp[0].ival);
                }
#line 4691 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 2295 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    (yyval.ival) = (yyvsp[0].ival);
                 }
#line 4699 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 2299 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                    (yyval.ival) = -(yyvsp[0].ival);
                 }
#line 4707 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 2304 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                     (yyval.def) = get_proc_str(yychar);
                     yyclearin;
                 }
#line 4716 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 2310 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                     (yyval.str) = get_init_proc(yychar);
                     yyclearin;
                 }
#line 4725 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 2319 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = list_make1((yyvsp[0].recattr));
                        if ((yyvsp[0].recattr)->cur_ntype) {
                            (yyvsp[0].recattr)->cur_ntype->index = (yyval.list)->length;
                            (yyvsp[0].recattr)->nest_typnames = lappend((yyvsp[0].recattr)->nest_typnames, (yyvsp[0].recattr)->cur_ntype);
                        }
                   }
#line 4737 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 2327 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].recattr));
                        if ((yyvsp[0].recattr)->cur_ntype) {
                            (yyvsp[0].recattr)->cur_ntype->index = (yyval.list)->length;
                            (yyvsp[0].recattr)->nest_typnames = lappend((yyvsp[0].recattr)->nest_typnames, (yyvsp[0].recattr)->cur_ntype);
                        }
                    }
#line 4749 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 2337 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_rec_attr	*attr = NULL;
                        if((yyvsp[-2].dtype)->typoid == REFCURSOROID) {
                            ereport(ERROR,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("ref cursor type nested by record is not supported yet."),
                                    errdetail("Define record type of ref cursor type is not supported yet."),
                                    errcause("feature not supported"),
                                    erraction("check define of record type")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));

                        attr->attrname = (yyvsp[-3].str);
                        attr->type = (yyvsp[-2].dtype);

                        attr->notnull = (yyvsp[-1].boolean);
                        attr->cur_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        attr->cur_ntype->index = -1;
                        List* nest_typnames = NIL;
                        attr->nest_typnames = search_external_nest_type(attr->type->typname, attr->type->typoid, 0, nest_typnames, attr->cur_ntype);
                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (attr->type->ttype == PLPGSQL_TTYPE_SCALAR)
                                attr->defaultvalue = (yyvsp[0].expr);
                            else {
                                const char* message = "default value for row or record variable is not supported";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                            }
                        }

                        if ((yyvsp[-1].boolean) && (yyvsp[0].expr) == NULL) {
                            const char* message = "variables must have default value";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("variables declared as NOT NULL must have "
                                "a default value.")));
                        }

                        (yyval.recattr) = attr;
                    }
#line 4802 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 2386 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifdef ENABLE_MULTIPLE_NODES
                        ereport(errstate,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("array or record type nesting is not supported in distributed database yet."),
                                errdetail("Define a record type of record is not supported in distributed database yet."),
                                errcause("feature not supported"),
                                erraction("check define of record type")));
                        u_sess->plsql_cxt.have_error = true;
#endif
                        if (IS_ANONYMOUS_BLOCK) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                    errmodule(MOD_PLSQL),
                                    errmsg("array or record type nesting is not supported in anonymous block yet."),
                                    errdetail("Define a record type of record is not supported yet."),
                                    errcause("feature not supported"),
                                    erraction("check define of record type")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        PLpgSQL_rec_attr	*attr = NULL;

                        attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));

                        attr->attrname = (yyvsp[-3].str);
                        attr->type = build_type_from_record_var((yyvsp[-2].ival), (yylsp[-2]));
                        PLpgSQL_rec_type *var_type = (PLpgSQL_rec_type *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];
                        attr->cur_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        attr->cur_ntype->typname = pstrdup(var_type->typname);
                        attr->cur_ntype->layer = 1;
                        if (var_type->nest_typnames != NIL) {
                            attr->nest_typnames = get_current_type_nest_type(var_type->nest_typnames, NULL, false);
                        }
                        
                        attr->notnull = (yyvsp[-1].boolean);
                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (attr->type->ttype == PLPGSQL_TTYPE_SCALAR)
                                attr->defaultvalue = (yyvsp[0].expr);
                            else {
                                ereport(errstate,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                                u_sess->plsql_cxt.have_error = true;
                            }
                        }

                        if ((yyvsp[-1].boolean) && (yyvsp[0].expr) == NULL) {
                            ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("variables declared as NOT NULL must have "
                                "a default value.")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        (yyval.recattr) = attr;
                    }
#line 4867 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 2447 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        ereport(ERROR,
                            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmodule(MOD_PLSQL),
                                errmsg("ref cursor type nested by record is not supported yet."),
                                errdetail("Define a record type of ref cursor type is not supported yet."),
                                errcause("feature not supported"),
                                erraction("check define of record type")));
                        u_sess->plsql_cxt.have_error = true;
                  }
#line 4882 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 2458 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_rec_attr        *attr = NULL;

                        attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));

                        attr->attrname = (yyvsp[-3].str);

                        PLpgSQL_type *var_type = ((PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)])->datatype;
                        PLpgSQL_var *varray_type = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];

                        if (varray_type->nest_table != NULL) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("nested table of type is not supported as record type attribute"),
                                parser_errposition((yylsp[-1]))));
                                u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_type *new_var_type = build_array_type_from_elemtype(var_type);
                        new_var_type->collectionType = var_type->collectionType;
                        new_var_type->tableOfIndexType = var_type->tableOfIndexType;
                        attr->type = new_var_type;
                        attr->notnull = (yyvsp[-1].boolean);
                        attr->cur_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        attr->cur_ntype->typname = pstrdup(varray_type->refname);
                        attr->cur_ntype->layer = 1;
                        if (varray_type->nest_typnames != NIL) {
                            attr->nest_typnames = get_current_type_nest_type(varray_type->nest_typnames, NULL, false);
                        }

                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (attr->type->ttype == PLPGSQL_TTYPE_SCALAR)
                                attr->defaultvalue = (yyvsp[0].expr);
                            else {
                                ereport(errstate,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                                u_sess->plsql_cxt.have_error = true;
                            }
                        }

                        if ((yyvsp[-1].boolean) && (yyvsp[0].expr) == NULL) {
                            ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("variables declared as NOT NULL must have "
                                "a default value.")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        (yyval.recattr) = attr;
                    }
#line 4939 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 2511 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_rec_attr        *attr = NULL;

                        attr = (PLpgSQL_rec_attr*)palloc0(sizeof(PLpgSQL_rec_attr));

                        attr->attrname = (yyvsp[-3].str);

                        PLpgSQL_type *var_type = ((PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)])->datatype;
                        PLpgSQL_var *table_type = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-2].ival)];

                        if (table_type->nest_table != NULL) {
                            ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("nested table of type is not supported as record type attribute"),
                                parser_errposition((yylsp[-1]))));
                                u_sess->plsql_cxt.have_error = true;
                        }
                        PLpgSQL_type *new_var_type = build_array_type_from_elemtype(var_type);
                        new_var_type->collectionType = var_type->collectionType;
                        new_var_type->tableOfIndexType = var_type->tableOfIndexType;
                        attr->type = new_var_type;
                        attr->notnull = (yyvsp[-1].boolean);
                        attr->cur_ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
                        attr->cur_ntype->typname = pstrdup(table_type->refname);
                        attr->cur_ntype->layer = 1;
                        if (table_type->nest_typnames != NIL) {
                            attr->nest_typnames = get_current_type_nest_type(table_type->nest_typnames, NULL, false);
                        }

                        if ((yyvsp[0].expr) != NULL)
                        {
                            if (attr->type->ttype == PLPGSQL_TTYPE_SCALAR)
                                attr->defaultvalue = (yyvsp[0].expr);
                            else {
                                ereport(errstate,
                                        (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                         errmsg("default value for row or record variable is not supported"),
                                         parser_errposition((yylsp[-1]))));
                                u_sess->plsql_cxt.have_error = true;
                            }
                        }

                        if ((yyvsp[-1].boolean) && (yyvsp[0].expr) == NULL) {
                            ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("variables declared as NOT NULL must have "
                                "a default value.")));
                            u_sess->plsql_cxt.have_error = true;
                        }

                        (yyval.recattr) = attr;
                    }
#line 4996 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 2567 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = 0;
                    }
#line 5004 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 2571 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = CURSOR_OPT_NO_SCROLL;
                    }
#line 5012 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 2575 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = CURSOR_OPT_SCROLL;
                    }
#line 5020 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 2581 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok;
                        tok = yylex();
                        pltsql_push_back_token(tok);

                        /* check cursor syntax, cursor query only accept select query */
                        {
                            (yyval.expr) = read_sql_stmt("");
                        }
                    }
#line 5035 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 2594 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.datum) = NULL;
                    }
#line 5043 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 2598 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_row *newp;
                        int i;
                        ListCell *l;

                        newp = (PLpgSQL_row *)palloc0(sizeof(PLpgSQL_row));
                        newp->dtype = PLPGSQL_DTYPE_ROW;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->rowtupdesc = NULL;
                        newp->nfields = list_length((yyvsp[-1].list));
                        newp->fieldnames = (char **)palloc(newp->nfields * sizeof(char *));
                        newp->argDefExpr = (PLpgSQL_expr**)palloc(newp->nfields * sizeof(PLpgSQL_expr*));
                        newp->varnos = (int *)palloc(newp->nfields * sizeof(int));
                        newp->isImplicit = true;
                        newp->addNamespace = false;
                        i = 0;
                        newp->needValDno = 0;
                        foreach (l, (yyvsp[-1].list))
                        {
                            PLpgSQL_variable *arg = (PLpgSQL_variable *) lfirst(l);
                            newp->fieldnames[i] = arg->refname;
                            newp->varnos[i] = arg->dno;
                            newp->argDefExpr[i] = arg->dtype == PLPGSQL_DTYPE_VAR ? ((PLpgSQL_var*)arg)->default_val :
                                                  arg->dtype == PLPGSQL_DTYPE_ROW ? ((PLpgSQL_row*)arg)->default_val :
                                                  arg->dtype == PLPGSQL_DTYPE_RECORD ? ((PLpgSQL_row*)arg)->default_val :
                                                  arg->dtype == PLPGSQL_DTYPE_REC ? ((PLpgSQL_rec*)arg)->default_val: NULL ;
                            if(newp->argDefExpr[i] == NULL)
                                newp->needValDno = i+1;
                            i++;
                        }
                        list_free_ext((yyvsp[-1].list));

                        plpgsql_adddatum((PLpgSQL_datum *) newp);
                        (yyval.datum) = (PLpgSQL_datum *) newp;
                    }
#line 5083 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 2636 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = list_make1((yyvsp[0].datum));
                    }
#line 5091 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 2640 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].datum));
                    }
#line 5099 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 2646 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_variable* arg = 
                                plpgsql_build_variable((yyvsp[-3].varname)->name, (yyvsp[-3].varname)->lineno, (yyvsp[-1].dtype), true);
                        if (arg != NULL) {
                            switch (arg->dtype) {
                                case PLPGSQL_DTYPE_VAR: {
                                    ((PLpgSQL_var*)arg)->default_val = (yyvsp[0].expr);
                                    break;
                                }
                                case PLPGSQL_DTYPE_ROW: 
                                case PLPGSQL_DTYPE_RECORD: {
                                    ((PLpgSQL_row*)arg)->default_val = (yyvsp[0].expr);
                                    break;
                                }
                                case PLPGSQL_DTYPE_REC: {
                                    ((PLpgSQL_rec*)arg)->default_val = (yyvsp[0].expr);
                                    break;
                                }
                                default: {
                                    ereport(ERROR,
                                        (errcode(ERRCODE_UNRECOGNIZED_NODE_TYPE),
                                            errmodule(MOD_PLSQL), 
                                            errmsg("unrecognized variable type for cursor argument.")));
                                }
                            }
                        }
                        (yyval.datum) = (PLpgSQL_datum*)arg;
                        pfree_ext((yyvsp[-3].varname)->name);
                        pfree((yyvsp[-3].varname));
                    }
#line 5134 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 123:
#line 2683 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            (yyval.dtype) = NULL;
                        }
#line 5142 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 2687 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                                const char* message = "cursor return type is only supposed in A compatibility";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor return type is only supposed in A compatibility")));
                            }
                            if ((yyvsp[0].dtype)->dtype != PLPGSQL_DTYPE_RECORD_TYPE && (yyvsp[0].dtype)->typinput.fn_oid != F_RECORD_IN) {
                                const char* message = "invalid cursor return type";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("invalid cursor return type; %s must be a record type", (yyvsp[0].dtype)->typname)));
                            }
                            (yyval.dtype) = (yyvsp[0].dtype);
                        }
#line 5164 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 2705 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                                const char* message = "cursor return type is only supposed in A compatibility";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor return type is only supposed in A compatibility")));
                            }
                            PLpgSQL_type* var_type = (PLpgSQL_type *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[0].ival)];
                            (yyval.dtype) = var_type;
                        }
#line 5180 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 2723 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_nsitem *nsi;

                        nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                (yyvsp[0].word).ident, NULL, NULL,
                                                NULL);
                        if (nsi == NULL) {
                            const char* message = "variables not exists";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_UNDEFINED_OBJECT),
                                     errmsg("variable \"%s\" does not exist",
                                            (yyvsp[0].word).ident),
                                     parser_errposition((yylsp[0]))));
                        }
                        (yyval.nsitem) = nsi;
                    }
#line 5202 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 2741 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_nsitem *nsi;

                        if (list_length((yyvsp[0].cword).idents) == 2)
                            nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                    strVal(linitial((yyvsp[0].cword).idents)),
                                                    strVal(lsecond((yyvsp[0].cword).idents)),
                                                    NULL,
                                                    NULL);
                        else if (list_length((yyvsp[0].cword).idents) == 3)
                            nsi = plpgsql_ns_lookup(plpgsql_ns_top(), false,
                                                    strVal(linitial((yyvsp[0].cword).idents)),
                                                    strVal(lsecond((yyvsp[0].cword).idents)),
                                                    strVal(lthird((yyvsp[0].cword).idents)),
                                                    NULL);
                        else
                            nsi = NULL;
                        if (nsi == NULL) {
                            int yylloc_bak = yylloc;
                            int isPkg =plpgsql_pkg_adddatum2ns((yyvsp[0].cword).idents);
                            yylloc = yylloc_bak;
                            if (isPkg < 0) {
                                const char* message = "variables not exists";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_UNDEFINED_OBJECT),
                                         errmsg("variable \"%s\" does not exist",
                                                NameListToString((yyvsp[0].cword).idents)),
                                         parser_errposition((yylsp[0]))));
                            }
                        }
                        (yyval.nsitem) = nsi;
                    }
#line 5240 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 2780 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = (yyvsp[0].word).ident;
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        /*
                         * Check to make sure name isn't already declared
                         * in the current block.
                         */
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].word).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;
                    }
#line 5260 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 2796 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].keyword));
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        /*
                         * Check to make sure name isn't already declared
                         * in the current block.
                         */
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].keyword), NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;
                    }
#line 5280 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 2812 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5297 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 2825 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5314 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 2838 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5331 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 2851 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5348 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 2864 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        if ((yyvsp[0].wdatum).idents != NIL) {
                            yyerror("syntax error");
                        }
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5368 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 2880 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        VarName* varname = NULL;
                        if ((yyvsp[0].wdatum).idents != NIL || strcmp((yyvsp[0].wdatum).ident, "bulk_exceptions") == 0) {
                            yyerror("syntax error");
                        }
                        varname = (VarName *)palloc0(sizeof(VarName));
                        varname->name = pstrdup((yyvsp[0].wdatum).ident);
                        varname->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if (plpgsql_ns_lookup(plpgsql_ns_top(), true,
                                              (yyvsp[0].wdatum).ident, NULL, NULL,
                                              NULL) != NULL)
                            yyerror("duplicate declaration");
                        (yyval.varname) = varname;

                    }
#line 5388 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 2897 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.list) = list_make1((yyvsp[0].varname)); }
#line 5394 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 2899 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
					if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
						    ereport(errstate,
                                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                 errmsg("not support declare multiple variable"),
                                 parser_errposition((yylsp[-2]))));

					(yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].varname)); 
				}
#line 5408 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 2910 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 5414 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 2912 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.boolean) = true; }
#line 5420 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 2916 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * If there's a lookahead token, read_datatype
                         * should consume it.
                         */
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.dtype) = read_datatype(yychar);
                        yyclearin;
                    }
#line 5434 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 2928 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.oid) = InvalidOid; }
#line 5440 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 2930 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.oid) = get_collation_oid(list_make1(makeString((yyvsp[0].word).ident)),
                                               false);
                    }
#line 5449 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 2935 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.oid) = get_collation_oid((yyvsp[0].cword).idents, false);
                    }
#line 5457 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 2941 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.boolean) = false; }
#line 5463 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 2943 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.boolean) = true; }
#line 5469 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 2947 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = NULL; }
#line 5475 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 2949 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.expr) = read_sql_expression(';', ";");
                    }
#line 5483 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 2955 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.expr) = NULL;
                    }
#line 5491 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 151:
#line 2959 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok;

                        (yyval.expr) = read_sql_expression2(',', ')', ")", &tok);

                        pltsql_push_back_token(tok);
                    }
#line 5503 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 156:
#line 2977 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.list) = NIL; }
#line 5509 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 2979 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 5515 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 158:
#line 2983 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            if ((yyvsp[0].stmt) == NULL)
                                (yyval.list) = (yyvsp[-1].list);
                            else
                                (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].stmt));
                        }
#line 5526 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 159:
#line 2990 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            if ((yyvsp[0].stmt) == NULL)
                                (yyval.list) = NIL;
                            else
                                (yyval.list) = list_make1((yyvsp[0].stmt));
                        }
#line 5537 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 160:
#line 3003 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 5543 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 161:
#line 3005 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5549 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 162:
#line 3007 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5555 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 163:
#line 3009 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5561 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 164:
#line 3011 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5567 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 165:
#line 3013 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5573 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 166:
#line 3015 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5579 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 167:
#line 3017 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5585 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 168:
#line 3019 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5591 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 169:
#line 3021 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5597 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 170:
#line 3023 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5603 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 171:
#line 3028 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = NULL;
                    }
#line 5611 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 172:
#line 3032 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_GOTO_LABEL, 0, "");
                        (yyval.str) = (yyvsp[-1].str);
                    }
#line 5620 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 173:
#line 3040 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * If label is not null, we record it in current execution
                         * block so that the later or former GOTO can redirect the plpgsql execution steps
                         */
                        record_stmt_label((yyvsp[-1].str), (yyvsp[0].stmt));
                        (yyval.stmt) = (yyvsp[0].stmt);
                    }
#line 5633 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 174:
#line 3050 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5639 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 175:
#line 3052 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5645 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 176:
#line 3054 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5651 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 177:
#line 3056 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5657 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 178:
#line 3058 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5663 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 179:
#line 3060 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5669 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 180:
#line 3062 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5675 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 181:
#line 3064 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5681 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 182:
#line 3066 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5687 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 183:
#line 3068 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5693 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 184:
#line 3070 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5699 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 185:
#line 3072 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5705 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 186:
#line 3074 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5711 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 187:
#line 3076 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5717 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 188:
#line 3078 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5723 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 189:
#line 3080 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5729 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 190:
#line 3082 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 5735 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 191:
#line 3085 "src/pltsql/gram.y" /* yacc.c:1646  */
    {u_sess->parser_cxt.isPerform = true;}
#line 5741 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 192:
#line 3086 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt *stmt;
                        if (enable_out_param_override() && u_sess->parser_cxt.stmt != NULL) {
                            stmt = (PLpgSQL_stmt*)u_sess->parser_cxt.stmt;
                            ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                        } else {
                            PLpgSQL_stmt_perform *newp;
                            newp = (PLpgSQL_stmt_perform *)palloc0(sizeof(PLpgSQL_stmt_perform));
                            newp->cmd_type = PLPGSQL_STMT_PERFORM;
                            newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->expr  = (yyvsp[0].expr);
                            newp->sqlString = plpgsql_get_curline_query();
                            stmt = (PLpgSQL_stmt*)newp;
                        }
                        u_sess->parser_cxt.stmt = NULL;
                        u_sess->parser_cxt.isPerform = false;
                        (yyval.stmt) = stmt;
                    }
#line 5764 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 193:
#line 3107 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_assign *newp;

                        newp = (PLpgSQL_stmt_assign *)palloc0(sizeof(PLpgSQL_stmt_assign));
                        newp->cmd_type = PLPGSQL_STMT_ASSIGN;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->varno = (yyvsp[-2].ival);
                        newp->expr  = (yyvsp[0].expr);
                        newp->sqlString = plpgsql_get_curline_query();
                        if (newp->expr) {
                            PLpgSQL_var* target = (PLpgSQL_var*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[newp->varno];
                            List* nest_typnames = NIL;
                            if (target->dtype == PLPGSQL_DTYPE_RECORD || target->dtype == PLPGSQL_DTYPE_ROW) {
                                nest_typnames = ((PLpgSQL_row*)target)->nest_typnames;
                            } else if (target->dtype == PLPGSQL_DTYPE_VAR) {
                                nest_typnames = target->nest_typnames;
                            }
                            checkTypeName(newp->expr->nest_typnames, nest_typnames);
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 5791 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 194:
#line 3130 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        cword_is_not_variable(&((yyvsp[-2].cword)), (yylsp[-2]));
                  }
#line 5799 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 195:
#line 3136 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_getdiag	 *newp;
                        ListCell		*lc;

                        newp = (PLpgSQL_stmt_getdiag *)palloc0(sizeof(PLpgSQL_stmt_getdiag));
                        newp->cmd_type = PLPGSQL_STMT_GETDIAG;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-4]));
                        newp->is_stacked = (yyvsp[-3].boolean);
                        newp->diag_items = (yyvsp[-1].list);
                        newp->has_cond = false;
                        newp->is_cond_item = false;
                        newp->cond_number = 0;
                        newp->sqlString = plpgsql_get_curline_query();

                        /*
                         * Check information items are valid for area option.
                         */
                        foreach(lc, newp->diag_items)
                        {
                            PLpgSQL_diag_item *ditem = (PLpgSQL_diag_item *) lfirst(lc);

                            switch (ditem->kind)
                            {
                                /* these fields are disallowed in stacked case */
                                case PLPGSQL_GETDIAG_RESULT_OID:
                                    if (newp->is_stacked) {
                                        const char* message = "diagnostics item is not allowed in GET STACKED DIAGNOSTICS";
                                        InsertErrorMessage(message, plpgsql_yylloc);
                                        ereport(errstate,
                                                (errcode(ERRCODE_SYNTAX_ERROR),
                                                 errmsg("diagnostics item %s is not allowed in GET STACKED DIAGNOSTICS",
                                                        plpgsql_getdiag_kindname(ditem->kind)),
                                                 parser_errposition((yylsp[-4]))));
                                    }
                                    break;
                                case PLPGSQL_GETDIAG_ROW_COUNT:
                                    if (!B_DIAGNOSTICS) {
                                        if (newp->is_stacked) {
                                            const char* message = "diagnostics item is not allowed in GET STACKED DIAGNOSTICS";
                                            InsertErrorMessage(message, plpgsql_yylloc);
                                            ereport(errstate,
                                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                                    errmsg("diagnostics item %s is not allowed in GET STACKED DIAGNOSTICS",
                                                            plpgsql_getdiag_kindname(ditem->kind)),
                                                    parser_errposition((yylsp[-4]))));
                                        }
                                    }
                                    break;
                                /* these fields are disallowed in current case */
                                case PLPGSQL_GETDIAG_ERROR_CONTEXT:
                                case PLPGSQL_GETDIAG_ERROR_DETAIL:
                                case PLPGSQL_GETDIAG_ERROR_HINT:
                                case PLPGSQL_GETDIAG_RETURNED_SQLSTATE:
                                case PLPGSQL_GETDIAG_MESSAGE_TEXT:
                                    if (!newp->is_stacked) {
                                        const char* message = "diagnostics item is not allowed in GET CURRENT DIAGNOSTICS";
                                        InsertErrorMessage(message, plpgsql_yylloc);
                                        ereport(errstate,
                                                (errcode(ERRCODE_SYNTAX_ERROR),
                                                 errmsg("diagnostics item %s is not allowed in GET CURRENT DIAGNOSTICS",
                                                        plpgsql_getdiag_kindname(ditem->kind)),
                                                 parser_errposition((yylsp[-4]))));
                                    }
                                    break;
                                case PLPGSQL_GETDIAG_B_NUMBER:
                                    break;
                                default:
                                    const char* message = "unrecognized diagnostic item kind";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    elog(errstate, "unrecognized diagnostic item kind: %d",
                                         ditem->kind);
                                    break;
                            }
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 5881 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 196:
#line 3214 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
							ereport(errstate, (errmodule(MOD_PARSER),
								errcode(ERRCODE_SYNTAX_ERROR),
								errmsg("Un-support feature"),
								errdetail("get diagitem syntax is supported only in B compatibility")));
						}
                        PLpgSQL_stmt_getdiag	 *newp;

                        newp = (PLpgSQL_stmt_getdiag *)palloc0(sizeof(PLpgSQL_stmt_getdiag));
                        newp->cmd_type = PLPGSQL_STMT_GETDIAG;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-6]));
                        newp->is_stacked = (yyvsp[-5].boolean);
                        newp->has_cond = true;
                        newp->is_cond_item = true;
                        newp->cond_number = (yyvsp[-2].ival);
                        newp->diag_items = (yyvsp[-1].list);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 5907 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 197:
#line 3236 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
							ereport(errstate, (errmodule(MOD_PARSER),
								errcode(ERRCODE_SYNTAX_ERROR),
								errmsg("Un-support feature"),
								errdetail("get diagitem syntax is supported only in B compatibility")));
						}
                        PLpgSQL_stmt_getdiag	 *newp;

                        newp = (PLpgSQL_stmt_getdiag *)palloc0(sizeof(PLpgSQL_stmt_getdiag));
                        newp->cmd_type = PLPGSQL_STMT_GETDIAG;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-6]));
                        newp->is_stacked = (yyvsp[-5].boolean);
                        newp->has_cond = true;
                        newp->is_cond_item = false;
                        newp->cond_number = (yyvsp[-2].ival);
                        newp->diag_items = (yyvsp[-1].list);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 5933 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 198:
#line 3260 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = false;
                    }
#line 5941 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 199:
#line 3264 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = false;
                    }
#line 5949 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 200:
#line 3268 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = true;
                    }
#line 5957 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 201:
#line 3274 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].diagitem));
                    }
#line 5965 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 202:
#line 3278 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = list_make1((yyvsp[0].diagitem));
                    }
#line 5973 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 203:
#line 3284 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = (yyvsp[-2].ival);
                        newp->kind = (yyvsp[0].ival);
                        newp->user_ident = NULL;

                        (yyval.diagitem) = newp;
                    }
#line 5988 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 204:
#line 3295 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
							ereport(errstate, (errmodule(MOD_PARSER),
								errcode(ERRCODE_SYNTAX_ERROR),
								errmsg("Un-support feature"),
								errdetail("get diagitem syntax is supported only in B compatibility")));
						}
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = (yyvsp[-2].ival);
                        newp->kind = PLPGSQL_GETDIAG_B_NUMBER;
                        newp->user_ident = NULL;

                        (yyval.diagitem) = newp;
                    }
#line 6009 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 205:
#line 3312 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
							ereport(errstate, (errmodule(MOD_PARSER),
								errcode(ERRCODE_SYNTAX_ERROR),
								errmsg("Un-support feature"),
								errdetail("get diagitem syntax is supported only in B compatibility")));
						}
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = 0;
                        newp->kind = (yyvsp[0].ival);
                        newp->user_ident = (yyvsp[-2].str);

                        (yyval.diagitem) = newp;
                    }
#line 6030 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 206:
#line 3331 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_ROW_COUNT;
                    }
#line 6038 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 207:
#line 3335 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_RESULT_OID;
                    }
#line 6046 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 208:
#line 3339 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_ERROR_DETAIL;
                    }
#line 6054 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 209:
#line 3343 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_ERROR_CONTEXT;
                    }
#line 6062 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 210:
#line 3347 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_ERROR_HINT;
                    }
#line 6070 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 211:
#line 3351 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_MESSAGE_TEXT;
                    }
#line 6078 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 212:
#line 3355 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = PLPGSQL_GETDIAG_RETURNED_SQLSTATE;
                    }
#line 6086 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 213:
#line 3362 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("get diagnostic number is only supported in database which dbcompatibility='B'.")));
                        (yyval.ival) = PLPGSQL_GETDIAG_B_NUMBER;
                    }
#line 6097 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 214:
#line 3369 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("get diagnostic number is only supported in database which dbcompatibility='B'.")));
                        (yyval.ival) = PLPGSQL_GETDIAG_ROW_COUNT;
                    }
#line 6108 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 215:
#line 3378 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW ||
                            (yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC) {
                            const char* message = "not a scalar variable";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("\"%s\" is not a scalar variable",
                                            NameOfDatum(&((yyvsp[0].wdatum)))),
                                     parser_errposition((yylsp[0]))));
                        }
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                    }
#line 6127 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 216:
#line 3393 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* just to give a better message than "syntax error" */
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 6136 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 217:
#line 3398 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 6145 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 218:
#line 3406 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[0].wdatum).datum, (yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW ||
                            (yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC) {
                            const char* message = "not a scalar variable";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("\"%s\" is not a scalar variable",
                                            NameOfDatum(&((yyvsp[0].wdatum)))),
                                     parser_errposition((yylsp[0]))));
                        }
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                    }
#line 6164 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 219:
#line 3422 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = (yyvsp[0].ival); }
#line 6170 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 220:
#line 3423 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = (atof((yyvsp[0].str)) + 0.5); }
#line 6176 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 221:
#line 3424 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = (atof((yyvsp[0].str)) + 0.5); }
#line 6182 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 222:
#line 3426 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
						Datum val = DirectFunctionCall1(bittoint4, DirectFunctionCall3(bit_in, CStringGetDatum((yyvsp[0].str)), ObjectIdGetDatum(InvalidOid), Int32GetDatum(-1)));
						(yyval.ival) = DatumGetInt32(val);
					}
#line 6191 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 223:
#line 3430 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = 1; }
#line 6197 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 224:
#line 3431 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = 0; }
#line 6203 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 225:
#line 3432 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = 0; }
#line 6209 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 226:
#line 3434 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = getUserVarVal((yyvsp[0].str));
                    }
#line 6217 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 227:
#line 3437 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = (atof((yyvsp[0].word).ident) + 0.5); }
#line 6223 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 228:
#line 3442 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = lappend((yyvsp[-2].list), (yyvsp[0].diagitem));
                    }
#line 6231 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 229:
#line 3446 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = list_make1((yyvsp[0].diagitem));
                    }
#line 6239 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 230:
#line 3453 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = (yyvsp[-2].ival);
                        newp->kind = (yyvsp[0].ival);
                        newp->user_ident = NULL;

                        (yyval.diagitem) = newp;
                    }
#line 6254 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 231:
#line 3464 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_diag_item *newp;

                        newp = (PLpgSQL_diag_item *)palloc(sizeof(PLpgSQL_diag_item));
                        newp->target = 0;
                        newp->kind = (yyvsp[0].ival);
                        newp->user_ident = (yyvsp[-2].str);

                        (yyval.diagitem) = newp;
                    }
#line 6269 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 232:
#line 3477 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CLASS_ORIGIN; }
#line 6275 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 233:
#line 3478 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_SUBCLASS_ORIGIN; }
#line 6281 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 234:
#line 3479 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CONSTRAINT_CATALOG; }
#line 6287 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 235:
#line 3480 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CONSTRAINT_SCHEMA; }
#line 6293 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 236:
#line 3481 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CONSTRAINT_NAME; }
#line 6299 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 237:
#line 3482 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CATALOG_NAME; }
#line 6305 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 238:
#line 3483 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_SCHEMA_NAME; }
#line 6311 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 239:
#line 3484 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_TABLE_NAME; }
#line 6317 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 240:
#line 3485 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_COLUMN_NAME; }
#line 6323 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 241:
#line 3486 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_CURSOR_NAME; }
#line 6329 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 242:
#line 3487 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_MESSAGE_TEXT; }
#line 6335 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 243:
#line 3488 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_MYSQL_ERRNO; }
#line 6341 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 244:
#line 3489 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.ival) = PLPGSQL_GETDIAG_B_RETURNED_SQLSTATE; }
#line 6347 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 245:
#line 3493 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                        AddNamespaceIfNeed((yyvsp[0].wdatum).dno, (yyvsp[0].wdatum).ident);
                    }
#line 6356 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 246:
#line 3500 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                        AddNamespaceIfNeed((yyvsp[0].wdatum).dno, (yyvsp[0].wdatum).ident);
                    }
#line 6365 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 247:
#line 3507 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                        AddNamespaceIfNeed((yyvsp[0].wdatum).dno, (yyvsp[0].wdatum).ident);
                    }
#line 6374 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 248:
#line 3515 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[-1].wdatum).datum, (yylsp[-1]));
                        if ((yyvsp[0].list) == NIL) {
                            (yyval.ival) = (yyvsp[-1].wdatum).dno;
                        } else {
                            if(IsA((Node*)linitial((yyvsp[0].list)), A_Indices) && list_length((yyvsp[0].list)) == 1) {
                                PLpgSQL_arrayelem       *newp;

                                newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
                                newp->dtype             = PLPGSQL_DTYPE_ARRAYELEM;
                                newp->subscript = (PLpgSQL_expr*)(((A_Indices*)linitial((yyvsp[0].list)))->uidx);
                                newp->arrayparentno = (yyvsp[-1].wdatum).dno;
                                /* initialize cached type data to "not valid" */
                                newp->parenttypoid = InvalidOid;
                                newp->assignattrno = -1;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);

                                (yyval.ival) = newp->dno;
                            } else {
                                PLpgSQL_assignlist       *newp;
                                newp = (PLpgSQL_assignlist *)palloc0(sizeof(PLpgSQL_assignlist));
                                newp->dtype             = PLPGSQL_DTYPE_ASSIGNLIST;
                                newp->assignlist = (yyvsp[0].list);
                                newp->targetno = (yyvsp[-1].wdatum).dno;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);
                                (yyval.ival) = newp->dno;
                            }
                        }

                    }
#line 6409 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 249:
#line 3546 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[-1].wdatum).datum, (yylsp[-1]));
                        if ((yyvsp[0].list) == NIL) {
                            (yyval.ival) = (yyvsp[-1].wdatum).dno;
                        } else {
                            if(IsA((Node*)linitial((yyvsp[0].list)), A_Indices) && list_length((yyvsp[0].list)) == 1) {
                                PLpgSQL_arrayelem       *newp;

                                newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
                                newp->dtype             = PLPGSQL_DTYPE_ARRAYELEM;
                                newp->subscript = (PLpgSQL_expr*)(((A_Indices*)linitial((yyvsp[0].list)))->uidx);
                                newp->arrayparentno = (yyvsp[-1].wdatum).dno;
                                /* initialize cached type data to "not valid" */
                                newp->parenttypoid = InvalidOid;
                                newp->assignattrno = -1;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);

                                (yyval.ival) = newp->dno;
                            } else {
                                PLpgSQL_assignlist       *newp;
                                newp = (PLpgSQL_assignlist *)palloc0(sizeof(PLpgSQL_assignlist));
                                newp->dtype             = PLPGSQL_DTYPE_ASSIGNLIST;
                                newp->assignlist = (yyvsp[0].list);
                                newp->targetno = (yyvsp[-1].wdatum).dno;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);
                                (yyval.ival) = newp->dno;
                            }
                        }

                    }
#line 6444 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 250:
#line 3577 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[-1].wdatum).datum, (yylsp[-1]));
                        if ((yyvsp[-1].wdatum).datum->dtype == PLPGSQL_DTYPE_VAR) {
                            check_autonomous_nest_tablevar((PLpgSQL_var*)(yyvsp[-1].wdatum).datum);
                        }
                        if ((yyvsp[0].list) == NIL) {
                            (yyval.ival) = (yyvsp[-1].wdatum).dno;
                        } else {
                            if(IsA((Node*)linitial((yyvsp[0].list)), A_Indices) && list_length((yyvsp[0].list)) == 1) {
                                PLpgSQL_tableelem       *newp;
                                newp = (PLpgSQL_tableelem *)palloc0(sizeof(PLpgSQL_tableelem));
                                newp->dtype = PLPGSQL_DTYPE_TABLEELEM;
                                newp->subscript = (PLpgSQL_expr*)(((A_Indices*)linitial((yyvsp[0].list)))->uidx);
                                newp->tableparentno = (yyvsp[-1].wdatum).dno;
                                /* initialize cached type data to "not valid" */
                                newp->tabletypoid = InvalidOid;
                                newp->assignattrno = -1;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);
                                (yyval.ival) = newp->dno;
                            } else {
                                PLpgSQL_assignlist       *newp;
                                newp = (PLpgSQL_assignlist *)palloc0(sizeof(PLpgSQL_assignlist));
                                newp->dtype             = PLPGSQL_DTYPE_ASSIGNLIST;
                                newp->assignlist = (yyvsp[0].list);
                                newp->targetno = (yyvsp[-1].wdatum).dno;
                                plpgsql_adddatum((PLpgSQL_datum *) newp);
                                (yyval.ival) = newp->dno;
                            }
                        }

                    }
#line 6480 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 251:
#line 3609 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[-1].wdatum).datum, (yylsp[-1]));
                        if ((yyvsp[0].list) == NIL) {
                            check_record_nest_tableof_index((yyvsp[-1].wdatum).datum);
                            (yyval.ival) = (yyvsp[-1].wdatum).dno;
                        } else {
                            if(IsA((Node*)linitial((yyvsp[0].list)), A_Indices) && list_length((yyvsp[0].list)) == 1) {
                                PLpgSQL_datum* target = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[-1].wdatum).dno];
                                if (target->dtype == PLPGSQL_DTYPE_VAR &&
                                    ((PLpgSQL_var*)target)->datatype->collectionType == PLPGSQL_COLLECTION_TABLE) {
                                    PLpgSQL_tableelem       *newp;
                                    newp = (PLpgSQL_tableelem *)palloc0(sizeof(PLpgSQL_tableelem));
                                    newp->dtype             = PLPGSQL_DTYPE_TABLEELEM;
                                    newp->subscript = (PLpgSQL_expr*)(((A_Indices*)linitial((yyvsp[0].list)))->uidx);
                                    newp->tableparentno = (yyvsp[-1].wdatum).dno;
                                    /* initialize cached type data to "not valid" */
                                    newp->tabletypoid = InvalidOid;
                                    newp->assignattrno = -1;
                                    plpgsql_adddatum((PLpgSQL_datum *) newp);

                                    (yyval.ival) = newp->dno; 
                                } else {
								    PLpgSQL_arrayelem	*newp;

                                    newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
                                    newp->dtype		= PLPGSQL_DTYPE_ARRAYELEM;
                                    newp->subscript	= (PLpgSQL_expr*)(((A_Indices*)linitial((yyvsp[0].list)))->uidx);
                                    newp->arrayparentno = (yyvsp[-1].wdatum).dno;
                                    /* initialize cached type data to "not valid" */
                                    newp->parenttypoid = InvalidOid;
                                    newp->assignattrno = -1;
                                    plpgsql_adddatum((PLpgSQL_datum *) newp);

                                    (yyval.ival) = newp->dno;
                                }
                            } else {
                                PLpgSQL_assignlist	*newp;
                                newp = (PLpgSQL_assignlist *)palloc0(sizeof(PLpgSQL_assignlist));
                                newp->dtype		= PLPGSQL_DTYPE_ASSIGNLIST;
                                newp->targetno = (yyvsp[-1].wdatum).dno;
                                newp->assignlist = (yyvsp[0].list);
                                plpgsql_adddatum((PLpgSQL_datum *) newp);
                                (yyval.ival) = newp->dno;
                            }
                        }
                    }
#line 6531 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 252:
#line 3656 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        check_assignable((yyvsp[-1].wdatum).datum, (yylsp[-1]));
                        if ((yyvsp[0].list) != NIL) {
                            PLpgSQL_assignlist	*newptr;
                            newptr = (PLpgSQL_assignlist *)palloc0(sizeof(PLpgSQL_assignlist));
                            newptr->dtype		= PLPGSQL_DTYPE_ASSIGNLIST;
                            newptr->targetno = (yyvsp[-1].wdatum).dno;
                            newptr->assignlist = (yyvsp[0].list);
                            plpgsql_adddatum((PLpgSQL_datum *) newptr);
                            (yyval.ival) = newptr->dno;
                        } else {
                            check_record_nest_tableof_index((yyvsp[-1].wdatum).datum);
                            (yyval.ival) = (yyvsp[-1].wdatum).dno;
                        }
                  }
#line 6551 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 253:
#line 3672 "src/pltsql/gram.y" /* yacc.c:1646  */
    {(yyval.list) = NIL; }
#line 6557 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 254:
#line 3673 "src/pltsql/gram.y" /* yacc.c:1646  */
    {(yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].plnode));}
#line 6563 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 255:
#line 3676 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.plnode) = (Node *) makeString((yyvsp[0].str));
                    }
#line 6571 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 256:
#line 3680 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        A_Indices *ai = makeNode(A_Indices);
                        ai->lidx = NULL;
                        ai->uidx = (Node *)(yyvsp[0].expr);
                        (yyval.plnode) = (Node *)ai;
                    }
#line 6582 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 257:
#line 3687 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        A_Indices *ai = makeNode(A_Indices);
                        ai->lidx = NULL;
                        ai->uidx = (Node *)(yyvsp[0].expr);
                        (yyval.plnode) = (Node *)ai;
                    }
#line 6593 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 258:
#line 3696 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = get_attrname(yychar);
                        yyclearin;
                    }
#line 6602 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 259:
#line 3702 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_goto *newp;

                        newp = (PLpgSQL_stmt_goto *)palloc0(sizeof(PLpgSQL_stmt_goto));
                        newp->cmd_type  = PLPGSQL_STMT_GOTO;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->label = (yyvsp[0].str);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 6618 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 260:
#line 3715 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[-1].word).ident;
                    }
#line 6626 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 261:
#line 3721 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_if *newp;

                        newp = (PLpgSQL_stmt_if *)palloc0(sizeof(PLpgSQL_stmt_if));
                        newp->cmd_type	= PLPGSQL_STMT_IF;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[-7]));
                        newp->cond		= (yyvsp[-6].expr);
                        newp->then_body	= (yyvsp[-5].list);
                        newp->elsif_list = (yyvsp[-4].list);
                        newp->else_body  = (yyvsp[-3].list);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 6645 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 262:
#line 3738 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = NIL;
                    }
#line 6653 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 263:
#line 3742 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_if_elsif *newp;

                        newp = (PLpgSQL_if_elsif *)palloc0(sizeof(PLpgSQL_if_elsif));
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->cond   = (yyvsp[-1].expr);
                        newp->stmts  = (yyvsp[0].list);

                        (yyval.list) = lappend((yyvsp[-3].list), newp);
                    }
#line 6668 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 264:
#line 3755 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = NIL;
                    }
#line 6676 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 265:
#line 3759 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = (yyvsp[0].list);
                    }
#line 6684 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 266:
#line 3765 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.stmt) = make_case((yylsp[-6]), (yyvsp[-5].expr), (yyvsp[-4].list), (yyvsp[-3].list));
                    }
#line 6692 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 267:
#line 3771 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_expr *expr = NULL;
                        int	tok = yylex();

                        if (tok != K_WHEN)
                        {
                            pltsql_push_back_token(tok);
                            expr = read_sql_expression(K_WHEN, "WHEN");
                        }
                        pltsql_push_back_token(K_WHEN);
                        (yyval.expr) = expr;
                    }
#line 6709 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 268:
#line 3786 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].casewhen));
                    }
#line 6717 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 269:
#line 3790 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = list_make1((yyvsp[0].casewhen));
                    }
#line 6725 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 270:
#line 3796 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_case_when *newp = (PLpgSQL_case_when *)palloc(sizeof(PLpgSQL_case_when));

                        newp->lineno	= plpgsql_location_to_lineno((yylsp[-2]));
                        newp->expr	= (yyvsp[-1].expr);
                        newp->stmts	= (yyvsp[0].list);
                        newp->sqlString = plpgsql_get_curline_query();
                        (yyval.casewhen) = newp;
                    }
#line 6739 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 271:
#line 3808 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.list) = NIL;
                    }
#line 6747 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 272:
#line 3812 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * proc_sect could return an empty list, but we
                         * must distinguish that from not having ELSE at all.
                         * Simplest fix is to return a list with one NULL
                         * pointer, which make_case() must take care of.
                         */
                        if ((yyvsp[0].list) != NIL)
                            (yyval.list) = (yyvsp[0].list);
                        else
                            (yyval.list) = list_make1(NULL);
                    }
#line 6764 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 273:
#line 3827 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_loop *newp;

                        newp = (PLpgSQL_stmt_loop *)palloc0(sizeof(PLpgSQL_stmt_loop));
                        newp->cmd_type = PLPGSQL_STMT_LOOP;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->label	  = (yyvsp[-2].str);
                        newp->body	  = (yyvsp[0].loop_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();

                        check_labels((yyvsp[-2].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-2].str), (PLpgSQL_stmt *)newp);
                    }
#line 6787 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 274:
#line 3846 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "label: loop"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'label:' is only supported in database which dbcompatibility='B'.")));

                        PLpgSQL_stmt_loop  *newp;
                        newp = (PLpgSQL_stmt_loop *)palloc0(sizeof(PLpgSQL_stmt_loop));
                        newp->cmd_type = PLPGSQL_STMT_LOOP;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->label    = u_sess->plsql_cxt.curr_compile_context->ns_top->name;
                        newp->body        = (yyvsp[0].loop_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();

                        if(strcmp(newp->label, "") == 0 || u_sess->plsql_cxt.curr_compile_context->ns_top->itemtype != PLPGSQL_NSTYPE_LABEL)
                        {
                            ereport(errstate,
                                   (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("The label name is invalid"),
                                    parser_errposition((yylsp[-1]))));
                        }

                        check_labels(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (PLpgSQL_stmt *)newp);
                    }
#line 6825 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 277:
#line 3885 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * Check for correct syntax
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility == B_FORMAT)
                        {
                            if((yyvsp[-1].expr_until_while_loop).endtoken != K_LOOP)
                                 ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR), errmsg("while-loop syntax is mixed with while-do syntax"), parser_errposition((yylsp[-2]))));
                        }

                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label	  = (yyvsp[-3].str);
                        newp->cond	  = (yyvsp[-1].expr_until_while_loop).expr;
                        newp->body	  = (yyvsp[0].loop_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition	  = true;

                        check_labels((yyvsp[-3].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                    }
#line 6860 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 278:
#line 3916 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                       /*
                         * When the database is in mysql compatible mode
                         * support "label: while-loop"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'label:while' is only supported in database which dbcompatibility='B'.")));
                        /*
                         * Check for correct syntax
                         */
                        if((yyvsp[-1].expr_until_while_loop).endtoken != K_LOOP)
                             ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR), errmsg("while-loop syntax is mixed with while-do syntax"), parser_errposition((yylsp[-2]))));

                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label       = u_sess->plsql_cxt.curr_compile_context->ns_top->name;
                        newp->cond        = (yyvsp[-1].expr_until_while_loop).expr;
                        newp->body        = (yyvsp[0].loop_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition   = true;

                        if(strcmp(newp->label, "") == 0 || u_sess->plsql_cxt.curr_compile_context->ns_top->itemtype != PLPGSQL_NSTYPE_LABEL)
                        {
                            ereport(errstate,
                                   (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("The label name is invalid"),
                                    parser_errposition((yylsp[-2]))));
                        }

                        check_labels(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (PLpgSQL_stmt *)newp);
                    }
#line 6907 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 279:
#line 3959 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "while-do"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                        {
                            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("Incorrect use of syntax while-loop")));
                        }
                        else
                        {
                            /*
                             * Check for correct syntax
                             */
                            if((yyvsp[-1].expr_until_while_loop).endtoken != K_DO)
                                 ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR), errmsg("while-loop syntax is mixed with while-do syntax"), parser_errposition((yylsp[-2]))));
                        }

                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label       = (yyvsp[-3].str);
                        newp->cond        = (yyvsp[-1].expr_until_while_loop).expr;
                        newp->body        = (yyvsp[0].while_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition   = true;

                        check_labels((yyvsp[-3].str), (yyvsp[0].while_body).end_label, (yyvsp[0].while_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                    }
#line 6951 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 280:
#line 3999 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "label:"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                        {
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'label:while' is only supported in database which dbcompatibility='B'.")));
                        }
                        /*
                         * Check for correct syntax
                         */
                        if((yyvsp[-1].expr_until_while_loop).endtoken != K_DO)
                             ereport(ERROR,
                                (errcode(ERRCODE_SYNTAX_ERROR), errmsg("while-loop syntax is mixed with while-do syntax"), parser_errposition((yylsp[-2]))));

                        PLpgSQL_stmt_while  *newp;
                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label    = u_sess->plsql_cxt.curr_compile_context->ns_top->name;
                        newp->cond        = (yyvsp[-1].expr_until_while_loop).expr;
                        newp->body        = (yyvsp[0].while_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition   = true;

                        if(strcmp(newp->label, "") == 0 || u_sess->plsql_cxt.curr_compile_context->ns_top->itemtype != PLPGSQL_NSTYPE_LABEL)
                        {
                            ereport(errstate,
                                   (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("The label name is invalid"),
                                    parser_errposition((yylsp[-2]))));
                        }
                        check_labels(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (yyvsp[0].while_body).end_label, (yyvsp[0].while_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (PLpgSQL_stmt *)newp);
                    }
#line 6998 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 281:
#line 4042 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "repeat"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'repeat' is only supported in database which dbcompatibility='B'.")));

                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-3]));
                        newp->label       = (yyvsp[-4].str);
                        newp->cond        = (yyvsp[0].repeat_condition).expr;
                        newp->body        = (yyvsp[-2].list);
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition   = false;

                        check_labels((yyvsp[-4].str), (yyvsp[0].repeat_condition).end_label, (yyvsp[0].repeat_condition).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-4].str), (PLpgSQL_stmt *)newp);
                    }
#line 7031 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 282:
#line 4071 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "repeat"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'label: repeat' is only supported in database which dbcompatibility='B'.")));

                        PLpgSQL_stmt_while *newp;

                        newp = (PLpgSQL_stmt_while *)palloc0(sizeof(PLpgSQL_stmt_while));
                        newp->cmd_type = PLPGSQL_STMT_WHILE;
                        newp->lineno   = plpgsql_location_to_lineno((yylsp[-3]));
                        newp->label       = u_sess->plsql_cxt.curr_compile_context->ns_top->name;
                        newp->cond        = (yyvsp[0].repeat_condition).expr;
                        newp->body        = (yyvsp[-2].list);
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->condition   = false;

                        if(strcmp(newp->label, "") == 0 || u_sess->plsql_cxt.curr_compile_context->ns_top->itemtype != PLPGSQL_NSTYPE_LABEL)
                        {
                            ereport(errstate,
                                   (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("The label name is invalid"),
                                    parser_errposition((yylsp[-3]))));
                        }

                        check_labels(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (yyvsp[0].repeat_condition).end_label, (yyvsp[0].repeat_condition).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label(u_sess->plsql_cxt.curr_compile_context->ns_top->name, (PLpgSQL_stmt *)newp);
                    }
#line 7072 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 287:
#line 4118 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* This runs after we've scanned the loop body */
                        if ((yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORI)
                        {
                            PLpgSQL_stmt_fori		*newp;

                            newp = (PLpgSQL_stmt_fori *) (yyvsp[-1].stmt);
                            newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->label	  = (yyvsp[-3].str);
                            newp->body	  = (yyvsp[0].loop_body).stmts;
                            newp->sqlString = plpgsql_get_curline_query();
                            (yyval.stmt) = (PLpgSQL_stmt *) newp;

                            /* register the stmt if it is labeled */
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        }
                        else
                        {
                            PLpgSQL_stmt_forq		*newp;

                            AssertEreport((yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORS ||
                                          (yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_FORC ||
                                          (yyvsp[-1].stmt)->cmd_type == PLPGSQL_STMT_DYNFORS,
                                            MOD_PLSQL,
                                            "unexpected node type.");
                            /* forq is the common supertype of all three */
                            newp = (PLpgSQL_stmt_forq *) (yyvsp[-1].stmt);
                            newp->lineno   = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->label	  = (yyvsp[-3].str);
                            newp->body	  = (yyvsp[0].loop_body).stmts;
                            newp->sqlString = plpgsql_get_curline_query();
                            (yyval.stmt) = (PLpgSQL_stmt *) newp;

                            /* register the stmt if it is labeled */
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        }

                        check_labels((yyvsp[-3].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        /* close namespace started in opt_block_label */
                        plpgsql_ns_pop();
                    }
#line 7118 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 288:
#line 4160 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* This runs after we've scanned the loop body */
                        /* A db FORALL support 3 types like below. We implemented the first one.  
                         * FORALL index_name IN lower_bound .. upper_bound
                         * FORALL index_name IN INDICES OF collection between lower_bound and upper_bound
                         * FORALL index_name IN VALUES OF index_collection
                         * forall_body can only have one statement.
                         */
                        if ((yyvsp[-2].stmt)->cmd_type == PLPGSQL_STMT_FORI)
                        {
                            PLpgSQL_stmt_fori		*newm;

                            newm = (PLpgSQL_stmt_fori *) (yyvsp[-2].stmt);
                            newm->lineno   = plpgsql_location_to_lineno((yylsp[-3]));
                            newm->label	  = NULL;
                            newm->save_exceptions = (yyvsp[-1].boolean);
                            newm->body	  = (yyvsp[0].list);
                            newm->sqlString = plpgsql_get_curline_query();
                            (yyval.stmt) = (PLpgSQL_stmt *) newm;
                        }
                        else {
                            const char* message = "please use \'FORALL index_name IN lower_bound .. upper_bound\'";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("please use \'FORALL index_name IN lower_bound .. upper_bound\'")));
                        }
                        /* close namespace started in opt_block_label */
                        plpgsql_ns_pop();
                    }
#line 7153 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 289:
#line 4193 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int			tok = yylex();
                        int			tokloc = yylloc;
                        if (tok == K_EXECUTE)
                        {
                            /* EXECUTE means it's a dynamic FOR loop */
                            PLpgSQL_stmt_dynfors	*newp;
                            PLpgSQL_expr			*expr;
                            int						term;

                            expr = read_sql_expression2(K_LOOP, K_USING,
                                                        "LOOP or USING",
                                                        &term);

                            newp = (PLpgSQL_stmt_dynfors *)palloc0(sizeof(PLpgSQL_stmt_dynfors));
                            newp->cmd_type = PLPGSQL_STMT_DYNFORS;
                            if ((yyvsp[-1].forvariable).rec)
                            {
                                newp->rec = (yyvsp[-1].forvariable).rec;
                                check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                            }
                            else if ((yyvsp[-1].forvariable).row)
                            {
                                newp->row = (yyvsp[-1].forvariable).row;
                                check_assignable((PLpgSQL_datum *) newp->row, (yylsp[-1]));
                            }
                            else if ((yyvsp[-1].forvariable).scalar)
                            {
                                /* convert single scalar to list */
                                newp->row = make_scalar_list1((yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).scalar, (yyvsp[-1].forvariable).dno,
                                                             (yyvsp[-1].forvariable).lineno, (yylsp[-1]));
                                /* no need for check_assignable */
                            }
                            else
                            {
                                const char* message = "loop variable of loop over rows must be a record or row variable or list of scalar variables";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                                         errmsg("loop variable of loop over rows must be a record or row variable or list of scalar variables"),
                                         parser_errposition((yylsp[-1]))));
                            }

                            newp->query = expr;

                            if (term == K_USING)
                            {
                                do
                                {
                                    expr = read_sql_expression2(',', K_LOOP,
                                                                ", or LOOP",
                                                                &term);
                                    newp->params = lappend(newp->params, expr);
                                } while (term == ',');
                            }

                            (yyval.stmt) = (PLpgSQL_stmt *) newp;
                        }
                        else if ((tok == T_DATUM || tok == T_PACKAGE_VARIABLE) &&
                                 yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_VAR &&
                                 ((PLpgSQL_var *) yylval.wdatum.datum)->datatype->typoid == REFCURSOROID)
                        {
                            /* It's FOR var IN cursor */
                            PLpgSQL_stmt_forc	*newp;
                            PLpgSQL_var			*cursor = (PLpgSQL_var *) yylval.wdatum.datum;

                            newp = (PLpgSQL_stmt_forc *) palloc0(sizeof(PLpgSQL_stmt_forc));
                            newp->cmd_type = PLPGSQL_STMT_FORC;
                            newp->curvar = yylval.wdatum.dno;

                            /* Should have had a single variable name */
                            if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row) {
                                const char* message = "cursor FOR loop must have only one target variable";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor FOR loop must have only one target variable"),
                                         parser_errposition((yylsp[-1]))));
                            }

                            /* can't use an unbound cursor this way */
                            if (cursor->cursor_explicit_expr == NULL) {
                                const char* message = "cursor FOR loop must use a bound cursor variable";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("cursor FOR loop must use a bound cursor variable"),
                                         parser_errposition(tokloc)));
                            }

                            /* collect cursor's parameters if any */
                            newp->argquery = read_cursor_args(cursor,
                                                             K_LOOP,
                                                             "LOOP");
                            TupleDesc tupleDesc = NULL;
                            if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK &&
                                cursor->cursor_explicit_expr->query != NULL) {
                                tupleDesc = getCursorTupleDesc(cursor->cursor_explicit_expr, false);
                            }

                            /* create loop's private RECORD variable */
                            newp->rec = plpgsql_build_record((yyvsp[-1].forvariable).name,
                                                            (yyvsp[-1].forvariable).lineno,
                                                            true,
                                                            tupleDesc);

                            (yyval.stmt) = (PLpgSQL_stmt *) newp;
                        }
                        else
                        {
                            PLpgSQL_expr	*expr1;
                            int				expr1loc;
                            bool			reverse = false;

                            /*
                             * We have to distinguish between two
                             * alternatives: FOR var IN a .. b and FOR
                             * var IN query. Unfortunately this is
                             * tricky, since the query in the second
                             * form needn't start with a SELECT
                             * keyword.  We use the ugly hack of
                             * looking for two periods after the first
                             * token. We also check for the REVERSE
                             * keyword, which means it must be an
                             * integer loop.
                             */
                            if (tok_is_keyword(tok, &yylval,
                                               K_REVERSE, "reverse"))
                                reverse = true;
                            else
                                pltsql_push_back_token(tok);

                            /*
                             * Read tokens until we see either a ".."
                             * or a LOOP. The text we read may not
                             * necessarily be a well-formed SQL
                             * statement, so we need to invoke
                             * read_sql_construct directly.
                             */
                            expr1 = read_sql_construct(DOT_DOT,
                                                       K_LOOP,
                                                       0,
                                                       "LOOP",
                                                       "SELECT ",
                                                       true,
                                                       false,
                                                       true,
                                                       &expr1loc,
                                                       &tok);

                            if (tok == DOT_DOT)
                            {
                                /* Saw "..", so it must be an integer loop */
                                PLpgSQL_expr		*expr2;
                                PLpgSQL_expr		*expr_by;
                                PLpgSQL_var			*fvar;
                                PLpgSQL_stmt_fori	*newp;

                                /* Check first expression is well-formed */
                                check_sql_expr(expr1->query, expr1loc, 7);

                                /* Read and check the second one */
                                expr2 = read_sql_expression2(K_LOOP, K_BY,
                                                             "LOOP",
                                                             &tok);

                                /* Get the BY clause if any */
                                if (tok == K_BY)
                                    expr_by = read_sql_expression(K_LOOP,
                                                                  "LOOP");
                                else
                                    expr_by = NULL;

                                /* Should have had a single variable name */
                                if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row) {
                                    const char* message = "integer FOR loop must have only one target variable";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                            (errcode(ERRCODE_SYNTAX_ERROR),
                                             errmsg("integer FOR loop must have only one target variable"),
                                             parser_errposition((yylsp[-1]))));
                                }

                                /* create loop's private variable */
                                fvar = (PLpgSQL_var *)
                                    plpgsql_build_variable((yyvsp[-1].forvariable).name,
                                                           (yyvsp[-1].forvariable).lineno,
                                                           plpgsql_build_datatype(INT4OID,
                                                                                  -1,
                                                                                  InvalidOid),
                                                           true);

                                newp = (PLpgSQL_stmt_fori *)palloc0(sizeof(PLpgSQL_stmt_fori));
                                newp->cmd_type = PLPGSQL_STMT_FORI;
                                newp->var	  = fvar;
                                newp->reverse  = reverse;
                                newp->lower	  = expr1;
                                newp->upper	  = expr2;
                                newp->step	  = expr_by;

                                (yyval.stmt) = (PLpgSQL_stmt *) newp;
                            }
                            else
                            {
                                /*
                                 * No "..", so it must be a query loop. We've
                                 * prefixed an extra SELECT to the query text,
                                 * so we need to remove that before performing
                                 * syntax checking.
                                 */
                                char				*tmp_query;
                                PLpgSQL_stmt_fors	*newp;

                                if (reverse) {
                                    const char* message = "cannot specify REVERSE in query FOR loop";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                            (errcode(ERRCODE_SYNTAX_ERROR),
                                             errmsg("cannot specify REVERSE in query FOR loop"),
                                             parser_errposition(tokloc)));
                                }

                                AssertEreport(strncmp(expr1->query, "SELECT ", 7) == 0,
                                                    MOD_PLSQL,
                                                    "It should be SELECT");
                                tmp_query = pstrdup(expr1->query + 7);
                                pfree_ext(expr1->query);
                                expr1->query = tmp_query;
                              
                                check_sql_expr(expr1->query, expr1loc, 0);
                                newp = (PLpgSQL_stmt_fors *)palloc0(sizeof(PLpgSQL_stmt_fors));
                                newp->cmd_type = PLPGSQL_STMT_FORS;
                                if ((yyvsp[-1].forvariable).rec)
                                {
#ifndef ENABLE_MULTIPLE_NODES
                                    /* only A format and not in upgrade, IMPLICIT_FOR_LOOP_VARIABLE is valid */
                                    if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT
                                        && IMPLICIT_FOR_LOOP_VARIABLE
                                        && u_sess->attr.attr_common.upgrade_mode == 0) {
                                        BuildForQueryVariable(expr1, &newp->row, &newp->rec, (yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).lineno);
                                        check_assignable((PLpgSQL_datum *)newp->rec ?
                                            (PLpgSQL_datum *)newp->rec : (PLpgSQL_datum *)newp->row, (yylsp[-1]));
                                    } else {
                                        /* check the sql */
                                        if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
                                            (void)getCursorTupleDesc(expr1, false, true);
                                        }
                                        newp->rec = (yyvsp[-1].forvariable).rec;
                                        check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                                    }
#else
                                    newp->rec = (yyvsp[-1].forvariable).rec;
                                    check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
#endif
                                }
                                else if ((yyvsp[-1].forvariable).row)
                                {
#ifndef ENABLE_MULTIPLE_NODES
                                    /* only A format and not in upgrade, IMPLICIT_FOR_LOOP_VARIABLE is valid */
                                    if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT
                                        && IMPLICIT_FOR_LOOP_VARIABLE
                                        && u_sess->attr.attr_common.upgrade_mode == 0) {
                                        BuildForQueryVariable(expr1, &newp->row, &newp->rec, (yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).lineno);
                                        check_assignable((PLpgSQL_datum *)newp->rec ?
                                            (PLpgSQL_datum *)newp->rec : (PLpgSQL_datum *)newp->row, (yylsp[-1]));
                                    } else {
                                        /* check the sql */
                                        if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
                                            (void)getCursorTupleDesc(expr1, false, true);
                                        }
                                        newp->row = (yyvsp[-1].forvariable).row;
                                        check_assignable((PLpgSQL_datum *) newp->row, (yylsp[-1]));
                                    }
#else
                                    newp->row = (yyvsp[-1].forvariable).row;
                                    check_assignable((PLpgSQL_datum *) newp->row, (yylsp[-1]));
#endif
                                }
                                else if ((yyvsp[-1].forvariable).scalar)
                                {
#ifndef ENABLE_MULTIPLE_NODES
                                    /* only A format and not in upgrade, IMPLICIT_FOR_LOOP_VARIABLE is valid */
                                    if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT
                                        && IMPLICIT_FOR_LOOP_VARIABLE
                                        && u_sess->attr.attr_common.upgrade_mode == 0) {
                                        BuildForQueryVariable(expr1, &newp->row, &newp->rec, (yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).lineno);
                                        check_assignable((PLpgSQL_datum *)newp->rec ?
                                            (PLpgSQL_datum *)newp->rec : (PLpgSQL_datum *)newp->row, (yylsp[-1]));
                                    } else {
                                        /* check the sql */
                                        if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
                                            (void)getCursorTupleDesc(expr1, false, true);
                                        }
                                        /* convert single scalar to list */
                                        newp->row = make_scalar_list1((yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).scalar, (yyvsp[-1].forvariable).dno, (yyvsp[-1].forvariable).lineno, (yylsp[-1]));
                                        /* no need for check_assignable */
                                    }
#else
                                    /* convert single scalar to list */
                                    newp->row = make_scalar_list1((yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).scalar, (yyvsp[-1].forvariable).dno, (yyvsp[-1].forvariable).lineno, (yylsp[-1]));
                                    /* no need for check_assignable */
#endif
                                }
                                else
                                {
#ifndef ENABLE_MULTIPLE_NODES
                                    if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
                                        BuildForQueryVariable(expr1, &newp->row, &newp->rec, (yyvsp[-1].forvariable).name, (yyvsp[-1].forvariable).lineno);
                                        check_assignable((PLpgSQL_datum *)newp->rec ?
                                            (PLpgSQL_datum *)newp->rec : (PLpgSQL_datum *)newp->row, (yylsp[-1]));
                                    } else {
                                        PLpgSQL_type dtype;
                                        dtype.ttype = PLPGSQL_TTYPE_REC;
                                        newp->rec = (PLpgSQL_rec *)
                                        plpgsql_build_variable((yyvsp[-1].forvariable).name,(yyvsp[-1].forvariable).lineno, &dtype, true);
                                        check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
                                    }
#else
                                    PLpgSQL_type dtype;
                                    dtype.ttype = PLPGSQL_TTYPE_REC;
                                    newp->rec = (PLpgSQL_rec *) 
                                    plpgsql_build_variable((yyvsp[-1].forvariable).name,(yyvsp[-1].forvariable).lineno, &dtype, true);
                                    check_assignable((PLpgSQL_datum *) newp->rec, (yylsp[-1]));
#endif
                                }

                                newp->query = expr1;
                                
                                (yyval.stmt) = (PLpgSQL_stmt *) newp;
                            }
                        }
                    }
#line 7490 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 290:
#line 4527 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int		tok;
                        int		expr1loc;
                        PLpgSQL_expr	*expr1;

                        /*
                         * Read tokens until we see either a ".."
                         * or a LOOP. The text we read may not
                         * necessarily be a well-formed SQL
                         * statement, so we need to invoke
                         * read_sql_construct directly.
                         */
                        expr1 = read_sql_construct(DOT_DOT,
                                                   0,
                                                   0,
                                                   "..",
                                                   "SELECT ",
                                                   true,
                                                   false,
                                                   true,
                                                   &expr1loc,
                                                   &tok);

                        if (DOT_DOT == tok)
                        {
                            /* Saw "..", so it must be an integer loop */
                            PLpgSQL_expr		*expr2 = NULL;
                            PLpgSQL_var		*fvar = NULL;
                            PLpgSQL_stmt_fori	*newm = NULL;

                            /* Check first expression is well-formed */
                            check_sql_expr(expr1->query, expr1loc, 7);

                            /* Read and check the second one */
                            expr2 = read_sql_construct6(K_MERGE,
                                                        K_INSERT, 
                                                        K_SELECT,
                                                        K_UPDATE,
                                                        K_DELETE,
                                                        K_SAVE,
                                                        "DML",
                                                        "SELECT ",
                                                        true,
                                                        false,
                                                        true,
                                                        NULL,
                                                        &tok);

                            pltsql_push_back_token(tok);

                            if (';' == tok) {
                                const char* message = "FORALL must follow DML statement.";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_FORALL_NEED_DML),
                                         errmsg("FORALL must follow DML statement.")));
                            }

                            /* should follow DML statement */
                            if (tok != K_INSERT && tok != K_UPDATE && tok != K_DELETE && tok != K_SELECT && tok != K_MERGE && tok != K_SAVE) { 
                                const char* message = "FORALL must follow DML statement.";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_FORALL_NEED_DML),
                                         errmsg("FORALL must follow DML statement.")));
                            }

                            if (tok == K_SAVE) {
                                CheckSaveExceptionsDML(errstate);
                            }

                            /* Should have had a single variable name */
                            if ((yyvsp[-1].forvariable).scalar && (yyvsp[-1].forvariable).row) {
                                const char* message = "integer FORALL must have just one target variable.";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                         errmsg("integer FORALL must have just one target variable")));
                            }

                            /* create loop's private variable */
                            fvar = (PLpgSQL_var *)
                                plpgsql_build_variable((yyvsp[-1].forvariable).name,
                                                       (yyvsp[-1].forvariable).lineno,
                                                       plpgsql_build_datatype(INT4OID, -1, InvalidOid),
                                                       true);

                            newm = (PLpgSQL_stmt_fori *)palloc0(sizeof(PLpgSQL_stmt_fori));
                            newm->cmd_type = PLPGSQL_STMT_FORI;
                            newm->var	 = fvar;
                            newm->reverse = false;
                            newm->lower	 = expr1;
                            newm->upper	 = expr2;
                            newm->step	 = NULL;

                            (yyval.stmt) = (PLpgSQL_stmt *) newm;
                        }
                        else {
                            const char* message = "please use \'FORALL index_name IN lower_bound .. upper_bound\'";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("please use \'FORALL index_name IN lower_bound .. upper_bound\'")));
                        }
                    }
#line 7600 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 291:
#line 4635 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
#ifndef ENABLE_MULTIPLE_NODES
                        (yyval.boolean) = true;
#else
                        const char* message = "SAVE EXCEPTIONS is not supported";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("%s.", message)));
                        (yyval.boolean) = false;
#endif
                    }
#line 7617 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 292:
#line 4648 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = false;
                    }
#line 7625 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 293:
#line 4670 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.forvariable).name = NameOfDatum(&((yyvsp[0].wdatum)));
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_RECORD)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = (PLpgSQL_rec *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else
                        {
                            int			tok;

                            (yyval.forvariable).scalar = (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                            /* check for comma-separated list */
                            tok = yylex();
                            pltsql_push_back_token(tok);
                            if (tok == ',')
                                (yyval.forvariable).row = read_into_scalar_list((yyval.forvariable).name,
                                                               (yyval.forvariable).scalar,
                                                               (yyval.forvariable).dno,
                                                               (yylsp[0]));
                        }
                    }
#line 7672 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 294:
#line 4713 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.forvariable).name = NameOfDatum(&((yyvsp[0].wdatum)));
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = (PLpgSQL_rec *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else
                        {
                            int			tok;

                            (yyval.forvariable).scalar = (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                            /* check for comma-separated list */
                            tok = yylex();
                            pltsql_push_back_token(tok);
                            if (tok == ',')
                                (yyval.forvariable).row = read_into_scalar_list((yyval.forvariable).name,
                                                               (yyval.forvariable).scalar,
                                                               (yyval.forvariable).dno,
                                                               (yylsp[0]));
                        }
                    }
#line 7712 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 295:
#line 4749 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.forvariable).name = NameOfDatum(&((yyvsp[0].wdatum)));
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_ROW)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = (PLpgSQL_row *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_REC)
                        {
                            (yyval.forvariable).scalar = NULL;
                            (yyval.forvariable).rec = (PLpgSQL_rec *) (yyvsp[0].wdatum).datum;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                        }
                        else
                        {
                            int			tok;

                            (yyval.forvariable).scalar = (yyvsp[0].wdatum).datum;
                            if ((yyvsp[0].wdatum).datum->dtype == PLPGSQL_DTYPE_VAR) {
                                check_autonomous_nest_tablevar((PLpgSQL_var*)(yyvsp[0].wdatum).datum);
                            }
                            (yyval.forvariable).rec = NULL;
                            (yyval.forvariable).row = NULL;
                            (yyval.forvariable).dno = (yyvsp[0].wdatum).dno;
                            /* check for comma-separated list */
                            tok = yylex();
                            pltsql_push_back_token(tok);
                            if (tok == ',')
                                (yyval.forvariable).row = read_into_scalar_list((yyval.forvariable).name,
                                                               (yyval.forvariable).scalar,
                                                               (yyval.forvariable).dno,
                                                               (yylsp[0]));
                        }
                    }
#line 7755 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 296:
#line 4788 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int			tok;

                        (yyval.forvariable).name = (yyvsp[0].word).ident;
                        (yyval.forvariable).lineno = plpgsql_location_to_lineno((yylsp[0]));
                        (yyval.forvariable).scalar = NULL;
                        (yyval.forvariable).rec = NULL;
                        (yyval.forvariable).row = NULL;
                        /* check for comma-separated list */
                        tok = yylex();
                        pltsql_push_back_token(tok);
                        if (tok == ',')
                            word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 7774 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 297:
#line 4803 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 7783 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 298:
#line 4810 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_foreach_a *newp;

                        newp = (PLpgSQL_stmt_foreach_a *)palloc0(sizeof(PLpgSQL_stmt_foreach_a));
                        newp->cmd_type = PLPGSQL_STMT_FOREACH_A;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-6]));
                        newp->label = (yyvsp[-7].str);
                        newp->slice = (yyvsp[-4].ival);
                        newp->expr = (yyvsp[-1].expr);
                        newp->body = (yyvsp[0].loop_body).stmts;
                        newp->sqlString = plpgsql_get_curline_query();

                        if ((yyvsp[-5].forvariable).rec)
                        {
                            newp->varno = (yyvsp[-5].forvariable).dno;
                            check_assignable((PLpgSQL_datum *) (yyvsp[-5].forvariable).rec, (yylsp[-5]));
                        }
                        else if ((yyvsp[-5].forvariable).row)
                        {
                            if ((yyvsp[-5].forvariable).scalar) {
                                newp->varno = (yyvsp[-5].forvariable).row->dno;
                            } else {
                                newp->varno = (yyvsp[-5].forvariable).dno;
                            }
                            check_assignable((PLpgSQL_datum *) (yyvsp[-5].forvariable).row, (yylsp[-5]));
                        }
                        else if ((yyvsp[-5].forvariable).scalar)
                        {
                            newp->varno = (yyvsp[-5].forvariable).dno;
                            check_assignable((yyvsp[-5].forvariable).scalar, (yylsp[-5]));
                        }
                        else
                        {
                            const char* message = "loop variable of FOREACH must be a known variable or list of variables";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                     errmsg("loop variable of FOREACH must be a known variable or list of variables"),
                                             parser_errposition((yylsp[-5]))));
                        }

                        check_labels((yyvsp[-7].str), (yyvsp[0].loop_body).end_label, (yyvsp[0].loop_body).end_label_location);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *) newp;

                        /* register the stmt if it is labeled */
                        record_stmt_label((yyvsp[-7].str), (PLpgSQL_stmt *)newp);
                    }
#line 7837 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 299:
#line 4862 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = 0;
                    }
#line 7845 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 300:
#line 4866 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.ival) = (yyvsp[0].ival);
                    }
#line 7853 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 301:
#line 4871 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.list) = list_make1((yyvsp[0].stmt)); }
#line 7859 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 302:
#line 4873 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.list) = list_make1((yyvsp[0].stmt)); }
#line 7865 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 303:
#line 4877 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_exit *newp;

                        newp = (PLpgSQL_stmt_exit *)palloc0(sizeof(PLpgSQL_stmt_exit));
                        newp->cmd_type = PLPGSQL_STMT_EXIT;
                        newp->is_exit  = (yyvsp[-2].boolean);
                        newp->lineno	  = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->label	  = (yyvsp[-1].str);
                        newp->cond	  = (yyvsp[0].expr);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 7883 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 304:
#line 4893 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = true;
                    }
#line 7891 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 305:
#line 4897 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.boolean) = false;
                    }
#line 7899 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 306:
#line 4901 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "leave label"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                             ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("'LEAVE label' is only in database which dbcompatibility='B'.")));

                        (yyval.boolean) = true;
                    }
#line 7915 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 307:
#line 4913 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * When the database is in mysql compatible mode
                         * support "iterate label"
                         */
                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT)
                             ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("'ITERATE label' is only in database which dbcompatibility='B'.")));

                        (yyval.boolean) = false;
                    }
#line 7931 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 308:
#line 4927 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int	tok;

                        tok = yylex();
                        if (tok == 0)
                            yyerror("unexpected end of function definition");

                        if (tok_is_keyword(tok, &yylval,
                                           K_NEXT, "next"))
                        {
                            u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                            (yyval.stmt) = make_return_next_stmt((yylsp[0]));
                        }
                        else if (tok_is_keyword(tok, &yylval,
                                                K_QUERY, "query"))
                        {
                            u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                            (yyval.stmt) = make_return_query_stmt((yylsp[0]));
                        }
                        else
                        {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = make_return_stmt((yylsp[0]));
                        }
                    }
#line 7961 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 309:
#line 4955 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
			if (!u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_pipelined) {
				const char* message = "PIPE statement cannot be used in non-pipelined functions";
				InsertErrorMessage(message, plpgsql_yylloc);
				ereport(ERROR, (errmodule(MOD_PLSQL), errcode(ERRCODE_SYNTAX_ERROR),
					errmsg("%s", message), errdetail("N/A"),
					errcause("A PIPE statement was used in non-pipelined function"),
					erraction("Use the PIPE statement only in pipelined functions"),
					parser_errposition((yylsp[-2]))));
			}
                    	PLpgSQL_stmt_pipe_row* newp = (PLpgSQL_stmt_pipe_row *)palloc0(sizeof(PLpgSQL_stmt_pipe_row));
		        newp->cmd_type = PLPGSQL_STMT_PIPE_ROW;
                    	newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                    	newp->expr = NULL;
                    	newp->sqlString = plpgsql_get_curline_query();
                    	newp->retvarno = -1;
                    	if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->pipelined_resistuple) {
				int token = yylex();
				bool supportedType = true;
				TupleDesc tupdesc = NULL;
				if (token == T_DATUM) {
					newp->retvarno = yylval.wdatum.dno;
					PLpgSQL_datum *returnVar = yylval.wdatum.datum;
					auto dtype = returnVar->dtype;
					supportedType = dtype == PLPGSQL_DTYPE_ROW ||
						dtype == PLPGSQL_DTYPE_RECORD ||
						dtype == PLPGSQL_DTYPE_CURSORROW ||
						dtype == PLPGSQL_DTYPE_REC;
					switch (dtype) {
						case PLPGSQL_DTYPE_REC:
						case PLPGSQL_DTYPE_CURSORROW:
							tupdesc = ((PLpgSQL_rec *) returnVar)->tupdesc;
							break;
						case PLPGSQL_DTYPE_RECORD:
						case PLPGSQL_DTYPE_ROW:
							tupdesc = ((PLpgSQL_row *) returnVar)->rowtupdesc;
							break;
						default:
							break;
					}
					if (tupdesc != NULL) {
						Oid retyType = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_rettype;
						int32 typmod;
						Oid realRetType = searchsubtypebytypeId(retyType, &typmod);
						TupleDesc expected = lookup_rowtype_tupdesc_copy(realRetType, typmod);
						const char* msg = "expression is of wrong type";
						convert_tuples_by_position(tupdesc, expected, gettext_noop(msg));
					}
				} else if (token == K_NULL) {
					newp->retvarno = -1;
				} else {
					supportedType = false;
				}
				if (!supportedType) {
					yyerror("unknown pipe row type");
				}
				if (yylex() != ')') {
					yyerror("syntax error");
				}
			 } else {
				newp->expr = read_sql_expression(')', ")");		
			}
			int token = yylex();
			if (token != ';') {
				yyerror("syntax error");
			}
			(yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 8034 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 310:
#line 5025 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_raise		*newp;
                        int	tok;
                        
                        PLpgSQL_row * row = NULL;
                        char message[32] = "";
                        StringInfoData ds;
                        initStringInfo(&ds);

                        newp = (PLpgSQL_stmt_raise *)palloc(sizeof(PLpgSQL_stmt_raise));

                        newp->cmd_type	= PLPGSQL_STMT_RAISE;
                        newp->lineno		= plpgsql_location_to_lineno((yylsp[0]));
                        newp->elog_level = ERROR;	/* default */
                        newp->condname	= NULL;
                        newp->message	= NULL;
                        newp->params		= NIL;
                        newp->options	= NIL;
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->hasExceptionInit = false;

                        tok = yylex();
                        if (tok == 0)
                            yyerror("unexpected end of function definition");

                        /*
                         * We could have just RAISE, meaning to re-throw
                         * the current error.
                         */
                        if (tok != ';')
                        {
                            if (T_CWORD == tok) {
                                yyerror("not found package");                      
                            }
                            if ((T_DATUM == tok || T_PACKAGE_VARIABLE == tok) && PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
                            {
                                PLpgSQL_expr *expr = NULL;
                                errno_t rc = EOK;

                                rc = sprintf_s(message, sizeof(message), "line:%d ", plpgsql_location_to_lineno((yylsp[0])));
                                securec_check_ss(rc, "\0", "\0");
                                appendStringInfoString(&ds, message);
                                appendStringInfoString(&ds,"%");

                                row = (PLpgSQL_row *)yylval.wdatum.datum;

                                /* condname is system embedded error name, so it is still null in this case. */
                                newp->hasExceptionInit = row->hasExceptionInit;
                                newp->condname = pstrdup(plpgsql_get_sqlstate(row->customErrorCode));
                                newp->message = pstrdup(ds.data);
                                pltsql_push_back_token(tok);
                                expr = read_sql_construct(';', 0, 0, ";",
                                                          "SELECT ", true, true, true, NULL, &tok);

                                if (tok != ';')
                                    yyerror("syntax error");

                                newp->params = lappend(newp->params, expr);
                            }
                            else
                            {
                                /*
                                 * First is an optional elog severity level.
                                 */
                                if (tok_is_keyword(tok, &yylval,
                                                   K_EXCEPTION, "exception"))
                                {
                                    newp->elog_level = ERROR;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_WARNING, "warning"))
                                {
                                    newp->elog_level = WARNING;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_NOTICE, "notice"))
                                {
                                    newp->elog_level = NOTICE;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_INFO, "info"))
                                {
                                    newp->elog_level = INFO;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_LOG, "log"))
                                {
                                    newp->elog_level = LOG;
                                    tok = yylex();
                                }
                                else if (tok_is_keyword(tok, &yylval,
                                                        K_DEBUG, "debug"))
                                {
                                    newp->elog_level = DEBUG1;
                                    tok = yylex();
                                }
                                
                                if (tok == 0)
                                    yyerror("unexpected end of function definition");

                            /*
                             * Next we can have a condition name, or
                             * equivalently SQLSTATE 'xxxxx', or a string
                             * literal that is the old-style message format,
                             * or USING to start the option list immediately.
                             */
                            if (tok == SCONST)
                            {
                                /* old style message and parameters */
                                newp->message = yylval.str;
                                /*
                                 * We expect either a semi-colon, which
                                 * indicates no parameters, or a comma that
                                 * begins the list of parameter expressions,
                                 * or USING to begin the options list.
                                 */
                                tok = yylex();
                                if (tok != ',' && tok != ';' && tok != K_USING)
                                    yyerror("syntax error");

                                while (tok == ',')
                                {
                                    PLpgSQL_expr *expr;

                                    expr = read_sql_construct(',', ';', K_USING,
                                                              ", or ; or USING",
                                                              "SELECT ",
                                                              true, true, true,
                                                              NULL, &tok);
                                    newp->params = lappend(newp->params, expr);
                                }
                            }
                            else if (tok != K_USING)
                            {
                                /* must be condition name or SQLSTATE */
                                if (tok_is_keyword(tok, &yylval,
                                                   K_SQLSTATE, "sqlstate"))
                                {
                                    /* next token should be a string literal */
                                    char   *sqlstatestr;

                                    if (yylex() != SCONST)
                                        yyerror("syntax error");
                                    sqlstatestr = yylval.str;

                                    if (strlen(sqlstatestr) != 5)
                                        yyerror("invalid SQLSTATE code");
                                    if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                                        yyerror("invalid SQLSTATE code");
                                    newp->condname = sqlstatestr;
                                }
                                else
                                {
                                    if (tok != T_WORD)
                                        yyerror("syntax error");
                                    newp->condname = yylval.word.ident;
                                    plpgsql_recognize_err_condition(newp->condname,
                                                                    false);
                                }
                                tok = yylex();
                                if (tok != ';' && tok != K_USING)
                                    yyerror("syntax error");
                            }

                            if (tok == K_USING)
                                newp->options = read_raise_options();
                            }
                        }
                        pfree_ext(ds.data);
                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 8214 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 311:
#line 5203 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
                            const char *message = "SIGNAL is supported only in B-format database.";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                (errmodule(MOD_PARSER),
                                    errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("SIGNAL is supported only in B-format database."),
                                    parser_errposition((yylsp[0]))));
                            (yyval.stmt) = NULL;/* not reached */		
                        }
                        int tok;
                        PLpgSQL_stmt_signal *newp;

                        newp = (PLpgSQL_stmt_signal *)palloc(sizeof(PLpgSQL_stmt_signal));

                        newp->cmd_type = PLPGSQL_STMT_SIGNAL;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        newp->sqlString = plpgsql_get_curline_query();

                        tok = yylex();
                        if (tok == 0) {
                            yyerror("unexpected end of function definition");
                        }

                        /* must be condition name or SQLSTATE */
                        if (tok_is_keyword(tok, &yylval, K_SQLSTATE, "sqlstate")) {
                            tok = yylex();

                            read_signal_sqlstate(newp, tok);
                        } else {
                            read_signal_condname(newp, tok);
                        }

                        tok = yylex();
                        read_signal_set(newp, tok);

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 8258 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 312:
#line 5245 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if (u_sess->attr.attr_sql.sql_compatibility != B_FORMAT) {
                            const char *message = "RESIGNAL is supported only in B-format database.";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                (errmodule(MOD_PARSER),
                                    errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("RESIGNAL is supported only in B-format database."),
                                    parser_errposition((yylsp[0]))));
                            (yyval.stmt) = NULL;/* not reached */		
                        }
                        int tok;
                        PLpgSQL_stmt_signal *newp;

                        newp = (PLpgSQL_stmt_signal *)palloc(sizeof(PLpgSQL_stmt_signal));

                        newp->cmd_type = PLPGSQL_STMT_RESIGNAL;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        newp->sqlString = plpgsql_get_curline_query();

                        tok = yylex();
                        if (tok == 0) {
                            yyerror("unexpected end of function definition");
                        }

                        if (tok == ';') {
                            newp->sqlerrstate = -1;
                            newp->sqlstate = NULL;
                            newp->condname = NULL;
                            newp->cond_info_item = NIL;
                        } else if (tok == T_WORD && pg_strcasecmp(yylval.word.ident, "set") == 0) {
                            newp->sqlerrstate = -1;
                            newp->sqlstate = NULL;
                            newp->condname = NULL;
                            newp->cond_info_item = read_signal_items();
                        } else if (tok_is_keyword(tok, &yylval, K_SQLSTATE, "sqlstate")) {
                            tok = yylex();
                            
                            read_signal_sqlstate(newp, tok);
                            
                            tok = yylex();
                            read_signal_set(newp, tok);
                        } else {
                            read_signal_condname(newp, tok);
                            
                            tok = yylex();
                            read_signal_set(newp, tok);
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 8314 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 313:
#line 5299 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.loop_body).stmts = (yyvsp[-4].list);
                        (yyval.loop_body).end_label = (yyvsp[-1].str);
                        (yyval.loop_body).end_label_location = (yylsp[-1]);
                    }
#line 8324 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 314:
#line 5307 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.while_body).stmts = (yyvsp[-4].list);
                        (yyval.while_body).end_label = (yyvsp[-1].str);
                        (yyval.while_body).end_label_location = (yylsp[-1]);
                    }
#line 8334 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 315:
#line 5315 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.repeat_condition).expr = (yyvsp[-3].expr);
                        (yyval.repeat_condition).end_label = (yyvsp[-1].str);
                        (yyval.repeat_condition).end_label_location = (yylsp[-1]);
                    }
#line 8344 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 316:
#line 5323 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = read_sql_expression(K_END, "end"); }
#line 8350 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 317:
#line 5337 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.stmt) = make_execsql_stmt(K_ALTER, (yylsp[0]));
                    }
#line 8358 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 318:
#line 5341 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.stmt) = make_execsql_stmt(K_INSERT, (yylsp[0]));
                    }
#line 8366 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 319:
#line 5345 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.stmt) = make_execsql_stmt(K_REPLACE, (yylsp[0]));
                    }
#line 8374 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 320:
#line 5349 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok = -1;

                        tok = yylex();
                        pltsql_push_back_token(tok);
                        (yyval.stmt) = make_execsql_stmt(K_SELECT, (yylsp[0]));
                    }
#line 8386 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 321:
#line 5358 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int			tok = -1;

                        tok = yylex();
                        pltsql_push_back_token(tok);
                        (yyval.stmt) = make_execsql_stmt(K_UPDATE, (yylsp[0]));
                    }
#line 8398 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 322:
#line 5366 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int			tok = -1;

			tok = yylex();
			pltsql_push_back_token(tok);
			(yyval.stmt) = make_execsql_stmt(K_DELETE, (yylsp[0]));
		    }
#line 8410 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 323:
#line 5374 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
		    	(yyval.stmt) = make_execsql_stmt(K_WITH, (yylsp[0]));
		    }
#line 8418 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 324:
#line 5378 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
		    	(yyval.stmt) = make_execsql_stmt(K_MERGE, (yylsp[0]));
		    }
#line 8426 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 325:
#line 5382 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                if (u_sess->attr.attr_sql.sql_compatibility == B_FORMAT)
                {
                    if(pltsql_is_token_match2(T_CWORD,'(')
                       || pltsql_is_token_match2(T_CWORD,';')
                       || pltsql_is_token_match(T_WORD))
                    {
                        (yyval.stmt) = NULL;
                    }
                    else if(pltsql_is_token_match2(K_CALL,'(') || pltsql_is_token_match2(K_CALL,';'))
                    {
                        int    tok = yylex();
                        char*  funcname = yylval.word.ident;
                        PLpgSQL_stmt *stmt = funcname_is_call(funcname, (yylsp[0]));
                        if(stmt != NULL)
                        {
                            (yyval.stmt) = stmt;
                        }
                        else
                        {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = make_execsql_stmt(K_CALL, (yylsp[0]));
                        }
                    }
                    else if(pltsql_is_token_match('(') || pltsql_is_token_match(';'))
                    {
                        char*  funcname = (char*) (yyvsp[0].keyword);
                        PLpgSQL_stmt *stmt = funcname_is_call(funcname, (yylsp[0]));
                        if(stmt != NULL)
                        {
                            (yyval.stmt) = stmt;
                        }
                        else
                        {
                            (yyval.stmt) = make_execsql_stmt(K_CALL, (yylsp[0]));
                        }
                    }
                    else
                    {
                        int    tok = yylex();
                        if(yylval.str != NULL && is_unreserved_keyword_func(yylval.str))
                        {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = NULL;
                        }
                        else
                        {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = make_execsql_stmt(K_CALL, (yylsp[0]));
                        }
                    }
                }
                else
                {
                    if(pltsql_is_token_match('(') || pltsql_is_token_match(';'))
                    {
                        char*  funcname = (char*) (yyvsp[0].keyword);
                        PLpgSQL_stmt *stmt = funcname_is_call(funcname, (yylsp[0]));
                        if(stmt != NULL)
                        {
                            (yyval.stmt) = stmt;
                        }
                        else
                        {
                            (yyval.stmt) = make_execsql_stmt(K_CALL, (yylsp[0]));
                        }
                    }
                    else
                    {
                        (yyval.stmt) = make_execsql_stmt(K_CALL, (yylsp[0]));
                    }
                }
            }
#line 8504 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 326:
#line 5456 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                int tok = -1;
                bool isCallFunc = false;
                bool FuncNoarg = false;

                if (0 == strcasecmp((yyvsp[0].word).ident, "DBMS_LOB")
                && (pltsql_is_token_match2('.', K_OPEN)
                || pltsql_is_token_match2('.', K_CLOSE)))
                    (yyval.stmt) = parse_lob_open_close((yylsp[0]));
                else
                {
                    tok = yylex();
                    if ('(' == tok)
                        isCallFunc = is_function((yyvsp[0].word).ident, false, false);
                    else if ('=' ==tok || COLON_EQUALS == tok || '[' == tok)
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    else if (';' == tok)
                    {
                        isCallFunc = is_function((yyvsp[0].word).ident, false, true);
                        FuncNoarg = true;
                    }

                    pltsql_push_back_token(tok);
                    if(isCallFunc)
                    {
                        if (FuncNoarg)
                        {
                            (yyval.stmt) = make_callfunc_stmt_no_arg((yyvsp[0].word).ident, (yylsp[0]));
                        }
                        else
                        {
                            PLpgSQL_stmt *stmt = make_callfunc_stmt((yyvsp[0].word).ident, (yylsp[0]), false, false);
                            if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                            {
                                ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                            }
                            else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                            {
                                ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                            }
                            (yyval.stmt) = stmt;
                        }
                    }
                    else
                    {
                        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                        (yyval.stmt) = make_execsql_stmt(T_WORD, (yylsp[0]));
                    }
                }
            }
#line 8559 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 327:
#line 5507 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                int tok = -1;
                bool isCallFunc = false;
                bool FuncNoarg = false;
                tok = yylex();
                if ('(' == tok)
                    isCallFunc = is_function((yyvsp[0].keyword), false, false);
                else if (';' == tok)
                {
                    isCallFunc = is_function((yyvsp[0].keyword), false, true);
                    FuncNoarg = true;
                }

                pltsql_push_back_token(tok);
                if(isCallFunc)
                {
                    if (FuncNoarg)
                    {
                        (yyval.stmt) = make_callfunc_stmt_no_arg((yyvsp[0].keyword), (yylsp[0]));
                    }
                    else
                    {
                         PLpgSQL_stmt *stmt = make_callfunc_stmt((yyvsp[0].keyword), (yylsp[0]), false, false);
                         if (stmt == NULL)
                         {
                             pltsql_push_back_token(tok);
                             (yyval.stmt) = NULL;
                             yyerror("syntax error");
                         }
                         else
                         {
                             if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                             {
                                 ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                             }
                             else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                             {
                                ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                             }
                             (yyval.stmt) = stmt;
                        }
                    }
                }
                else
                {
                    pltsql_push_back_token(tok);
                    (yyval.stmt) = NULL;
                    yyerror("syntax error");
                }
            }
#line 8614 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 328:
#line 5558 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                int tok = -1;
                bool FuncNoarg = false;
                char* serverName = NULL;
                PLwdatum *objtypwdatum = NULL;
                PLwdatum objtypwdatumtemp;
                objtypwdatumtemp.datum = yylval.wdatum.datum;
                objtypwdatumtemp.idents = yylval.wdatum.idents;
                objtypwdatumtemp.quoted = yylval.wdatum.quoted;
                objtypwdatumtemp.ident = yylval.wdatum.ident;
                objtypwdatumtemp.dno = yylval.wdatum.dno;
                objtypwdatum = &objtypwdatumtemp;
                PLpgSQL_row* row = (PLpgSQL_row*)yylval.wdatum.datum;
                HeapTuple typtuple = NULL;
                Form_pg_type typeform = NULL;
                char* nspname = NULL;
                StringInfoData ds;
                char* name = NULL;
                bool isCallFunc = false;

                initStringInfo(&ds);
                typtuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(row->datatype->typoid));
                if (!HeapTupleIsValid(typtuple))
                    ereport(errstate,
                                (errcode(ERRCODE_UNDEFINED_OBJECT),
                                errmsg("object type with OID %u does not exist", row->datatype->typoid)));
                typeform = (Form_pg_type)GETSTRUCT(typtuple);
                nspname = get_namespace_name(typeform->typnamespace);
                if (NULL != nspname) {
                    appendStringInfo(&ds, "%s.",nspname);
                }
                appendStringInfo(&ds, "%s",objtypwdatumtemp.ident);
                ReleaseSysCache(typtuple);
                name = pstrdup(ds.data);
                pfree_ext(ds.data);

                tok = yylex();
                if ('(' == tok)
                    isCallFunc = is_function(name, false, false);
                else if (';' == tok) {
                    isCallFunc = is_function(name, false, false);
                    FuncNoarg = true;
                }
                pltsql_push_back_token(tok);
                if (isCallFunc) {
                    if (FuncNoarg) {
                        (yyval.stmt) = make_callfunc_stmt_no_arg(name, (yylsp[0]));
                    } else {
                        PLpgSQL_stmt* stmt = make_callfunc_stmt(name, (yylsp[0]), false, false, NULL, -1, false, objtypwdatum);
                        if (stmt->cmd_type == PLPGSQL_STMT_PERFORM) {
                            ((PLpgSQL_stmt_perform*)stmt)->expr->is_funccall = true;
                        } else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL) {
                            ((PLpgSQL_stmt_execsql*)stmt)->sqlstmt->is_funccall = true;
                        }
                        (yyval.stmt) = stmt;
                    }
                } else {
                    (yyval.stmt) = make_execsql_stmt(T_OBJECT_TYPE_VAR_METHOD, (yylsp[0]));
                }
            }
#line 8679 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 329:
#line 5619 "src/pltsql/gram.y" /* yacc.c:1646  */
    {

                char *name = NULL;
                bool isCallFunc = false;
                bool FuncNoarg = false;
                checkFuncName((yyvsp[-1].cword).idents);
                MemoryContext colCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
                name = NameListToString((yyvsp[-1].cword).idents);
                (void)MemoryContextSwitchTo(colCxt);
                isCallFunc = is_function(name, false, false, (yyvsp[-1].cword).idents);
                if (isCallFunc)
                {    
                    if (FuncNoarg)
                        (yyval.stmt) = make_callfunc_stmt_no_arg(name, (yylsp[-1]), false, (yyvsp[-1].cword).idents); 
                    else 
                    {    
                        PLpgSQL_stmt *stmt = make_callfunc_stmt(name, (yylsp[-1]), false, true, (yyvsp[-1].cword).idents);
                        if (stmt == NULL) {
                            const char* message = "call func stmt was null";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(ERROR,
                                    (errcode(ERRCODE_DATA_EXCEPTION),
                                        errmsg("call func stmt was null"),
                                        errdetail("call func stmt was null, you can try to recompile the function")));
                        }
                        if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                        {    
                            ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                        }    
                        else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                        {    
                            ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                        }    
                        (yyval.stmt) = stmt;
                    }    
                }    
                else 
                {    
                    (yyval.stmt) = make_execsql_stmt(T_CWORD, (yylsp[-1])); 
                }    
            }
#line 8725 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 330:
#line 5661 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                char *name = NULL;
                bool isCallFunc = false;
                checkFuncName((yyvsp[-1].cword).idents);
                MemoryContext colCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
                name = NameListToString((yyvsp[-1].cword).idents);
                (void)MemoryContextSwitchTo(colCxt);
                isCallFunc = is_function(name, false, true, (yyvsp[-1].cword).idents);
                bool FuncNoarg = true;

                if (isCallFunc)
                {
                    if (FuncNoarg)
                        (yyval.stmt) = make_callfunc_stmt_no_arg(name, (yylsp[-1]), true, (yyvsp[-1].cword).idents);
                    else
                    {
                        PLpgSQL_stmt *stmt = make_callfunc_stmt(name, (yylsp[-1]), false, true, (yyvsp[-1].cword).idents);
                        if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
                        {
                            ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
                        }
                        else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
                        {
                            ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
                        }
                        (yyval.stmt) = stmt;
                    }
                }
                else
                {
                    (yyval.stmt) = make_execsql_stmt(T_CWORD, (yylsp[-1]));
                }
            }
#line 8763 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 331:
#line 5695 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                check_table_index(yylval.wdatum.datum, "trim");
                int dno = yylval.wdatum.dno;
                StringInfoData sqlBuf;
                initStringInfo(&sqlBuf);
                appendStringInfo(&sqlBuf, "array_trim(");
                CastArrayNameToArrayFunc(&sqlBuf, yylval.wdatum.idents);

                int tok = yylex();

                if (';' == tok)
                {
                    appendStringInfo(&sqlBuf, " 1)");
                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                } else {
                    if('(' == tok) {
                        int tok1 = yylex();
                        if (tok1 == ')') {
                            int tok2 = yylex();
                            if (tok2 == ';') {
                                appendStringInfo(&sqlBuf, " 1)");
                                (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                            } else {
                                pltsql_push_back_token(tok);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            }
                        } else if (tok1 != ICONST && tok1 != T_WORD && tok1 != SCONST && tok1 != T_DATUM) {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = NULL;
                            yyerror("syntax error");
                        } else {
                            if (ICONST == tok1) {
                                appendStringInfo(&sqlBuf, " %d)", yylval.ival);
                            } else if (SCONST == tok1) {
                                appendStringInfo(&sqlBuf, " %s)  ", yylval.str);
                            } else if (T_WORD == tok1) {
                                appendStringInfo(&sqlBuf, " %s)", yylval.word.ident);
                            } else {
                                char *datName = NameOfDatum(&yylval.wdatum);
                                appendStringInfo(&sqlBuf, " %s)  ",  datName);
                                pfree_ext(datName);
                            }
                            int tok2 = yylex();
                            if (tok2 != ')') {
                                pltsql_push_back_token(tok);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            } else {
                                int tok3 = yylex();
                                if (tok3 != ';') {
                                    pltsql_push_back_token(tok);
                                    (yyval.stmt) = NULL;
                                    yyerror("syntax error");
                                } else {
                                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                                }
                            }
                        }
                    } else {
                        pltsql_push_back_token(tok);
                        (yyval.stmt) = NULL;
                        yyerror("syntax error");
                    }
                }
                pfree_ext(sqlBuf.data);
            }
#line 8835 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 332:
#line 5763 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                check_table_index(yylval.wdatum.datum, "extend");
                int dno = yylval.wdatum.dno;
                StringInfoData sqlBuf;

                initStringInfo(&sqlBuf);
                appendStringInfo(&sqlBuf, "array_extendnull(");
                CastArrayNameToArrayFunc(&sqlBuf, yylval.wdatum.idents);

                int tok = yylex();
                if (';' == tok)
                {
                    appendStringInfo(&sqlBuf, " 1)");
                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                } else {
                    if('(' == tok) {
                        int tok1 = yylex();
                        if (tok1 == ')') {
                            int tok2 = yylex();
                            if (tok2 == ';') {
                                appendStringInfo(&sqlBuf, " 1)");
                                (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                            } else {
                                pltsql_push_back_token(tok2);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            }
                        } else if (tok1 != ICONST && tok1 != T_WORD && tok1 != SCONST && tok1 != T_DATUM) {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = NULL;
                            yyerror("syntax error");
                        } else {
                            if (ICONST == tok1) {
                                appendStringInfo(&sqlBuf, " %d)  ",  yylval.ival);
                            } else if (SCONST == tok1) {
                                appendStringInfo(&sqlBuf, " %s)  ", yylval.str);
                            } else if (T_WORD == tok1) {
                                appendStringInfo(&sqlBuf, " %s)  ", yylval.word.ident);
                            } else {
                                char *datName = NameOfDatum(&yylval.wdatum);
                                appendStringInfo(&sqlBuf, " %s)  ",  datName);
                                pfree_ext(datName);
                            }
                            int tok2 = yylex();
                            if (tok2 != ')') {
                                pltsql_push_back_token(tok);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            } else {
                                int tok3 = yylex();
                                if (tok3 != ';') {
                                    pltsql_push_back_token(tok);
                                    (yyval.stmt) = NULL;
                                    yyerror("syntax error");
                                } else {
                                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                                }
                            }
                        }
                    } else {
                        pltsql_push_back_token(tok);
                        (yyval.stmt) = NULL;
                        yyerror("syntax error");
                    }
                }
                pfree_ext(sqlBuf.data);
            }
#line 8907 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 333:
#line 5831 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                StringInfoData sqlBuf;
                Oid indexType = get_table_index_type(yylval.wdatum.datum, NULL);
                List* idents = yylval.wdatum.idents;
                int dno = yylval.wdatum.dno;
                initStringInfo(&sqlBuf);

                int tok = yylex();

                if (';' == tok)
                {
                    if (indexType == VARCHAROID || indexType == INT4OID) {
                        appendStringInfo(&sqlBuf, "array_indexby_delete(");
                        CastArrayNameToArrayFunc(&sqlBuf, idents, false);
                        appendStringInfo(&sqlBuf, ")");
                    } else {
                        appendStringInfo(&sqlBuf, "array_delete(");
                        CastArrayNameToArrayFunc(&sqlBuf, idents, false);
                        appendStringInfo(&sqlBuf, ")");
                    }
                    
                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                } else {
                    if('(' == tok) {
                        int tok1 = yylex();
                        if (tok1 == ')') {
                            int tok2 = yylex();
                            if (tok2 == ';') {
                                if (indexType == VARCHAROID || indexType == INT4OID) {
                                    appendStringInfo(&sqlBuf, "array_indexby_delete(");
                                    CastArrayNameToArrayFunc(&sqlBuf, idents, false);
                                    appendStringInfo(&sqlBuf, ")");
                                } else {
                                    appendStringInfo(&sqlBuf, "array_delete(");
                                    CastArrayNameToArrayFunc(&sqlBuf, idents, false);
                                    appendStringInfo(&sqlBuf, ")");
                                }
                                
                                (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                            } else {
                                pltsql_push_back_token(tok);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            }
                        } else if (tok1 != ICONST && tok1 != T_WORD && tok1 != SCONST && tok1 != T_DATUM && tok1 != '-') {
                            pltsql_push_back_token(tok);
                            (yyval.stmt) = NULL;
                            yyerror("syntax error");
                        } else {
                            if (tok1 == '-') {
                                if (indexType == VARCHAROID) {
                                    yyerror("syntax error");
                                }

                                int tok3 = yylex();
                                if (tok3 != ICONST && tok3 != T_WORD && tok3 != T_DATUM) {
                                    yyerror("syntax error");
                                }

                                if (ICONST == tok3) {
                                    if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'-%d\')", yylval.ival);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "-%d)", yylval.ival);
                                    }
                                } else if (T_WORD == tok3) {
                                    if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'-%s\')", yylval.word.ident);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "-%s)", yylval.word.ident);
                                    }

                                } else {
                                    char *datName = NameOfDatum(&yylval.wdatum);
                                    if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'-%s\')", datName);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "-%s)", datName);
                                    }

                                    pfree_ext(datName);
                                }
                            } else {
                                if (ICONST == tok1) {
                                    if (indexType == VARCHAROID) {
                                        appendStringInfo(&sqlBuf, "array_varchar_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%d\')", yylval.ival);
                                    } else if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%d\')", yylval.ival);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "%d)", yylval.ival);
                                    }
                                } else if (SCONST == tok1) {
                                    if (indexType == VARCHAROID) {
                                        appendStringInfo(&sqlBuf, "array_varchar_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", yylval.str);
                                    } else if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", yylval.str);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "%s)", yylval.str);
                                    }

                                } else if (T_WORD == tok1) {
                                    if (indexType == VARCHAROID) {
                                        appendStringInfo(&sqlBuf, "array_varchar_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", yylval.word.ident);
                                    } else if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", yylval.word.ident);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "%s)", yylval.word.ident);
                                    }

                                } else {
                                    char *datName = NameOfDatum(&yylval.wdatum);
                                    if (indexType == VARCHAROID) {
                                        appendStringInfo(&sqlBuf, "array_varchar_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", datName);
                                    } else if (indexType == INT4OID) {
                                        appendStringInfo(&sqlBuf, "array_integer_delete(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "\'%s\')", datName);
                                    } else {
                                        appendStringInfo(&sqlBuf, "array_deleteidx(");
                                        CastArrayNameToArrayFunc(&sqlBuf, idents);
                                        appendStringInfo(&sqlBuf, "%s)", datName);
                                    }

                                    pfree_ext(datName);
                                }
                            }
                            
                            int tok2 = yylex();
                            if (tok2 != ')') {
                                pltsql_push_back_token(tok);
                                (yyval.stmt) = NULL;
                                yyerror("syntax error");
                            } else {
                                int tok3 = yylex();
                                if (tok3 != ';') {
                                    pltsql_push_back_token(tok);
                                    (yyval.stmt) = NULL;
                                    yyerror("syntax error");
                                } else {
                                    (yyval.stmt) = make_callfunc_stmt(sqlBuf.data, (yylsp[0]), false, false, NULL, dno);
                                }
                            }
                        }
                    } else {
                        pltsql_push_back_token(tok);
                        (yyval.stmt) = NULL;
                        yyerror("syntax error");
                    }
                }
                pfree_ext(sqlBuf.data);
            }
#line 9095 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 334:
#line 6017 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_dynexecute *newp;
                        PLpgSQL_expr *expr;
                        int endtoken;


                        if((endtoken = yylex()) != K_IMMEDIATE)
                        {
                            pltsql_push_back_token(endtoken);
                        }

                        expr = read_sql_construct(K_INTO, K_USING, ';',
                                                      "INTO or USING or ;",
                                                      "SELECT ",
                                                      true, true, true,
                                                      NULL, &endtoken);
                        newp = (PLpgSQL_stmt_dynexecute*)palloc0(sizeof(PLpgSQL_stmt_dynexecute));
                        newp->cmd_type = PLPGSQL_STMT_DYNEXECUTE;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[0]));
                        newp->query = expr;
                        newp->into = false;
                        newp->strict = false;
                        newp->rec = NULL;
                        newp->row = NULL;
                        newp->params = NIL;
                        newp->out_row = NULL;
                        newp->isinouttype = false;
                        newp->ppd = NULL;
                        newp->isanonymousblock = true;
                        newp->sqlString = plpgsql_get_curline_query();

                        if (endtoken == K_BULK)
                        {
                            yyerror("syntax error");
                        }

                        /* If we found "INTO", collect the argument */
                        
                        if (endtoken == K_INTO)
                        {
                            if (newp->into)			/* multiple INTO */
                                yyerror("syntax error");
                            newp->into = true;
                            (void)read_into_target(&newp->rec, &newp->row, &newp->strict, K_EXECUTE, false);
                            endtoken = yylex();
                        }
                        /* If we found "USING", collect the argument */
                        if(endtoken == K_USING)
                        {
                            PLpgSQL_row *out_row;
                            if (newp->params)		/* multiple USING */
                                    yyerror("syntax error");

                            read_using_target(&(newp->params), &out_row );
                            if(out_row)
                            {
                                newp->into   = true;
                                newp->strict = true;
                                newp->isinouttype = true;
                            }
                            endtoken = yylex();
                            if( out_row && newp->row )
                            {
                                const char* message = "target into is conflicted with using out (inout)";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                                            errmsg("target into is conflicted with using out (inout)"),
                                            errdetail("\"select clause\" can't has out parameters, can only use \"into\"")));
                                u_sess->plsql_cxt.have_error = true;
                            }
                            newp->out_row = newp->row ? newp->row:out_row;

                        }
                        if (endtoken != ';')
                            yyerror("syntax error");

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 9179 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 335:
#line 6099 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_open *newp;
                        int				  tok;
                        int					 endtoken;
                        PLpgSQL_row			*out_row = NULL;

                        newp = (PLpgSQL_stmt_open *)palloc0(sizeof(PLpgSQL_stmt_open));
                        newp->cmd_type = PLPGSQL_STMT_OPEN;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->curvar = (yyvsp[0].ival);
                        newp->cursor_options = CURSOR_OPT_FAST_PLAN;
                        newp->sqlString = plpgsql_get_curline_query();

#ifndef ENABLE_MULTIPLE_NODES
                        if ((newp->curvar < 0 || newp->curvar >= u_sess->plsql_cxt.curr_compile_context->plpgsql_nDatums)
                            && u_sess->attr.attr_common.plsql_show_all_error) 
                        {
                            ereport(ERROR,
                                    (errcode(ERRCODE_SYNTAX_ERROR), errmsg("open cursor error"), parser_errposition((yylsp[0]))));
                        }
#endif
                        PLpgSQL_var* cur_var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[(yyvsp[0].ival)];

                        if (cur_var->cursor_explicit_expr == NULL)
                        {
                            /* be nice if we could use opt_scrollable here */
                            tok = yylex();
                            if (tok_is_keyword(tok, &yylval,
                                               K_NO, "no"))
                            {
                                tok = yylex();
                                if (tok_is_keyword(tok, &yylval,
                                                   K_SCROLL, "scroll"))
                                {
                                    newp->cursor_options |= CURSOR_OPT_NO_SCROLL;
                                    tok = yylex();
                                }
                            }
                            else if (tok_is_keyword(tok, &yylval,
                                                    K_SCROLL, "scroll"))
                            {
                                newp->cursor_options |= CURSOR_OPT_SCROLL;
                                tok = yylex();
                            }

                            if (tok != K_FOR)
                                yyerror("syntax error, expected \"FOR\"");

                            tok = yylex();
                            if (tok == K_EXECUTE)
                            {
                                newp->dynquery = read_sql_stmt("select ");
                            }
                            else
                            {
                                pltsql_push_back_token(tok);
                                
                                if (tok == K_SELECT || tok == K_WITH)
                                {
                                    newp->query = read_sql_stmt("");
                                }
                                else
                                {
                                    newp->dynquery =
                                    read_sql_expression2(K_USING, ';',
                                                         "USING or ;",
                                                         &endtoken);
                                    /* If we found "USING", collect argument(s) */
                                    if(K_USING == endtoken)
                                    {
                                        read_using_target(&(newp->params), &out_row);
                                        if(NULL != out_row)
                                            yyerror("syntax error");
                                        if(NULL == newp->params)
                                            yyerror("syntax error");					  	

                                        endtoken = pltsql_yylex();
                                    }

                                    if(';' != endtoken )
                                        yyerror("syntax error");
                                }
                            }
#ifndef ENABLE_MULTIPLE_NODES
			    if (newp->query != NULL && u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
                                (void)getCursorTupleDesc(newp->query, false, true);
			    }
                            else if (newp->dynquery != NULL && u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK)
			    {
                                (void)getCursorTupleDesc(newp->dynquery, false, true);
			    }
#endif
                        }
                        else
                        {
                            /* predefined cursor query, so read args */
                            newp->argquery = read_cursor_args(cur_var, ';', ";");
                        }

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 9285 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 336:
#line 6203 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_fetch *fetch = (yyvsp[-2].fetch);
                        PLpgSQL_rec	   *rec;
                        PLpgSQL_row	   *row;

                        /* We have already parsed everything through the INTO keyword */
                        (void)read_into_target(&rec, &row, NULL, -1, false);

                        if (yylex() != ';')
                            yyerror("syntax error");

                        /*
                         * We don't allow multiple rows in PL/pgSQL's FETCH
                         * statement, only in MOVE.
                         */
                        if (fetch->returns_multiple_rows) {
                            const char* message = "FETCH statement cannot return multiple rows";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("FETCH statement cannot return multiple rows"),
                                     parser_errposition((yylsp[-3]))));
                        }

                        fetch->lineno = plpgsql_location_to_lineno((yylsp[-3]));
                        fetch->rec		= rec;
                        fetch->row		= row;
                        fetch->curvar	= (yyvsp[-1].ival);
                        fetch->is_move	= false;
                        fetch->bulk_collect = false;
                        fetch->sqlString = plpgsql_get_curline_query();

                        CheckParallelCursorOpr(fetch);

                        (yyval.stmt) = (PLpgSQL_stmt *)fetch;
                    }
#line 9326 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 337:
#line 6240 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_fetch *fetch = (yyvsp[-6].fetch);
                        if (fetch->has_direction) {
                            yyerror("unexpected fetch direction statement");
                        }

                        /* only A_FORMAT can use bulk collect into */
                        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                             yyerror("syntax error");
                        }

                        PLpgSQL_datum *datum = (yyvsp[-1].datum);
                        fetch->rec		= NULL;
                        fetch->row		= NULL;

                        if (datum->dtype == PLPGSQL_DTYPE_REC) {
                            fetch->rec = (PLpgSQL_rec *)datum;
                        } else {
                            fetch->row = (PLpgSQL_row *)datum;
                            check_bulk_into_type((PLpgSQL_row*)datum);
                        }

                        fetch->lineno = plpgsql_location_to_lineno((yylsp[-7]));
                        fetch->curvar	= (yyvsp[-5].ival);
                        fetch->how_many	= FETCH_ALL;
                        fetch->is_move	= false;
                        fetch->expr		= (yyvsp[0].expr);
                        fetch->returns_multiple_rows = true;
                        fetch->bulk_collect = true;
                        fetch->sqlString = plpgsql_get_curline_query();

                        CheckParallelCursorOpr(fetch);

                        (yyval.stmt) = (PLpgSQL_stmt *)fetch;
                    }
#line 9366 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 338:
#line 6278 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_fetch *fetch = (yyvsp[-2].fetch);

                        fetch->lineno = plpgsql_location_to_lineno((yylsp[-3]));
                        fetch->curvar	= (yyvsp[-1].ival);
                        fetch->is_move	= true;
                        fetch->bulk_collect = false;
                        fetch->sqlString = plpgsql_get_curline_query();

                        CheckParallelCursorOpr(fetch);

                        (yyval.stmt) = (PLpgSQL_stmt *)fetch;
                    }
#line 9384 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 339:
#line 6294 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.fetch) = read_fetch_direction();
                    }
#line 9392 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 340:
#line 6300 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.expr) = read_sql_expression(';', ";");
                    }
#line 9400 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 341:
#line 6304 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.expr) = NULL;
                    }
#line 9408 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 342:
#line 6310 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_datum *datum = NULL;
                        PLpgSQL_rec *rec;
                        PLpgSQL_row *row;
                        (void)read_into_target(&rec, &row, NULL, -1, true);

                        if (rec != NULL) {
                            datum = (PLpgSQL_datum *)rec;
                        } else {
                            datum = (PLpgSQL_datum *)row;
                        }
                        (yyval.datum) = datum;
                    }
#line 9426 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 343:
#line 6326 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_close *newp;

                        newp = (PLpgSQL_stmt_close *)palloc(sizeof(PLpgSQL_stmt_close));
                        newp->cmd_type = PLPGSQL_STMT_CLOSE;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->curvar = (yyvsp[-1].ival);
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 9442 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 344:
#line 6340 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* We do building a node for NULL for GOTO */
                        PLpgSQL_stmt_null *newp;

                        newp = (PLpgSQL_stmt_null *)palloc(sizeof(PLpgSQL_stmt_null));
                        newp->cmd_type = PLPGSQL_STMT_NULL;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->sqlString = plpgsql_get_curline_query();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                    }
#line 9458 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 345:
#line 6354 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_commit *newp;

                        newp = (PLpgSQL_stmt_commit *)palloc(sizeof(PLpgSQL_stmt_commit));
                        newp->cmd_type = PLPGSQL_STMT_COMMIT;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-1]));
                        newp->sqlString = plpgsql_get_curline_query();
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                        record_stmt_label((yyvsp[-2].str), (PLpgSQL_stmt *)newp);
                    }
#line 9475 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 346:
#line 6371 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 9483 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 347:
#line 6378 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 9491 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 348:
#line 6382 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 9499 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 349:
#line 6388 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = NULL;
                    }
#line 9507 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 350:
#line 6392 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 9515 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 351:
#line 6398 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[-1].str) == NULL) {
                            PLpgSQL_stmt_rollback *newp;

                            newp = (PLpgSQL_stmt_rollback *) palloc(sizeof(PLpgSQL_stmt_rollback));
                            newp->cmd_type = PLPGSQL_STMT_ROLLBACK;
                            newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->sqlString = plpgsql_get_curline_query();
                            plpgsql_ns_pop();

                            (yyval.stmt) = (PLpgSQL_stmt *)newp;
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        } else {
                            PLpgSQL_stmt_savepoint *newp;

                            newp = (PLpgSQL_stmt_savepoint *) palloc(sizeof(PLpgSQL_stmt_savepoint));
                            newp->cmd_type = PLPGSQL_STMT_SAVEPOINT;
                            newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                            newp->sqlString = plpgsql_get_curline_query();
                            newp->opType = PLPGSQL_SAVEPOINT_ROLLBACKTO;
                            newp->spName = (yyvsp[-1].str);
                            plpgsql_ns_pop();

                            (yyval.stmt) = (PLpgSQL_stmt *)newp;
                            record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                        }
                    }
#line 9547 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 352:
#line 6428 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_savepoint *newp;

                        newp = (PLpgSQL_stmt_savepoint *) palloc(sizeof(PLpgSQL_stmt_savepoint));
                        newp->cmd_type = PLPGSQL_STMT_SAVEPOINT;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->opType = PLPGSQL_SAVEPOINT_CREATE;
                        newp->spName = (yyvsp[-1].str);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                        record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                    }
#line 9566 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 353:
#line 6443 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_stmt_savepoint *newp;

                        newp = (PLpgSQL_stmt_savepoint *) palloc(sizeof(PLpgSQL_stmt_savepoint));
                        newp->cmd_type = PLPGSQL_STMT_SAVEPOINT;
                        newp->lineno = plpgsql_location_to_lineno((yylsp[-2]));
                        newp->sqlString = plpgsql_get_curline_query();
                        newp->opType = PLPGSQL_SAVEPOINT_RELEASE;
                        newp->spName = (yyvsp[-1].str);
                        plpgsql_ns_pop();

                        (yyval.stmt) = (PLpgSQL_stmt *)newp;
                        record_stmt_label((yyvsp[-3].str), (PLpgSQL_stmt *)newp);
                    }
#line 9585 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 354:
#line 6460 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].wdatum).datum->dtype != PLPGSQL_DTYPE_VAR) {
                            const char* message = "cursor variable must be a simple variable";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("cursor variable must be a simple variable"),
                                     parser_errposition((yylsp[0]))));
                        }

                        if (((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->datatype->typoid != REFCURSOROID) {
                            const char* message = "variable must be of type cursor or refcursor";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("variable \"%s\" must be of type cursor or refcursor",
                                            ((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->refname),
                                     parser_errposition((yylsp[0]))));
                        }
                        if (((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->ispkg) {
                            if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                                u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_autonomous) {
                                const char* message =
                                    "package cursor referenced in autonomous procedure is not supported yet";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("cursor referenced by \"%s\" in autonomous procedure is not supported yet",
                                         (yyvsp[0].wdatum).ident),
                                     parser_errposition((yylsp[0]))));
                            }
                        }
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                    }
#line 9624 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 355:
#line 6495 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].wdatum).datum->dtype != PLPGSQL_DTYPE_VAR) {
                            const char* message = "cursor variable must be a simple variable";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("cursor variable must be a simple variable"),
                                     parser_errposition((yylsp[0]))));
                        }

                        if (((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->datatype->typoid != REFCURSOROID) {
                            const char* message = "variable must be of type cursor or refcursor";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                                     errmsg("variable \"%s\" must be of type cursor or refcursor",
                                            ((PLpgSQL_var *) (yyvsp[0].wdatum).datum)->refname),
                                     parser_errposition((yylsp[0]))));
                        }
                        if (list_length((yyvsp[0].wdatum).idents) == 3) {
                            const char* message =
                                "cursor referenced in schema.package.cursor format is not supported yet";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("cursor referenced by \"%s\" is not supported yet",
                                            (yyvsp[0].wdatum).ident),
                                     parser_errposition((yylsp[0]))));
                        }
                        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_autonomous) {
                            const char* message =
                                "package cursor referenced in autonomous procedure is not supported yet";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                     errmsg("cursor referenced by \"%s\" in autonomous procedure is not supported yet",
                                            (yyvsp[0].wdatum).ident),
                                     parser_errposition((yylsp[0]))));
                        }
                        if (enable_plpgsql_gsdependency()) {
                            gsplsql_build_gs_variable_dependency((yyvsp[0].wdatum).idents);
                        }
                        (yyval.ival) = (yyvsp[0].wdatum).dno;
                    }
#line 9674 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 356:
#line 6541 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* just to give a better message than "syntax error" */
                        word_is_not_variable(&((yyvsp[0].word)), (yylsp[0]));
                    }
#line 9683 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 357:
#line 6546 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* just to give a better message than "syntax error" */
                        cword_is_not_variable(&((yyvsp[0].cword)), (yylsp[0]));
                    }
#line 9692 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 358:
#line 6553 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.exception_block) = NULL; }
#line 9698 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 359:
#line 6555 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /*
                         * We use a mid-rule action to add these
                         * special variables to the namespace before
                         * parsing the WHEN clauses themselves.  The
                         * scope of the names extends to the end of the
                         * current block.
                         */
                        PLpgSQL_exception_block *newp = (PLpgSQL_exception_block *)palloc(sizeof(PLpgSQL_exception_block));

                        (yyval.exception_block) = newp;
                    }
#line 9715 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 360:
#line 6568 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_exception_block *newp = (yyvsp[-1].exception_block);
                        newp->exc_list = (yyvsp[0].list);

                        (yyval.exception_block) = newp;
                    }
#line 9726 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 361:
#line 6577 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            (yyval.list) = lappend((yyvsp[-1].list), (yyvsp[0].exception));
                        }
#line 9734 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 362:
#line 6581 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            (yyval.list) = list_make1((yyvsp[0].exception));
                        }
#line 9742 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 363:
#line 6587 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        PLpgSQL_exception *newp;

                        newp = (PLpgSQL_exception *)palloc0(sizeof(PLpgSQL_exception));
                        newp->lineno     = plpgsql_location_to_lineno((yylsp[-3]));
                        newp->conditions = (yyvsp[-2].condition);
                        newp->action	    = (yyvsp[0].list);

                        (yyval.exception) = newp;
                    }
#line 9757 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 364:
#line 6600 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            PLpgSQL_condition	*old;

                            for (old = (yyvsp[-2].condition); old->next != NULL; old = old->next)
                                /* skip */ ;
                            old->next = (yyvsp[0].condition);
                            (yyval.condition) = (yyvsp[-2].condition);
                        }
#line 9770 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 365:
#line 6609 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            (yyval.condition) = (yyvsp[0].condition);
                        }
#line 9778 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 366:
#line 6615 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                            if (strcmp((yyvsp[0].str), "sqlstate") != 0)
                            {
                                if (PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
                                {
                                    PLpgSQL_condition *newp = NULL;
                                    PLpgSQL_row* row = (PLpgSQL_row*)yylval.wdatum.datum;
                                    if (!row->ispkg) {
                                        row = ( PLpgSQL_row* ) u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[yylval.wdatum.dno];
                                    } else {
                                        PLpgSQL_package* pkg = row->pkg;
                                        row = ( PLpgSQL_row* )pkg->datums[row->dno];
                                    }
                                    TupleDesc	rowtupdesc = row ? row->rowtupdesc : NULL;

                                    if(rowtupdesc && 
                                        0 == strcmp(format_type_be(rowtupdesc->tdtypeid), "exception"))
                                    {
                                        newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                                        newp->sqlerrstate = row->customErrorCode;
                                        newp->condname = pstrdup(row->refname);
                                        newp->next = NULL;
                                    }

                                    if(NULL == newp) {
                                        const char* message = "unrecognized exception condition ";
                                        InsertErrorMessage(message, plpgsql_yylloc);
                                        ereport(errstate,
                                                (errcode(ERRCODE_UNDEFINED_OBJECT),
                                                errmsg("unrecognized exception condition \"%s\"",
                                                        row? row->refname : "??")));
                                    }
                                    (yyval.condition) = newp;
                                }
                                else {
                                    u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
                                    (yyval.condition) = plpgsql_parse_err_condition((yyvsp[0].str));
                                }
                            }
                            else
                            {
                                PLpgSQL_condition *newp;
                                char   *sqlstatestr;

                                /* next token should be a string literal */
                                if (yylex() != SCONST)
                                    yyerror("syntax error");
                                sqlstatestr = yylval.str;

                                if (strlen(sqlstatestr) != 5)
                                    yyerror("invalid SQLSTATE code");
                                if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
                                    yyerror("invalid SQLSTATE code");

                                newp = (PLpgSQL_condition *)palloc(sizeof(PLpgSQL_condition));
                                newp->sqlerrstate =
                                    MAKE_SQLSTATE(sqlstatestr[0],
                                                  sqlstatestr[1],
                                                  sqlstatestr[2],
                                                  sqlstatestr[3],
                                                  sqlstatestr[4]);
                                newp->condname = sqlstatestr;
                                newp->next = NULL;

                                (yyval.condition) = newp;
                            }
                        }
#line 9850 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 367:
#line 6684 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        /* 
                         * support invoking function with out
                         * argument in a := expression 
                         */
                        PLpgSQL_stmt *stmt = NULL;
                        int tok = -1;
                        char *name = NULL;
                        bool isCallFunc = false;
                        PLpgSQL_expr* expr = NULL;
                        List* funcNameList = NULL;
                        PLwdatum *objtypwdatum = NULL;
                        PLwdatum objtypwdatumtemp;
                        if (!enable_out_param_override() && u_sess->parser_cxt.isPerform) {
                            u_sess->parser_cxt.isPerform = false;
                        }
                        if (pltsql_is_token_match2(T_WORD, '(') || 
                            pltsql_is_token_match2(T_CWORD,'('))
                        {
                            tok = yylex();
                            if (T_WORD == tok) {
                                name = yylval.word.ident;
                                isCallFunc = is_function(name, true, false);
                            } else {
                                checkFuncName(yylval.cword.idents);
                                funcNameList = yylval.cword.idents;
                                name = NameListToString(yylval.cword.idents);
                                isCallFunc = is_function(name, true, false, yylval.cword.idents);
                            }

                        } else if (u_sess->parser_cxt.isPerform && enable_out_param_override()) {
                            u_sess->parser_cxt.isPerform = false;
                            const char* message = "perform not support expression when open guc proc_outparam_override";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                        (errcode(ERRCODE_SYNTAX_ERROR),
                                        errmsg("perform not support expression when open guc proc_outparam_override")));
                            u_sess->plsql_cxt.have_error = true;
                        } else if (pltsql_is_token_match2(T_OBJECT_TYPE_VAR_METHOD,'(')) {
                            PLpgSQL_row *row = NULL;
                            HeapTuple typtuple = NULL;
                            Form_pg_type typeform = NULL;
                            char* nspname = NULL;
                            StringInfoData ds;
                            tok = yylex();
                            objtypwdatumtemp.datum = yylval.wdatum.datum;
                            objtypwdatumtemp.idents = yylval.wdatum.idents;
                            objtypwdatumtemp.quoted = yylval.wdatum.quoted;
                            objtypwdatumtemp.ident = yylval.wdatum.ident;
                            objtypwdatumtemp.dno = yylval.wdatum.dno;
                            objtypwdatum = &objtypwdatumtemp;
                            initStringInfo(&ds);
                            row = (PLpgSQL_row*)yylval.wdatum.datum;
                            typtuple = SearchSysCache1(TYPEOID,ObjectIdGetDatum(row->datatype->typoid));
                            if (!HeapTupleIsValid(typtuple))
                                ereport(errstate,
                                            (errcode(ERRCODE_UNDEFINED_OBJECT),
                                            errmsg("object type with OID %u does not exist", row->datatype->typoid)));
                            typeform = (Form_pg_type)GETSTRUCT(typtuple);
                            nspname = get_namespace_name(typeform->typnamespace);
                            if (NULL != nspname) {
                                appendStringInfo(&ds, "%s.",nspname);
                            }
                            appendStringInfo(&ds, "%s",objtypwdatumtemp.ident);
                            ReleaseSysCache(typtuple);
                            name = pstrdup(ds.data);
                            pfree_ext(ds.data);
                            isCallFunc = is_function(name, true, false);
                        }

                        if (isCallFunc || (enable_out_param_override() && u_sess->parser_cxt.isPerform))
                        {
                            if (u_sess->parser_cxt.isPerform) {
                                stmt = make_callfunc_stmt(name, yylloc, false, false, funcNameList, -1, true, objtypwdatum);
                            } else {
                                stmt = make_callfunc_stmt(name, yylloc, true, false, funcNameList, -1, isCallFunc, objtypwdatum);
                            }
                            if (u_sess->parser_cxt.isPerform) {
                                u_sess->parser_cxt.stmt = (void*)stmt;
                            }
                            if (PLPGSQL_STMT_EXECSQL == stmt->cmd_type)
                                expr = ((PLpgSQL_stmt_execsql*)stmt)->sqlstmt;
                            else if (PLPGSQL_STMT_PERFORM == stmt->cmd_type)
                                expr = ((PLpgSQL_stmt_perform*)stmt)->expr;

                            expr->is_funccall = true;
                            (yyval.expr) = expr;
                        }
                        else
                        {
                            if (name != NULL) {
                                pltsql_push_back_token(tok);
                                name = NULL;
                            }

                            if (!pltsql_is_token_match2(T_CWORD,'('))
                            {
                                if (pltsql_is_token_match(T_CWORD)) {
                                    List* wholeName = yylval.cword.idents;
                                    plpgsql_pkg_add_unknown_var_to_namespace(wholeName);
                                }
                            }
                            expr = read_sql_expression(';', ";");
#ifndef ENABLE_MULTIPLE_NODES
                            if (enable_out_param_override() && PLSQL_COMPILE_OUTPARAM
                                && !IsInitdb && IsNormalProcessingMode()) {
                                CheckOutParamIsConst(expr);
                            }
#endif
                            (yyval.expr) = expr;
                            
                        }	
                    }
#line 9968 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 368:
#line 6800 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = read_sql_expression(']', "]"); }
#line 9974 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 369:
#line 6804 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = read_sql_expression(')', ")"); }
#line 9980 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 370:
#line 6808 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = read_sql_expression(K_THEN, "THEN"); }
#line 9986 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 371:
#line 6812 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = read_sql_expression(K_LOOP, "LOOP"); }
#line 9992 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 372:
#line 6816 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        int tok = -1;

                        (yyval.expr_until_while_loop).expr = read_sql_expression2(K_LOOP, K_DO, "LOOP or DO", &tok);
                        (yyval.expr_until_while_loop).endtoken = tok;

                        if(u_sess->attr.attr_sql.sql_compatibility != B_FORMAT && tok == K_DO)
                            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("'while-do' is only supported in database which dbcompatibility='B'.")));
                    }
#line 10007 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 373:
#line 6829 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        plpgsql_ns_push(NULL);
                        (yyval.str) = NULL;
                    }
#line 10016 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 374:
#line 6834 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        plpgsql_ns_push((yyvsp[-1].str));
                        (yyval.str) = (yyvsp[-1].str);
                    }
#line 10025 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 375:
#line 6841 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = NULL;
                    }
#line 10033 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 376:
#line 6845 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].str);
                    }
#line 10041 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 377:
#line 6851 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = NULL; }
#line 10047 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 378:
#line 6853 "src/pltsql/gram.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 10053 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 379:
#line 6860 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        (yyval.str) = (yyvsp[0].word).ident;
                    }
#line 10061 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;

  case 380:
#line 6864 "src/pltsql/gram.y" /* yacc.c:1646  */
    {
                        if ((yyvsp[0].wdatum).ident == NULL && (yyvsp[0].wdatum).idents == NULL) {
                            yyerror("syntax error");
                        } else if ((yyvsp[0].wdatum).ident != NULL) {
                            (yyval.str) = (yyvsp[0].wdatum).ident;
                        } else {
                            char* names = NameListToString((yyvsp[0].wdatum).idents);
                            (yyval.str) = names;
                        }
                    }
#line 10076 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
    break;


#line 10080 "src/pltsql/pl_gram.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
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
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 7007 "src/pltsql/gram.y" /* yacc.c:1906  */


#define MAX_EXPR_PARAMS  1024

typedef struct {
    YYSTYPE lval; /* semantic information */
    YYLTYPE lloc; /* offset in scanbuf */
    int tok;     /* token */
    int leng;
} TokenData;
static DList* push_token_stack(int tok, DList* tokenstack);
static TokenData* build_token_data(int token);

/*
 * Check whether a token represents an "unreserved keyword".
 * We have various places where we want to recognize a keyword in preference
 * to a variable name, but not reserve that keyword in other contexts.
 * Hence, this kluge.
 */
static bool
tok_is_keyword(int token, union YYSTYPE *lval,
               int kw_token, const char *kw_str)
{
    if (token == kw_token)
    {
        /* Normal case, was recognized by scanner (no conflicting variable) */
        return true;
    }
    else if (token == T_DATUM)
    {
        /*
         * It's a variable, so recheck the string name.  Note we will not
         * match composite names (hence an unreserved word followed by "."
         * will not be recognized).
         */
        if (!lval->wdatum.quoted && lval->wdatum.ident != NULL &&
            strcmp(lval->wdatum.ident, kw_str) == 0)
            return true;
    }
    return false;				/* not the keyword */
}

/*
 * Convenience routine to complain when we expected T_DATUM and got T_WORD,
 * ie, unrecognized variable.
 */
static void
word_is_not_variable(PLword *word, int location)
{
    const char* message = "it is not a known variable ";
    InsertErrorMessage(message, plpgsql_yylloc);
    ereport(errstate,
            (errcode(ERRCODE_SYNTAX_ERROR),
             errmsg("\"%s\" is not a known variable",
                    word->ident),
             parser_errposition(location)));
}

/* Same, for a CWORD */
static void
cword_is_not_variable(PLcword *cword, int location)
{
    const char* message = "it is not a known variable ";
    InsertErrorMessage(message, plpgsql_yylloc);
    ereport(errstate,
            (errcode(ERRCODE_SYNTAX_ERROR),
             errmsg("\"%s\" is not a known variable",
                    NameListToString(cword->idents)),
             parser_errposition(location)));
}

/*
 * Convenience routine to complain when we expected T_DATUM and got
 * something else.  "tok" must be the current token, since we also
 * look at yylval and yylloc.
 */
static void
current_token_is_not_variable(int tok)
{
    if (tok == T_WORD)
        word_is_not_variable(&(yylval.word), yylloc);
    else if (tok == T_CWORD)
        cword_is_not_variable(&(yylval.cword), yylloc);
    else
        yyerror("syntax error");
}

/*
 * Brief		: handle the C-style function IN arguments.
 * Description	: 
 * Notes		:
 */ 
static void 
yylex_inparam(StringInfoData* func_inparam, 
              int *nparams,
              int * tok,
              int *tableof_func_dno,
              int *tableof_var_dno)
{
    PLpgSQL_expr * expr =   NULL;

    if (nparams != NULL) {
        if (*nparams) {
            appendStringInfoString(func_inparam, ",");
        } 
        /* 
        * handle the problem that the function 
        * arguments can only be variable. the argment validsql is set FALSE to
        * ignore sql expression check to the "$n" type of arguments. 
        */
        expr = read_sql_construct(',', ')', 0, ",|)", "", true, false, false, NULL, tok);

        if (*nparams >= MAX_EXPR_PARAMS) {
            const char* message = "too many variables specified in SQL statement ";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                    errmsg("too many variables specified in SQL statement ,more than %d", MAX_EXPR_PARAMS)));
        }

        (*nparams)++;
    } else {
        expr = read_sql_construct(',', ')', 0, ",|)", "", true, false, false, NULL, tok);
    }

    if (*tableof_func_dno != -1 && *tableof_func_dno != expr->tableof_func_dno) {
        yyerror("do not support more than 2 table of index by variables call functions in function");
    } else {
        *tableof_func_dno = expr->tableof_func_dno;
    }

    *tableof_var_dno = expr->tableof_var_dno;
    /* 
     * handle the problem that the function 
     * arguments can only be variable. After revising, the arguments can be any
     * expression. 
     */
    appendStringInfoString(func_inparam, expr->query);
}

/*
 * Brief		: handle the C-style function OUT arguments.
 * Description	: 
 * Notes		:
 */
static int 
yylex_outparam(char** fieldnames,
               int *varnos,
               int nfields,
               PLpgSQL_row **row,
               PLpgSQL_rec **rec,
               int *token,
               int *retvarno,
               bool is_push_back,
               bool overload)
{
    *token = yylex();
    int loc = yylloc;
    if (T_DATUM == *token)
    {
        if (PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            int varno = yylval.wdatum.dno;
            TokenData* temptokendata = build_token_data(*token);
            int temtoken = yylex();
            pltsql_push_back_token(temtoken);
            yylloc = temptokendata->lloc;
            yylval = temptokendata->lval;
            u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
            /* if is a.b.c.d? */
            if ('.' == temtoken) {
                varnos[nfields] = read_assignlist(is_push_back, token);
            } else {
                varnos[nfields] = varno;
                *row = (PLpgSQL_row *)yylval.wdatum.datum;
                *retvarno = varno;
            }
            if (is_push_back) {
                yylloc = temptokendata->lloc;
                yylval = temptokendata->lval;
                u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
            }
            pfree_ext(temptokendata);
        }
        else if (PLPGSQL_DTYPE_RECORD == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.dno;
            *row = (PLpgSQL_row *)yylval.wdatum.datum;
        }
        else if (PLPGSQL_DTYPE_REC == yylval.wdatum.datum->dtype || PLPGSQL_DTYPE_CURSORROW == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.dno;
            *rec = (PLpgSQL_rec *)yylval.wdatum.datum;
        }
        else if (PLPGSQL_DTYPE_VAR == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.dno;
        }
        /* when out param is rec.a which res is not define, we pass the check */
        else if (PLPGSQL_DTYPE_RECFIELD == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.datum->dno;
        }
        else {
            const char* message = "it is not a scalar variable ";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("\"%s\" is not a scalar variable",
                NameOfDatum(&yylval.wdatum))));
        }
    }
    else if (T_VARRAY_VAR == *token || T_TABLE_VAR == *token)
    {
        /* Temporary processing array value for out param */
        check_assignable(yylval.wdatum.datum, yylloc);
        fieldnames[nfields] = CopyNameOfDatum(&yylval.wdatum);
        varnos[nfields] = read_assignlist(is_push_back, token);
    }
    else if (T_PACKAGE_VARIABLE == *token)
    {
        if (PLPGSQL_DTYPE_ROW == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            int varno = yylval.wdatum.dno;
            TokenData* temptokendata = build_token_data(*token);
            int temtoken = yylex();
            pltsql_push_back_token(temtoken);
            yylloc = temptokendata->lloc;
            yylval = temptokendata->lval;
            u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
            /* if is a.b.c.d? */
            if ('.' == temtoken) {
                varnos[nfields] = read_assignlist(is_push_back, token);
            } else {
                varnos[nfields] = varno;
                *retvarno = varno;
                *row = (PLpgSQL_row *)yylval.wdatum.datum;
            }
            if (is_push_back) {
                yylloc = temptokendata->lloc;
                yylval = temptokendata->lval;
                u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
            }
            pfree_ext(temptokendata);
        }
        else if (PLPGSQL_DTYPE_RECORD == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
            varnos[nfields] = yylval.wdatum.dno;
            *row = (PLpgSQL_row *)yylval.wdatum.datum;
        }
        else if (PLPGSQL_DTYPE_VAR == yylval.wdatum.datum->dtype)
        {
            check_assignable(yylval.wdatum.datum, yylloc);
            int type_flag = -1;
            get_datum_tok_type(yylval.wdatum.datum, &type_flag);
            if (type_flag == -1) {
                fieldnames[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
                varnos[nfields] = yylval.wdatum.dno;
            } else {
                /* Temporary processing array value for out param */
                fieldnames[nfields] = CopyNameOfDatum(&yylval.wdatum);
                varnos[nfields] = read_assignlist(is_push_back, token);
            }
        }
        else {
            const char* message = "it is not a scalar variable ";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("\"%s\" is not a scalar variable",
                NameOfDatum(&yylval.wdatum))));
        }
    }
    else if (overload)
    {
        fieldnames[nfields] = NULL;
        varnos[nfields] = -1;
    }
    return loc;
}

static int read_assignlist(bool is_push_back, int* token)
{    
    int varno = yylval.wdatum.dno;
    TokenData* temptokendata = build_token_data(*token);
    DList* tokenstack = NULL;
    List* assignlist = NULL;

    for (;;) {
        int temtoken = yylex();
        if ('(' != temtoken && '[' != temtoken && '.' != temtoken) {
            pltsql_push_back_token(temtoken);
            break;
        }
        
        if (is_push_back) {
            TokenData* temptokendata1 = build_token_data(temtoken);
            tokenstack = dlappend(tokenstack, temptokendata1);
        }
        
        PLpgSQL_expr* expr = NULL;
        char* attrname = NULL;
        if ('(' == temtoken) {
            expr = read_sql_construct6(')', 0, 0, 0, 0, 0, ")", "SELECT ",
                true, true, true, NULL, NULL, tokenstack, true);
        } else if ('[' == temtoken) {
            expr = read_sql_construct6(']', 0, 0, 0, 0, 0, "]", "SELECT ",
                true, true, true, NULL, NULL, tokenstack, true);
        } else {
            int attrtoken = pltsql_yylex_single();
            attrname = get_attrname(attrtoken);
            if (tokenstack != NULL) {
                TokenData* temptokendata1 = build_token_data(attrtoken);
                tokenstack = dlappend(tokenstack, temptokendata1);
            }
        }

        MemoryContext oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_cxt);
        if (expr != NULL) {
            A_Indices *ai = makeNode(A_Indices);
            ai->lidx = NULL;
            ai->uidx = (Node*)expr;
            assignlist = lappend(assignlist, (Node*)ai);
        } else {
            Value* v = makeString(pstrdup(attrname));
            assignlist = lappend(assignlist, (Node*)v);
        }
        MemoryContextSwitchTo(oldCxt);
    }
    if (assignlist != NULL) {
        MemoryContext oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_cxt);
        PLpgSQL_assignlist *newp = NULL;
        newp = (PLpgSQL_assignlist*)palloc0(sizeof(PLpgSQL_assignlist));
        newp->dtype = PLPGSQL_DTYPE_ASSIGNLIST;
        newp->assignlist = assignlist;
        newp->targetno = varno;
        varno = plpgsql_adddatum((PLpgSQL_datum*)newp);
        MemoryContextSwitchTo(oldCxt);
    }

    if (is_push_back) {
        if (tokenstack != NULL) {
            push_back_token_stack(tokenstack);
        }

        /* need restore yylloc or yylval if push back twice */
        yylloc = temptokendata->lloc;
        yylval = temptokendata->lval;
        u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
    }
    
    pfree_ext(temptokendata);
    return varno;
}

/* push token in tokenstack */
static DList* push_token_stack(int tok, DList* tokenstack)
{
    TokenData* temptokendata = NULL;
    if (tokenstack != NULL) {
        temptokendata = build_token_data(tok);
        tokenstack = dlappend(tokenstack, temptokendata);
    }
    return tokenstack;
}

/* build a TokenData for input token  */
static TokenData* build_token_data(int token)
{
    TokenData* temptokendata = (TokenData*)palloc0(sizeof(TokenData));
    temptokendata->lloc = yylloc;
    temptokendata->lval = yylval;
    temptokendata->tok = token;
    temptokendata->leng = u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng;
    return temptokendata;
}

/* push back token stored in tokenstack */
static int push_back_token_stack(DList* tokenstack)
{
    int tok = 0;
    if(tokenstack->length > MAX_PUSHBACKS) {
        dlist_free(tokenstack, true);
        yyerror("array with subscript words too much for overloaded function.");
    } else {
        DListCell* dlc = NULL;
        for (dlc = dlist_tail_cell(tokenstack); dlc != NULL; dlc = lprev(dlc)) {
            /* pltsql_push_back_token function only push yylloc and yylval,
             * so assign the token value to them.
             */
            TokenData* n = (TokenData*)lfirst(dlc);
            tok = n->tok;
            yylloc = n->lloc;
            yylval = n->lval;
            u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = n->leng;
            pltsql_push_back_token(tok);
        }
        dlist_free(tokenstack, true);
    }
    return tok;
}

/*
 * Brief: passe the called function name and return it into output.
 * Description: 
 * in s: original idents
 * inout output:  splited idents.
 * in numidnets: max ident number.
 * returns: void
 * Notes	: No need to consider double quoted ident because it has been handled in lex.
 * Ident here has been downcase if no double quoted, and been without quote if it had double quote before.
 */ 
void 
plpgsql_parser_funcname(const char *s, char **output, int numidents)
{
    int			ident_num = 0;
    char *outer_ptr = NULL;
    char *p = NULL;
    char delimiter[] = ".";
    errno_t ret;
    char *str = (char *)palloc0(sizeof(char) * (strlen(s) + 1));

    for (int i = 0; i < numidents; i++)
        output[i] = (char *)palloc0(sizeof(char) * NAMEDATALEN * 2);

    ret = strcpy_s(str, sizeof(char) * (strlen(s) + 1), s);
    securec_check(ret, "\0", "\0");

    p = strtok_s(str, delimiter, &outer_ptr);

    while (p != NULL)
    {
        ret = strcpy_s(output[ident_num++], sizeof(char) * NAMEDATALEN * 2, p);
        securec_check(ret, "\0", "\0");
        p = strtok_s(NULL, delimiter, &outer_ptr);
    }

    pfree_ext(str);
}

static int get_func_out_arg_num(char* p_argmodes, int all_arg)
{
    if (all_arg == 0 || p_argmodes == NULL) {
        return 0;
    }
    int out_arg_num = 0;
    for (int i = 0; i < all_arg; i++) {
        if (p_argmodes[i] == 'o' || p_argmodes[i] == 'b') {
            out_arg_num++;
        }
    }
    return out_arg_num;
}

/*
 * Brief		: handle A-style function call.
 * Description	: handle A-style function call. First read the function 
 *  			  name, then read the arguments. At last assembing the function
 *				  name and arguments into the postgresql-style function call.
 * in sqlstart: the sql stmt string to be handle.
 * in location: the location number to erreport.
 * in is_assign: is an assign stmt or not.
 * returns: the parsed stmt.
 * Notes		:
 */ 
static PLpgSQL_stmt *
make_callfunc_stmt(const char *sqlstart, int location, bool is_assign, bool eaten_first_token, List* funcNameList, int arrayFuncDno, bool isCallFunc, PLwdatum *objtypwdatum)
{
    int nparams = 0;
    int nfields = 0;
    int narg = 0;
    int inArgNum = 0;
    int i= 0;
    int tok = 0;
    Oid *p_argtypes = NULL;
    char *cp[3];
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    /* pos_outer is the postion got by matching the name. */
    int pos_outer = 0;
    /* pos_inner is the position got by its real postion in function invoke */
    int pos_inner = -1;
    int	varnos[FUNC_MAX_ARGS] = {-1};
    bool	namedarg[FUNC_MAX_ARGS];
    char*	namedargnamses[FUNC_MAX_ARGS];
    char *fieldnames[FUNC_MAX_ARGS];
    bool outParamInvalid = false;
    bool is_plpgsql_func_with_outparam = false;
    int out_param_dno = -1;
    List *funcname = NIL;
    PLpgSQL_row *row = NULL;
    PLpgSQL_rec *rec = NULL;
    PLpgSQL_expr* expr = NULL;
    HeapTuple proctup = NULL;
    Form_pg_proc procStruct;
    FuncCandidateList clist = NULL;
    StringInfoData func_inparas;
    bool noargs = FALSE;
    int ndefaultargs = 0;
    StringInfoData argname;
    int j = 0;
    int placeholders = 0;
    char *quoted_sqlstart = NULL;
    bool is_have_tableof_index_func = false;

    MemoryContext oldCxt = NULL;
    bool	multi_func = false;
    const char *varray_delete = "array_delete(\"";
    const char *varray_indexby_delete = "array_indexby_delete(\"";
    const char *varray_deleteidx = "array_deleteidx(\"";
    const char *varray_deletevarchar = "array_varchar_delete(\"";
    const char *varray_deleteinteger = "array_integer_delete(\"";
    const char *varray_extend= "array_extendnull(\"";
    const char *varray_trim = "array_trim(\"";

    /*get the function's name*/
    cp[0] = NULL;
    cp[1] = NULL;
    cp[2] = NULL;

    if (sqlstart != NULL && (strncmp(sqlstart, varray_delete, strlen(varray_delete)) == 0
        || strncmp(sqlstart, varray_indexby_delete, strlen(varray_indexby_delete)) == 0
        || strncmp(sqlstart, varray_deleteidx, strlen(varray_deleteidx)) == 0
        || strncmp(sqlstart, varray_extend, strlen(varray_extend)) == 0
        || strncmp(sqlstart, varray_trim, strlen(varray_trim)) == 0
        || strncmp(sqlstart, varray_deletevarchar, strlen(varray_deletevarchar)) == 0
        || strncmp(sqlstart, varray_deleteinteger, strlen(varray_deleteinteger)) == 0)) {
        unsigned int chIdx = 0;

        if (strncmp(sqlstart, varray_delete, strlen(varray_delete)) == 0) {
            chIdx = strlen(varray_delete);
        } else if (strncmp(sqlstart, varray_indexby_delete, strlen(varray_indexby_delete)) == 0) {
            chIdx = strlen(varray_indexby_delete);
            is_have_tableof_index_func = true;
        } else if (strncmp(sqlstart, varray_deleteidx, strlen(varray_deleteidx)) == 0) {
            chIdx = strlen(varray_deleteidx);
        } else if (strncmp(sqlstart, varray_deletevarchar, strlen(varray_deletevarchar)) == 0) {
            chIdx = strlen(varray_deletevarchar);
            is_have_tableof_index_func = true;
        } else if (strncmp(sqlstart, varray_deleteinteger, strlen(varray_deleteinteger)) == 0) {
            chIdx = strlen(varray_deleteinteger);
            is_have_tableof_index_func = true;
        } else if (strncmp(sqlstart, varray_trim, strlen(varray_trim)) == 0) {
            chIdx = strlen(varray_trim);
        } else {
            chIdx = strlen(varray_extend);
        }

        initStringInfo(&func_inparas);
        appendStringInfoString(&func_inparas, "SELECT ");
        appendStringInfoString(&func_inparas, sqlstart);

        if (arrayFuncDno == -1) {
            return NULL;
        }

        expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
        expr->dtype = PLPGSQL_DTYPE_EXPR;
        expr->query = pstrdup(func_inparas.data);
        expr->plan = NULL;
        expr->paramnos = NULL;
        expr->ns = plpgsql_ns_top();
        expr->idx = (uint32)-1;
        expr->out_param_dno = -1;
        expr->is_have_tableof_index_func = is_have_tableof_index_func;

        PLpgSQL_stmt_assign *perform = (PLpgSQL_stmt_assign*)palloc0(sizeof(PLpgSQL_stmt_assign));
        perform->cmd_type = PLPGSQL_STMT_ASSIGN;;
        perform->lineno   = plpgsql_location_to_lineno(location);
        perform->varno = arrayFuncDno;
        perform->expr  = expr;
        perform->sqlString = pstrdup(func_inparas.data);
        pfree_ext(func_inparas.data);
        return (PLpgSQL_stmt *)perform;
    }

    /* the function make_callfunc_stmt is only to assemble a sql statement, so the context is set to tmp context */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    if (funcNameList == NULL) {
        plpgsql_parser_funcname(sqlstart, cp, 3);
    } else {
        funcname= funcNameList;
    }
    if (funcNameList == NULL) {
        if (cp[2] && cp[2][0] != '\0')
            funcname = list_make3(makeString(cp[0]), makeString(cp[1]), makeString(cp[2]));
        else if (cp[1] && cp[1][0] != '\0')
            funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
        else
            funcname = list_make1(makeString(cp[0]));
    }


    /* search the function */
    clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
    if (!clist)
    {
        const char* message = "function not exist";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function \"%s\" doesn't exist ", sqlstart)));
        return NULL;
    }

    bool isSystemObj = false;
    if (clist->next || (clist->next && enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj)))
    {
        multi_func = true;
        char* schemaname = NULL;
        char* pkgname = NULL;
        char* funcStrName = NULL;
        if (funcNameList != NULL) {
            DeconstructQualifiedName(funcNameList, &schemaname, &funcStrName, &pkgname);
        } else {
            DeconstructQualifiedName(funcname, &schemaname, &funcStrName, &pkgname);
        }
        if (IsPackageFunction(funcname) == false && IsPackageSchemaOid(SchemaNameGetSchemaOid(schemaname, true)) == false &&
            (schemaname == NULL || strncmp(schemaname, "gms_lob", strlen("gms_lob"))))
        {
            const char* message = "function is not exclusive";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
            (errcode(ERRCODE_DUPLICATE_FUNCTION),
             errmsg("function \"%s\" isn't exclusive ", sqlstart),
             errdetail("The overload function must be package function or function with PACKAGE keyword.And do not mix overload functions of O style and PG style."),
             errcause("The overload function must be package function.")));
        }
    }

    if (multi_func == false)
    {
        proctup = SearchSysCache(PROCOID,
                                 ObjectIdGetDatum(clist->oid),
                                 0, 0, 0);

        /*
         * function may be deleted after clist be searched.
         */
        if (!HeapTupleIsValid(proctup))
        {
            const char* message = "function does not exist";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function \"%s\" doesn't exist ", sqlstart)));
            return NULL;
        }
        /* get the all args informations, only "in" parameters if p_argmodes is null */
        narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
        if (p_argmodes) {
            for (i = 0; i < narg; i++) {
                if (p_argmodes[i] == 'i' || p_argmodes[i] == 'b') {
                    inArgNum++;
                }
            }
        } else {
            inArgNum = narg;
        }
        procStruct = (Form_pg_proc) GETSTRUCT(proctup);
        ndefaultargs = procStruct->pronargdefaults;
        ReleaseSysCache(proctup);
    }

    initStringInfo(&func_inparas);

    tok = yylex();

    /* 
     *  check the format for the function without parameters
     *  if eaten_first_token is true,first token is '(' ,so if next token is ')',it means
     *  no args
     */
    if (NULL != objtypwdatum) {
        noargs = false;
    } else {
        if (eaten_first_token==true) {
            if (tok == ')')
                noargs = TRUE;
        } else {
            if ((tok = yylex()) == ')')
                noargs = TRUE;
        }
        pltsql_push_back_token(tok);
    }

    if (isCallFunc && is_function_with_plpgsql_language_and_outparam(clist->oid)) {
        is_assign = false;
        is_plpgsql_func_with_outparam = true;
    }
    if (u_sess->parser_cxt.isPerform) {
        is_assign = false;
    }
    /* has any "out" parameters, user execsql stmt */
    if (is_assign && !(clist->next && enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj)))
    {
        appendStringInfoString(&func_inparas, "SELECT ");
    }
    else
    {
        appendStringInfoString(&func_inparas, "CALL ");
    }

    int tableof_func_dno = -1;
    int tableof_var_dno = -1;
    /*
     * Properly double-quote schema name and function name to handle uppercase
     * and special characters when making 'CALL func_name;' statement.
     */
    quoted_sqlstart = NameListToQuotedString(funcname);
    appendStringInfoString(&func_inparas, quoted_sqlstart);
    pfree_ext(quoted_sqlstart);

    appendStringInfoString(&func_inparas, "(");

    /* analyse all parameters */
    if (noargs)
    {
        i = 0;
        tok = yylex();
    }
    else if (!multi_func)
    {
        if (p_argmodes)
        {
            for ( i = 0; i < narg ; i++)
            {
                initStringInfo(&argname);
                pos_outer = -1;
                if ('o' == p_argmodes[i] || 'b' == p_argmodes[i])
                    pos_inner++;

                /*
                 * see if an arg is 'defargname => realargname' type.
                 * if so, p_argnames must be searched to match the defined
                 * argument name.
                 */
                if (pltsql_is_token_match2(T_DATUM, PARA_EQUALS)
                    || pltsql_is_token_match2(T_WORD, PARA_EQUALS))
                {
                    int varno = -1;
                    /*For objtype.member methods, explicit add a self parameter*/
                    if (0  == i && NULL != objtypwdatum) {
                        if ('b' == p_argmodes[i]) {
                            nfields++;
                            pos_outer++;
                            yylex_object_type_selfparam(fieldnames, varnos, pos_outer, &row, &rec, &varno, objtypwdatum);
                            processFunctionRecordOutParam(varno, clist->oid, &out_param_dno);
                        }
                        CastTypeVariableNameToString(&func_inparas, objtypwdatum->idents, false);
                        nparams++;
                        i++;
                        if ((tok = yylex()) == ')') {
                            break;
                        }
                        pltsql_push_back_token(tok);
                        j = 1;
                    }
                    tok = yylex();
                    if (T_DATUM == tok)
                        appendStringInfoString(&argname, NameOfDatum(&yylval.wdatum));
                    else
                        appendStringInfoString(&argname, yylval.word.ident);
                    pltsql_push_back_token(tok);
                    /*
                     * search the p_argnames to match the right argname with current arg,
                     * so the real postion of current arg can be determined 
                     */
                    bool argMatch = false;
                    for (j = 0; j < narg; j++)
                    {
                        if ('o' == p_argmodes[j] || 'b' == p_argmodes[j])
                            pos_outer++;
                        if (0 == pg_strcasecmp(argname.data, p_argnames[j]))
                        {
                            /*
                             * if argmodes is 'i', just append the text, else, 
                             * row or rec should be assigned to store the out arg values
                             */
                            switch (p_argmodes[j])
                            {
                                case 'i':
                                    yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                                    check_tableofindex_args(tableof_var_dno, p_argtypes[j]);
                                    break;
                                case 'o':
                                case 'b':
                                    if (is_assign && 'o' == p_argmodes[j])
                                    {
                                        (void)yylex();
                                        (void)yylex();
                                        tok = yylex();
                                        if (T_DATUM == tok || T_VARRAY_VAR == tok 
                                            || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok)
                                        {
                                            pltsql_push_back_token(tok);
                                            (void)read_sql_expression2(',', ')', ",|)", &tok);
                                        }
                                        else
                                            yyerror("syntax error");
                                        break;
                                    }

                                    if (nparams)
                                        appendStringInfoChar(&func_inparas, ',');

                                    tok = yylex();
                                    if (T_DATUM == tok)
                                        appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                                    else
                                        appendStringInfoString(&func_inparas, yylval.word.ident);

                                    appendStringInfoString(&func_inparas, "=>");
                                    /* pass => */
                                    (void)yylex();
                                    yylex_outparam(fieldnames, varnos, pos_outer, &row, &rec, &tok, &varno, true);
                                    processFunctionRecordOutParam(varno, clist->oid, &out_param_dno);
                                    if ((!enable_out_param_override() && p_argmodes[j] == 'b' && is_assign)
                                            || T_DATUM == tok || T_VARRAY_VAR == tok 
                                            || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok)
                                    {
                                        nfields++;
                                        pltsql_push_back_token(tok);
                                        /* don't need inparam add ',' */
                                        yylex_inparam(&func_inparas, NULL, &tok, &tableof_func_dno, &tableof_var_dno);
                                        check_tableofindex_args(tableof_var_dno, p_argtypes[j]);
                                    } else {
                                        outParamInvalid = true;
                                    }
                                    nparams++;
                                    break;
                                default:
                                    const char* message = "parameter mode  not exist";
                                    InsertErrorMessage(message, plpgsql_yylloc);
                                    ereport(errstate,
                                            (errcode(ERRCODE_UNEXPECTED_NODE_STATE),
                                             errmsg("parameter mode %c doesn't exist", p_argmodes[j])));
                            }
                            argMatch = true;
                            break;
                        }
                    }
                    if (!argMatch) {
                        const char* message = "invoking function error,check function";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, no argments match \"%s\"", sqlstart, argname.data)));
                    }
                    if (outParamInvalid) {
                        const char* message = "invoking function error,check function";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, no destination for argments \"%s\"", sqlstart, argname.data)));
                    }
                }
                else
                {
                    int varno = -1;
                    /*For objtype.member methods, explicit add a self parameter*/
                    if (0  == i && NULL != objtypwdatum) {
                        if ('b' == p_argmodes[i]) {
                            nfields++;
                            pos_outer++;
                            yylex_object_type_selfparam(fieldnames, varnos, pos_outer, &row, &rec, &varno, objtypwdatum);
                            processFunctionRecordOutParam(varno, clist->oid, &out_param_dno);
                        }
                        CastTypeVariableNameToString(&func_inparas, objtypwdatum->idents, false);
                        nparams++;
                        if ((tok = yylex()) == ')') {
                            i++;
                            break;
                        }
                        pltsql_push_back_token(tok);
                        continue;
                    }
                    tok = yylex();
                    /* p_argmodes may be null, 'i'->in , 'o'-> out ,'b' inout,others error */
                    switch (p_argmodes[i])
                    {
                        case 'i':
                            if (T_PLACEHOLDER == tok)
                                placeholders++;
                            pltsql_push_back_token(tok);
                            yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                            check_tableofindex_args(tableof_var_dno, p_argtypes[i]);
                            break;
                        case 'o':
                            /*
                             * if the function is in an assign expr, just ignore the 
                             * out parameters.
                             */
                            if (is_assign)
                            {
                                if (T_DATUM == tok || T_VARRAY_VAR == tok || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok || T_PLACEHOLDER == tok)
                                {
                                    pltsql_push_back_token(tok);
                                    (void)read_sql_expression2(',', ')', ",|)", &tok);
                                }
                                else
                                    yyerror("syntax error");
                                break;
                            }

                            if (T_PLACEHOLDER == tok)
                            {
                                placeholders++;
                            }
                            else if (T_DATUM == tok || T_VARRAY_VAR == tok || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok || T_PLACEHOLDER == tok)
                            {
                                nfields++;
                            } else {
                                outParamInvalid = true;
                            }

                            pltsql_push_back_token(tok);
                            yylex_outparam(fieldnames, varnos, pos_inner, &row, &rec, &tok, &varno, true);
                            processFunctionRecordOutParam(varno, clist->oid, &out_param_dno);
                            pltsql_push_back_token(tok);
                            yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                            check_tableofindex_args(tableof_var_dno, p_argtypes[i]);
                            break;
                        case 'b':
                            if (is_assign)
                            {
                                if (!enable_out_param_override() 
                                        || (enable_out_param_override() 
                                               && (T_DATUM == tok || T_VARRAY_VAR == tok 
                                                      || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok)))
                                {
                                    pltsql_push_back_token(tok);
                                    yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                                }
                                else
                                    yyerror("syntax error");
                                break;
                            }

                            if (T_PLACEHOLDER == tok)
                                placeholders++;
                            pltsql_push_back_token(tok);
                            yylex_outparam(fieldnames, varnos, pos_inner, &row, &rec, &tok, &varno, true);
                            processFunctionRecordOutParam(varno, clist->oid, &out_param_dno);
                            if (T_DATUM == tok || T_VARRAY_VAR == tok || T_TABLE_VAR == tok || T_PACKAGE_VARIABLE == tok) {
                                nfields++;
                            } else {
                                outParamInvalid = true;
                            }
                            
                            pltsql_push_back_token(tok);
                            
                            yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                            check_tableofindex_args(tableof_var_dno, p_argtypes[i]);
                            break;
                        default:
                            const char* message = "parameter mode  not exist";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                    (errcode(ERRCODE_UNEXPECTED_NODE_STATE),
                                     errmsg("parameter mode %c doesn't exist", p_argmodes[i])));
                    }
                    if (is_plpgsql_func_with_outparam && outParamInvalid) {
                        const char* message = "invoking function error,check function";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, no destination for argments \"%s\"", sqlstart, argname.data)));
                    }

                }

                if (')' == tok)
                {
                    i++;
                    break;
                }

                if (narg - 1 == i) {
                    const char* message = "maybe input something superfluous";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \")\", maybe input something superfluous.", sqlstart)));
                }

                if (',' != tok) {
                    const char* message = "invoking function error,check function";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \",\"", sqlstart)));
                }
                pfree_ext(argname.data);
            }
        }
        else
        {	
            for ( i = 0; i < narg; i++)
            {
                /*For objtype.member methods, explicit add a self parameter*/
                if (0  == i && NULL != objtypwdatum) {
                    CastTypeVariableNameToString(&func_inparas, objtypwdatum->idents, false);
                    nparams++;
                    if ((tok = yylex()) == ')') {
                        i++;
                        break;
                    }
                    pltsql_push_back_token(tok);
                    continue;
                }
                tok = yylex();
                if (T_PLACEHOLDER == tok)
                    placeholders++;
                pltsql_push_back_token(tok);
        
                yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
        
                if (')' == tok)
                {
                    i++;
                    break;
                }
        
                if (narg - 1 == i) {
                    const char* message = "maybe input something superfluous";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("when invoking function %s, expected \")\", maybe input something superfluous.", sqlstart)));
                }
            }
        }
    }
    else
    {
        while (true)
        {
            int varno = -1;
            if (0 == nfields && NULL != objtypwdatum) {
                CastTypeVariableNameToString(&func_inparas, objtypwdatum->idents, false);
                yylex_object_type_selfparam(fieldnames, varnos, pos_outer, &row, &rec, &varno, objtypwdatum);
                namedarg[nfields] = false;
                namedargnamses[nfields] = NULL;
                nparams++;
                nfields++;
                if ((tok = yylex()) == ')') {
                    break;
                }
                pltsql_push_back_token(tok);
                continue;
            }
            /* for named arguemnt */
            if (pltsql_is_token_match2(T_DATUM, PARA_EQUALS)
                || pltsql_is_token_match2(T_WORD, PARA_EQUALS))
            {
                tok = yylex();
                if (nparams)
                    appendStringInfoString(&func_inparas, ",");
                if (T_DATUM == tok)
                {
                    appendStringInfoString(&func_inparas, NameOfDatum(&yylval.wdatum));
                    namedargnamses[nfields] = pstrdup(NameOfDatum(&yylval.wdatum));
                }
                else
                {
                    appendStringInfoString(&func_inparas, yylval.word.ident);
                    namedargnamses[nfields] = yylval.word.ident;
                }

                appendStringInfoString(&func_inparas, "=>");
                (void)yylex();
                yylex_outparam(fieldnames, varnos, nfields, &row, &rec, &tok, &varno, true, true);
                if (varno != -1 && enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj)) {
                    int dtype = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[varno]->dtype;
                    if (dtype == PLPGSQL_DTYPE_ROW) {
                        out_param_dno = yylval.wdatum.dno;
                    }
                }
                pltsql_push_back_token(tok);
                expr = read_sql_construct(',', ')', 0, ",|)", "", true, false, false, NULL, &tok);
                appendStringInfoString(&func_inparas, expr->query);
                pfree_ext(expr->query);
                pfree_ext(expr);

                nparams++;
                namedarg[nfields] = true;
            }
            else
            {
                yylex_outparam(fieldnames, varnos, nfields, &row, &rec, &tok, &varno, true, true);
                if (varno != -1 && enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj)) {
                    int dtype = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[varno]->dtype;
                    if (dtype == PLPGSQL_DTYPE_ROW) {
                        out_param_dno = yylval.wdatum.dno;
                    }
                }
                pltsql_push_back_token(tok);
                yylex_inparam(&func_inparas, &nparams, &tok, &tableof_func_dno, &tableof_var_dno);
                namedarg[nfields] = false;
                namedargnamses[nfields] = NULL;
            }
            nfields++;
            if (')' == tok)
            {
                break;
            }
        }
    }
    appendStringInfoString(&func_inparas, ")");
    /* read the end token */
    if ((tok = yylex()) != ';')
    {
        if (!is_assign)
        {
            if (u_sess->parser_cxt.isPerform) {
                const char* message = "perform not support expression when open guc proc_outparam_override";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("perform not support expression when open guc proc_outparam_override")));
                u_sess->plsql_cxt.have_error = true;
            } else {
                const char* message = "maybe input something superfluous";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                    errmsg("when invoking function %s, maybe input something superfluous.", sqlstart)));
                u_sess->plsql_cxt.have_error = true;
            }
        }
        else
        {
            /* there may be other thing after the function invoke, just append it */
            pltsql_push_back_token(tok);
            expr = read_sql_construct(';', 0, 0, ";", "", true, false, true, NULL, &tok);
            appendStringInfoString(&func_inparas, expr->query);
            pfree_ext(expr->query);
            pfree_ext(expr);
        }
    }
    
    if (!multi_func && inArgNum - i >  ndefaultargs) {
        const char* message = "function has no enough parameters";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function %s has no enough parameters", sqlstart)));
    }

    (void)MemoryContextSwitchTo(oldCxt);

    /* generate the expression */
    expr 				= (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(func_inparas.data);
    expr->plan			= NULL;
    expr->paramnos 		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->idx			= (uint32)-1;
    expr->out_param_dno	= out_param_dno;
    expr->is_have_tableof_index_func = tableof_func_dno != -1 ? true : false;

    if (multi_func)
    {
        PLpgSQL_function *function = NULL;
        PLpgSQL_execstate *estate = (PLpgSQL_execstate*)palloc(sizeof(PLpgSQL_execstate));
        expr->func = (PLpgSQL_function *) palloc0(sizeof(PLpgSQL_function));
        function = expr->func;
        function->fn_is_trigger = PLPGSQL_NOT_TRIGGER;
        function->fn_input_collation = InvalidOid;
        function->out_param_varno = -1;		/* set up for no OUT param */
        function->resolve_option = GetResolveOption();
        function->fn_cxt = CurrentMemoryContext;

        PLpgSQL_compile_context* curr_compile = u_sess->plsql_cxt.curr_compile_context;
        estate->ndatums = curr_compile->plpgsql_nDatums;
        estate->datums = (PLpgSQL_datum **)palloc(sizeof(PLpgSQL_datum *) * curr_compile->plpgsql_nDatums);
        for (int i = 0; i < curr_compile->plpgsql_nDatums; i++)
            estate->datums[i] = curr_compile->plpgsql_Datums[i];

        function->cur_estate = estate;
        function->cur_estate->func = function;
        
        Oid funcid = getMultiFuncInfo(func_inparas.data, expr);
        if (expr->func != NULL)
            pfree_ext(expr->func);
        if (estate->datums != NULL)
            pfree_ext(estate->datums);
        if (estate != NULL)
            pfree_ext(estate);
        expr->func = NULL;
        int	all_arg = 0;
        if (OidIsValid(funcid))
        {
            proctup = SearchSysCache(PROCOID,
                                     ObjectIdGetDatum(funcid),
                                     0, 0, 0);
            
            /*
             * function may be deleted after clist be searched.
             */
            if (!HeapTupleIsValid(proctup))
            {
                const char* message = "function does not exist";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_UNDEFINED_FUNCTION),
                         errmsg("function \"%s\" doesn't exist ", sqlstart)));
                return NULL;
            }
            /* get the all args informations, only "in" parameters if p_argmodes is null */
            all_arg = get_func_arg_info(proctup, &p_argtypes, &p_argnames, &p_argmodes);
            procStruct = (Form_pg_proc) GETSTRUCT(proctup);
            ndefaultargs = procStruct->pronargdefaults;
            narg = procStruct->pronargs;
            ReleaseSysCache(proctup);

            if (!(enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj))) {
                for (int k = 0; k < all_arg; k++)  {
                    check_tableofindex_args(varnos[k], p_argtypes[k]);
               }
            }

            /* if there is no "out" parameters ,use perform stmt,others use exesql */
            if ((0 == all_arg || NULL == p_argmodes))
            {
                PLpgSQL_stmt_perform *perform = NULL;
                perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
                perform->cmd_type = PLPGSQL_STMT_PERFORM;
                perform->lineno   = plpgsql_location_to_lineno(location);
                perform->expr	  = expr;
                perform->sqlString = plpgsql_get_curline_query();
                return (PLpgSQL_stmt *)perform;
            }
            else if (all_arg >= narg)
            {
                int out_arg_num = get_func_out_arg_num(p_argmodes, all_arg);
                /* out arg number > 1 should build a row */
                bool need_build_row = need_build_row_for_func_arg(&rec, &row, out_arg_num, all_arg, varnos, p_argmodes);
                if (need_build_row)
                {
                    int new_nfields = 0;
                    int i = 0, j = 0;
                    bool varnosInvalid = false;
                    for (i = 0; i < all_arg; i++)
                    {
                        if (p_argmodes[i] == 'i')
                            continue;	
                        else
                            new_nfields++;
                    }
                    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
                    row->dtype = PLPGSQL_DTYPE_ROW;
                    row->refname = pstrdup("*internal*");
                    row->lineno = plpgsql_location_to_lineno(location);
                    row->rowtupdesc = NULL;
                    row->nfields = new_nfields;
                    row->fieldnames = (char **)palloc0(sizeof(char *) * new_nfields);
                    row->varnos = (int *)palloc0(sizeof(int) * new_nfields);
                    row->isImplicit = true;

                    /* fetch out argument from fieldnames into row->fieldnames */
                    for (i = 0; i < nfields; i++)
                    {
                        if (namedarg[i] == false)
                        {
                            if (p_argmodes[i] == 'i')
                                continue;
                            else if (varnos[i] >= 0)
                            {
                                row->fieldnames[j] = fieldnames[i];
                                row->varnos[j] = varnos[i];
                                j++;
                            } else {
                                varnosInvalid = true;
                            }
                        }
                        else
                        {
                            char argmode = FUNC_PARAM_IN;
                            pos_outer = -1;
                            for (int curpos = 0; curpos < all_arg; curpos++)
                            {
                                char* paraname = p_argnames[curpos];

                                if (p_argmodes[curpos] != 'i')
                                    pos_outer++;
                                else
                                {
                                    continue;
                                }

                                if (paraname != NULL && !strcmp(paraname, namedargnamses[i]))
                                {
                                    if (!p_argmodes)
                                        argmode = FUNC_PARAM_IN;
                                    else
                                        argmode = p_argmodes[curpos];

                                    break;
                                }
                            }
                            
                            if (argmode == 'i')
                                continue;

                            if (fieldnames[i] == NULL)
                            {
                                const char* message = "Named argument can not be a const";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                        errmsg("Named argument \"%s\" can not be a const", namedargnamses[i])));
                            }

                            if (row->varnos[pos_outer] > 0)
                            {
                                const char* message = "parameter is assigned more than once";
                                InsertErrorMessage(message, plpgsql_yylloc);
                                ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("parameter \"%s\" is assigned more than once", row->fieldnames[pos_outer])));
                            }
                            if (varnos[i] >= 0)
                            {
                                row->fieldnames[pos_outer] = fieldnames[i];
                                row->varnos[pos_outer] = varnos[i];
                            } else {
                                varnosInvalid = true;
                            }
                        }
                    }
                    if (varnosInvalid) {
                        pfree_ext(row->refname);
                        pfree_ext(row->fieldnames);
                        pfree_ext(row->varnos);
                        pfree_ext(row);
                    } else if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
                        expr->out_param_dno = plpgsql_adddatum((PLpgSQL_datum *)row);
                    } else {
                        plpgsql_adddatum((PLpgSQL_datum *)row);
                    }
                }

                PLpgSQL_stmt_execsql * execsql = NULL;
                execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
                execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
                execsql->lineno  = plpgsql_location_to_lineno(location);
                execsql->sqlstmt = expr;
                execsql->into	 = row || rec ? true:false;
                execsql->bulk_collect = false;
                execsql->strict  = true;
                execsql->rec	 = rec;
                execsql->row	 = row;
                execsql->placeholders = placeholders;
                execsql->multi_func = multi_func;
                execsql->object_rel_value = false;
                if (u_sess->parser_cxt.isPerform) {
                    execsql->sqlString = func_inparas.data;
                } else {
                    execsql->sqlString = plpgsql_get_curline_query();
                }
                return (PLpgSQL_stmt *)execsql;
            }
        }
        else
        {
            const char* message = "function is not exclusive";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
            (errcode(ERRCODE_DUPLICATE_FUNCTION),
             errmsg("function \"%s\" isn't exclusive ", sqlstart)));
        }
    }
    else
    {
        /*
         * if have out parameters and no row/rec, generate row type to store the results.
         * if have row/rec and more than 2 out parameters , also generate row type.
         */
        if (((nfields && NULL == rec &&  NULL == row) || nfields > 1) && !outParamInvalid)
        {
            row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
            row->dtype = PLPGSQL_DTYPE_ROW;
            row->refname = pstrdup("*internal*");
            row->lineno = plpgsql_location_to_lineno(location);
            row->rowtupdesc = NULL;
            row->nfields = nfields;
            row->fieldnames = (char **)palloc(sizeof(char *) * nfields);
            row->varnos = (int *)palloc(sizeof(int) * nfields);
            row->isImplicit = true;
            while (--nfields >= 0)
            {
                row->fieldnames[nfields] = fieldnames[nfields];
                row->varnos[nfields] = varnos[nfields];
            }
            if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
                expr->out_param_dno = plpgsql_adddatum((PLpgSQL_datum *)row);
            } else {
                plpgsql_adddatum((PLpgSQL_datum *)row);
            }
        }
        /* has invalid out param, set it to null */
        if ((rec != NULL || row != NULL) && outParamInvalid)
        {
            rec = NULL;
            row = NULL;
        }
        
        /* if there is no "out" parameters ,use perform stmt,others use exesql */
        if (0 == narg || NULL == p_argmodes)
        {
            PLpgSQL_stmt_perform *perform = NULL;
            perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
            perform->cmd_type = PLPGSQL_STMT_PERFORM;
            perform->lineno   = plpgsql_location_to_lineno(location);
            perform->expr	  = expr;
            perform->sqlString = plpgsql_get_curline_query();
            return (PLpgSQL_stmt *)perform;
        }
        else
        {
            PLpgSQL_stmt_execsql * execsql = NULL;
            execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
            execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
            execsql->lineno  = plpgsql_location_to_lineno(location);
            execsql->sqlstmt = expr;
            execsql->into	 = row || rec ? true:false;
            execsql->bulk_collect = false;
            execsql->strict  = true;
            execsql->rec	 = rec;
            execsql->row	 = row;
            execsql->placeholders = placeholders;
            execsql->multi_func = multi_func;
            execsql->object_rel_value = false;
            execsql->sqlString = plpgsql_get_curline_query();
            return (PLpgSQL_stmt *)execsql;
        }
    }

    pfree_ext(func_inparas.data);
    /* skip compile warning */
    return NULL;
}


static bool
is_unreserved_keyword_func(const char *name)
{
    int i;
    char *func_name[] = {"absolute", "alias", "backward", "constant","current", "debug", "detail","dump" ,"errcode","error", "exceptions", "first", "forward", "hint", "index", "info", "last", "log","message", "message_text", "multiset", "next", "no", "notice", "option", "package", "instantiation","pg_exception_context", "pg_exception_detail", "pg_exception_hint", "query", "record", "relative", "result_oid", "returned_sqlstate", "reverse","row_count","sroll", "slice", "stacked","sys_refcursor","use_column","use_variable","variable_conflict","varray","warning"};
    for (i = 0; i <= 45; i++) {
        if (pg_strcasecmp(func_name[i], name) == 0) {
            return true;
        }
    }
    return false;
}
/*
 * Brief                : check if it is an log function invoke
 * Description  : 
 * in keyword: keyword info
 * in no_parenthesis: if no-parenthesis function is called.
 * in name: the function name
 * returns: true if is a log function.
 * Notes                : 
 */
static bool
is_unreservedkeywordfunction(int kwnum, bool no_parenthesis, const char *name)
{
    if (kwnum >= 0 && no_parenthesis && ScanKeywordCategories[kwnum] == UNRESERVED_KEYWORD && !is_unreserved_keyword_func(name))
        return false;
    else
        return true;
}

/*
 * Brief		: check if it is an function invoke
 * Description	: 
 * in name: the ident name
 * in is_assign: is an assign stmt or not.
 * in no_parenthesis: if no-parenthesis function is called.
 * returns: true if is a function.
 * Notes		: 
 */ 
static bool 
is_function(const char *name, bool is_assign, bool no_parenthesis, List* funcNameList)
{
    List *funcname = NIL;
    FuncCandidateList clist = NULL;
    bool have_outargs = false;
    bool have_inoutargs = false;
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    Oid *p_argtypes = NULL;
    HeapTuple proctup = NULL;
    int narg = 0;
    int i = 0;
    char *cp[3] = {0};
    bool isNull = false;
    Datum typeIdDatum = InvalidOid;
    char typMethodKind = OBJECTTYPE_NULL_PROC;

    /* the function is_function is only to judge if it's a function call, so memory used in it is all temp */
    AutoContextSwitch plCompileCxtGuard(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    if (funcNameList == NULL) {
        plpgsql_parser_funcname(name, cp, 3);
    }

    /* 
     * if A.B.C, it's not a function invoke, because can not use function of 
     * other database. if A.B, then it must be a function invoke. 
     */
    if (cp[0] && cp[0][0] != '\0')
    {
        /* these are function in pg_proc who are overloaded, so they can not be
         * execured by := or direct call.
         */
        if (pg_strcasecmp("ts_stat", cp[0]) == 0 ||
            pg_strcasecmp("ts_token_type", cp[0]) == 0 ||
            pg_strcasecmp("ts_parse", cp[0]) == 0 ||
            pg_strcasecmp("dblink_get_notify", cp[0]) == 0 ||
            pg_strcasecmp("ts_debug", cp[0]) == 0)
            return false;

        int kwnum = ScanKeywordLookup(cp[0], &ScanKeywords);
        /* function name can not be reserved keyword */
        if (kwnum >= 0 && ScanKeywordCategories[kwnum] == RESERVED_KEYWORD)
            return false;
        /* function name can not be unreserved keyword when no-parenthesis function is called. except log function*/
        if (!is_unreservedkeywordfunction(kwnum, no_parenthesis, cp[0]))
	{
            return false;
        }
        if (funcNameList == NULL) {
            if (cp[2] && cp[2][0] != '\0') 
                funcname = list_make3(makeString(cp[0]), makeString(cp[1]),makeString(cp[2]));
            else if (cp[1] && cp[1][0] != '\0')
                funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
            else
                funcname = list_make1(makeString(cp[0]));
            /* search the function */
            clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
        } else {
            clist = FuncnameGetCandidates(funcNameList, -1, NIL, false, false, false);
        }
        if (!clist)
        {
            if (!is_assign) {
                const char* message = "function does not exist";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function \"%s\" doesn't exist ", name)));
            }
            return false;
        }
        else if (clist->next)
        {
            if (is_assign)
                return false;
            else
                return true;
        }
        else
        {
            proctup = SearchSysCache(PROCOID,
                             ObjectIdGetDatum(clist->oid),
                             0, 0, 0);
            if (!HeapTupleIsValid(proctup))
            {
                if (!is_assign) {
                    const char* message = "function does not exist";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                        (errcode(ERRCODE_UNDEFINED_FUNCTION),
                         errmsg("function \"%s\" doesn't exist ", name)));
                }
                return false;
            }
            bool ispackage = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_package, &isNull);
            typeIdDatum = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_packageid, &isNull);
            /*Find method type from pg_object*/
            HeapTuple objTuple = SearchSysCache2(PGOBJECTID, ObjectIdGetDatum(clist->oid), CharGetDatum(OBJECT_TYPE_PROC));
            if (HeapTupleIsValid(objTuple)) {
                typMethodKind = GET_PROTYPEKIND(SysCacheGetAttr(PGOBJECTID, objTuple, Anum_pg_object_options, &isNull));
                ReleaseSysCache(objTuple);
            }
            if (!isNull && !ispackage && OidIsValid(ObjectIdGetDatum(typeIdDatum)) && (typMethodKind == OBJECTTYPE_DEFAULT_CONSTRUCTOR_PROC || typMethodKind == OBJECTTYPE_CONSTRUCTOR_PROC)) {
                ReleaseSysCache(proctup);
                return false;
            }

            /* get the all args informations, only "in" parameters if p_argmodes is null */
            narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
            if (p_argmodes)
            {
                for (i = 0; i < narg; i++)
                {
                    if ('o' == p_argmodes[i])
                    {
                        have_outargs = true;
                        break;
                    }
                }
                for (i = 0; i < narg; i++)
                {
                    if ('b' == p_argmodes[i])
                    {
                        have_inoutargs = true;
                        break;
                    }
                }
            }
            ReleaseSysCache(proctup);
        }

        if (have_inoutargs && is_function_with_plpgsql_language_and_outparam(clist->oid))
            return true;
        if (!have_outargs && is_assign)
            return false;
        
        return true;/* passed all test */
    } else if (funcNameList != NULL) {
        clist = FuncnameGetCandidates(funcNameList, -1, NIL, false, false, false);
        if (!clist)
        {
            if (!is_assign) {
                const char* message = "function does not exist";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_UNDEFINED_FUNCTION),
                     errmsg("function \"%s\" doesn't exist ", name)));
            }
            return false;
        }
        bool isSystemObj = false;
        if (clist->next && !(enable_out_param_override() && oid_is_function(clist->oid, & isSystemObj)))
        {
            if (is_assign)
                return false;
            else
                return true;
        }
        while(clist)
        {
            proctup = SearchSysCache(PROCOID,
                             ObjectIdGetDatum(clist->oid),
                             0, 0, 0);
            if (!HeapTupleIsValid(proctup))
            {
                if (!is_assign) {
                    const char* message = "function does not exist";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                        (errcode(ERRCODE_UNDEFINED_FUNCTION),
                         errmsg("function \"%s\" doesn't exist ", name)));
                }
                return false;
            }
            bool ispackage = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_package, &isNull);
            typeIdDatum = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_packageid, &isNull);
            /*Find method type from pg_object*/
            HeapTuple objTuple = SearchSysCache2(PGOBJECTID, ObjectIdGetDatum(clist->oid), CharGetDatum(OBJECT_TYPE_PROC));
            if (HeapTupleIsValid(objTuple)) {
                typMethodKind = GET_PROTYPEKIND(SysCacheGetAttr(PGOBJECTID, objTuple, Anum_pg_object_options, &isNull));
                ReleaseSysCache(objTuple);
            }
            if (!isNull && !ispackage && OidIsValid(ObjectIdGetDatum(typeIdDatum)) && (typMethodKind == OBJECTTYPE_DEFAULT_CONSTRUCTOR_PROC || typMethodKind == OBJECTTYPE_CONSTRUCTOR_PROC)) {
                ReleaseSysCache(proctup);
                return false;
            }
            /* get the all args informations, only "in" parameters if p_argmodes is null */
            narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
            if (p_argmodes)
            {
                for (i = 0; i < narg; i++)
                {
                    if ('o' == p_argmodes[i])
                    {
                        have_outargs = true;
                        break;
                    }
                }
                for (i = 0; i < narg; i++)
                {
                    if ('b' == p_argmodes[i])
                    {
                        have_inoutargs = true;
                        break;
                    }
                }
            }

            ReleaseSysCache(proctup);
            if (have_inoutargs && is_function_with_plpgsql_language_and_outparam(clist->oid))
                return true;

            if (!have_outargs && is_assign &&
                !(clist->next && enable_out_param_override() && oid_is_function(clist->oid, &isSystemObj)))
                return false;

            clist = clist->next;
        }

        return true;/* passed all test */
    }
    return false;
}

static void checkFuncName(List* funcname)
{
    MemoryContext colCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    char* name = NameListToString(funcname);
    Oid packageOid = InvalidOid;
    Oid namespaceId = InvalidOid;
    if (list_length(funcname) > 2) {
        if (list_length(funcname) == 3) {
            char* schemaname = strVal(linitial(funcname));
            char* pkgname = strVal(lsecond(funcname));
            namespaceId = SchemaNameGetSchemaOid(schemaname, false);
            if (OidIsValid(namespaceId)) {
                packageOid = PackageNameGetOid(pkgname, namespaceId);
            }
        }
        if (!OidIsValid(packageOid)) {
            const char* message = "function does not exist";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("cross-database references are not implemented: %s", name)));
            u_sess->plsql_cxt.have_error = true;
        }
    }
    (void)MemoryContextSwitchTo(colCxt);
}

static bool oid_is_function(Oid funcid, bool* isSystemObj)
{
    HeapTuple proctup = NULL;
    if (OidIsValid(funcid)) {
        proctup = SearchSysCache(PROCOID, ObjectIdGetDatum(funcid), 0, 0, 0);
        if (HeapTupleIsValid(proctup)) {
            bool isNull;
            Datum protypeDatum = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_prokind, &isNull);
            char protype;
            if (!isNull) {
                protype = DatumGetChar(protypeDatum);
            } else {
                protype = PROKIND_FUNCTION;
            }
            Datum pronamespaceDatum = SysCacheGetAttr(PROCOID, proctup, Anum_pg_proc_pronamespace, &isNull);
            Oid pronamespace = DatumGetObjectId(pronamespaceDatum);
            Assert(OidIsValid(pronamespace));
            ReleaseSysCache(proctup);
            if (IsPackageSchemaOid(pronamespace) || IsSystemNamespace(pronamespace)) {
                *isSystemObj = true;
                return false;
            }
            if (PROC_IS_FUNC(protype)) {
                return true;
            }
        }
    }
    return false;
}

static void checkTypeName(List* nest_typnames, List* target_nest_typnames)
{
    if (!u_sess->attr.attr_sql.enable_pltype_name_check ||
    nest_typnames == NIL || target_nest_typnames == NIL)
        return;
    
    ListCell* lc = NULL;
    int i = 0;
    foreach (lc, nest_typnames) {
        PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)lfirst(lc);
        ListCell* target_typnames = target_nest_typnames->head;
        PLpgSQL_nest_type* target_ntype = (PLpgSQL_nest_type*)lfirst(target_typnames);
        char* functypname = NULL;
        char* pkgtypname = NULL;
        List* found_list = NIL;
        bool found = false;
        List* funcname = NIL;
        FuncCandidateList clist = NULL;
        char *cp[3] = {0};
            
        for (i = 0; i < target_nest_typnames->length; i++) {
            if (target_ntype->layer == ntype->layer &&
               (target_ntype->index == -1 || target_ntype->index == ntype->index)) {
                    found_list = lappend(found_list, target_ntype);
            }
            if (target_typnames) {
                target_typnames = target_typnames->next;
                if (target_typnames) {
                    target_ntype = (PLpgSQL_nest_type*)lfirst(target_typnames);
                }
            }
        }
        if (strchr(ntype->typname, '.') == NULL) {
            getPkgFuncTypeName(ntype->typname, &functypname, &pkgtypname);
        }
        if (found_list != NIL) {
            ListCell* lcf = NULL;
            foreach(lcf, found_list) {
                target_ntype  = (PLpgSQL_nest_type*)lfirst(lcf);
                bool is_row   = strcasecmp(ntype->typname, "ROW") == 0;
                bool is_equal = strcmp(ntype->typname, target_ntype->typname) == 0;
                bool func_name_equal = functypname ? strcmp(functypname, target_ntype->typname) == 0 : false;
                bool pkg_name_equal  = pkgtypname ? strcmp(pkgtypname, target_ntype->typname) == 0 : false;

                if (is_row || is_equal || func_name_equal || pkg_name_equal) {
                    found = true;
                }
            }
        }
        if (found) {
            continue;
        }

        plpgsql_parser_funcname(ntype->typname, cp, 3);
        for (int i = 0; i < (sizeof(cp) / sizeof(cp[0]) - 1) ; i++) {
            if (cp[i] && cp[i][0] != '\0') {
                if (i != 0) {
                    funcname = list_make2(funcname, makeString(cp[i]));
                } else {
                    funcname = list_make1(makeString(cp[i]));
                }
            }
        }
        clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
	    if (clist != NULL) {
            found = true;
	    }

        if (found) {
            continue;
        }
        char *mes = NULL;
        char *report_mes = "Wrong type of expression, should not use type ";
        int length = strlen(ntype->typname) + strlen(report_mes) + 3;
        mes = (char*)palloc0(length);
        errno_t rc = snprintf_s(mes, length, length -1, "%s\"%s\"", report_mes, ntype->typname);
        securec_check_ss(rc, "", "");
        InsertErrorMessage(mes, plpgsql_yylloc);
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg(mes)));
    }
}

static List* get_current_type_nest_type(List* old_nest_typenames, char* typname, bool add_current_type, bool is_varray)
{
    List* nest_typnames = NIL;
    ListCell* lc = NULL;
    if (old_nest_typenames != NIL) {
        int i = 0;
        PLpgSQL_nest_type *new_ntypes = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type) * old_nest_typenames->length);
        foreach (lc, old_nest_typenames) {
            PLpgSQL_nest_type *old_ntype = (PLpgSQL_nest_type *)lfirst(lc);
            PLpgSQL_nest_type *new_ntype = new_ntypes + i;
            new_ntype->typname = pstrdup(old_ntype->typname);
            new_ntype->layer = old_ntype->layer + 1;
            if (old_ntype->layer == 0 && is_varray){
                new_ntype->index = -1;
            } else{
                new_ntype->index = old_ntype->index;
            }
            nest_typnames = lappend(nest_typnames, new_ntype);
            i++;
        }
    }
    if (add_current_type) {
        PLpgSQL_nest_type *ntype = (PLpgSQL_nest_type *)palloc(sizeof(PLpgSQL_nest_type));
        ntype->typname = pstrdup(typname);
        ntype->layer = 0;
        ntype->index = -1;
        nest_typnames = lappend(nest_typnames, ntype);
    }
    return nest_typnames;
}

/*
 * @brief is_datatype
 *  check if a given type is a datatype
 * @param name      constructed type name
 * @return true     is datatype
 * @return false    not a datatype
 */
static bool is_paren_friendly_datatype(TypeName *name)
{
    Type type_tup = LookupTypeNameExtended(NULL, name, NULL, false);
    if (!HeapTupleIsValid(type_tup)) {
        return false;
    }
    ReleaseSysCache(type_tup);

    /* deal with internal type casts, e.g. timestamp(6) */
    Oid typoid = LookupTypeNameOid(name);
    bool preferred = false;
    char category = TYPCATEGORY_INVALID;
    get_type_category_preferred(typoid, &category, &preferred);
    if (category != TYPCATEGORY_ARRAY && category != TYPCATEGORY_COMPOSITE
        && category != TYPCATEGORY_TABLEOF && category != TYPCATEGORY_TABLEOF_VARCHAR
        && category != TYPCATEGORY_TABLEOF_INTEGER) {
        return false;
    }
    return true;
}

/*
 * @brief init_array_parse_context
 *  Initialize array parse context.
 */
static inline void init_array_parse_context(ArrayParseContext *context)
{
    context->list_left_bracket = NULL;
    context->list_right_bracket = NULL;
    context->list_array_state = NULL;
    context->list_datatype = NULL;
    context->array_is_empty = false;
    context->array_is_nested = false;
}

/*
 * @brief push_array_parse_stack
 *  Push array parse context stacks.
 * @param context       array parse context
 * @param parenlevel    parentheses level, set -1 to push nothing
 * @param state         array state flag, set -1 to push nothing
 */
static inline void push_array_parse_stack(ArrayParseContext *context, int parenlevel, int state)
{
    if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
        return;
    }
    if (likely(parenlevel >= 0)) {
        context->list_left_bracket = lcons_int(parenlevel, context->list_left_bracket);
        context->list_right_bracket = lcons_int(parenlevel, context->list_right_bracket);
    }
    if (state >= 0) {
        context->list_array_state = lcons_int(state, context->list_array_state);
    }
}

/*
 * @brief construct_array_start
 *  Extracted from read_sql_constuct, start of T_VARRAY/T_TABLE array FSM.
 * @param ds            sql string builder
 * @param context       array state context
 * @param var_type      element type
 * @param tok           current token, should be T_VARRAY/T_TABLE
 * @param parenlevel    parentheses stack
 * @param loc           T_VARRAY starting location
 */
static bool construct_array_start(StringInfo ds, ArrayParseContext *context, PLpgSQL_type *var_type, int *tok,
                                  int parenlevel, int loc)
{
    /* Save array token for datatype casts */
    context->list_datatype = lcons(var_type, context->list_datatype);
    *tok = yylex(); /* always yylex to parentheses */
    pltsql_push_back_token(*tok);
    /* varray constructor */
    if (*tok == '(') {
        appendStringInfoString(ds, "ARRAY");
        push_array_parse_stack(context, parenlevel, ARRAY_START);
    } else if (*tok == '[') {
        yyerror("syntax error");    /* typename + '[' is not allowed */
        return false;   /* compiler happy */
    } else {    /* coerce? no need for extra check, leave it to main parser */
        plpgsql_append_source_text(ds, loc, yylloc);
    }
    return true;
}

/*
 * @brief get_real_elemtype
 *  Get the real element type from a table or array
 * @param type              any type
 * @return PLpgSQL_type*    element type
 */
static PLpgSQL_type *get_real_elemtype(PLpgSQL_type *type)
{
    HeapTuple type_tup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(type->typoid));
    if (!HeapTupleIsValid(type_tup)) {
        ereport(ERROR, (errmodule(MOD_PLSQL),
                        errcode(ERRCODE_CACHE_LOOKUP_FAILED),
                        errmsg("cache lookup failed for type %u, type Oid is invalid", type->typoid)));
    }
    if (((Form_pg_type)GETSTRUCT(type_tup))->typcategory == TYPCATEGORY_ARRAY &&
        type->collectionType == PLPGSQL_COLLECTION_NONE) {
        type->collectionType = PLPGSQL_COLLECTION_ARRAY;
    }
    if (type->collectionType != PLPGSQL_COLLECTION_TABLE &&
        type->collectionType != PLPGSQL_COLLECTION_ARRAY) {
        /* Guarding condition(white list) */
        ReleaseSysCache(type_tup);
        return type;
    }
    PLpgSQL_type* typ = NULL;
    Oid base_oid = ((Form_pg_type)GETSTRUCT(type_tup))->typelem;
    HeapTuple base_type_tup = SearchSysCache1(TYPEOID, ObjectIdGetDatum(base_oid));
    if (!HeapTupleIsValid(base_type_tup)) {
        ereport(ERROR, (errmodule(MOD_PLSQL),
                        errcode(ERRCODE_CACHE_LOOKUP_FAILED),
                        errmsg("cache lookup failed for type %u, type Oid is invalid", base_oid)));
    }
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL)
    {
        typ = build_datatype(base_type_tup, type->atttypmod, 0);
    } else {
        typ = build_datatype(base_type_tup, type->atttypmod,
            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_input_collation);
    }
    typ->collectionType = type->collectionType;
    ReleaseSysCache(base_type_tup);
    ReleaseSysCache(type_tup);
    pfree_ext(type);
    return typ;
}

/*
 * @brief construct_object_type
 *  Construct object type to strings.
 * @param ds            sql string builder
 * @param context       array state context
 * @param name          object typename
 * @param tok           current token, should be T_WORD/T_CWORD
 * @param parenlevel    parentheses stack
 * @param curloc        current parsing location, usually the following token
 * @param loc           T_WORD/T_CWORD starting location
 */
static bool construct_object_type(StringInfo ds, ArrayParseContext *context, TypeName *name, int *tok,
                                  int parenlevel, int curloc, int loc)
{
    if (*tok != '(' || u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
        curloc = yylloc;    /* new token location */
        plpgsql_append_source_text(ds, loc, curloc);
        context->array_is_nested = false;
        return true;
    }

    if (context->array_is_nested) {
        push_array_parse_stack(context, parenlevel, ARRAY_ACCESS);
        curloc = yylloc;    /* new token location */
        plpgsql_append_source_text(ds, loc, curloc);
        context->array_is_nested = false;
        return true;
    }

    /*
     * Support various datatype constructor. We expect the T_WORD with format like:
     *   'var := foo(bar)' or inside other objects.
     */
    char *name_str = NameListToString(name->names);
    int toksz = strlen(name_str);
    if (yylloc - curloc != toksz || !is_paren_friendly_datatype(name)) { /* NOTE: curloc here is T_WORD/T_CWORD location */
        curloc = yylloc;    /* new token location */
        plpgsql_append_source_text(ds, loc, curloc);
        pfree_ext(name_str);
        return true;
    }
    PLpgSQL_type *type = parse_datatype(name_str, loc);\

    type = get_real_elemtype(type);
    if (type->collectionType == PLPGSQL_COLLECTION_ARRAY || \
        type->collectionType == PLPGSQL_COLLECTION_TABLE) {
        pfree_ext(name_str);
        return construct_array_start(ds, context, type, tok, parenlevel, loc);
    }
    if (type->typtyp == TYPTYPE_ABSTRACT_OBJECT)
        appendStringInfo(ds, name_str);
    else
        appendStringInfo(ds, "ROW");
    pfree_ext(name_str);
    return true;
}

/*
 * @brief construct_word
 *  Construct T_WORD, extracted from read_sql_constuct.
 * @param ds            sql string builder
 * @param context       array state context
 * @param tok           current token, should be T_WORD
 * @param parenlevel    parentheses stack
 * @param loc           location of T_WORD
 */
static bool construct_word(StringInfo ds, ArrayParseContext *context, int *tok, int parenlevel, int loc)
{
    char *name = NULL;
    name = yylval.word.ident;
    int curloc = yylloc;
    *tok = yylex();
    pltsql_push_back_token(*tok);
    return construct_object_type(ds, context, makeTypeName(name), tok, parenlevel, curloc, loc);
}

/*
 * @brief construct_cword
 *  Construct T_CWORD, extracted from read_sql_constuct.
 * @param ds            sql string builder
 * @param context       array state context
 * @param tok           current token, should be T_CWORD
 * @param parenlevel    parentheses stack
 * @param loc           location of T_CWORD
 */
static bool construct_cword(StringInfo ds, ArrayParseContext *context, int *tok, int parenlevel, int loc)
{
    int nnames = 0;
    List *idents = yylval.cword.idents;
    char *word1 = NULL;
    char *word2 = NULL;
    char *word3 = NULL;
    if (list_length(idents) == 2) {
        word1 = strVal(linitial(idents));
        word2 = strVal(lsecond(idents));
    } else if (list_length(idents) == 3) {
        word1 = strVal(linitial(idents));
        word2 = strVal(lsecond(idents));
        word3 = strVal(lthird(idents));
    } else {
        yyerror("syntax error");
    }
    if (enable_plpgsql_gsdependency()) {
        FuncCandidateList clist = FuncnameGetCandidates(idents, -1, NIL, false, false, true);
        if (clist == NULL) {
            gsplsql_build_gs_variable_dependency(idents);
        }
    }
    if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
        int dno = -1;
        char *name = NameListToString(idents);
        if (plpgsql_ns_lookup(plpgsql_ns_top(), false, word1, word2, word3, &nnames) == NULL) {
            dno = plpgsql_pkg_add_unknown_var_to_namespace(yylval.cword.idents);
        }
        if (dno != -1) {
            appendStringInfoString(ds, name);
            appendStringInfoString(ds, " ");
        }
        pfree_ext(name);
        /* This originally was a fall through to default branch */
        *tok = yylex();
        int curloc = yylloc;
        pltsql_push_back_token(*tok);
        plpgsql_append_source_text(ds, loc, curloc);
        return true;
    }
    int curloc = yylloc;
    *tok = yylex();
    pltsql_push_back_token(*tok);
    bool result;
    CreatePlsqlType oldCreatePlsqlType = u_sess->plsql_cxt.createPlsqlType;
    PG_TRY();
    {
        set_create_plsql_type_not_check_nsp_oid();
        result = construct_object_type(ds, context, makeTypeNameFromNameList(idents), tok, parenlevel, curloc, loc);
        set_create_plsql_type(oldCreatePlsqlType);
    }
    PG_CATCH();
    {
        set_create_plsql_type(oldCreatePlsqlType);
        PG_RE_THROW();
    }
    PG_END_TRY();
    return result;
}

/* Convenience routine to read an expression with one possible terminator */
static PLpgSQL_expr *
read_sql_expression(int until, const char *expected)
{
    return read_sql_construct(until, 0, 0, expected,
                              "SELECT ", true, true, true, NULL, NULL);
}

/* Convenience routine to read an expression with two possible terminators */
static PLpgSQL_expr *
read_sql_expression2(int until, int until2, const char *expected,
                     int *endtoken)
{
    return read_sql_construct(until, until2, 0, expected,
                              "SELECT ", true, true, true, NULL, endtoken);
}

/* Convenience routine to read a SQL statement that must end with ';' */
static PLpgSQL_expr *
read_sql_stmt(const char *sqlstart)
{
    return read_sql_construct(';', 0, 0, ";",
                              sqlstart, false, true, true, NULL, NULL);
}

/*
 * Brief		: read a sql construct who has 4 expected tokens.
 * Description	: just like read_sql_construct function below, except that 
 * 				  this function has 4 expected tokens.
 * Notes		:
 */ 

/*
 * Read a SQL construct and build a PLpgSQL_expr for it.
 *
 * until:		token code for expected terminator
 * until2:		token code for alternate terminator (pass 0 if none)
 * until3:		token code for another alternate terminator (pass 0 if none)
 * until4:		token code for alternate terminator (pass 0 if none)
 * until5:		token code for alternate terminator (pass 0 if none)
 * expected:	text to use in complaining that terminator was not found
 * sqlstart:	text to prefix to the accumulated SQL text
 * isexpression: whether to say we're reading an "expression" or a "statement"
 * valid_sql:   whether to check the syntax of the expr (prefixed with sqlstart)
 * trim:		trim trailing whitespace
 * startloc:	if not NULL, location of first token is stored at *startloc
 * endtoken:	if not NULL, ending token is stored at *endtoken
 *				(this is only interesting if until2 or until3 isn't zero)
 */
static PLpgSQL_expr *
read_sql_construct6(int until,
                   int until2,
                   int until3,
                   int until4,
                   int until5,
                   int until6,
                   const char *expected,
                   const char *sqlstart,
                   bool isexpression,
                   bool valid_sql,
                   bool trim,
                   int *startloc,
                   int *endtoken,
                   DList* tokenstack,
                   bool issaveexpr)
{
    int					tok = 0;
    int					prev_tok = 0;
    StringInfoData		ds;
    IdentifierLookup	save_IdentifierLookup;
    int					startlocation = -1;
    int					parenlevel = 0;
    PLpgSQL_expr		*expr;
    MemoryContext		oldCxt = NULL;

    /* 
     * buf stores the cursor attribute internal variables or 
     * varray ident with operate function, so NAMEDATALEN + 128 is enough 
     */
    char				buf[NAMEDATALEN + 128];
    bool				ds_changed = false;
    ArrayParseContext	context;
    List				*idents = 0;
    List*				nest_typnames = NIL;
    int					comma_cnt = 0;
    int					typname_indexs[MAX_LAYER] = {0};
    const char			left_bracket[2] = "[";
    const char			right_bracket[2] = "]";
    const char			left_parentheses[2] = "(";
    const char			right_parentheses[2] = ")";
    int					loc = 0;
    int					curloc = 0;
    int					brack_cnt = 0;
    int					nest_layers = 0;
    int					left_brace_count = 0;
    int					right_brace_count = 0;
    bool				stop_count = false;
    int					stop_tok;
    bool				record_typename = false;
    int					bracket_diff=0;
    /* mark if there are 2 table of index by var call functions in an expr */
    int tableof_func_dno = -1;
    int tableof_var_dno = -1;
    bool is_have_tableof_index_var = false;
    List* tableof_index_list = NIL;

    PLpgSQL_compile_context* curr_compile = u_sess->plsql_cxt.curr_compile_context;
    /*
     * ds will do a lot of enlarge, which need to relloc the space, and the old space 
     * will be return to current context. So if we don't switch the context, the original 
     * context will be Plpgsql function context, which has a long term life cycle, 
     * may cause memory accumulation.
     */
    oldCxt = MemoryContextSwitchTo(curr_compile->compile_tmp_cxt);
    initStringInfo(&ds);
    MemoryContextSwitchTo(oldCxt);

    appendStringInfoString(&ds, sqlstart);

    /* special lookup mode for identifiers within the SQL text */
    save_IdentifierLookup = curr_compile->plpgsql_IdentifierLookup;
    curr_compile->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;

    init_array_parse_context(&context);
    for (;;)
    {
        prev_tok = tok;
        tok = yylex();
        bracket_diff = left_brace_count - right_brace_count;
        record_typename = bracket_diff < MAX_LAYER && bracket_diff >= 0;
        if (tok == ',' && record_typename) {
            typname_indexs[bracket_diff] = typname_indexs[bracket_diff] + 1;
        }
        if (tok == '\"' || tok == '\'') {
            if (stop_count && stop_tok == tok) {
                stop_count = false;
            } else {
                stop_count = true;
                stop_tok = tok;
            }
        }
        tokenstack = push_token_stack(tok, tokenstack);
        loc = yylloc;
        if (startlocation < 0)			/* remember loc of first token */
            startlocation = yylloc;
        if (tok == until && parenlevel == 0)
            break;
        if (tok == until2 && parenlevel == 0)
            break;
        if (tok == until3 && parenlevel == 0)
            break;
        if (tok == until4 && parenlevel == 0)
            break;
        if (tok == until5 && parenlevel == 0)
            break;
        if (tok == until6 && parenlevel == 0)
            break;
        if (tok == '(' || tok == '[')
            parenlevel++;
        else if (tok == ')' || tok == ']')
        {
            parenlevel--;
            if (parenlevel < 0)
                yyerror("mismatched parentheses", true);
        }
        /*
         * End of function definition is an error, and we don't expect to
         * hit a semicolon either (unless it's the until symbol, in which
         * case we should have fallen out above).
         */
        if (tok == 0 || tok == ';')
        {
            if (parenlevel != 0)
                yyerror("mismatched parentheses", true);
            if (isexpression) {
                const char* message = "missing something at end of SQL expression";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("missing \"%s\" at end of SQL expression",
                                expected),
                         parser_errposition(yylloc)));
            } else {
                const char* message = "missing something at end of SQL expression";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("missing \"%s\" at end of SQL statement",
                                expected),
                         parser_errposition(yylloc)));
            }
        }

        switch (tok)
        {		     
            case T_SQL_FOUND:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_found__ ");
                ds_changed = true;
                break;
            case T_CURSOR_FOUND:	
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_found__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_NOTFOUND:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_notfound__ ");
                ds_changed = true;
                break;
            case T_CURSOR_NOTFOUND:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_notfound__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_ISOPEN:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_isopen__ ");
                ds_changed = true;
                break;
            case T_CURSOR_ISOPEN:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_isopen__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_SQL_ROWCOUNT:
                appendStringInfo(&ds, " __gsdb_sql_cursor_attri_rowcount__ ");
                ds_changed = true;
                break;
            case T_CURSOR_ROWCOUNT:
                appendStringInfo(&ds, " __gsdb_cursor_attri_%d_rowcount__ ", yylval.ival);
                ds_changed = true;
                break;
            case T_PACKAGE_CURSOR_NOTFOUND:
                appendStringInfo(&ds, " %s.__gsdb_cursor_attri_%d_notfound__ ",
                    yylval.wdatum.ident, yylval.wdatum.dno);
                ds_changed = true;
                break;
            case T_PACKAGE_CURSOR_ISOPEN:
                appendStringInfo(&ds, " %s.__gsdb_cursor_attri_%d_isopen__ ",
                    yylval.wdatum.ident, yylval.wdatum.dno);
                ds_changed = true;
                break;
            case T_PACKAGE_CURSOR_ROWCOUNT:
                appendStringInfo(&ds, " %s.__gsdb_cursor_attri_%d_rowcount__ ",
                    yylval.wdatum.ident, yylval.wdatum.dno);
                ds_changed = true;
                break;
            case T_PACKAGE_CURSOR_FOUND:
                appendStringInfo(&ds, " %s.__gsdb_cursor_attri_%d_found__ ",
                    yylval.wdatum.ident, yylval.wdatum.dno);
                ds_changed = true;
                break;
            case T_VARRAY_VAR:
            {
                idents = yylval.wdatum.idents;
                PLpgSQL_var* var = (PLpgSQL_var*)(yylval.wdatum.datum);
                if (idents == NIL) {
                    AddNamespaceIfPkgVar(yylval.wdatum.ident, save_IdentifierLookup);
                }
                tok = yylex();
                if (tok == '(' || tok == '[') {
                    push_array_parse_stack(&context, parenlevel, ARRAY_ACCESS);
                } else if (OidIsValid(var->datatype->tableOfIndexType) && 
                    (',' == tok || ')' == tok || ';' == tok)) {
                    is_have_tableof_index_var = true;
                }
                curloc = yylloc;
                pltsql_push_back_token(tok);
                if (list_length(idents) >= 3) {
                    plpgsql_cast_reference_list(idents, &ds, false);
                    ds_changed = true;
                    break;
                } else {
                    plpgsql_append_source_text(&ds, loc, curloc);
                    ds_changed = true;
                    break;
                }
            }
            case T_ARRAY_FIRST:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID) {
                    appendStringInfo(&ds, "ARRAY_VARCHAR_FIRST(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents, false);
                } else if (indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INTEGER_FIRST(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents, false);
                } else {
                    appendStringInfo(&ds, "ARRAY_LOWER(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                    appendStringInfo(&ds, "1 ");
                }
                
                int tok = yylex();
                if(tok == '(') {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    appendStringInfo(&ds, ") ");
                    ds_changed = true;
                    pltsql_push_back_token(tok);
                }
                break;
            }
            case T_ARRAY_LAST:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID) {
                    appendStringInfo(&ds, "ARRAY_VARCHAR_LAST(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents, false);
                } else if (indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INTEGER_LAST(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents, false);
                } else {
                    appendStringInfo(&ds, "ARRAY_UPPER(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                    appendStringInfo(&ds, "1 ");
                }
                
                int tok = yylex();
                if(tok == '(') {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    appendStringInfo(&ds, ") ");
                    ds_changed = true;
                    pltsql_push_back_token(tok);
                }
                break;
            }
            case T_ARRAY_COUNT:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID || indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INDEXBY_LENGTH(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                    appendStringInfo(&ds, "1 ");
                } else {
                    appendStringInfo(&ds, "ARRAY_LENGTH(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                    appendStringInfo(&ds, "1 ");
                }

                int tok = yylex();
                if(tok == '(') {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    appendStringInfo(&ds, ") ");
                    ds_changed = true;
                    pltsql_push_back_token(tok);
                }
                break;
            }
            case T_ARRAY_EXISTS:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID) {
                    appendStringInfo(&ds, "ARRAY_VARCHAR_EXISTS(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else if (indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INTEGER_EXISTS(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else {
                    appendStringInfo(&ds, "ARRAY_EXISTS(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                }
                
                int tok = yylex();
                if('(' == tok) {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    pltsql_push_back_token(tok);
                    yyerror("syntax error");
                }
                break;
            }
            case T_ARRAY_PRIOR:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID) {
                    appendStringInfo(&ds, "ARRAY_VARCHAR_PRIOR(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else if (indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INTEGER_PRIOR(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else {
                    appendStringInfo(&ds, "ARRAY_PRIOR(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                }

                int tok = yylex();
                if('(' == tok) {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    pltsql_push_back_token(tok);
                    yyerror("syntax error");
                }
                break;
            }
            case T_ARRAY_NEXT:
            {
                Oid indexType = get_table_index_type(yylval.wdatum.datum, &tableof_func_dno);
                if (indexType == VARCHAROID) {
                    appendStringInfo(&ds, "ARRAY_VARCHAR_NEXT(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else if (indexType == INT4OID) {
                    appendStringInfo(&ds, "ARRAY_INTEGER_NEXT(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                } else {
                    appendStringInfo(&ds, "ARRAY_NEXT(");
                    CastArrayNameToArrayFunc(&ds, yylval.wdatum.idents);
                }

                int tok = yylex();
                if('(' == tok) {
                    push_array_parse_stack(&context, -1, NOT_AN_ARRAY);
                    parenlevel++;
                    ds_changed = true;
                    tokenstack = push_token_stack(tok, tokenstack);
                } else {
                    pltsql_push_back_token(tok);
                    yyerror("syntax error");
                }
                break;
            }
            case ']':
                brack_cnt--;
                /* fall through */
            case ')':
                if (!stop_count) {
                    right_brace_count++;
                    bracket_diff = left_brace_count - right_brace_count;
                    record_typename = bracket_diff < MAX_LAYER && bracket_diff >= 0;
                }
                if (context.list_right_bracket && context.list_right_bracket->length
                    && linitial_int(context.list_right_bracket) == parenlevel) {
                    /* append bracket instead of parentheses */
                    appendStringInfoString(&ds, right_bracket);
                    ds_changed = true;
                    context.list_right_bracket = list_delete_first(context.list_right_bracket);

                    if (context.array_is_empty) {
                        /* NOTE: empty array state will remain ARRAY_START until now */
                        PLpgSQL_type *type = (PLpgSQL_type *)linitial(context.list_datatype);
                        plpgsql_append_object_typename(&ds, type);
                        appendStringInfoString(&ds, left_bracket);
                        appendStringInfoString(&ds, right_bracket);
                        context.array_is_empty = false;
                    }

                    /*
                     * N-D Array accessing pattern:
                     * If another pair of brackets(parentheses) are right behind an array
                     * access statment like 'array(i)' or 'array[i], they are very likely
                     * to be part of the array statement too. Therefore, we need to make
                     * sure we interpret them as brackets.
                     */
                    if (IS_ARRAY_STATE(context.list_array_state, ARRAY_ACCESS)) {
                        tok = yylex();
                        pltsql_push_back_token(tok);
                        if (tok == '(' || tok == '[') { /* array(1)(1) */
                            push_array_parse_stack(&context, parenlevel, -1);
                            break;
                        } else if (tok == '.') { /* array(1).col(1) */
                            context.array_is_nested = true;
                        }
                    } else {
                        /* It is important to pop the array dno stack at the end. */
                        context.list_datatype = list_delete_first(context.list_datatype);
                    }

                    /* always pop the array state stack, including ARRAY_ACCESS */
                    context.list_array_state = list_delete_first(context.list_array_state);
                } else {
                    /* pop excess array state stack, see case '(' below */
                    if (IS_ARRAY_STATE(context.list_array_state, NOT_AN_ARRAY)) {
                        context.list_array_state = list_delete_first(context.list_array_state);
                    }

                    if (tok ==']') {
                        appendStringInfoString(&ds, right_bracket);
                    } else {
                        appendStringInfoString(&ds, right_parentheses);
                    }
                }
                ds_changed = true;
                break;
            case '[':
                brack_cnt++;
                /* fall through */
            case '(':
                if (!stop_count)
                    left_brace_count++;
                if (context.list_left_bracket && context.list_left_bracket->length
                    && linitial_int(context.list_left_bracket) == parenlevel - 1) {
                    appendStringInfoString(&ds, left_bracket);
                    context.list_left_bracket = list_delete_first(context.list_left_bracket);

                    /* in the case of '()' or '[]', we need to append NULL */
                    prev_tok = tok;
                    tok = yylex();
                    pltsql_push_back_token(tok);
                    if ((prev_tok == '(' && tok == ')') || (prev_tok == '[' && tok == ']')) {
                        if (IS_ARRAY_STATE(context.list_array_state, ARRAY_ACCESS)) {
                            yyerror("empty index");
#ifndef ENABLE_MULTIPLE_NODES
                            if (u_sess->attr.attr_common.plsql_show_all_error) {
                                u_sess->plsql_cxt.have_error = true;
                                tok = yylex(); // lex ) or ]
                                tok = yylex(); // lex ;
                                return NULL;
                            }
#endif
                        }
                        context.array_is_empty = true;
                    } else if (IS_ARRAY_STATE(context.list_array_state, ARRAY_START)) {
                        SET_ARRAY_STATE(context.list_array_state, ARRAY_ELEMENT);
                        /* always append left parentheses at start of each element */
                        appendStringInfoString(&ds, left_parentheses);
                    }
                } else {
                    /* array state stack up with none  */
                    if (IS_ARRAY_STATE(context.list_array_state, ARRAY_ELEMENT) ||
                        IS_ARRAY_STATE(context.list_array_state, NOT_AN_ARRAY)) {
                        context.list_array_state = lcons_int(NOT_AN_ARRAY, context.list_array_state);
                    }

                    if (tok =='[') {
                        appendStringInfoString(&ds, left_bracket);
                    } else {
                        appendStringInfoString(&ds, left_parentheses);
                    }
                }
                ds_changed = true;
                break;
            case '.':
                tok = yylex();
                pltsql_push_back_token(tok);
                curloc = yylloc;
                if (context.array_is_nested && tok != T_WORD && tok != T_CWORD) {
                    context.list_array_state = list_delete_first(context.list_array_state);
                    context.array_is_nested = false;
                }
                plpgsql_append_source_text(&ds, loc, curloc);
                ds_changed = true;
                break;
            case T_TABLE_VAR:
            {
                /* 
                 * table var name may be schema.pkg.table_var
                 * so the name length should NAMEDATALEN with quotation marks *3
                 */
                const int tablevar_namelen = NAMEDATALEN * 3 + 6;
                char tableName1[tablevar_namelen] = {0};
                idents = yylval.wdatum.idents;
                if (idents == NIL) {
                    AddNamespaceIfPkgVar(yylval.wdatum.ident, save_IdentifierLookup);
                }
                copy_table_var_indents(tableName1, yylval.wdatum.ident, tablevar_namelen);
                PLpgSQL_datum* datum = yylval.wdatum.datum;
                int var_dno = yylval.wdatum.dno;
                if (datum->dtype == PLPGSQL_DTYPE_VAR) {
                    check_autonomous_nest_tablevar((PLpgSQL_var*)datum);
                }
                tok = yylex();
                if('(' == tok) {
                    push_array_parse_stack(&context, parenlevel, ARRAY_ACCESS);
                    tableof_index_list = lappend_int(tableof_index_list, ((PLpgSQL_var*)datum)->dno);
                } else if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && K_MULTISET == tok) {
                    Oid typeOid = get_table_type(datum);
                    read_multiset(&ds, tableName1, typeOid);
                    ds_changed = true;
                    break;
                } else {
                    PLpgSQL_var* var = (PLpgSQL_var*)datum;
                    if (OidIsValid(var->datatype->tableOfIndexType) && 
                        (',' == tok || ')' == tok || ';' == tok)) {
                        is_have_tableof_index_var = true;
                        /* tableof_var_dno is  only used for args */
                        if (',' == tok || ')' == tok) {
                            tableof_var_dno = var_dno;
                        }
                    }
                }

                curloc = yylloc;
                pltsql_push_back_token(tok);
                if (list_length(idents) >= 3) {
                    plpgsql_cast_reference_list(idents, &ds, false);
                    ds_changed = true;
                    break;
                } else {
                    plpgsql_append_source_text(&ds, loc, curloc);
                    ds_changed = true;
                    break;
                }
            }
            case T_PACKAGE_VARIABLE:
            {
                int type_flag = -1;
                get_datum_tok_type(yylval.wdatum.datum, &type_flag);
                idents = yylval.wdatum.idents;
                int var_dno = yylval.wdatum.dno;

                if (enable_plpgsql_gsdependency()) {
                    gsplsql_build_gs_variable_dependency(idents);
                }
                if (type_flag == PLPGSQL_TOK_TABLE_VAR) {
                    /*
                     * table var name may be schema.pkg.table_var
                     * so the name length should NAMEDATALEN with quotation marks *3
                     */
                    const int tablevar_namelen = NAMEDATALEN * 3 + 6;

                    char tableName1[tablevar_namelen] = {0};
                    idents = yylval.wdatum.idents;
                    copy_table_var_indents(tableName1, yylval.wdatum.ident, tablevar_namelen);
                    PLpgSQL_datum* datum = yylval.wdatum.datum;
                    tok = yylex();
                    if('(' == tok)
                    {
                        push_array_parse_stack(&context, parenlevel, NOT_AN_ARRAY);
                    } else if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && K_MULTISET == tok) {
                        Oid typeOid = get_table_type(datum);
                        read_multiset(&ds, tableName1, typeOid);
                        ds_changed = true;
                        break;
                    } else {
                        PLpgSQL_var* var = (PLpgSQL_var*)datum;
                        if (OidIsValid(var->datatype->tableOfIndexType) &&
                            (',' == tok || ')' == tok || ';' == tok)) {
                            is_have_tableof_index_var = true;
                            /* tableof_var_dno is  only used for args */
                            if (',' == tok || ')' == tok) {
                                tableof_var_dno = var_dno;
                            }
                        }
                    }
                    curloc = yylloc;
                    pltsql_push_back_token(tok);
                    if (PkgVarNeedCast(idents)) {
                        plpgsql_cast_reference_list(idents, &ds, true);
                    } else {
                        plpgsql_append_source_text(&ds, loc, curloc);
                    }
                    ds_changed = true;
                    break;
                } else if (type_flag == PLPGSQL_TOK_VARRAY_VAR) {
                    tok = yylex();
                    if (tok == '(' || tok == '[') {
                        push_array_parse_stack(&context, parenlevel, ARRAY_ACCESS);
                    }
                    curloc = yylloc;
                    pltsql_push_back_token(tok);
                    if (PkgVarNeedCast(idents)) {
                        plpgsql_cast_reference_list(idents, &ds, true);
                    } else {
                        plpgsql_append_source_text(&ds, loc, curloc);
                    }
                    ds_changed = true;
                    break;
                } else {
                    tok = yylex();
                    curloc = yylloc;
                    pltsql_push_back_token(tok);

                    if (PkgVarNeedCast(idents)) {
                        plpgsql_cast_reference_list(idents, &ds, true);
                    } else {
                        plpgsql_append_source_text(&ds, loc, curloc);
                    }

                    ds_changed = true;
                    break;
                }
            }
            case T_TABLE:
            {
                int dno = yylval.wdatum.datum->dno;
                PLpgSQL_var *var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
                /* table of index by only support: a = a(); */
                if ((OidIsValid(var->datatype->tableOfIndexType) && var->nest_table == NULL) ||
                   	(var->nest_table != NULL && var->isIndexByTblOf)) {
                    TokenData* temptokendata = build_token_data(tok);
                    int first_tok = yylex();
                    DList* tokenstack = NULL;
                    TokenData* temptokendata1 = build_token_data(first_tok);
                    tokenstack = dlappend(tokenstack, temptokendata1);
                    if (first_tok != '(') {
                        yyerror("table of index by does not support syntax");
                    }

                    int second_tok = yylex();
                    temptokendata1 = build_token_data(second_tok);
                    if (second_tok != ')') {
                        yyerror("table of index by does not support syntax");
                    }
                    tokenstack = dlappend(tokenstack, temptokendata1);
                    /* restore yylex */
                    push_back_token_stack(tokenstack);
                    yylloc = temptokendata->lloc;
                    yylval = temptokendata->lval;
                    u_sess->plsql_cxt.curr_compile_context->plpgsql_yyleng = temptokendata->leng;
                }
                if (left_brace_count == 0)
                {
                    nest_layers = var->nest_layers;
                }
                if (record_typename) {
                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                    ntype->typname = pstrdup(var->refname);
                    ntype->layer = bracket_diff;
                    ntype->index = typname_indexs[ntype->layer] + 1;
                    nest_typnames = lappend(nest_typnames, ntype);
                }
                ds_changed = construct_array_start(&ds, &context, var->datatype, &tok, parenlevel, loc);
                break;
            }
            case T_VARRAY:
            {
                if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                    /* non A_FORMAT case, append 'NULL' and leave it. */
                    int first_tok = yylex();
                    tokenstack= push_token_stack(first_tok, tokenstack);
                    
                    int second_tok = yylex();
                    tokenstack = push_token_stack(first_tok, tokenstack);
                    if (first_tok == '(' && second_tok == ')') {
                        snprintf(buf, sizeof(buf), " NULL ");
                        appendStringInfoString(&ds, buf);
                    }
                    ds_changed = true;
                    break;
                }
                int dno = yylval.wdatum.datum->dno;
                PLpgSQL_var *var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
                if (left_brace_count == 0)
                {
                    nest_layers = var->nest_layers;
                }
                if (record_typename) {
                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                    ntype->typname = pstrdup(var->refname);
                    ntype->layer = bracket_diff;
                    ntype->index = typname_indexs[ntype->layer] + 1;
                    nest_typnames = lappend(nest_typnames, ntype);
                }
                ds_changed = construct_array_start(&ds, &context, var->datatype, &tok, parenlevel, loc);
                break;
            }
            case T_RECORD:
            {
                if (record_typename) {
                    int dno = yylval.wdatum.datum->dno;
                    PLpgSQL_rec_type* rec_type = (PLpgSQL_rec_type *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                    ntype->typname = pstrdup(rec_type->typname);
                    ntype->layer = bracket_diff;
                    ntype->index = typname_indexs[ntype->layer] + 1;
                    nest_typnames = lappend(nest_typnames, ntype);
                }
                break;
            }
            case T_DATUM:
                {
                    idents = yylval.wdatum.idents;
                    int dno = yylval.wdatum.datum->dno;
                    PLpgSQL_datum *datum = (PLpgSQL_datum *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
                    if (datum && datum->dtype == PLPGSQL_DTYPE_RECFIELD) {
                        PLpgSQL_recfield *rec_field = (PLpgSQL_recfield *)datum;
                        PLpgSQL_rec *rec = (PLpgSQL_rec *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[rec_field->recparentno];
                        MemoryContext old_cxt = NULL;
                        if (u_sess->plsql_cxt.curr_compile_context->compile_cxt != NULL)
                            old_cxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_cxt);
                        rec->field_need_check = lappend_int(rec->field_need_check, dno);
                        if (old_cxt != NULL) {
                            MemoryContextSwitchTo(old_cxt);
                        }
                    }
                    if(prev_tok != '.' && list_length(idents) >= 3) {
                        plpgsql_cast_reference_list(idents, &ds, false);
                        ds_changed = true;
                        break;
                    } else {
                        tok = yylex();
                        curloc = yylloc;
                        pltsql_push_back_token(tok);
                        plpgsql_append_source_text(&ds, loc, curloc);
                        ds_changed = true;
                        break;
                    }
                }
            case T_WORD:
            {
                if (record_typename) {
                    char *name = yylval.word.ident;
                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                    ntype->typname = pstrdup(name);
                    ntype->layer = bracket_diff;
                    ntype->index = typname_indexs[ntype->layer] + 1;
                    nest_typnames = lappend(nest_typnames, ntype);
                }
                AddNamespaceIfPkgVar(yylval.word.ident, save_IdentifierLookup);
                ds_changed = construct_word(&ds, &context, &tok, parenlevel, loc);
                break;
            }
            case T_CWORD:
            {
                if (record_typename) {
                    List* name_list = yylval.cword.idents;
                    switch(name_list->length) {
                        case 2:
                        {
                            char* packageName =  strVal(linitial(name_list));
                            char* typeName =  strVal(lsecond(name_list));
                            Oid namespaceOid = getCurrentNamespace();
                            Oid pkgOid = packageName ? PackageNameGetOid(packageName, namespaceOid) : InvalidOid;
                            if (pkgOid != InvalidOid && namespaceOid != InvalidOid) {
                                Oid type_oid = LookupTypeInPackage(name_list, typeName, pkgOid, namespaceOid);
                                if (type_oid != InvalidOid) {
                                    char* castTypeName = CastPackageTypeName(typeName, pkgOid, pkgOid != InvalidOid, true);
                                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                                    ntype->typname = pstrdup(castTypeName);
                                    ntype->layer = bracket_diff;
                                    ntype->index = typname_indexs[ntype->layer] + 1;
                                    nest_typnames = lappend(nest_typnames, ntype);
                                }
                            }
                            break;
                        }
                        case 3:
                        {
                            char* namesapceName =  strVal(linitial(name_list));
                            char* packageName =  strVal(lsecond(name_list));
                            char* typeName =  strVal(lthird(name_list));
                            Oid namespaceOid = namesapceName ? get_namespace_oid(namesapceName, true) : InvalidOid;
                            Oid pkgOid = packageName ? PackageNameGetOid(packageName, namespaceOid) : InvalidOid;
                            if (pkgOid != InvalidOid && namespaceOid != InvalidOid) {
                                Oid type_oid = LookupTypeInPackage(name_list, typeName, pkgOid, namespaceOid);
                                if (type_oid != InvalidOid) {
                                    char* castTypeName = CastPackageTypeName(typeName, pkgOid, pkgOid != InvalidOid, true);
                                    PLpgSQL_nest_type* ntype = (PLpgSQL_nest_type*)palloc(sizeof(PLpgSQL_nest_type));
                                    ntype->typname = pstrdup(castTypeName);
                                    ntype->layer = bracket_diff;
                                    ntype->index = typname_indexs[ntype->layer] + 1;
                                    nest_typnames = lappend(nest_typnames, ntype);
                                }
                            }
                            break;
                        }
                    }
                }
                ds_changed = construct_cword(&ds, &context, &tok, parenlevel, loc);
                break;
            }
            case T_OBJECT_TYPE_VAR_METHOD:
                {
                    appendStringInfo(&ds, " %s(", yylval.wdatum.ident);
                    CastTypeVariableNameToString(&ds, yylval.wdatum.idents, false);
                    ds_changed = true;

                    parenlevel++;
                    // Look ahead for another token to specify var.method format
                    tok = yylex();
                    if ('(' == tok) {
                        int tok1 = yylex();
                        /*if var.method()*/
                        if (tok1 == ')') {
                            appendStringInfo(&ds, ")");
                            parenlevel--;
                        } else { /* method with parameters*/
                            appendStringInfo(&ds, ",");
                            pltsql_push_back_token(tok1);
                        }
                    } else {
                        /*var.method*/
                        appendStringInfo(&ds, ")");
                        parenlevel--;
                        pltsql_push_back_token(tok);
                    }
                }
                break;
            default:
                tok = yylex();

                if(tok > INT_MAX)
                {
                    const char* message = "token value is bigger than INT_MAX";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_UNDEFINED_FUNCTION),
                             errmsg("token value %d is bigger than INT_MAX ", tok)));
                }

                curloc = yylloc;
                pltsql_push_back_token(tok);
                if (tok == T_VARRAY_VAR && pushed_bulk_exception()) {
                    plpgsql_append_source_text(&ds, loc, curloc - 4);
                } else {
                    plpgsql_append_source_text(&ds, loc, curloc);
                }
                ds_changed = true;
                break;
        }

        /* peek one token */
        tok = yylex();
        pltsql_push_back_token(tok);

        /* we are expecting an element, but a seperator/end of array is found, need coerce */
        if ((tok == ',' || tok == ')') && IS_ARRAY_STATE(context.list_array_state, ARRAY_ELEMENT)) {
            SET_ARRAY_STATE(context.list_array_state, ARRAY_COERCE);
        }

        /* this is done after seperator is appended, get next element */
        if (IS_ARRAY_STATE(context.list_array_state, ARRAY_SEPERATOR)) {
            /* always append left parentheses at start of each element */
            appendStringInfoString(&ds, left_parentheses);
            /* Note: we assume that all seperators takes EXACTLY ONE iteration to process */
            SET_ARRAY_STATE(context.list_array_state, ARRAY_ELEMENT);
        }

        /* add coerce at the end of each element, then append the seperator next iteration */
        if (IS_ARRAY_STATE(context.list_array_state, ARRAY_COERCE)) {
            /* always append right parentheses at end of each element */
            appendStringInfoString(&ds, right_parentheses);
            if (bracket_diff > nest_layers) {
                plpgsql_append_object_typename(&ds, (PLpgSQL_type *)linitial(context.list_datatype));
            }
            SET_ARRAY_STATE(context.list_array_state, ARRAY_SEPERATOR);
        }
    }

    if (brack_cnt != 0) {
        yyerror("mismatched brackets");
    }

    curr_compile->plpgsql_IdentifierLookup = save_IdentifierLookup;

    if (startloc)
        *startloc = startlocation;
    if (endtoken)
        *endtoken = tok;

    /* give helpful complaint about empty input */
    if (startlocation >= yylloc)
    {
        if (isexpression)
            yyerror("missing expression");
        else
            yyerror("missing SQL statement");
    }

    if (!ds_changed)
        plpgsql_append_source_text(&ds, startlocation, yylloc);
    
    /* trim any trailing whitespace, for neatness */
    if (trim)
    {
        while (ds.len > 0 && scanner_isspace(ds.data[ds.len - 1]))
            ds.data[--ds.len] = '\0';
    }
    if (issaveexpr) {
        oldCxt = MemoryContextSwitchTo(curr_compile->compile_cxt);
    }

    if (tableof_index_list != NULL && tableof_func_dno != -1) {
        ListCell* cell = NULL;
        foreach (cell, tableof_index_list) {
            int dno = lfirst_int(cell);
            if (dno != tableof_func_dno) {
                yyerror("do not support more than 2 table of index by variables call functions in an expr");
            }
        }
    }
    list_free_ext(tableof_index_list);

    expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->isouttype 		= false;
    expr->idx			= (uint32)-1;
    expr->out_param_dno = -1;
    expr->is_have_tableof_index_var = is_have_tableof_index_var;
    expr->tableof_var_dno = tableof_var_dno;
    expr->is_have_tableof_index_func = tableof_func_dno != -1 ? true : false;
    expr->tableof_func_dno = tableof_func_dno;
    expr->nest_typnames = nest_typnames ? nest_typnames : NIL;

    pfree_ext(ds.data);

    if (valid_sql)
        check_sql_expr(expr->query, startlocation, strlen(sqlstart));

    if (issaveexpr) {
        MemoryContextSwitchTo(oldCxt);
    }

    return expr;
}

/*
 * read multiset grammer
 */
static void read_multiset(StringInfoData* ds, char* tableName1, Oid typeOid1)
{
    /*
     * table var name may be schema.pkg.table_var
     * so the name length should NAMEDATALEN with quotation marks *3
     */
    const int tablevar_namelen = NAMEDATALEN * 3 + 6;

    /* len = funcname + tablename1 + tablename2 */
    const int funclen = tablevar_namelen * 2 + 128;
    char funcName[funclen] = {0};
    int tok1 = yylex();
    errno_t rc = EOK;
    bool isTableVar = false;
    int tok_type = -1;

    switch (tok1)
    {
        case K_UNION:
            rc = snprintf_s(funcName, sizeof(funcName), sizeof(funcName) - 1, " array_union");
            securec_check_ss(rc, "", "");
            break;
        case K_INTERSECT:
            rc = snprintf_s(funcName, sizeof(funcName), sizeof(funcName) - 1, " array_intersect");
            securec_check_ss(rc, "", "");
            break;
        case K_EXCEPT:
            rc = snprintf_s(funcName, sizeof(funcName), sizeof(funcName) - 1, " array_except");
            securec_check_ss(rc, "", "");
            break;
        default:
            yyerror("unexpected keyword after multiset.");
    }

    int suffixlen = tablevar_namelen * 2 + 64;
    char suffix[suffixlen] = {0};
    int tok2 = yylex();
    int tok3;
    Oid typeOid2 = InvalidOid;
    char tableName2[tablevar_namelen] = {0};
    if (K_DISTINCT == tok2) {
        tok3 = yylex();
        tok_type = -1;
        if (tok3 == T_PACKAGE_VARIABLE) {
            get_datum_tok_type(yylval.wdatum.datum, &tok_type);
        }
        isTableVar = (tok3 == T_TABLE_VAR) || (tok_type == PLPGSQL_TOK_TABLE_VAR);
        if (isTableVar) {
            typeOid2 = get_table_type(yylval.wdatum.datum);
            copy_table_var_indents(tableName2, yylval.wdatum.ident, tablevar_namelen);
            rc = snprintf_s(suffix, sizeof(suffix), sizeof(suffix) - 1, "_distinct(\"%s\", \"%s\")", tableName1, tableName2);
            securec_check_ss(rc, "", "");
        } else {
            yyerror("unexpected keyword after distinct.");
        }
    } else if (K_ALL == tok2) {
        tok3 = yylex();
        tok_type = -1;
        if (tok3 == T_PACKAGE_VARIABLE) {
            get_datum_tok_type(yylval.wdatum.datum, &tok_type);
        }
        isTableVar = (tok3 == T_TABLE_VAR) || (tok_type == PLPGSQL_TOK_TABLE_VAR);
        if (isTableVar) {
            typeOid2 = get_table_type(yylval.wdatum.datum);
            copy_table_var_indents(tableName2, yylval.wdatum.ident, tablevar_namelen);
            rc = snprintf_s(suffix, sizeof(suffix), sizeof(suffix) - 1, "(\"%s\", \"%s\")", tableName1, tableName2);
            securec_check_ss(rc, "", "");
        } else {
            yyerror("unexpected keyword after all.");
        }
    } else {
        tok_type = -1;
        if (tok2 == T_PACKAGE_VARIABLE) {
            get_datum_tok_type(yylval.wdatum.datum, &tok_type);
        }
        isTableVar = (tok2 == T_TABLE_VAR) || (tok_type == PLPGSQL_TOK_TABLE_VAR);
        if (isTableVar) {
            typeOid2 = get_table_type(yylval.wdatum.datum);
            copy_table_var_indents(tableName2, yylval.wdatum.ident, tablevar_namelen);
            rc = snprintf_s(suffix, sizeof(suffix), sizeof(suffix) - 1, "(\"%s\", \"%s\")", tableName1, tableName2);
            securec_check_ss(rc, "", "");
        } else {
            yyerror("unexpected type after multiset.");
        }
    }

    if (typeOid1 != typeOid2) {
        ereport(errstate,
            (errcode(ERRCODE_DATATYPE_MISMATCH),
            errmsg("%s and %s type are not match.", tableName1, tableName2)));
        u_sess->plsql_cxt.have_error = true;
    }
    strlcat(funcName, suffix, sizeof(funcName));
    appendStringInfoString(ds, funcName);
}

static void CastTypeVariableNameToString(StringInfoData* ds, List* idents, bool needDot)
{
    char* name1 = NULL;
    char* name2 = NULL;

    switch (list_length(idents)) {
        case 1:
            /* object type var */
            name1 = strVal(linitial(idents));
            appendStringInfo(ds, "%s", name1);
            break;
        case 2:
            /* package.var */
            name1 = strVal(linitial(idents));
            name2 = strVal(lsecond(idents));
            appendStringInfo(ds,"%s.%s", name1, name2);
            break;
        default:
            yyerror("syntax error of array functions");
            break;
    }
    
    if (needDot) {
        appendStringInfo(ds, ", ");
    } else {
        appendStringInfo(ds, " ");
    }
}

static bool copy_table_var_indents(char* tableName, char* idents, int tablevar_namelen)
{
    List* namelist = NIL;
    ListCell* l = NULL;
    errno_t rc = 0;
    StringInfoData	ds;
    initStringInfo(&ds);

    if (!SplitIdentifierString(idents, '.', &namelist))
        ereport(ERROR, (errcode(ERRCODE_INVALID_NAME), errmsg("invalid name syntax")));

    if (namelist == NIL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_NAME), errmsg("invalid name syntax")));

    foreach (l, namelist) {
        char* curname = (char*)lfirst(l);
        appendStringInfoString(&ds, curname);
        if (l->next != NULL) {
            appendStringInfoString(&ds, "\".\"");
        }
    }
    rc = snprintf_s(tableName, tablevar_namelen, tablevar_namelen - 1, "%s", ds.data);
    securec_check_ss(rc, "", "");
    pfree_ext(ds.data);
    list_free_ext(namelist);

    return true;
}

/*
 * Read a SQL construct and build a PLpgSQL_expr for it.
 *
 * until:		token code for expected terminator
 * until2:		token code for alternate terminator (pass 0 if none)
 * until3:		token code for another alternate terminator (pass 0 if none)
 * expected:	text to use in complaining that terminator was not found
 * sqlstart:	text to prefix to the accumulated SQL text
 * isexpression: whether to say we're reading an "expression" or a "statement"
 * valid_sql:   whether to check the syntax of the expr (prefixed with sqlstart)
 * trim:		trim trailing whitespace
 * startloc:	if not NULL, location of first token is stored at *startloc
 * endtoken:	if not NULL, ending token is stored at *endtoken
 *				(this is only interesting if until2 or until3 isn't zero)
 */
static PLpgSQL_expr *
read_sql_construct(int until,
                   int until2,
                   int until3,
                   const char *expected,
                   const char *sqlstart,
                   bool isexpression,
                   bool valid_sql,
                   bool trim,
                   int *startloc,
                   int *endtoken)
{
    return read_sql_construct6(until, until2, until3, until3, 0, 0,
                               expected, sqlstart, isexpression, valid_sql, trim, startloc, endtoken);
}
/*
 * get function declare or definition string in package.
 */
static DefElem*
get_proc_str(int tok)
{
    int     blocklevel = 0;
    int     pre_tok = 0;
    DefElem *def;
    tok = yylex();
    def = makeDefElem(NULL, NULL); /* make an empty DefElem */
    def->location = yylloc;
    StringInfoData      ds;  
    bool is_defining_proc = false;
    char * spec_proc_str = NULL;
    int loc = yylloc;
    int curloc = yylloc;
    bool in_begin = false;
    int in_procedure = 0;
    MemoryContext oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    initStringInfo(&ds);
    MemoryContextSwitchTo(oldCxt);
    if (u_sess->parser_cxt.is_procedure==false){
        appendStringInfoString(&ds, " CREATE OR REPLACE FUNCTION ");
    } else {
        appendStringInfoString(&ds, " CREATE OR REPLACE PROCEDURE ");
    }
    def->begin_location = ds.len;
    u_sess->parser_cxt.in_package_function_compile = true;
    while(true)
    {    

        if (tok == YYEOF) {
            if (u_sess->parser_cxt.is_procedure==false){
                yyerror("function is not ended correctly");
                break;
            } else {
                yyerror("procedure is not ended correctly");
                break;
            }
        }

        if (!in_begin && (tok == K_PROCEDURE || tok == K_FUNCTION)) {
			in_procedure++;
		}

        if (tok == ';' && !is_defining_proc) 
        {
            break;
        }
        /* procedure or function can have multiple blocklevel*/
        if (tok == K_BEGIN) 
        {
            blocklevel++;
            in_begin = true;
        }

        /* if have is or as,it means in body*/
        if ((tok == K_IS || tok == K_AS) && !is_defining_proc)
        {
            if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL && 
                u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling) {
                yyerror("not allow define function or procedure in package specification");
                break;
            }
            is_defining_proc = true;
        }

        if (tok == K_END) 
        {
            tok = yylex();
            /* process end loop*/
            if (tok == K_LOOP || tok == K_WHILE || tok == K_REPEAT) {
                continue;
            }
            if (blocklevel == 1 && (pre_tok == ';' || pre_tok == K_BEGIN) && (tok == ';' || tok == 0))
            {
                if(in_procedure < 1){
                    curloc = yylloc;
                    plpgsql_append_source_text(&ds, loc, curloc);          
                    break;
                } else {
                    in_procedure--;
                    blocklevel--;
                }
            }
            if (blocklevel > 1 && (pre_tok == ';' || pre_tok == K_BEGIN) && (tok == ';' || tok == 0))
            {
                blocklevel--;
            }
            in_begin = false;
            if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && blocklevel == 1 && pre_tok == ';' && tok == T_WORD && in_procedure == 0)
            {
                curloc = yylloc;
                plpgsql_append_source_text(&ds, loc, curloc);
                tok = yylex();
                break;
            }
        }
        pre_tok = tok;

        tok = yylex();
        curloc = yylloc;
        plpgsql_append_source_text(&ds, loc, curloc);

        loc = yylloc;
    }    
    u_sess->parser_cxt.in_package_function_compile = false;

    /* 
        To use the SQL Parser, we add a string to make SQL parser recognize. 
        Package specification use SQL parser parse the string too.
    */
    if (is_defining_proc == false) {
        appendStringInfoString(&ds, " AS BEGIN ");
        appendStringInfoString(&ds, " END;\n");
        u_sess->parser_cxt.isFunctionDeclare = true;
    } else {
        u_sess->parser_cxt.isFunctionDeclare = false;
    }

    def->defname = pstrdup(ds.data);
    spec_proc_str = pstrdup(ds.data);
    pfree_ext(ds.data);
    return def;
}
static char* get_init_proc(int tok) 
{
    int startlocation = yylloc;
    char* init_package_str = NULL;
    StringInfoData      ds;  
    initStringInfo(&ds);
    while(true)
    {  
        /* 
          package instantiation still need a token 'END' as end,
          but it's from package end, For details about how to instantiate
          a package, see the package instantiation grammer.
        */
        if (tok == K_BEGIN)
            startlocation = yylloc;
        if (tok == YYEOF) {
            yyerror("procedure is not ended correctly");
            break;
        }
        if (tok == K_END) 
        {
            tok = yylex();
            if (tok == ';' || tok == 0)
            {
                break;
            }
        }
        tok = yylex();
    } 
    /*
      package instantiation not need specify end.
    */
    plpgsql_append_source_text(&ds, startlocation, yylloc);
    init_package_str = pstrdup(ds.data);
    pfree_ext(ds.data);
    return init_package_str;
}

/* get assign attrname of arrayelement */
static char * get_attrname(int tok)
{
    if (tok == YYEMPTY || tok == '.') {
        tok = pltsql_yylex_single();
    }
    plpgsql_location_to_lineno(yylloc);
    switch (tok) {
	case T_ARRAY_FIRST:
	case T_ARRAY_LAST:
	case T_ARRAY_COUNT:
	case T_ARRAY_EXTEND:
    case T_CWORD:/* composite name not OK */
        yyerror("syntax error");
        break;
    case T_REFCURSOR:
	case T_VARRAY:
	case T_VARRAY_VAR:
    case T_TABLE:
    case T_TABLE_VAR:
	case T_RECORD:
    case T_DATUM:
        if (yylval.wdatum.idents != NIL) { /* composite name not OK */
            yyerror("syntax error");
        } else {
            return yylval.wdatum.ident;
        }
        break;
	case T_WORD:
	    return yylval.word.ident;
	    break;
    default:
        if (pltsql_is_token_keyword(tok)) {
            return pstrdup(yylval.keyword);
        } else {
            yyerror("missing or illegal attribute name");
        }
        break;
    }
    return NULL;
}

static PLpgSQL_type *
read_datatype(int tok)
{
    StringInfoData		ds;
    char			   *type_name;
    int					startlocation;
    PLpgSQL_type		*result = NULL;
    int					parenlevel = 0;
    List                *dtnames = NIL;

    /* Should only be called while parsing DECLARE sections */
    AssertEreport(u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup == IDENTIFIER_LOOKUP_DECLARE,
                        MOD_PLSQL,
                        "Should only be called while parsing DECLARE sections");

    /* Often there will be a lookahead token, but if not, get one */
    if (tok == YYEMPTY)
        tok = yylex();

    startlocation = yylloc;

    /*
     * If we have a simple or composite identifier, check for %TYPE
     * and %ROWTYPE constructs.
     */
    bool iskeyword = pltsql_is_token_keyword(tok);
    if (tok == T_WORD || tok == T_VARRAY_VAR || tok == T_TABLE_VAR || iskeyword)
    {
        char   *dtname;
        if (iskeyword) {
            dtname = pstrdup(yylval.keyword);
        } else if (tok == T_WORD) {
            dtname = yylval.word.ident;
        } else {
            dtname = yylval.wdatum.ident;
        }

        tok = yylex();
        if (tok == '%')
        {
            tok = yylex();
            if (tok_is_keyword(tok, &yylval,
                               K_TYPE, "type"))
            {
                result = plpgsql_parse_wordtype(dtname);
                if (result) {
                    if(iskeyword) {
                        pfree_ext(dtname);
                    }
                    return result;
                }
            }
            else if (tok_is_keyword(tok, &yylval,
                                    K_ROWTYPE, "rowtype"))
            {
                PLpgSQL_nsitem *ns;
                ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, 
                            dtname, NULL, NULL, NULL);
                if (ns && ns->itemtype == PLPGSQL_NSTYPE_VAR)
                {
                    PLpgSQL_var* var = (PLpgSQL_var*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[ns->itemno];
                    if(var && var->datatype 
                           && var->datatype->typoid == REFCURSOROID)
                    {
                        PLpgSQL_type *newp = plpgsql_build_datatype(UNKNOWNOID, -1, InvalidOid);
                        newp->cursorExpr = var->cursor_explicit_expr;
                        newp->cursorDno = var->dno;
                        return newp;
                    }
                } else if (ns && ns->itemtype == PLPGSQL_NSTYPE_ROW)
                {
                    PLpgSQL_row* row = (PLpgSQL_row*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[ns->itemno];
                    if (row && row->rowtupdesc && row->rowtupdesc->tdtypeid != RECORDOID)
                    {
                        if (OidIsValid(row->rowtupdesc->tdtypeid))
                            return plpgsql_build_datatype(row->rowtupdesc->tdtypeid, -1, InvalidOid);
                    }
                }
                result = plpgsql_parse_wordrowtype(dtname);
                if (result) {
                    if(iskeyword) {
                        pfree_ext(dtname);
                    }
                    return result;
                }
            }
        }
        if(iskeyword) {
           pfree_ext(dtname);
        }
    }
    else if (tok == T_CWORD)
    {
        dtnames = yylval.cword.idents;

        tok = yylex();
        if (tok == '%')
        {
            tok = yylex();
            if (tok_is_keyword(tok, &yylval,
                               K_TYPE, "type"))
            {
                TypeDependExtend* typeDependExtend = NULL;
                if (enable_plpgsql_gsdependency()) {
                    InstanceTypeNameDependExtend(&typeDependExtend);
                }
                /* find val.col%TYPE first */
                HeapTuple tup = NULL;
                int collectionType = PLPGSQL_COLLECTION_NONE;
                Oid tableOfIndexType = InvalidOid;
                int32 typMod = -1;
                tup = FindRowVarColType(dtnames, &collectionType, &tableOfIndexType, &typMod, typeDependExtend);
                if (tup != NULL) {
                    Oid typOid = typeTypeId(tup);
                    ReleaseSysCache(tup);
                    PLpgSQL_type* type = plpgsql_build_datatype(typOid, typMod, InvalidOid, typeDependExtend);
                    if (OidIsValid(tableOfIndexType)) {
                        type->collectionType = collectionType;
                        type->tableOfIndexType = tableOfIndexType;
                    }
                    return type;
                }

                /* find pkg.var%TYPE second */
                PLpgSQL_datum* datum = GetPackageDatum(dtnames);
                if (datum != NULL) {
                    if (datum->dtype == PLPGSQL_DTYPE_VAR) {
                        PLpgSQL_var* var = (PLpgSQL_var*)datum;
                        Oid typOid =  var->datatype->typoid;
                        int32 typmod = var->datatype->atttypmod;
                        Oid collation = var->datatype->collation;
                        int collectionType = var->datatype->collectionType;
                        Oid tableOfIndexType = var->datatype->tableOfIndexType;
                        if (var->pkg != NULL && enable_plpgsql_gsdependency()) {
                            typeDependExtend->objectName = pstrdup(var->refname);
                            typeDependExtend->packageName = pstrdup(var->pkg->pkg_signature);
                            typeDependExtend->schemaName = get_namespace_name(var->pkg->namespaceOid);
                        }
                        PLpgSQL_type* type = plpgsql_build_datatype(typOid, typmod, collation, typeDependExtend);
                        type->collectionType = collectionType;
                        type->tableOfIndexType = tableOfIndexType;
                        return type;
                    } else if (datum->dtype == PLPGSQL_DTYPE_ROW){
                        PLpgSQL_row* row = (PLpgSQL_row*)datum;
                        if (row->rowtupdesc && row->rowtupdesc->tdtypeid != RECORDOID &&
                            OidIsValid(row->rowtupdesc->tdtypeid)) {
                            if (row->pkg != NULL && enable_plpgsql_gsdependency()) {
                                typeDependExtend->objectName = pstrdup(row->refname);
                                typeDependExtend->packageName = pstrdup(row->pkg->pkg_signature);
                                typeDependExtend->schemaName = get_namespace_name(row->pkg->namespaceOid);
                            }
                            return plpgsql_build_datatype(row->rowtupdesc->tdtypeid, -1, InvalidOid, typeDependExtend);
                        }
                    }
                }
                result = plpgsql_parse_cwordtype(dtnames, typeDependExtend);
                if (result)
                    return result;
                if (enable_plpgsql_undefined()) {
                    Oid tryUndefObjOid = gsplsql_try_build_exist_pkg_undef_var(dtnames);
                    if (OidIsValid(tryUndefObjOid)) {
                        typeDependExtend->undefDependObjOid = tryUndefObjOid;
                        typeDependExtend->dependUndefined = true;
                        return plpgsql_build_datatype(UNDEFINEDOID, -1, InvalidOid, typeDependExtend);
                    }
                }
            }
            else if (tok_is_keyword(tok, &yylval,
                                    K_ROWTYPE, "rowtype"))
            {
                PLpgSQL_datum* datum = GetPackageDatum(dtnames);
                if (datum != NULL && datum->dtype == PLPGSQL_DTYPE_ROW) {
                    PLpgSQL_row* row = (PLpgSQL_row*)datum;
                    if (row->rowtupdesc && row->rowtupdesc->tdtypeid != RECORDOID)
                    {
                        if (OidIsValid(row->rowtupdesc->tdtypeid))
                            return plpgsql_build_datatype(row->rowtupdesc->tdtypeid, -1, InvalidOid);
                    }
                }
                result = plpgsql_parse_cwordrowtype(dtnames);
                if (result)
                    return result;
            }
        }
    }

    while (tok != ';')
    {
        if (tok == 0)
        {
            if (parenlevel != 0)
                yyerror("mismatched parentheses", true);
            else
                yyerror("incomplete data type declaration");
            break;
        }
        /* Possible followers for datatype in a declaration */
        if (tok == K_COLLATE || tok == K_NOT || tok == '=' || tok == COLON_EQUALS || tok == K_DEFAULT || tok == K_INDEX)
            break;
        /* Possible followers for datatype in a cursor_arg list */
        if ((tok == ',' || tok == ')') && parenlevel == 0)
            break;
        if (tok == '(')
            parenlevel++;
        else if (tok == ')')
            parenlevel--;

        tok = yylex();
    }

    /* set up ds to contain complete typename text */
    initStringInfo(&ds);
    plpgsql_append_source_text(&ds, startlocation, yylloc);
    type_name = ds.data;

    if (type_name[0] == '\0') {
#ifndef ENABLE_MULTIPLE_NODES
        if (u_sess->attr.attr_common.plsql_show_all_error) {
            const char* message = "missing data type declaration";
            InsertErrorMessage(message, yylloc);
        }
#endif
        ereport(ERROR,
            (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmsg("missing data type declaration"),
                parser_errposition(yylloc)));
        if (tok == ';') {
            pltsql_push_back_token(tok);
        }
    } else {
        u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
        result = parse_datatype(type_name, startlocation);

        pfree_ext(ds.data);

        pltsql_push_back_token(tok);
    }
    
    return result;
}

static PLpgSQL_stmt *
make_execsql_stmt(int firsttoken, int location)
{
    StringInfoData		ds;
    IdentifierLookup	save_IdentifierLookup;
    PLpgSQL_stmt_execsql *execsql;
    PLpgSQL_expr		*expr;
    PLpgSQL_row			*row = NULL;
    PLpgSQL_rec			*rec = NULL;
    int					tok;
    int					prev_tok;
    int                                 prev_prev_tok = 0;
    int					curloc;
    bool				have_into = false;
    bool				have_bulk_collect = false;
    bool				have_strict = false;
    bool				array_begin = false;
    int					into_start_loc = -1;
    int					into_end_loc = -1;
    int					placeholders = 0;
    int					parenlevel = 0;
    List				*list_bracket = 0;		/* stack structure bracket tracker */
    List				*list_bracket_loc = 0;	/* location tracker */
    bool                                label_begin = false;
    initStringInfo(&ds);

    /* special lookup mode for identifiers within the SQL text */
    save_IdentifierLookup = u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup;
    u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;

    /*
     * Scan to the end of the SQL command.  Identify any INTO-variables
     * clause lurking within it, and parse that via read_into_target().
     *
     * Because INTO is sometimes used in the main SQL grammar, we have to be
     * careful not to take any such usage of INTO as a PL/pgSQL INTO clause.
     * There are currently three such cases:
     *
     * 1. SELECT ... INTO.  We don't care, we just override that with the
     * PL/pgSQL definition.
     *
     * 2. INSERT INTO.  This is relatively easy to recognize since the words
     * must appear adjacently; but we can't assume INSERT starts the command,
     * because it can appear in CREATE RULE or WITH.  Unfortunately, INSERT is
     * *not* fully reserved, so that means there is a chance of a false match;
     * but it's not very likely.
     *
     * 3. ALTER TABLE PARTITION MERGE or SPLIT INTO.
     * We just check for ALTER as the command's first token.
     *
     * Fortunately, INTO is a fully reserved word in the main grammar, so
     * at least we need not worry about it appearing as an identifier.
     *
     * Any future additional uses of INTO in the main grammar will doubtless
     * break this logic again ... beware!
     */
    tok = firsttoken;

    /* For support InsertStmt:Insert into table_name values record_var */
    bool insert_stmt = false;
    bool prev_values = false;
    bool is_user_var = false;
    bool insert_record = false;
    bool insert_array_record = false; 
    int values_end_loc = -1;
    int before_semi_loc = -1;
    const char* err_msg = "The label name can only contain letters, digits and underscores";
    PLpgSQL_row* row_data = NULL;
    PLpgSQL_rec* rec_data = NULL;
    PLpgSQL_var* array_data = NULL;
    for (;;)
    {
        prev_tok = tok;
        tok = yylex();
        if (tok == COMMENTSTRING) {
            prev_prev_tok = prev_tok;
        }

        if (have_into && into_end_loc < 0)
            into_end_loc = yylloc;		/* token after the INTO part */

        /* Only support one record variable after insertstmt's values token */
        if (insert_record && tok != ';' && tok != ' ') {
            yyerror("unexpected record variable number in insert stmt.");
        }

        if (tok == ';') {
            before_semi_loc = yylloc;
            break;
        }

        if (tok == 0)
            yyerror("unexpected end of function definition");

        if(tok == T_PLACEHOLDER)
        {
            placeholders++;
        }

        if (tok == K_BULK) {
            /* only A_FORMAT can use bulk collect into */
            if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
                 yyerror("syntax error");
            }

            have_bulk_collect = true;
            into_start_loc = yylloc; /* save BULK COLLECT INTO starting point */
            tok = yylex();
            if (tok != K_COLLECT) {
                yyerror("expect \'COLLECT\' after \'BULK\'");
            }
            tok = yylex();
            if (tok != K_INTO) {
                yyerror("expect \'INTO\' after \'BULK COLLECT\'");
            }
        }

        if (tok == K_INTO)
        {
            if (prev_tok == K_INSERT || prev_tok == K_REPLACE || (prev_tok == COMMENTSTRING && (prev_prev_tok == K_INSERT || prev_prev_tok == K_REPLACE))) {
                insert_stmt = true;
                continue;	/* INSERT INTO is not an INTO-target */
            }
            if (firsttoken == K_ALTER)
                continue;	/* ALTER ... INTO is not an INTO-target */
            if (prev_tok == K_MERGE)
                continue;	/* MERGE INTO is not an INTO-target */
            if (have_into || is_user_var)
                yyerror("INTO specified more than once");
            have_into = true;
            if (!have_bulk_collect) {
                into_start_loc = yylloc;
            }
            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_NORMAL;
            is_user_var = read_into_target(&rec, &row, &have_strict, firsttoken, have_bulk_collect);
            if (is_user_var) {
                u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = save_IdentifierLookup;
                have_into = false;
            } else {
                u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_EXPR;
            }
        }

	/*
	 * When the database is in mysql compatible mode, 
	 * the loop syntax of mysql is compatible (label: loop)
	 */
        if (tok == ':' && prev_tok == T_WORD)
        {
            StringInfoData  lb;
            initStringInfo(&lb);
            int  lb_end = yylloc;
            int  tok1 = yylex();
            if(tok1 == K_LOOP || tok1 == K_WHILE || tok1 == K_REPEAT)
            {
                if(u_sess->attr.attr_sql.sql_compatibility == B_FORMAT)
                {
                    int  count = 0;
                    label_begin =  true;
                    pltsql_push_back_token(tok1);
                    pltsql_push_back_token(tok);
                    plpgsql_append_source_text(&lb, location, lb_end);

                    for(int i = lb.len-1; i > 0; i--)
                    {
                        if(lb.data[i] == ' ')
                        {
                            count++;
                        }
                        else
                            break;
                    }
                    if(count > 0 && lb.len-count > 0)
                    {
                        char*  name = NULL;
                        errno_t rc = 0;
                        int num = -1;

                        int len = Min(NAMEDATALEN, lb.len - count + 1);
                        name = (char*)palloc(len);
                        rc = strncpy_s(name, len, lb.data, len - 1);
                        securec_check_c(rc, "\0", "\0");
                        num = strspn(pg_strtolower(name), "abcdefghijklmnopqrstuvwxyz0123456789_");

                        if(num != len - 1 || (name[0] >= '0' && name[0] <= '9')) {
                            pfree(name);
                            pfree_ext(lb.data);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("%s", err_msg),
                                    parser_errposition(location + num)));
                        }

                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(name));
                        pfree(name);
                    }
                    else {
                        int valid_len = lb.len;
                        if(lb.len >= NAMEDATALEN)
                        {
                            lb.data[NAMEDATALEN - 1] = '\0';
                            valid_len = NAMEDATALEN - 1;
                        }
                        int len = -1;
                        len = strspn(pg_strtolower(lb.data), "abcdefghijklmnopqrstuvwxyz0123456789_");

                        if(len != valid_len) {
                            pfree_ext(lb.data);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("%s", err_msg),
                                    parser_errposition(location + len)));
                        }
                        if(lb.data[0] >= '0' && lb.data[0] <= '9') {
                            pfree_ext(lb.data);
                            ereport(errstate,
                                    (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("%s", err_msg),
                                    parser_errposition(location)));
                        }
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(lb.data));
                    }
                    pfree_ext(lb.data);
                    break;
                }
                else
                    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                            errmsg("'label:' is only supported in database which dbcompatibility='B'.")));
            }
        }

        if ((tok == T_LABELLOOP || tok == T_LABELWHILE || tok == T_LABELREPEAT) && prev_tok == T_WORD)
        {
            StringInfoData  lb;
            initStringInfo(&lb);
            int  lb_end = yylloc;
            if(u_sess->attr.attr_sql.sql_compatibility == B_FORMAT)
            {
                int  count = 0;
                label_begin =  true;
                pltsql_push_back_token(tok);
                plpgsql_append_source_text(&lb, location, lb_end);

                for(int i = lb.len-1; i > 0; i--)
                {
                    if(lb.data[i] == ' ')
                    {
                        count++;
                    }
                    else
                        break;
                }
                if(count > 0 && lb.len-count > 0)
                {
                    char*  name = NULL;
                    errno_t rc = 0;
                    int len = -1;

                    name = (char*)palloc(lb.len-count+1);
                    rc = strncpy_s(name, lb.len-count+1, lb.data, lb.len-count);
                    securec_check_c(rc, "\0", "\0");
                    len = strspn(pg_strtolower(name), "abcdefghijklmnopqrstuvwxyz0123456789_");

                    if(len != lb.len - count || (name[0] >= '0' && name[0] <= '9')) {
                        pfree(name);
                        pfree_ext(lb.data);
                        ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("%s", err_msg),
                                parser_errposition(location + len)));
                    }

                    if(lb.len-count >= NAMEDATALEN)
                    {
                        char*   namedata = NULL;
                        errno_t rc = 0;
                        namedata = (char*)palloc(NAMEDATALEN);
                        rc = strncpy_s(namedata, NAMEDATALEN, name, NAMEDATALEN-1);
                        securec_check_c(rc, "\0", "\0");
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(namedata));
                        pfree(namedata);
                    }
                    else
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(name));
                    pfree(name);
                }
                else
                {
                    int len = -1;
                    len = strspn(pg_strtolower(lb.data), "abcdefghijklmnopqrstuvwxyz0123456789_");

                    if(len != lb.len) {
                        pfree_ext(lb.data);
                        ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("%s", err_msg),
                                parser_errposition(location + len)));
                    }
                    if(lb.data[0] >= '0' && lb.data[0] <= '9') {
                        pfree_ext(lb.data);
                        ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("%s", err_msg),
                                parser_errposition(location)));
                    }
                    if(lb.len >= NAMEDATALEN)
                    {
                        char*   namedata = NULL;
                        errno_t rc = 0;
                        namedata = (char*)palloc(NAMEDATALEN);
                        rc = strncpy_s(namedata, NAMEDATALEN, lb.data, NAMEDATALEN-1);
                        securec_check_c(rc, "\0", "\0");
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(namedata));
                        pfree(namedata);
                    }
                    else
                        plpgsql_ns_additem(PLPGSQL_NSTYPE_LABEL, 0, pg_strtolower(lb.data));
                }
                pfree_ext(lb.data);
                break;
            }
            else
                ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                        errmsg("'label:' is only supported in database which dbcompatibility='B'.")));
        }

        if (tok == T_CWORD && prev_tok!=K_SELECT 
                           && prev_tok!= K_PERFORM) {
            List  *dtname = yylval.cword.idents;
            tok = yylex();
            if (tok == '(') {
                pltsql_push_back_token(tok);
                continue;
            }
            pltsql_push_back_token(tok);
            plpgsql_pkg_add_unknown_var_to_namespace(dtname);
            continue;
        }

        /*
         * A_FORMAT VARRAY related handling
         * To make parentheses-based varray access compatible with current
         * PL/pgSQL array, we need to replace all parentheses with brackets.
         * Like what we did in read_sql_contruct5(), we interpret the input
         * query with array states.
         *
         * When array_begin is set to true, it means we have an array comming
         * in next iteration, we need to append a bracket instead of parentheses.
         * Or, array_begin is set to false that the query statement will remain
         * unchanged.
         */
        if (u_sess->attr.attr_sql.sql_compatibility != A_FORMAT) {
            continue;
        }


        /* Find 'values' position for InsertStmt */
        if (insert_stmt && tok == T_WORD) {
            if (pg_strcasecmp(yylval.word.ident, "values") == 0) {
                tok = yylex();
                if (tok == '(') {
                    prev_values = false;
                } else {
                    prev_values = true;
                }
                pltsql_push_back_token(tok);
                continue;
            }
        }

        /* Check the variable type after 'values' */
        if (insert_stmt && prev_values && (tok == T_WORD || tok == T_VARRAY_VAR || tok == T_TABLE_VAR)) {
            if (tok == T_WORD) {
                PLpgSQL_nsitem* ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, yylval.word.ident, NULL, NULL, NULL);
                if (ns == NULL) {
                    yyerror("insert an nonexistent variable.");
                    continue;
                }

                PLpgSQL_datum* datum = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[ns->itemno];
                insert_record = true;
                prev_values = false;
                values_end_loc = yylloc;
                if (PLPGSQL_DTYPE_RECORD == datum->dtype 
                        || PLPGSQL_DTYPE_ROW == datum->dtype) {
                    row_data = (PLpgSQL_row*)datum;
                    continue;
                } else if (PLPGSQL_DTYPE_REC == datum->dtype) {
                    rec_data = (PLpgSQL_rec*)datum;
                    if (rec_data->tupdesc == NULL) {
                        yyerror("unsupported insert into table from record type without desc, "
                                "may need set behavior_compat_options to allow_procedure_compile_check.");
                    }
                    continue;
                }

                yyerror("unsupported insert into table from non record type.", true);
            } else if (tok == T_VARRAY_VAR || tok == T_TABLE_VAR) {
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_VAR) {
                    array_data = (PLpgSQL_var *) yylval.wdatum.datum;
                    insert_array_record = true;
                    prev_values = false;
                    values_end_loc = yylloc;
                    continue;
                }
                yyerror("unsupported insert into table from non record type.");
            } else {
                yyerror("unsupported insert into table from non record type.");
            }
        }

        if (tok == '(' || tok == '[')
            parenlevel++;
        else if (tok == ')' || tok == ']')
        {
            parenlevel--;
            if (parenlevel < 0)
                yyerror("mismatched parentheses", true);
        }
        switch(tok) {
            case T_VARRAY_VAR:
                curloc = yylloc;    /* always save current location before yylex() */
                tok = yylex();
                if (tok == '(' || tok == '[')
                    array_begin = true;
                pltsql_push_back_token(tok);
                break;
            case T_TABLE_VAR:
                curloc = yylloc;    /* always save current location before yylex() */
                tok = yylex();
                if (tok == '(')
                    array_begin = true;
                else if (tok == '[') 
                    yyerror("syntax error");

                pltsql_push_back_token(tok);
                break;
            case ']':
            case ')':
                if (list_bracket && linitial_int(list_bracket) == parenlevel) {
                    prev_tok = tok;
                    curloc = yylloc;    /* always save current location before yylex() */
                    tok = yylex();
                    if (tok == '(' || tok == '[' || tok == '.') {
                        array_begin = true; /* N-D array access */
                    }
                    pltsql_push_back_token(tok);

                    /* stack pop */
                    list_bracket = list_delete_first(list_bracket);
                    if (prev_tok == ')') {
                        /* append location of right bracket */
                        list_bracket_loc = lappend_int(list_bracket_loc, curloc - location);
                    }
                }
                break;
            case '[':
            case '(':
                if (array_begin) {
                    /* stack push */
                    list_bracket = lcons_int(parenlevel - 1, list_bracket);
                    if (tok == '(') {
                        /* cancat location of left bracket */
                        list_bracket_loc = lcons_int(yylloc - location, list_bracket_loc);
                    }
                    array_begin = false;
                }
                break;
            case T_WORD:
            case T_CWORD:
                if (array_begin) {
                    curloc = yylloc;    /* always save current location before yylex() */
                    tok = yylex();
                    if (tok != '(' && tok != '[') {
                        array_begin = false;
                    }
                    pltsql_push_back_token(tok);
                }
                break;
            case '.':
                if (array_begin) {
                    curloc = yylloc;    /* always save current location before yylex() */
                    tok = yylex();
                    if (tok != T_WORD && tok != T_CWORD) {
                        array_begin = false;
                    }
                    pltsql_push_back_token(tok);
                }
                break;
            default:
                /* do nothing */
                break;
        }

    }

    u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = save_IdentifierLookup;

    if (have_into)
    {
        /*
         * Insert an appropriate number of spaces corresponding to the
         * INTO text, so that locations within the redacted SQL statement
         * still line up with those in the original source text.
         */
        plpgsql_append_source_text(&ds, location, into_start_loc);
        appendStringInfoSpaces(&ds, into_end_loc - into_start_loc);
        plpgsql_append_source_text(&ds, into_end_loc, yylloc);

        if (parenlevel != 0 ||
            list_length(list_bracket) != 0 ||
            list_length(list_bracket_loc) % 2)
        {
            yyerror("mismatched parentheses");
        }
        plpgsql_process_stmt_array(&ds, list_bracket_loc);
        list_free_ext(list_bracket_loc);
    } else if (insert_record) {
        /* Support grammar 'Insert into table_name values record_type' in Procedure.
         * because SQL Parser does's support that, so record type will be broken into
         * record variable's sub attribute variable for 'Insert into table_name 
         * values(var,...).
         */
        plpgsql_append_source_text(&ds, location, values_end_loc);
        appendStringInfoString(&ds, "(");
        if (row_data != NULL) {
        int nfields = row_data->nfields;
            for (int i = 0; i < nfields; i++) {
                appendStringInfo(&ds, "%s.%s", row_data->refname, row_data->fieldnames[i]);
                if (i != (nfields - 1)) {
                    appendStringInfoString(&ds, ",");
                }
            }
        } else {
            int nattr = rec_data->tupdesc->natts;
            for (int i = 0; i < nattr; i++) {
               Form_pg_attribute pg_att_form = TupleDescAttr(rec_data->tupdesc, i);
               appendStringInfo(&ds, "%s.%s", rec_data->refname, NameStr(pg_att_form->attname));
               if (i != (nattr - 1)) {
                   appendStringInfoString(&ds, ",");
               }
               PLpgSQL_recfield* rec_field = NULL;
               rec_field = (PLpgSQL_recfield*)palloc0(sizeof(PLpgSQL_recfield));
               rec_field->dtype = PLPGSQL_DTYPE_RECFIELD;
               rec_field->fieldname = pstrdup(NameStr(pg_att_form->attname));
               
               int nnames = 0;
               PLpgSQL_nsitem* ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, rec_data->refname, NameStr(pg_att_form->attname), NULL, &nnames);
               if (ns == NULL) {
                   yyerror("insert an nonexistent variable");
               }
               rec_field->recparentno = ns->itemno;
               (void)plpgsql_adddatum((PLpgSQL_datum*)rec_field);
            }
        }
        appendStringInfoString(&ds, ")");
    } else if (insert_array_record) {
        /* Support grammar 'Insert into table_name values array[][]...' in Procedure.
         * because SQL Parser does's support that, so record type will be broken into
         * record variable's sub attribute variable for 'Insert into table_name 
         * values(array[][].var,...).
         */
        Oid base_type_oid = get_element_type(array_data->datatype->typoid);
        if (!OidIsValid(base_type_oid)) {
           yyerror("invalid array's element type for insert.");
        }

        HeapTuple type_tuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(base_type_oid));     
        if (!HeapTupleIsValid(type_tuple)) {
            yyerror("invalid array's element type tuple for insert.");
        }

        Form_pg_type type_pg_type = (Form_pg_type)GETSTRUCT(type_tuple);
        int rel_oid = type_pg_type->typrelid;
        ReleaseSysCache(type_tuple);
        HeapTuple rel_tuple = SearchSysCache1(RELOID, ObjectIdGetDatum(rel_oid));
        if (!HeapTupleIsValid(rel_tuple)) {
            yyerror("invalid type's rel tuple for insert.", true);
        } else {

            plpgsql_append_source_text(&ds, location, values_end_loc);
            appendStringInfoString(&ds, "(");
            int replace_start_poc = ds.len;
            Form_pg_class rel_pg_class = (Form_pg_class) GETSTRUCT(rel_tuple);
            int att_num = rel_pg_class->relnatts;
            ReleaseSysCache(rel_tuple);

            for (int i = 1; i <= att_num; i++) {
                HeapTuple att_tuple = SearchSysCache2(ATTNUM, ObjectIdGetDatum(rel_oid), Int16GetDatum(i));
                if (!HeapTupleIsValid(att_tuple)) {
                    yyerror("invalid element att tuple for insert.");
                }

                Form_pg_attribute att_pg_attribute = (Form_pg_attribute)GETSTRUCT(att_tuple);
                plpgsql_append_source_text(&ds, values_end_loc, before_semi_loc);
                appendStringInfo(&ds, ".%s", att_pg_attribute->attname.data);
                if (i != att_num) {
                    appendStringInfoString(&ds, ",");
                }

                ReleaseSysCache(att_tuple);
            }
            int replace_end_poc = ds.len - 1;
            appendStringInfoString(&ds, ")");

            int bracket_level = 0;
            /* replace table/array's '('/')' to '['/']' SQL supported */ 
            for (int i = replace_start_poc; i <= replace_end_poc; i++) {
                if (ds.data[i] == '(') {
                    bracket_level++;
                    if (bracket_level == 1) {
                        ds.data[i] = '[';
                    }
                } else if (ds.data[i] == ')') {
                    if (bracket_level == 1) {
                        ds.data[i] = ']';
                    }
                    bracket_level--;
                }
            }
        }
    } else if (label_begin)
    {
        appendStringInfoString(&ds, "\n");
    } else {
        plpgsql_append_source_text(&ds, location, yylloc);

        if (parenlevel != 0 ||
            list_length(list_bracket) != 0 ||
            list_length(list_bracket_loc) % 2)
        {
            yyerror("mismatched parentheses");
        }
        plpgsql_process_stmt_array(&ds, list_bracket_loc);
        list_free_ext(list_bracket_loc);
    }

    /* trim any trailing whitespace, for neatness */
    while (ds.len > 0 && scanner_isspace(ds.data[ds.len - 1]))
        ds.data[--ds.len] = '\0';

    expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns			= plpgsql_ns_top();
    expr->idx			= (uint32)-1;
    expr->out_param_dno = -1;
    pfree_ext(ds.data);

    check_sql_expr(expr->query, location, 0);
#ifndef ENABLE_MULTIPLE_NODES
    if (firsttoken == K_SELECT && u_sess->attr.attr_sql.sql_compatibility == A_FORMAT && ALLOW_PROCEDURE_COMPILE_CHECK) {
        (void)getCursorTupleDesc(expr, false, true);
    }
#endif
    execsql = (PLpgSQL_stmt_execsql *)palloc(sizeof(PLpgSQL_stmt_execsql));
    execsql->cmd_type = PLPGSQL_STMT_EXECSQL;
    execsql->lineno  = plpgsql_location_to_lineno(location);
    execsql->sqlstmt = expr;
    execsql->into	 = have_into;
    execsql->bulk_collect = have_bulk_collect;
    execsql->strict	 = have_strict;
    execsql->rec	 = rec;
    execsql->row	 = row;
    execsql->placeholders = placeholders;
    execsql->object_rel_value = false;
    execsql->sqlString = plpgsql_get_curline_query();

    return (PLpgSQL_stmt *) execsql;
}


/*
 * Read FETCH or MOVE direction clause (everything through FROM/IN).
 */
static PLpgSQL_stmt_fetch *
read_fetch_direction(void)
{
    PLpgSQL_stmt_fetch *fetch;
    int			tok;
    bool		check_FROM = true;

    /*
     * We create the PLpgSQL_stmt_fetch struct here, but only fill in
     * the fields arising from the optional direction clause
     */
    fetch = (PLpgSQL_stmt_fetch *) palloc0(sizeof(PLpgSQL_stmt_fetch));
    fetch->cmd_type = PLPGSQL_STMT_FETCH;
    /* set direction defaults: */
    fetch->direction = FETCH_FORWARD;
    fetch->how_many  = 1;
    fetch->expr		 = NULL;
    fetch->returns_multiple_rows = false;
    fetch->bulk_collect = false;
    fetch->has_direction = true;
    fetch->sqlString = plpgsql_get_curline_query();

    tok = yylex();
    if (tok == 0)
        yyerror("unexpected end of function definition");

    if (tok_is_keyword(tok, &yylval,
                       K_NEXT, "next"))
    {
        /* use defaults */
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_PRIOR, "prior"))
    {
        fetch->direction = FETCH_BACKWARD;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_FIRST, "first"))
    {
        fetch->direction = FETCH_ABSOLUTE;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_LAST, "last"))
    {
        fetch->direction = FETCH_ABSOLUTE;
        fetch->how_many  = -1;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_ABSOLUTE, "absolute"))
    {
        fetch->direction = FETCH_ABSOLUTE;
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        check_FROM = false;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_RELATIVE, "relative"))
    {
        fetch->direction = FETCH_RELATIVE;
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        check_FROM = false;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_ALL, "all"))
    {
        fetch->how_many = FETCH_ALL;
        fetch->returns_multiple_rows = true;
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_FORWARD, "forward"))
    {
        complete_direction(fetch, &check_FROM);
    }
    else if (tok_is_keyword(tok, &yylval,
                            K_BACKWARD, "backward"))
    {
        fetch->direction = FETCH_BACKWARD;
        complete_direction(fetch, &check_FROM);
    }
    else if (tok == K_FROM || tok == K_IN)
    {
        /* empty direction */
        check_FROM = false;
    }
    else if (tok == T_DATUM || tok == T_PACKAGE_VARIABLE)
    {
        /* Assume there's no direction clause and tok is a cursor name */
        pltsql_push_back_token(tok);
        fetch->has_direction = false;
        check_FROM = false;
    }
    else
    {
        /*
         * Assume it's a count expression with no preceding keyword.
         * Note: we allow this syntax because core SQL does, but we don't
         * document it because of the ambiguity with the omitted-direction
         * case.  For instance, "MOVE n IN c" will fail if n is a variable.
         * Perhaps this can be improved someday, but it's hardly worth a
         * lot of work.
         */
        pltsql_push_back_token(tok);
        fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                           "FROM or IN",
                                           NULL);
        fetch->returns_multiple_rows = true;
        check_FROM = false;
    }

    /* check FROM or IN keyword after direction's specification */
    if (check_FROM)
    {
        tok = yylex();
        if (tok != K_FROM && tok != K_IN)
            yyerror("expected FROM or IN");
    }

    return fetch;
}

/*
 * Process remainder of FETCH/MOVE direction after FORWARD or BACKWARD.
 * Allows these cases:
 *   FORWARD expr,  FORWARD ALL,  FORWARD
 *   BACKWARD expr, BACKWARD ALL, BACKWARD
 */
static void
complete_direction(PLpgSQL_stmt_fetch *fetch,  bool *check_FROM)
{
    int			tok;

    tok = yylex();
    if (tok == 0)
        yyerror("unexpected end of function definition");

    if (tok == K_FROM || tok == K_IN)
    {
        *check_FROM = false;
        return;
    }

    if (tok == K_ALL)
    {
        fetch->how_many = FETCH_ALL;
        fetch->returns_multiple_rows = true;
        *check_FROM = true;
        return;
    }

    pltsql_push_back_token(tok);
    fetch->expr = read_sql_expression2(K_FROM, K_IN,
                                       "FROM or IN",
                                       NULL);
    fetch->returns_multiple_rows = true;
    *check_FROM = false;
}


static PLpgSQL_stmt *
make_return_stmt(int location)
{
    PLpgSQL_stmt_return *newp;
    int token = -1;

    newp = (PLpgSQL_stmt_return *)palloc0(sizeof(PLpgSQL_stmt_return));
    newp->cmd_type = PLPGSQL_STMT_RETURN;
    newp->lineno   = plpgsql_location_to_lineno(location);
    newp->expr	  = NULL;
    newp->retvarno = -1;
    newp->sqlString = plpgsql_get_curline_query();

    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_retset)
    {
        if (yylex() != ';') {
            const char* message = "RETURN cannot have a parameter in function returning set";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("RETURN cannot have a parameter in function returning set"),
                     errhint("Use RETURN NEXT or RETURN QUERY."),
                     parser_errposition(yylloc)));
        }
    }
    else if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_pipelined) {
	if (yylex() != ';') {
	    const char* message = "pipe error";
	    InsertErrorMessage(message, plpgsql_yylloc);
	    ereport(ERROR, (errmodule(MOD_PLSQL), errcode(ERRCODE_SYNTAX_ERROR),
                                errmsg("RETURN statement in a pipelined function cannot contains an expression"), errdetail("%s", message),
                                errcause("A RETURN statement in a pipelined function contains an expression, \
                                which is not allowed. \
                                Pipelined functions must return values to the caller by using the PIPE statement."),
                                erraction("Remove the expression from the RETURN statement \
                                and use a PIPE statement to return values. \
                                Alternatively, convert the function into a non-pipelined function")));
	}
    }

    // adapting A db, where return value is independent from OUT parameters 
    else if (';' == (token = yylex()) && u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->out_param_varno >= 0)
        newp->retvarno = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->out_param_varno;
    else
    {
        pltsql_push_back_token(token);
        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_rettype == VOIDOID)
        {
            if (yylex() != ';') {
                const char* message = "RETURN cannot have a parameter in function returning void";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                         errmsg("RETURN cannot have a parameter in function returning void"),
                         parser_errposition(yylloc)));
            }
        }
        else if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_retistuple)
        {
            int     tok = yylex();
            if(tok < 0)
            {
                const char* message = "token value is smaller than 0 ";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("token value %d is smaller than 0 ", tok)));
                return NULL;
            }

            switch (tok)
            {
                case K_NULL:
                    /* we allow this to support RETURN NULL in triggers */
                    break;

                case T_DATUM:
                    if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                        yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC ||
                        yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_RECORD||
                        yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_CURSORROW)
                        newp->retvarno = yylval.wdatum.dno;
                    else {
                        const char* message = "RETURN must specify a record or row variable in function returning row";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                                (errcode(ERRCODE_DATATYPE_MISMATCH),
                                 errmsg("RETURN must specify a record or row variable in function returning row"),
                                 parser_errposition(yylloc)));
                    }
                    break;

                default:
                    const char* message = "RETURN must specify a record or row variable in function returning row";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_DATATYPE_MISMATCH),
                             errmsg("RETURN must specify a record or row variable in function returning row"),
                             parser_errposition(yylloc)));
                    break;
            }
            if (yylex() != ';')
            yyerror("syntax error");
        }
        else
        {
            /*
             * Note that a well-formed expression is _required_ here;
             * anything else is a compile-time error.
             */
            if (u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
                newp->retvarno = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->out_param_varno;
            }
            newp->expr = read_sql_expression(';', ";");
            if (newp->expr->is_have_tableof_index_var) {
                yyerror("table of index type is not supported as function return type.");
            }
        }
    }

    return (PLpgSQL_stmt *) newp;
}


static PLpgSQL_stmt *
make_return_next_stmt(int location)
{
    PLpgSQL_stmt_return_next *newp;

    if (!u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_retset) {
        const char* message = "cannot use RETURN NEXT in a non-SETOF function";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("cannot use RETURN NEXT in a non-SETOF function"),
                 parser_errposition(location)));
    }

    newp = (PLpgSQL_stmt_return_next *)palloc0(sizeof(PLpgSQL_stmt_return_next));
    newp->cmd_type	= PLPGSQL_STMT_RETURN_NEXT;
    newp->lineno		= plpgsql_location_to_lineno(location);
    newp->expr		= NULL;
    newp->retvarno	= -1;
    newp->sqlString = plpgsql_get_curline_query();

    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->out_param_varno >= 0)
    {
        if (yylex() != ';') {
            const char* message = "RETURN NEXT cannot have a parameter in function with OUT parameters";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_DATATYPE_MISMATCH),
                     errmsg("RETURN NEXT cannot have a parameter in function with OUT parameters"),
                     parser_errposition(yylloc)));
        }
        newp->retvarno = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->out_param_varno;
    }
    else if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_retistuple)
    {
        switch (yylex())
        {
            case T_DATUM:
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_RECORD ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC)
                    newp->retvarno = yylval.wdatum.dno;
                else {
                    const char* message = "RETURN NEXT must specify a record or row variable in function returning row";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_DATATYPE_MISMATCH),
                             errmsg("RETURN NEXT must specify a record or row variable in function returning row"),
                             parser_errposition(yylloc)));
                }
                break;

            default:
                const char* message = "RETURN NEXT must specify a record or row variable in function returning row";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_DATATYPE_MISMATCH),
                         errmsg("RETURN NEXT must specify a record or row variable in function returning row"),
                         parser_errposition(yylloc)));
                break;
        }
        if (yylex() != ';')
            yyerror("syntax error");
    }
    else
        newp->expr = read_sql_expression(';', ";");

    return (PLpgSQL_stmt *) newp;
}


static PLpgSQL_stmt *
make_return_query_stmt(int location)
{
    PLpgSQL_stmt_return_query *newp;
    int			tok;

    if (!u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_retset) {
        const char* message = "cannot use RETURN QUERY in a non-SETOF function";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_DATATYPE_MISMATCH),
                 errmsg("cannot use RETURN QUERY in a non-SETOF function"),
                 parser_errposition(location)));
    }

    newp = (PLpgSQL_stmt_return_query *)palloc0(sizeof(PLpgSQL_stmt_return_query));
    newp->cmd_type = PLPGSQL_STMT_RETURN_QUERY;
    newp->lineno = plpgsql_location_to_lineno(location);
    newp->sqlString = plpgsql_get_curline_query();

    /* check for RETURN QUERY EXECUTE */
    if ((tok = yylex()) != K_EXECUTE)
    {
        /* ordinary static query */
        pltsql_push_back_token(tok);
        newp->query = read_sql_stmt("");
    }
    else
    {
        /* dynamic SQL */
        int		term;

        newp->dynquery = read_sql_expression2(';', K_USING, "; or USING",
                                             &term);
        if (term == K_USING)
        {
            do
            {
                PLpgSQL_expr *expr;

                expr = read_sql_expression2(',', ';', ", or ;", &term);
                newp->params = lappend(newp->params, expr);
            } while (term == ',');
        }
    }

    return (PLpgSQL_stmt *) newp;
}


/* convenience routine to fetch the name of a T_DATUM */
static char *
NameOfDatum(PLwdatum *wdatum)
{
    if (wdatum->ident)
        return wdatum->ident;
    AssertEreport(wdatum->idents != NIL,
                        MOD_PLSQL,
                        "It should not be null");
    return NameListToString(wdatum->idents);
}

/* convenience routine to copy the name of a T_DATUM */
static char *
CopyNameOfDatum(PLwdatum *wdatum)
{
    if (wdatum->ident)
        return pstrdup(wdatum->ident);
    AssertEreport(wdatum->idents != NIL,
                        MOD_PLSQL,
                        "It should not be null");
    return NameListToString(wdatum->idents);
}

static void check_record_nest_tableof_index(PLpgSQL_datum* datum)
{
    if (datum->dtype == PLPGSQL_DTYPE_RECORD || datum->dtype == PLPGSQL_DTYPE_ROW) {
        PLpgSQL_row* row = (PLpgSQL_row*)datum;
        for (int i = 0; i < row->nfields; i++) {
            PLpgSQL_datum* row_element = NULL; 

            /* check wether attisdropped */
            if (row->varnos[i] == -1 && row->fieldnames[i] == NULL) {
                continue;
            }

            if (row->ispkg) {
                row_element = (PLpgSQL_datum*)(row->pkg->datums[row->varnos[i]]);
            } else {
                row_element = (PLpgSQL_datum*)(u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[row->varnos[i]]);
            }
            /* Notice: do not deal record nest record nest table of index, because table of index type can not get */
            if (row_element->dtype == PLPGSQL_DTYPE_VAR) {
                PLpgSQL_var* var_element = (PLpgSQL_var*)row_element;
                if (OidIsValid(var_element->datatype->tableOfIndexType)) {
                    yyerror("record nested table of index variable do not support entire assign");
                }
            } 
        }
    }
}

static void
check_assignable(PLpgSQL_datum *datum, int location)
{
    PLpgSQL_var *var = NULL;
    switch (datum->dtype)
    {
        case PLPGSQL_DTYPE_VAR:
            if (((PLpgSQL_var *) datum)->isconst) {
                const char* message = "variable is declared CONSTANT";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_ERROR_IN_ASSIGNMENT),
                         errmsg("\"%s\" is declared CONSTANT",
                                ((PLpgSQL_var *) datum)->refname),
                         parser_errposition(location)));
            }
            break;
        case PLPGSQL_DTYPE_ROW:
            /* always assignable? */
            break;
        case PLPGSQL_DTYPE_REC:
            /* always assignable?  What about NEW/OLD? */
            break;
        case PLPGSQL_DTYPE_RECORD:
            break;
        case PLPGSQL_DTYPE_RECFIELD:
            /* always assignable? */
            break;
        case PLPGSQL_DTYPE_ARRAYELEM:
            var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[((PLpgSQL_arrayelem *)datum)->arrayparentno];
            if (var->isconst) {
                const char* message = "array is declared CONSTANT";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate, (errcode(ERRCODE_ERROR_IN_ASSIGNMENT),
                                errmsg("\"%s\" is declared CONSTANT", var->refname),
                                parser_errposition(location)));
                u_sess->plsql_cxt.have_error = true;
            }
            break;
        case PLPGSQL_DTYPE_TABLEELEM:
            var = (PLpgSQL_var *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[((PLpgSQL_tableelem *)datum)->tableparentno];
            if (var->isconst) {
                const char* message = "table is declared CONSTANT";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate, (errcode(ERRCODE_ERROR_IN_ASSIGNMENT),
                                   errmsg("\"%s\" is declared CONSTANT", var->refname),
                                   parser_errposition(location)));
                u_sess->plsql_cxt.have_error = true;
            }
            break;
        case PLPGSQL_DTYPE_UNKNOWN:
        case PLPGSQL_DTYPE_CURSORROW:
            /* package variable? */
            break;
        default:
            elog(errstate, "unrecognized dtype: %d", datum->dtype);
            break;
    }
}

static Oid get_table_index_type(PLpgSQL_datum* datum, int *tableof_func_dno)
{
    PLpgSQL_var* var = (PLpgSQL_var*)datum;
    if (OidIsValid(var->datatype->tableOfIndexType) && tableof_func_dno != NULL) {
        if (*tableof_func_dno == -1) {
            *tableof_func_dno = var->dno;
        } else if (*tableof_func_dno != var->dno) {
            yyerror("do not support more than 2 table of index by variables call functions in an expr");
        }
    }
   
    return var->datatype->tableOfIndexType;
}

static void check_bulk_into_type(PLpgSQL_row* row)
{
    if (row->nfields == 0) {
        char errormsg[128] = {0};
        errno_t rc = EOK;
        rc = snprintf_s(errormsg, sizeof(errormsg), sizeof(errormsg) - 1,
            "bulk collect into target can't be null.");
        securec_check_ss(rc, "", "");
        yyerror(errormsg);
    }

    for (int i = 0; i < row->nfields; i++) {
        PLpgSQL_var* var = (PLpgSQL_var*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[row->varnos[i]];
        if (var->datatype != NULL && var->datatype->collectionType == PLPGSQL_COLLECTION_TABLE && var->datatype->tableOfIndexType == VARCHAROID) {
            char errormsg[128] = {0};
            errno_t rc = EOK;
            rc = snprintf_s(errormsg, sizeof(errormsg), sizeof(errormsg) - 1,
                "index by varchar type %s don't support bulk collect into.", var->refname);
            securec_check_ss(rc, "", "");
            yyerror(errormsg);
        }
    }
}

static void check_tableofindex_args(int tableof_var_dno, Oid argtype)
{
    if (tableof_var_dno < 0 || u_sess->plsql_cxt.curr_compile_context == NULL) {
        return ;
    }
    PLpgSQL_datum* tableof_var_datum = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[tableof_var_dno];
    if (tableof_var_datum == NULL) {
        return ;
    } else if (tableof_var_datum->dtype == PLPGSQL_DTYPE_VAR) {
        PLpgSQL_var* var = (PLpgSQL_var*)tableof_var_datum;
        Oid base_oid = InvalidOid;
        Oid indexby_oid = InvalidOid;
        
        if (isTableofType(argtype, &base_oid, &indexby_oid)) {
            if (var->datatype->tableOfIndexType != indexby_oid ||
                var->datatype->typoid != base_oid) {
                yyerror("procedure table of arg types not match");
            }
        }
    } else if (tableof_var_datum->dtype == PLPGSQL_DTYPE_RECORD) {
        check_record_nest_tableof_index(tableof_var_datum);
    }  
}

static void check_table_index(PLpgSQL_datum* datum, char* funcName)
{
    PLpgSQL_var* var = (PLpgSQL_var*)datum;
    if ((var->datatype->tableOfIndexType == VARCHAROID || var->datatype->tableOfIndexType == INT4OID) &&
        var->nest_table == NULL) {
        char errormsg[128] = {0};
        errno_t rc = EOK;
        rc = snprintf_s(errormsg, sizeof(errormsg), sizeof(errormsg) - 1,
            "index by type don't support %s function", funcName);
        securec_check_ss(rc, "", "");
        yyerror(errormsg);
    }
}
/*
 * check if the table type has index by,
 * return real type
 */
static Oid get_table_type(PLpgSQL_datum* datum)
{
    PLpgSQL_var* var = (PLpgSQL_var*)datum;
    if (OidIsValid(var->datatype->tableOfIndexType)) {
        yyerror("multiset don't support index by table of type.");
    }
    Oid typeOid = var->datatype->typoid;
    if (!OidIsValid(typeOid)) {
        ereport(errstate,
            (errcode(ERRCODE_UNDEFINED_OBJECT),
            errmsg("type %s type Oid is invalid.", var->datatype->typname)));
        u_sess->plsql_cxt.have_error = true;
    }
    return typeOid;
}

/* get the attrno of attribute when assign value to arrary element attibute. */
static AttrNumber get_assign_attrno(PLpgSQL_datum* target,  char* attrname)
{
    int nsubscripts = 0;
    Oid parenttypoid;
    int32 arraytypmod;
    Oid arraytypoid;
    Oid elemtypoid;
    AttrNumber attrno = -1;
    PLpgSQL_arrayelem* arrayelem = NULL;
    PLpgSQL_tableelem* tableelem = NULL;

    /*
     * To handle constructs like x[1][2] := something, we have to
     * be prepared to deal with a chain of arrayelem datums. Chase
     * back to find the base array datum.
     */
    if (target->dtype == PLPGSQL_DTYPE_ARRAYELEM) {
        do {
            arrayelem = (PLpgSQL_arrayelem*)target;
            if (nsubscripts >= MAXDIM) {
                const char* message = "number of array dimensions exceeds the maximum allowed in assignment.";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                        errmodule(MOD_PLSQL),
                        errmsg("number of array dimensions exceeds the maximum allowed in assignment."),
                        errdetail("number of array dimensions (%d) exceeds the maximum allowed (%d) in assignment. ",
                            nsubscripts + 1, MAXDIM),
                        errcause("too large array dimensions"),
                        erraction("reduce the array dimensions")));
                u_sess->plsql_cxt.have_error = true;
            }
            nsubscripts++;
            target = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[arrayelem->arrayparentno];
        } while (target->dtype == PLPGSQL_DTYPE_ARRAYELEM);
    } else {
        do {
            tableelem = (PLpgSQL_tableelem*)target;
            if (nsubscripts >= MAXDIM) {
                const char* message = "number of array dimensions exceeds the maximum allowed in assignment.";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                        errmodule(MOD_PLSQL),
                        errmsg("number of array dimensions exceeds the maximum allowed in assignment."),
                        errdetail("number of array dimensions (%d) exceeds the maximum allowed (%d) in assignment. ",
                            nsubscripts + 1, MAXDIM),
                        errcause("too large array dimensions"),
                        erraction("reduce the array dimensions")));
                u_sess->plsql_cxt.have_error = true;
            }
            nsubscripts++;
            target = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[tableelem->tableparentno];
        } while (target->dtype == PLPGSQL_DTYPE_TABLEELEM);
    }
    /* array element type must be PLPGSQL_DTYPE_VAR, otherwise user referencing array incorrectly. */
    if (target->dtype != PLPGSQL_DTYPE_VAR) {
        const char* message = "subscripted object in assignment is not an array";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
            (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmodule(MOD_PLSQL),
                errmsg("subscripted object in assignment is not an array"),
                errdetail("subscripted variable in assignment is not an array"),
                errcause("incorrectly referencing variables"),
                erraction("modify assign variable")));
        u_sess->plsql_cxt.have_error = true;
    }

	/* get the array element typoid */
    PLpgSQL_var* var = (PLpgSQL_var*)target;
    parenttypoid = var->datatype->typoid;
    arraytypmod = var->datatype->atttypmod;
    arraytypoid = getBaseTypeAndTypmod(parenttypoid, &arraytypmod);
    elemtypoid = get_element_type(arraytypoid);
    if (!OidIsValid(elemtypoid)) {
        const char* message = "subscripted object in assignment is not an array";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
            (errcode(ERRCODE_DATATYPE_MISMATCH),
                errmodule(MOD_PLSQL),
                errmsg("subscripted object in assignment is not an array"),
                errdetail("subscripted variable \"%s\" in assignment is not an array", var->refname),
                errcause("incorrectly referencing variables"),
                erraction("modify assign variable")));
        u_sess->plsql_cxt.have_error = true;
    }

	/* get tupledesc by typoid */
    TupleDesc elemtupledesc =
        lookup_rowtype_tupdesc_noerror(elemtypoid, arraytypmod,true);
    if (elemtupledesc == NULL){
        const char* message = "array element type is not composite in assignment";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(ERROR,
            (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
                errmodule(MOD_PLSQL),
                errmsg("array element type is not composite in assignment"),
                errdetail("array variable \"%s\" must be composite when assign value to attibute", var->refname),
                errcause("incorrectly referencing variables"),
                erraction("modify assign variable")));
    }

	/* search the matched attribute */
    for (int i = 0; i < elemtupledesc->natts; i++) {
        if (namestrcmp(&(elemtupledesc->attrs[i].attname), attrname) == 0) {
            attrno = i;
            break;
        }
    }
    ReleaseTupleDesc(elemtupledesc);

	/* attrno = -1 means there is no matched attribute */
    if (attrno == -1) {
        const char* message = "attribute does not exists";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
        (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
            errmodule(MOD_PLSQL),
            errmsg("attribute \"%s\" does not exist", attrname),
            errdetail("attribute \"%s\" does not exist in array variable \"%s\"", attrname, var->refname),
            errcause("incorrectly referencing variables"),
            erraction("modify assign variable")));
        u_sess->plsql_cxt.have_error = true;
	}

    return attrno;
}

/*
 * Brief		: support array variable as select into and using out target. 
 * Description	: if an array element is detected, add it to the u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[]
 *				  and return true, else return false.  
 * Notes		: 
 */ 
static bool 
read_into_using_add_arrayelem(char **fieldnames, int *varnos, int *nfields, int tmpdno, int *tok)
{
    PLpgSQL_arrayelem *newp = NULL;
    char           tokExpected[2];
    int            toktmp = 0;

    /* validation of arguments */
    if ((NULL == fieldnames) || (NULL == varnos) || (NULL == nfields) || (NULL == tok))
    {
        const char* message = "pointer is null in read_into_add_arrayelem function!";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                 errmsg(" pointer is null in read_into_add_arrayelem function! ")));
        return false;
    }

    if (('[' == *tok) || ('(' == *tok))
    {
        if ('[' == *tok)
            toktmp = ']';
        else
            toktmp = ')';

        tokExpected[0] = (char)toktmp;
        tokExpected[1] = '\0';

        newp = (PLpgSQL_arrayelem *)palloc0(sizeof(PLpgSQL_arrayelem));
        newp->arrayparentno = tmpdno;
        newp->dtype      = PLPGSQL_DTYPE_ARRAYELEM;
        /* get the array index expression */
        newp->subscript = read_sql_expression(toktmp, &tokExpected[0]);
        
        if(NULL == newp->subscript) {
            const char* message = " error near arrary name!";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                  errmsg(" error near arrary name! ")));
            u_sess->plsql_cxt.have_error = true;
        }
        newp->assignattrno = -1;

        *tok = yylex();
        if (*tok == '.'){
            
            newp->assignattrno = get_assign_attrno((PLpgSQL_datum *)newp, get_attrname(*tok));
            *tok = yylex();
            if(*tok == '.') {
                yyerror("assign value to deep level attribute is not supported in SELECT/FETCH INTO method");
            }
        }
        plpgsql_adddatum((PLpgSQL_datum *)newp);

        fieldnames[*nfields]   = pstrdup("arrayelem");
        varnos[(*nfields)++]   = newp->dno;

        /* is an array element, return true */
        return true;
    }

    return false;
}

/*
 * Brief		: support array variable as select into and using out target. 
 * Description	: if an array element is detected, add it to the u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[]
 *				  and return true, else return false.  
 * Notes		: 
 */ 
static bool 
read_into_using_add_tableelem(char **fieldnames, int *varnos, int *nfields, int tmpdno, int *tok)
{
    PLpgSQL_tableelem *newp = NULL;
    char           tokExpected[2];
    int            toktmp = 0;

    /* validation of arguments */
    if ((NULL == fieldnames) || (NULL == varnos) || (NULL == nfields) || (NULL == tok))
    {
        const char* message = " pointer is null in read_into_add_arrayelem function!";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNEXPECTED_NULL_VALUE),
                 errmsg(" pointer is null in read_into_add_arrayelem function! ")));
        u_sess->plsql_cxt.have_error = true;
        return false;
    }

    if (('[' == *tok) || ('(' == *tok))
    {
        if ('[' == *tok)
            toktmp = ']';
        else
            toktmp = ')';

        tokExpected[0] = (char)toktmp;
        tokExpected[1] = '\0';

        newp = (PLpgSQL_tableelem *)palloc0(sizeof(PLpgSQL_tableelem));
        newp->tableparentno = tmpdno;
        newp->dtype      = PLPGSQL_DTYPE_TABLEELEM;
        /* get the array index expression */
        newp->subscript = read_sql_expression(toktmp, &tokExpected[0]);
        
        if(NULL == newp->subscript) {
            const char* message = " error near arrary name!";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                  errmsg(" error near arrary name! ")));
            u_sess->plsql_cxt.have_error = true;
        }
        newp->assignattrno = -1;

        *tok = yylex();
        if (*tok == '.'){
            *tok = yylex();
            newp->assignattrno = get_assign_attrno((PLpgSQL_datum *)newp, get_attrname(*tok));
            *tok = yylex();
        }
        plpgsql_adddatum((PLpgSQL_datum *)newp);

        fieldnames[*nfields]   = pstrdup("arrayelem");
        varnos[(*nfields)++]   = newp->dno;

        /* is an array element, return true */
        return true;
    }

    return false;
}

/*
 * Read the argument of an INTO clause.  On entry, we have just read the
 * INTO keyword. If it is into_user_defined_variable_list_clause return true.
 */
static bool
read_into_target(PLpgSQL_rec **rec, PLpgSQL_row **row, bool *strict, int firsttoken, bool bulk_collect)
{
    int			tok;

    /* Set default results */
    *rec = NULL;
    *row = NULL;
    if (strict) {
        if (DB_IS_CMPT(PG_FORMAT | B_FORMAT) && firsttoken == K_SELECT && SELECT_INTO_RETURN_NULL) {
            *strict = false;
        } else {
            *strict = true;
        }
    }
#ifdef ENABLE_MULTIPLE_NODES
    if (strict)
        *strict = true;
#endif
    tok = yylex();
    if (tok == '@' || tok == SET_USER_IDENT) {
        return true;
    }
    if (strict && tok == K_STRICT)
    {
        *strict = true;
        tok = yylex();
    } else if (strict && bulk_collect) {
        /* bulk into target can be assigned null */
        *strict = false;
    }

    /*
     * Currently, a row or record variable can be the single INTO target,
     * but not a member of a multi-target list.  So we throw error if there
     * is a comma after it, because that probably means the user tried to
     * write a multi-target list.  If this ever gets generalized, we should
     * probably refactor read_into_scalar_list so it handles all cases.
     */
    switch (tok)
    {
        case T_DATUM:
        case T_VARRAY_VAR:
        case T_TABLE_VAR:
        case T_PACKAGE_VARIABLE:
            if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW
            || yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_RECORD)
            {
                check_assignable(yylval.wdatum.datum, yylloc);
                *row = (PLpgSQL_row *) yylval.wdatum.datum;
                tok = yylex();
                if (tok == ',') {
                    const char* message = "record or row variable cannot be part of multiple-item INTO list";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("record or row variable cannot be part of multiple-item INTO list"),
                             parser_errposition(yylloc)));
                }
                if (tok == '.') {
                    const char* message = "Improper use of '.*'. The '.*' operator cannot be used with a row type variable.";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("Improper use of '.*'. The '.*' operator cannot be used with a row type variable."),
                             parser_errposition(yylloc)));
                }
                if (!DB_IS_CMPT(PG_FORMAT) && (tok == T_DATUM || tok == T_VARRAY_VAR
                    || tok == T_TABLE_VAR || tok == T_PACKAGE_VARIABLE)) {
                    const char* message = "syntax error, expected \",\"";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("syntax error, expected \",\""),
                             parser_errposition(yylloc)));
                }
                pltsql_push_back_token(tok);
            }
            else if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC||
                     yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_CURSORROW)
            {
                check_assignable(yylval.wdatum.datum, yylloc);
                *rec = (PLpgSQL_rec *) yylval.wdatum.datum;
                tok = yylex();
                if (tok == ',') {
                    const char* message = "record or row variable cannot be part of multiple-item INTO list";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("record or row variable cannot be part of multiple-item INTO list"),
                             parser_errposition(yylloc)));
                }
                if (!DB_IS_CMPT(PG_FORMAT) && (tok == T_DATUM || tok == T_VARRAY_VAR
                    || tok == T_TABLE_VAR || tok == T_PACKAGE_VARIABLE)) {
                    const char* message = "syntax error, expected \",\"";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("syntax error, expected \",\""),
                             parser_errposition(yylloc)));
                }
                pltsql_push_back_token(tok);
            }
            else
            {
                *row = read_into_array_table_scalar_list(NameOfDatum(&(yylval.wdatum)),
                                             yylval.wdatum.datum, yylval.wdatum.dno, yylloc, bulk_collect);
            }
            break;
        case  T_PLACEHOLDER:
                *row = read_into_placeholder_scalar_list(yylval.word.ident, yylloc);
            break;
        default:
            /* just to give a better message than "syntax error" */
            current_token_is_not_variable(tok);
            break;
    }
    return false;
}

/*
 *  Read the argument of an USING IN / OUT clause. 
 */
static void
read_using_target(List  **in_expr, PLpgSQL_row **out_row)
{
    int				tok		= 0;
    int				out_nfields=   0;
    int				out_varnos[1024] = {0};
    char			*out_fieldnames[1024] = {0};
    bool          	isin	= false;

    *in_expr = NULL;
    *out_row = NULL;

    do 
    {
        tok=yylex();
        if(K_IN == tok)
        {
            tok = yylex();
            isin = true; 
        }

        if(K_OUT == tok)
        {
            char * tempvar = NULL;
            int    tempdno = 0;
            PLpgSQL_expr * tempexpr = NULL;

            tok = yylex();

            switch(tok)
            {
                case T_DATUM:
                    tempvar = pstrdup(NameOfDatum(&(yylval.wdatum)));
                    tempdno = yylval.wdatum.dno;
                    pltsql_push_back_token(tok);
                    tempexpr  =read_sql_construct(',',';',',',", or ;","SELECT ",true,true,true,NULL,&tok);
                    tempexpr->isouttype = true;
                    *in_expr=lappend((*in_expr), tempexpr);

                    if (!read_into_using_add_arrayelem(out_fieldnames, out_varnos, &out_nfields, tempdno, &tok)) 
                    {
                        out_fieldnames[out_nfields] = tempvar;
                        out_varnos[out_nfields++]	= tempdno;
                    }
                    else
                    {
                        if (isin) {
                            const char* message = " using can't support array parameter with in out ";
                            InsertErrorMessage(message, plpgsql_yylloc);
                            ereport(errstate,
                                (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg(" using can't support array parameter with in out !")));
                        }

                    }

                    break;

                default:
                    const char* message = "not all the parameters are scalar variables.";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                                    errmsg("not all the parameters are scalar variables.")));
            }
        }
        else
        {
            PLpgSQL_expr * expr = NULL;
            pltsql_push_back_token(tok);
            expr  = read_sql_construct(',',';',',',", or ;","SELECT ",true,true,true,NULL,&tok);
            *in_expr=lappend((*in_expr), expr);

            isin = false;

        }

    }while(tok== ',');

    /*
    * We read an extra, non-comma token from yylex(), so push it
    * back onto the input stream
    */
    pltsql_push_back_token(tok);

    if(out_nfields)
    {
        (*out_row) = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
        (*out_row)->dtype = PLPGSQL_DTYPE_ROW;
        (*out_row)->refname = pstrdup("*internal*");
        (*out_row)->lineno = plpgsql_location_to_lineno(yylloc);
        (*out_row)->rowtupdesc = NULL;
        (*out_row)->nfields = out_nfields;
        (*out_row)->fieldnames = (char **)palloc(sizeof(char *) * out_nfields);
        (*out_row)->varnos = (int *)palloc(sizeof(int) * out_nfields);
        (*out_row)->isImplicit = true;
        while (--out_nfields >= 0)
        {
            (*out_row)->fieldnames[out_nfields] = out_fieldnames[out_nfields];
            (*out_row)->varnos[out_nfields] = out_varnos[out_nfields];
        }
        plpgsql_adddatum((PLpgSQL_datum *)(*out_row));
    }
}

/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars.
 */
static PLpgSQL_row *
read_into_scalar_list(char *initial_name,
                      PLpgSQL_datum *initial_datum,
                      int initial_dno,
                      int initial_location)
{
    int				 nfields;
    char			*fieldnames[1024] = {NULL};
    int				 varnos[1024] = {0};
    PLpgSQL_row		*row;
    int				 tok;

    check_assignable(initial_datum, initial_location);
    fieldnames[0] = initial_name;
    varnos[0]	  = initial_dno;
    nfields		  = 1;

    while ((tok = yylex()) == ',')
    {
        /* Check for array overflow */
        if (nfields >= 1024) {
            const char* message = "too many INTO variables specified.";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO variables specified"),
                     parser_errposition(yylloc)));
            u_sess->plsql_cxt.have_error = true;
            break;
        }

        tok = yylex();
        switch (tok)
        {
            case T_DATUM:
                check_assignable(yylval.wdatum.datum, yylloc);
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC) {
                    const char* message = "variable is not a scalar variable";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("\"%s\" is not a scalar variable",
                                    NameOfDatum(&(yylval.wdatum))),
                             parser_errposition(yylloc)));
                    u_sess->plsql_cxt.have_error = true;
                    break;
                }
                fieldnames[nfields] = NameOfDatum(&(yylval.wdatum));
                varnos[nfields++]	= yylval.wdatum.dno;
                break;

            default:
                /* just to give a better message than "syntax error" */
                current_token_is_not_variable(tok);
                break;
        }
    }

    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    pltsql_push_back_token(tok);

    row = (PLpgSQL_row *)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = nfields;
    row->fieldnames = (char **)palloc(sizeof(char *) * nfields);
    row->varnos = (int *)palloc(sizeof(int) * nfields);
    row->isImplicit = true;
    while (--nfields >= 0)
    {
        row->fieldnames[nfields] = fieldnames[nfields];
        row->varnos[nfields] = varnos[nfields];
    }

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}

/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars.
 */
static PLpgSQL_row *
read_into_array_table_scalar_list(char *initial_name,
                      PLpgSQL_datum *initial_datum,
                      int initial_dno,
                      int initial_location,
                      bool bulk_collect)
{
    int				 nfields = 0;
    char			*fieldnames[1024] = {NULL};
    int				 varnos[1024] = {0};
    PLpgSQL_row		*row;
    int				 tok;
    int				 toktmp = 0;
    int 			 tmpdno = 0;
    int              type_flag = -1;
    bool             isarrayelem = false;
    char* 			 nextname = NULL;

    check_assignable(initial_datum, initial_location);

    tmpdno = initial_dno;
    tok = yylex();
    get_datum_tok_type(initial_datum, &type_flag);

    if (type_flag == PLPGSQL_TOK_TABLE_VAR) {
        isarrayelem = read_into_using_add_tableelem(fieldnames, varnos, &nfields, tmpdno, &tok);
    } else if (type_flag == PLPGSQL_TOK_VARRAY_VAR) {
        isarrayelem = read_into_using_add_arrayelem(fieldnames, varnos, &nfields, tmpdno, &tok);
    } else {
        isarrayelem = false;
    }
    if (!isarrayelem)
    {
        fieldnames[0] = initial_name;
        varnos[0]	  = initial_dno;
        nfields		  = 1;
    }
    while (',' == tok)
    {
        /* Check for array overflow */
        if (nfields >= 1024)
        {
            const char* message = "too many INTO variables specified";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO variables specified"),
                     parser_errposition(yylloc)));
            return NULL;
        }

        toktmp = yylex();
        type_flag = -1;

        switch (toktmp)
        {
            case T_DATUM:
                check_assignable(yylval.wdatum.datum, yylloc);
                
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC) {
                    const char* message = "it not a scalar variable";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("\"%s\" is not a scalar variable",
                                    NameOfDatum(&(yylval.wdatum))),
                            parser_errposition(yylloc)));
                }

                tmpdno = yylval.wdatum.dno;
                nextname = NameOfDatum(&(yylval.wdatum));
                fieldnames[nfields] = nextname;
                varnos[nfields++] = tmpdno;
                tok = yylex();
                break;
            case T_PACKAGE_VARIABLE:
                check_assignable(yylval.wdatum.datum, yylloc);
                
                if (yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_ROW ||
                    yylval.wdatum.datum->dtype == PLPGSQL_DTYPE_REC) {
                    const char* message = "it not a scalar variable";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg("\"%s\" is not a scalar variable",
                                    NameOfDatum(&(yylval.wdatum))),
                            parser_errposition(yylloc)));
                    u_sess->plsql_cxt.have_error = true;
                }

                tmpdno = yylval.wdatum.dno;
                nextname = NameOfDatum(&(yylval.wdatum));
                type_flag = -1;
                get_datum_tok_type(yylval.wdatum.datum, &type_flag);
                if (type_flag == -1) {
                    fieldnames[nfields] = nextname;
                    varnos[nfields++] = tmpdno;
                    tok = yylex();
                    break;
                } else if (type_flag == PLPGSQL_TOK_TABLE_VAR) {
                    tok = yylex();
                    if (tok < -1)
                        return NULL;
                    if (!read_into_using_add_tableelem(fieldnames, varnos, &nfields, tmpdno, &tok))
                    {
                        if (bulk_collect) {
                            fieldnames[nfields] = nextname;
                            varnos[nfields++] = tmpdno;
                            break;
                        }
                        const char* message = "error when read table var subscript";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg(" error when read table var subscript! ")));
                        u_sess->plsql_cxt.have_error = true;
                        return NULL;
                    }		
                    break;
                } else {
                    tok = yylex();
                    if (tok < -1)
                        return NULL;
                    if (!read_into_using_add_arrayelem(fieldnames, varnos, &nfields, tmpdno, &tok))
                    {
                        if (bulk_collect) {
                            fieldnames[nfields] = nextname;
                            varnos[nfields++] = tmpdno;
                            break;
                        }
                        const char* message = "error when read array var subscript";
                        InsertErrorMessage(message, plpgsql_yylloc);
                        ereport(errstate,
                            (errcode(ERRCODE_SYNTAX_ERROR),
                             errmsg(" error when read array var subscript! ")));
                        u_sess->plsql_cxt.have_error = true;
                        return NULL;
                    }		
                    break;
                }
            case T_VARRAY_VAR:
                check_assignable(yylval.wdatum.datum, yylloc);
                tmpdno = yylval.wdatum.dno;
                nextname = NameOfDatum(&(yylval.wdatum));
                tok = yylex();
                if (tok < -1)
                    return NULL;
                if (!read_into_using_add_arrayelem(fieldnames, varnos, &nfields, tmpdno, &tok))
                {
                    if (bulk_collect) {
                        fieldnames[nfields] = nextname;
                        varnos[nfields++] = tmpdno;
                        break;
                    }
                    const char* message = "error near arrary name!";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg(" error near arrary name! ")));
                    return NULL;
                }
                break;
            case T_TABLE_VAR:
                check_assignable(yylval.wdatum.datum, yylloc);
                tmpdno = yylval.wdatum.dno;
                nextname = NameOfDatum(&(yylval.wdatum));
                tok = yylex();
                if (tok < -1)
                    return NULL;
                if (!read_into_using_add_tableelem(fieldnames, varnos, &nfields, tmpdno, &tok))
                {
                    if (bulk_collect) {
                        fieldnames[nfields] = nextname;
                        varnos[nfields++] = tmpdno;
                        break;
                    }
                    const char* message = "error near table name";
                    InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(errstate,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg(" error near table name! ")));
                    u_sess->plsql_cxt.have_error = true;
                    return NULL;
                }		
                break;
            default:
                tok = yylex();
                if (tok < -1)
                    return NULL;

                /* just to give a better message than "syntax error" */
                current_token_is_not_variable(tok);
                break;
        }
    }

    if (!DB_IS_CMPT(PG_FORMAT) && (tok == T_DATUM || tok == T_VARRAY_VAR
                    || tok == T_TABLE_VAR || tok == T_PACKAGE_VARIABLE)) {
        const char* message = "syntax error, expected \",\"";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
            (errcode(ERRCODE_SYNTAX_ERROR),
            errmsg("syntax error, expected \",\""),
            parser_errposition(yylloc)));
    }
    
    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    pltsql_push_back_token(tok);

    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = nfields;
    row->fieldnames = (char**)palloc(sizeof(char *) * nfields);
    row->varnos = (int*)palloc(sizeof(int) * nfields);
    row->isImplicit = true;
    while (--nfields >= 0)
    {
        row->fieldnames[nfields] = fieldnames[nfields];
        row->varnos[nfields] = varnos[nfields];
    }

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}

/*
 * Given the first datum and name in the INTO list, continue to read
 * comma-separated scalar variables until we run out. Then construct
 * and return a fake "row" variable that represents the list of
 * scalars. The function is for placeholders.
 */
static PLpgSQL_row *
read_into_placeholder_scalar_list(char *initial_name,
                      int initial_location)
{
    int				 intoplaceholders;
    PLpgSQL_row		*row;
    int				 tok;

    intoplaceholders		  = 1;

    while ((tok = yylex()) == ',')
    {
        /* Check for array overflow */
        if ( intoplaceholders >= 1024 ) {
            const char* message = "too many INTO placeholders specified";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                     errmsg("too many INTO placeholders specified"),
                     parser_errposition(yylloc)));
            u_sess->plsql_cxt.have_error = true;
            break;
        }

        tok = yylex();
        switch (tok)
        {
            case T_PLACEHOLDER:
                intoplaceholders++;
                break;

            default:
                const char* message = "invalid placeholder specified";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("invalid placeholder specified"),
                         parser_errposition(yylloc)));
                break;
        }
    }

    /*
     * We read an extra, non-comma token from yylex(), so push it
     * back onto the input stream
     */
    pltsql_push_back_token(tok);

    row = (PLpgSQL_row*)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = plpgsql_location_to_lineno(initial_location);
    row->rowtupdesc = NULL;
    row->nfields = 0;
    row->fieldnames = NULL;
    row->varnos = NULL;
    row->intoplaceholders = intoplaceholders;
    row->intodatums = NULL;
    row->isImplicit = true;

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}



/*
 * Convert a single scalar into a "row" list.  This is exactly
 * like read_into_scalar_list except we never consume any input.
 *
 * Note: lineno could be computed from location, but since callers
 * have it at hand already, we may as well pass it in.
 */
static PLpgSQL_row *
make_scalar_list1(char *initial_name,
                  PLpgSQL_datum *initial_datum,
                  int initial_dno,
                  int lineno, int location)
{
    PLpgSQL_row		*row;

    check_assignable(initial_datum, location);

    row = (PLpgSQL_row *)palloc0(sizeof(PLpgSQL_row));
    row->dtype = PLPGSQL_DTYPE_ROW;
    row->refname = pstrdup("*internal*");
    row->lineno = lineno;
    row->rowtupdesc = NULL;
    row->nfields = 1;
    row->fieldnames = (char **)palloc(sizeof(char *));
    row->varnos = (int *)palloc(sizeof(int));
    row->fieldnames[0] = initial_name;
    row->varnos[0] = initial_dno;
    row->isImplicit = true;

    plpgsql_adddatum((PLpgSQL_datum *)row);

    return row;
}

/*
 * When the PL/pgSQL parser expects to see a SQL statement, it is very
 * liberal in what it accepts; for example, we often assume an
 * unrecognized keyword is the beginning of a SQL statement. This
 * avoids the need to duplicate parts of the SQL grammar in the
 * PL/pgSQL grammar, but it means we can accept wildly malformed
 * input. To try and catch some of the more obviously invalid input,
 * we run the strings we expect to be SQL statements through the main
 * SQL parser.
 *
 * We only invoke the raw parser (not the analyzer); this doesn't do
 * any database access and does not check any semantic rules, it just
 * checks for basic syntactic correctness. We do this here, rather
 * than after parsing has finished, because a malformed SQL statement
 * may cause the PL/pgSQL parser to become confused about statement
 * borders. So it is best to bail out as early as we can.
 *
 * It is assumed that "stmt" represents a copy of the function source text
 * beginning at offset "location", with leader text of length "leaderlen"
 * (typically "SELECT ") prefixed to the source text.  We use this assumption
 * to transpose any error cursor position back to the function source text.
 * If no error cursor is provided, we'll just point at "location".
 */
static void
check_sql_expr(const char *stmt, int location, int leaderlen)
{
    sql_error_callback_arg cbarg;
    ErrorContextCallback  syntax_errcontext;
    MemoryContext oldCxt;

    if (!u_sess->plsql_cxt.curr_compile_context->plpgsql_check_syntax)
        return;

    cbarg.location = location;
    cbarg.leaderlen = leaderlen;

    syntax_errcontext.callback = plpgsql_sql_error_callback;
    syntax_errcontext.arg = &cbarg;
    syntax_errcontext.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &syntax_errcontext;

    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
    RawParserHook parser_hook= raw_parser;
#if (!defined(ENABLE_MULTIPLE_NODES)) && (!defined(ENABLE_PRIVATEGAUSS))
    if (u_sess->attr.attr_sql.whale || u_sess->attr.attr_sql.dolphin || DB_IS_CMPT(D_FORMAT)) {
        int id = GetCustomParserId();
        if (id >= 0 && g_instance.raw_parser_hook[id] != NULL) {
            parser_hook = (RawParserHook)g_instance.raw_parser_hook[id];
        }
    }
#endif
    (void)parser_hook(stmt, NULL);
    MemoryContextSwitchTo(oldCxt);

    /* Restore former ereport callback */
    t_thrd.log_cxt.error_context_stack = syntax_errcontext.previous;
}

static void
plpgsql_sql_error_callback(void *arg)
{
    sql_error_callback_arg *cbarg = (sql_error_callback_arg *) arg;
    int			errpos;

    /*
     * First, set up internalerrposition to point to the start of the
     * statement text within the function text.  Note this converts
     * location (a byte offset) to a character number.
     */
    parser_errposition(cbarg->location);

    /*
     * If the core parser provided an error position, transpose it.
     * Note we are dealing with 1-based character numbers at this point.
     */
    errpos = geterrposition();
    if (errpos > cbarg->leaderlen)
    {
        int		myerrpos = getinternalerrposition();

        if (myerrpos > 0)		/* safety check */
            internalerrposition(myerrpos + errpos - cbarg->leaderlen - 1);
    }

    /* In any case, flush errposition --- we want internalerrpos only */
    errposition(0);
}

/*
 * Parse a SQL datatype name and produce a PLpgSQL_type structure.
 *
 * The heavy lifting is done elsewhere.  Here we are only concerned
 * with setting up an errcontext link that will let us give an error
 * cursor pointing into the plpgsql function source, if necessary.
 * This is handled the same as in check_sql_expr(), and we likewise
 * expect that the given string is a copy from the source text.
 */
static PLpgSQL_type *
parse_datatype(const char *string, int location)
{
    Oid			type_id;
    int32		typmod;
    int expr_len = 0;
    sql_error_callback_arg cbarg;
    ErrorContextCallback  syntax_errcontext;
    MemoryContext oldCxt = NULL;
    PLpgSQL_type* datatype = NULL;
    cbarg.location = location;
    cbarg.leaderlen = 0;

    syntax_errcontext.callback = plpgsql_sql_error_callback;
    syntax_errcontext.arg = &cbarg;
    syntax_errcontext.previous = t_thrd.log_cxt.error_context_stack;
    t_thrd.log_cxt.error_context_stack = &syntax_errcontext;


    /*
     * parseTypeString is only for getting type_id and typemod, who are both scalars, 
     * so memory used in it is all temp.
     */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);


    u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
    /* Let the main parser try to parse it under standard SQL rules */
    TypeDependExtend* typeDependExtend = NULL;
    if (enable_plpgsql_gsdependency()) {
        InstanceTypeNameDependExtend(&typeDependExtend);
        CreatePlsqlType oldCreatePlsqlType = u_sess->plsql_cxt.createPlsqlType;
        PG_TRY();
        {
            set_create_plsql_type_not_check_nsp_oid();
            parseTypeString(string, &type_id, &typmod, typeDependExtend);
            set_create_plsql_type(oldCreatePlsqlType);
        }
        PG_CATCH();
        {
            set_create_plsql_type(oldCreatePlsqlType);
            PG_RE_THROW();
        }
        PG_END_TRY();
    } else {
        parseTypeString(string, &type_id, &typmod, typeDependExtend);
    }

    (void)MemoryContextSwitchTo(oldCxt);

    /* Restore former ereport callback */
    t_thrd.log_cxt.error_context_stack = syntax_errcontext.previous;

    /* Okay, build a PLpgSQL_type data structure for it */
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL)
    {
        datatype = plpgsql_build_datatype(type_id, typmod, 0, typeDependExtend);
    } else {
        datatype = plpgsql_build_datatype(type_id, typmod,
                                  u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_input_collation, typeDependExtend);
    }
    if (!(u_sess->hook_cxt.forTsdbHook && DB_IS_CMPT(PG_FORMAT))) {
        datatype->defaultvalues = get_default_plpgsql_expr_from_typeoid(type_id, &expr_len);
    }
    return datatype;
}

/* Build a arrary_type by elem_type. */
static PLpgSQL_type * build_array_type_from_elemtype(PLpgSQL_type *elem_type)
{
    PLpgSQL_type *arrary_type;
    Oid arraytypoid = get_array_type(elem_type->typoid);
    if (arraytypoid == InvalidOid) {
        ereport(errstate,
            (errmodule(MOD_PLSQL),
            errcode(ERRCODE_UNDEFINED_OBJECT),
            errmsg("array type not found"),
            errdetail("array type of \"%s\" does not exist.", elem_type->typname),
            errcause("undefined object"),
            erraction("check typename")));
    }
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL) {
        arrary_type = plpgsql_build_datatype(arraytypoid, elem_type->atttypmod, 0);
    } else {
        arrary_type = plpgsql_build_datatype(arraytypoid, elem_type->atttypmod,
            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_input_collation);
    }
    return arrary_type;
}

static PLpgSQL_var* plpgsql_build_nested_variable(PLpgSQL_var *nest_table, bool isconst, char* name, int lineno)
{
    PLpgSQL_var *build_nest_table = NULL;
    PLpgSQL_type *new_var_type = NULL;
    char nestname[NAMEDATALEN] = {0};
    errno_t rc = EOK;
    rc = snprintf_s(nestname, sizeof(nestname), sizeof(nestname) - 1, "%s_nest", name);
    securec_check_ss(rc, "", "");
    /* set nested table type */
    new_var_type = build_array_type_from_elemtype(nest_table->datatype);
    new_var_type->tableOfIndexType = nest_table->datatype->tableOfIndexType;
    new_var_type->collectionType = nest_table->datatype->collectionType;
    build_nest_table = (PLpgSQL_var *)plpgsql_build_variable(nestname, lineno, new_var_type, true);
    build_nest_table->isconst = isconst;
    build_nest_table->default_val = NULL;
    build_nest_table->nest_layers = nest_table->nest_layers;
    if (nest_table->nest_table != NULL) {
        build_nest_table->nest_table = plpgsql_build_nested_variable(nest_table->nest_table, isconst, name, lineno);
    }
    return build_nest_table;
}

static int get_nest_tableof_layer(PLpgSQL_var *var, const char *typname, int errstate)
{
    int depth = 0;
    while (var != NULL) {
        depth++;
        var = var->nest_table;
    }
    if (depth + 1 > MAXDIM) {
        ereport(errstate,
            (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
                errmodule(MOD_PLSQL),
                errmsg("Layer number of nest tableof type exceeds the maximum allowed."),
                errdetail("Define nest tableof type \"%s\" layers (%d) exceeds the maximum allowed (%d).", typname, depth + 1, MAXDIM),
                errcause("too many nested layers"),
                erraction("check define of table of type")));
        u_sess->plsql_cxt.have_error = true;
    }
    return depth + 1;
}

/* 
 * look up composite type from namespace
 */
static PLpgSQL_nsitem* lookupCompositeType(char* functypname, char* pkgtypname)
{
    PLpgSQL_nsitem* ns = NULL;
    /* if compile func, search func namesapce first */
    if (functypname != NULL) {
        ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, functypname, NULL, NULL, NULL);
        if (ns != NULL || pkgtypname == NULL) {
            return ns;
        }
    }

    /* next search package namespace */
    PLpgSQL_package* pkg = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package;
    ns = plpgsql_ns_lookup(pkg->public_ns, false, pkgtypname, NULL, NULL, NULL);
    if (ns != NULL) {
        return ns;
    }

    /* may compile pkg spec now, search top again */
    ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, pkgtypname, NULL, NULL, NULL);
    if (ns != NULL) {
        return ns;
    }

    /* if compile package body, search private namespace */
    if (!pkg->is_spec_compiling) {
        /* private type should cast '$' */
        StringInfoData  castTypName;
        initStringInfo(&castTypName);
        appendStringInfoString(&castTypName, "$");
        appendStringInfoString(&castTypName, pkgtypname);
 
        /* search private namespace, when complie, it is the top */
        ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, castTypName.data, NULL, NULL, NULL);
        pfree_ext(castTypName.data);
    }

    return ns;
}

static Oid getOldTypeOidByTypeName(char** typname, char** schamaName, char* functypname, char* pkgtypname, Oid* pkgoid)
{
    Oid nameSpaceOid = InvalidOid;
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL) {
        *pkgoid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid;
        *typname = functypname;
        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_searchpath != NULL) {
            nameSpaceOid = linitial_oid(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_searchpath->schemas);
        }
    } else {
        *pkgoid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
        *typname = pkgtypname;
        nameSpaceOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->namespaceOid;
    }

    if (OidIsValid(nameSpaceOid)) {
        *schamaName = get_namespace_name(nameSpaceOid);
    } else {
        nameSpaceOid = getCurrentNamespace();
    }

    Oid oldtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(*typname),
        ObjectIdGetDatum(nameSpaceOid));
    return oldtypeoid;
}

static RangeVar* createRangeVarForCompositeType(char* schamaName, char* typname)
{
    RangeVar *r = makeNode(RangeVar);
    r->catalogname = NULL;
    r->schemaname = schamaName;
    r->relname = pstrdup(typname);
    r->relpersistence = RELPERSISTENCE_PERMANENT;
    r->location = -1;
    r->ispartition = false;
    r->isbucket = false;
    r->buckets = NIL;
    return r;
}

static void buildDependencyForCompositeType(Oid newtypeoid)
{
    char* funcname = "inline_code_block";
    bool ignore = u_sess->plsql_cxt.curr_compile_context != NULL &&
                    u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                    (strcmp(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_signature, funcname) == 0 ||
                    (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_func != NULL &&
                    strcmp(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->parent_func->fn_signature, funcname) == 0));
    if (ignore) {
        return;
    }
    ObjectAddress myself, referenced;
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL) {
        myself.classId = ProcedureRelationId;
        myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid;
        myself.objectSubId = 0;
    } else {
        myself.classId = PackageRelationId;
        myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
        myself.objectSubId = 0;
    }
    referenced.classId = TypeRelationId;
    referenced.objectId = newtypeoid;
    referenced.objectSubId = 0;
    recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
    CommandCounterIncrement();
}

static bool checkDuplicateAttrName(TupleDesc tupleDesc)
{
    int attrnum = tupleDesc->natts;
    for (int i = 0; i < attrnum; i++) {
        Form_pg_attribute attr1 = &tupleDesc->attrs[i];
        for (int j = i + 1; j < attrnum; j++) {
            Form_pg_attribute attr2 = &tupleDesc->attrs[j];
            if (strcmp(NameStr(attr1->attname), NameStr(attr2->attname)) == 0) {
                return true;
            }
        }
    }
    return false;
}
#ifndef ENABLE_MULTIPLE_NODES
static bool checkAllAttrName(TupleDesc tupleDesc)
{
    int attrnum = tupleDesc->natts;
    for (int i = 0; i < attrnum; i++) {
        Form_pg_attribute pg_att_form = &tupleDesc->attrs[i];
        char* att_name = NameStr(pg_att_form->attname);
        if (strcmp(att_name, "?column?") != 0) {
            return false;
        }
    }
    return true;
}
#endif
static Oid createCompositeTypeForCursor(PLpgSQL_var* var, PLpgSQL_expr* expr)
{
#ifdef ENABLE_MULTIPLE_NODES
    return InvalidOid;
#endif
    TupleDesc tupleDesc = getCursorTupleDesc(expr, false);
    if (tupleDesc == NULL || tupleDesc->natts == 0) {
        return InvalidOid;
    }
    /* 
     * if cursor is select a,a from t1, cannot have same attr name.
     */
    bool isHaveDupName = checkDuplicateAttrName(tupleDesc);
    if (isHaveDupName) {
        return InvalidOid;
    }

    /* concatenate name string with function name for composite type, which to avoid conflict. */
    char* functypname = NULL;
    char* pkgtypname = NULL;
    char* typname = NULL;
    char* schamaname = NULL;
    Oid pkgoid = InvalidOid;
    getPkgFuncTypeName(var->refname, &functypname, &pkgtypname);
    Oid oldtypeoid = getOldTypeOidByTypeName(&typname, &schamaname, functypname, pkgtypname, &pkgoid);
    Oid newtypeoid = InvalidOid;
    if (OidIsValid(oldtypeoid)) {
            /* already build one, just use it */
            newtypeoid = oldtypeoid;
    } else {
        RangeVar* r = createRangeVarForCompositeType(schamaname, typname);

        List* codeflist = NULL;
        int attrnum = tupleDesc->natts;
        for (int i = 0; i < attrnum; i++) {
            ColumnDef *n = makeNode(ColumnDef);
            Form_pg_attribute attr = &tupleDesc->attrs[i];
            n->colname = pstrdup(NameStr(attr->attname));
            n->typname = makeTypeNameFromOid(attr->atttypid, attr->atttypmod);
            n->inhcount = 0;
            n->is_local = true;
            n->is_not_null = false;
            n->is_from_type = false;
            n->storage = 0;
            n->raw_default = NULL;
            n->cooked_default = NULL;
            n->collClause = NULL;
            n->clientLogicColumnRef=NULL;
            n->collOid = InvalidOid;
            n->constraints = NIL;
            codeflist = lappend(codeflist, (Node*)n);
        }
        DefineCompositeType(r, codeflist);
        newtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(typname),
            ObjectIdGetDatum(getCurrentNamespace()));
        pfree_ext(r);
        ListCell* cell = NULL;
        foreach (cell, codeflist) {
            ColumnDef* n = (ColumnDef*)lfirst(cell);
            pfree_ext(n->colname);
        }
        list_free_deep(codeflist);

        /* build dependency on created composite type. */
        buildDependencyForCompositeType(newtypeoid);
    }
    
    pfree_ext(functypname);
    pfree_ext(pkgtypname);
    return newtypeoid;
}

/*
 * the record type will be nested or referenced by another package, check if it valid.
 */
static void check_record_type(PLpgSQL_rec_type * var_type, int location, bool check_nested)
{
    /* for now, record type with table of with index by, is not supported be nested */
    PLpgSQL_type* type = NULL;
    char* errstr = NULL;
    if (check_nested) {
        errstr = "nested.";
    } else {
        errstr = "referenced by another package.";
    }
    for (int i = 0; i < var_type->attrnum; i++) {
        type = var_type->types[i];
        if (type->ttype == PLPGSQL_TTYPE_SCALAR && OidIsValid(type->tableOfIndexType)) {
            ereport(errstate,
                (errmodule(MOD_PLSQL),
                errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                errmsg("record type with table of attribute is not suppoted to be %s", errstr),
                errdetail("attribute \"%s\" of record type \"%s\" is table of with index by, which is not supported to be %s",
                    var_type->attrnames[i],var_type->typname, errstr),
                errcause("feature not suppoted"),
                parser_errposition(location),
                erraction("modify type definition")));
        }
    }
}

/*
 * Build a composite type by execute SQL, when record type is nested.
 */
static PLpgSQL_type* build_type_from_record_var(int dno, int location, bool for_proc)
{
    PLpgSQL_type *newp = NULL;
    PLpgSQL_rec_type * var_type = (PLpgSQL_rec_type *)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
    if (!for_proc) {
        check_record_type(var_type, location);
    }

    /* concatenate name string with function name for composite type, which to avoid conflict. */
    char*  functypname = NULL;
    char*  pkgtypname = NULL;
    getPkgFuncTypeName(var_type->typname, &functypname, &pkgtypname);
    /* look up composite type from namespace, if exists, we have already build one. */
    PLpgSQL_nsitem* ns = lookupCompositeType(functypname, pkgtypname);
    
    if (ns == NULL) {
        /* we need to build a composite type, and drop any remaining types. */
        Oid newtypeoid = InvalidOid;
        char* typname;
        char* schamaname = NULL;
        Oid pkgoid = InvalidOid;
        
        Oid oldtypeoid = getOldTypeOidByTypeName(&typname, &schamaname, functypname, pkgtypname, &pkgoid);
        if (OidIsValid(oldtypeoid)) {
            /* already build one, just use it */
            if(IsPackageDependType(oldtypeoid, pkgoid)) {
                newtypeoid = oldtypeoid;
                if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE) {
                    (void)gsplsql_flush_undef_ref_type_dependency(newtypeoid);
                }
            } else {
                ereport(errstate,
                    (errmodule(MOD_PLSQL),
                        errcode(ERRCODE_DUPLICATE_OBJECT),
                        errmsg("duplicate type name"),
                        errdetail("type name of \"%s\" duplicated with an existed type when build package or function type.", typname),
                        errcause("duplicate type name"),
                        erraction("modify type name")));
            }
        } else {
            RangeVar* r = createRangeVarForCompositeType(schamaname, typname);
            List* codeflist = NULL;
            int attrnum = var_type->attrnum;
            for (int i = 0; i < attrnum; i++) {
                ColumnDef *n = makeNode(ColumnDef);
                n->colname = pstrdup(var_type->attrnames[i]);
                n->typname = make_typename_from_datatype(var_type->types[i]);
                n->inhcount = 0;
                n->is_local = true;
                n->is_not_null = false;
                n->is_from_type = false;
                n->storage = 0;
                n->raw_default = NULL;
                n->cooked_default = NULL;
                n->collClause = NULL;
                n->clientLogicColumnRef=NULL;
                n->collOid = InvalidOid;
                n->constraints = NIL;
                codeflist = lappend(codeflist, (Node*)n);
            }

            DefineCompositeType(r, codeflist);
            newtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(typname),
                ObjectIdGetDatum(getCurrentNamespace()));
            record_inline_subprogram_type(newtypeoid);
            pfree_ext(r);
            list_free_deep(codeflist);

            /* build dependency on created composite type. */
            buildDependencyForCompositeType(newtypeoid);
            if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE) {
                (void)gsplsql_flush_undef_ref_type_dependency(newtypeoid);
            }
        }

        /* build datatype of the created composite type. */
        newp = plpgsql_build_datatype(newtypeoid, -1, InvalidOid);
        newp->dtype = PLPGSQL_DTYPE_COMPOSITE;
        newp->ttype = PLPGSQL_TTYPE_ROW;

        /* add the composite type to datum and namespace. */
        int varno = plpgsql_adddatum((PLpgSQL_datum*)newp);
        plpgsql_ns_additem(PLPGSQL_NSTYPE_COMPOSITE, varno, typname);

    } else {
        /* we have already build one, just take it from datums. */
        if (ns->itemtype == PLPGSQL_NSTYPE_COMPOSITE) {
            newp = (PLpgSQL_type*)(u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[ns->itemno]);
            /* build a new one, avoid conflict by array or table of */
            newp = plpgsql_build_datatype(newp->typoid, -1, InvalidOid);
            newp->dtype = PLPGSQL_DTYPE_COMPOSITE;
            newp->ttype = PLPGSQL_TTYPE_ROW;
        } else {
            ereport(errstate,
                (errcode(ERRCODE_DUPLICATE_OBJECT),
                    errmsg("duplicate declaration"),
                    errdetail("record %s is duplicate with other objects.", var_type->typname)));
            u_sess->plsql_cxt.have_error = true;
        }
    }
    pfree_ext(functypname);
    pfree_ext(pkgtypname);
    return newp;
}

static Oid plpgsql_build_package_record_type(const char* typname, List* list, bool add2namespace)
{
    Oid oldtypeoid = InvalidOid;
    Oid newtypeoid = InvalidOid;
    char* schamaName = NULL;
    Oid pkgOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
    Oid pkgNamespaceOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->namespaceOid;
    if (OidIsValid(pkgNamespaceOid)) {
        schamaName = get_namespace_name(pkgNamespaceOid);
    } else {
        pkgNamespaceOid = getCurrentNamespace();
    }
    char* casttypename = CastPackageTypeName(typname,
        pkgOid, true,
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling);
    if (strlen(casttypename) >= NAMEDATALEN ) {
        ereport(errstate,
            (errcode(ERRCODE_NAME_TOO_LONG),
                errmsg("type name too long"),
                errdetail("record name %s with package name %s should be less the %d letters.",
                    typname,
                    u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_signature,
                    NAMEDATALEN)));
        u_sess->plsql_cxt.have_error = true;
        pfree_ext(casttypename);
    }

    oldtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(casttypename), ObjectIdGetDatum(pkgNamespaceOid));
    bool oldTypeOidIsValid = OidIsValid(oldtypeoid);
    if (oldTypeOidIsValid) {
        /* already build on, just use it */
        if(IsPackageDependType(oldtypeoid, pkgOid)) {
            newtypeoid = oldtypeoid;
            if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE) {
                (void)gsplsql_flush_undef_ref_type_dependency(newtypeoid);
            }
        } else {
            ereport(errstate,
                (errmodule(MOD_PLSQL),
                    errcode(ERRCODE_DUPLICATE_OBJECT),
                    errmsg("duplicate type name"),
                    errdetail("type name of \"%s\" duplicated with an existed type when build package type.", casttypename),
                    errcause("duplicate type name"),
                    erraction("modify package type name")));
        }
    } else {
        RangeVar *r = makeNode(RangeVar);
        r->catalogname = NULL;
        r->schemaname = schamaName;
        r->relname = pstrdup(casttypename);
        r->relpersistence = RELPERSISTENCE_PERMANENT;
        r->location = -1;
        r->ispartition = false;
        r->isbucket = false;
        r->buckets = NIL;

        List* codeflist = NULL;
        ListCell* cell = NULL;
        PLpgSQL_rec_attr* attr = NULL;
    
        foreach (cell, list) {
            attr = (PLpgSQL_rec_attr*)lfirst(cell);
            codeflist = lappend(codeflist, make_columnDef_from_attr(attr));
        }

        DefineCompositeType(r, codeflist);
    
        newtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(casttypename),
            ObjectIdGetDatum(getCurrentNamespace()));

        /* build dependency on created composite type. */
        ObjectAddress myself, referenced;
        myself.classId = PackageRelationId;
        myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
        myself.objectSubId = 0;
        referenced.classId = TypeRelationId;
        referenced.objectId = newtypeoid;
        referenced.objectSubId = 0;
        recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
        CommandCounterIncrement();
        pfree_ext(r);
        list_free_deep(codeflist);
        if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE) {
            gsplsql_build_ref_type_dependency(newtypeoid);
        }
    }

    PLpgSQL_type *newtype = NULL;
    newtype = plpgsql_build_datatype(newtypeoid, -1, InvalidOid);
    newtype->dtype = PLPGSQL_DTYPE_COMPOSITE;
    newtype->ttype = PLPGSQL_TTYPE_ROW;

    int varno = plpgsql_adddatum((PLpgSQL_datum*)newtype);

    plpgsql_ns_additem(PLPGSQL_NSTYPE_COMPOSITE, varno, casttypename);
    pfree_ext(casttypename);

    return newtypeoid;
}

static void  plpgsql_build_package_array_type(const char* typname,Oid elemtypoid, char arraytype, TypeDependExtend* dependExtend)
{
    char typtyp;
    ObjectAddress myself, referenced;

    char* casttypename = CastPackageTypeName(typname, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid, true,
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling);
    if (strlen(casttypename) >= NAMEDATALEN ) {
        ereport(errstate,
            (errcode(ERRCODE_NAME_TOO_LONG),
                errmsg("type name too long"),
                errdetail("array or nested table type name %s with package name %s should be less the %d letters.",
                    typname, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_signature, NAMEDATALEN)));
        u_sess->plsql_cxt.have_error = true;
    }

    Oid pkgNamespaceOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->namespaceOid;
    if (!OidIsValid(pkgNamespaceOid)) {
        pkgNamespaceOid = getCurrentNamespace();
    }

    Oid pkgOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
    Oid oldtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(casttypename),
        ObjectIdGetDatum(pkgNamespaceOid));
    bool oldtypeoidIsValid = OidIsValid(oldtypeoid);
    if (oldtypeoidIsValid) {
        /* alread build one, just return */
        if(IsPackageDependType(oldtypeoid, pkgOid)) {
            if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE) {
                (void)gsplsql_flush_undef_ref_type_dependency(oldtypeoid);
            }
            return;
        } else {
            ereport(errstate,
                (errmodule(MOD_PLSQL),
                    errcode(ERRCODE_DUPLICATE_OBJECT),
                    errmsg("duplicate type name"),
                    errdetail("type name of \"%s\" duplicated with an existed type when build package type.", casttypename),
                    errcause("duplicate type name"),
                    erraction("modify package type name")));
        }
    }

    if (arraytype == TYPCATEGORY_TABLEOF ||
        arraytype == TYPCATEGORY_TABLEOF_VARCHAR ||
        arraytype == TYPCATEGORY_TABLEOF_INTEGER) {
        typtyp = TYPTYPE_TABLEOF;
        if (UNDEFINEDOID != elemtypoid) {
            elemtypoid = get_array_type(elemtypoid);
        }
    } else if (arraytype == TYPCATEGORY_VARRAY) {
        typtyp = TYPTYPE_VARRAY;
        if (UNDEFINEDOID != elemtypoid) {
            elemtypoid = get_array_type(elemtypoid);
        }
    } else {
        typtyp = TYPTYPE_BASE;
    }
    Oid ownerId = InvalidOid;
    ownerId = GetUserIdFromNspId(pkgNamespaceOid);
    if (!OidIsValid(ownerId)) {
        ownerId = GetUserId();
    }

    referenced = TypeCreate(InvalidOid, /* force the type's OID to this */
        casttypename,               /* Array type name */
        pkgNamespaceOid,            /* Same namespace as parent */
        InvalidOid,                 /* Not composite, no relationOid */
        0,                          /* relkind, also N/A here */
        ownerId,                    /* owner's ID */
        -1,                         /* Internal size (varlena) */
        typtyp,                     /* Not composite - typelem is */
        arraytype,                  /* type-category (array or table of) */
        false,                      /* array types are never preferred */
        DEFAULT_TYPDELIM,           /* default array delimiter */
        F_ARRAY_IN,                 /* array input proc */
        F_ARRAY_OUT,                /* array output proc */
        F_ARRAY_RECV,               /* array recv (bin) proc */
        F_ARRAY_SEND,               /* array send (bin) proc */
        InvalidOid,                 /* typmodin procedure - none */
        InvalidOid,                 /* typmodout procedure - none */
        F_ARRAY_TYPANALYZE,         /* array analyze procedure */
        elemtypoid,                 /* array element type - the rowtype */
        false,                       /* yes, this is an array type */
        InvalidOid,                 /* this has no array type */
        InvalidOid,                 /* domain base type - irrelevant */
        NULL,                       /* default value - none */
        NULL,                       /* default binary representation */
        false,                      /* passed by reference */
        'd',                        /* alignment - must be the largest! */
        'x',                        /* fully TOASTable */
        -1,                         /* typmod */
        0,                          /* array dimensions for typBaseType */
        false,                      /* Type NOT NULL */
        get_typcollation(elemtypoid),
        dependExtend);

    CommandCounterIncrement();

    /* build dependency on created composite type. */
    myself.classId = PackageRelationId;
    myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
    myself.objectSubId = 0;
    recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
    CommandCounterIncrement();
    if (CompileWhich() == PLPGSQL_COMPILE_PACKAGE && typtyp != TYPTYPE_TABLEOF) {
        (void)gsplsql_build_ref_type_dependency(referenced.objectId);
    }
    pfree_ext(casttypename);
}

static void plpgsql_build_func_array_type(const char* typname,Oid elemtypoid, char arraytype, int32 atttypmod, TypeDependExtend* dependExtend)
{
    if(u_sess->attr.attr_sql.sql_compatibility !=  A_FORMAT 
        || u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL) {
        return;
    }
    Oid typoid = InvalidOid;
    char typtyp;
    Oid pkgNamespaceOid = getCurrentNamespace();
    Oid fn_oid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid;
    Oid parentFuncOid = InvalidOid;
    if (OidIsValid(fn_oid) && !u_sess->plsql_cxt.isCreateFunction)  {
        Relation desc;
        ScanKeyData skey[1];
        SysScanDesc sysscan;
        HeapTuple tuple;

        ScanKeyInit(&skey[0], Anum_pg_proc_packageid, BTEqualStrategyNumber, F_OIDEQ, ObjectIdGetDatum(fn_oid));
        desc = heap_open(ProcedureRelationId, AccessShareLock);
        sysscan = systable_beginscan(desc, ProcedureNameArgsNspNewIndexId, true, NULL, 1, skey);

        /* get all functions under packageoid */
        while (HeapTupleIsValid(tuple = systable_getnext(sysscan))) {
                parentFuncOid = fn_oid;
                break;
            }

        systable_endscan(sysscan);
        heap_close(desc, NoLock);
    }
 
   /* 用户创建了子程序，然后在新的 session 中调用外层函数时，此时不会在内核中解析函数体，
    * u_sess->parser_cxt.has_subprogram 始终为 false，导致外层程序自定义类型未创建，
    * 在子程序中调用不到外层程序自定义的类型而报错，此处先查找依赖于外层程序的子程序，找到
    * 代表该程序含有子程序，则创建type;
    * 直接调用不会走到 ProcedureCreate_extend 中调用 DeleteSubprogramDenpendOnProcedure
    */ 
    long counter  = 0;
    counter = DeleteSubprogramDenpendOnProcedure(ProcedureRelationId, fn_oid, false);
    // 只在含有子程序的 procedure/function 创建.
    if (u_sess->parser_cxt.has_subprogram || OidIsValid(parentFuncOid) || counter > 0) {
        char* casttypename = CastPackageTypeName(typname,
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid, false, true);
        if (strlen(casttypename) >= NAMEDATALEN ) {
            ereport(errstate,
                (errcode(ERRCODE_NAME_TOO_LONG),
                    errmsg("type name too long"),
                    errdetail("record name %s with function name %s should be less the %d letters.",
                        typname,
                        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_signature,
                        NAMEDATALEN)));
            u_sess->plsql_cxt.have_error = true;
            pfree_ext(casttypename);
        }

        Oid oldtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(casttypename),
            ObjectIdGetDatum(pkgNamespaceOid));
        if (OidIsValid(oldtypeoid)) {
            if(IsPackageDependType(oldtypeoid, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid)) {
                return;
            }
        }
        if (arraytype == TYPCATEGORY_TABLEOF ||
            arraytype == TYPCATEGORY_TABLEOF_VARCHAR ||
            arraytype == TYPCATEGORY_TABLEOF_INTEGER) {
            typtyp = TYPTYPE_TABLEOF;
        } else {
            typtyp = TYPTYPE_BASE;
        }
        Oid ownerId = GetUserId();		
		
        ObjectAddress address = TypeCreate(InvalidOid, /* force the type's OID to this */
            casttypename,               /* Array type name */
            pkgNamespaceOid,               /* Same namespace as parent */
            InvalidOid,                 /* Not composite, no relationOid */
            0,                          /* relkind, also N/A here */
            ownerId,                    /* owner's ID */
            -1,                         /* Internal size (varlena) */
            typtyp,               /* Not composite - typelem is */
            arraytype,          /* type-category (array or table of) */
            false,                      /* array types are never preferred */
            DEFAULT_TYPDELIM,           /* default array delimiter */
            F_ARRAY_IN,                 /* array input proc */
            F_ARRAY_OUT,                /* array output proc */
            F_ARRAY_RECV,               /* array recv (bin) proc */
            F_ARRAY_SEND,               /* array send (bin) proc */
            InvalidOid,                 /* typmodin procedure - none */
            InvalidOid,                 /* typmodout procedure - none */
            F_ARRAY_TYPANALYZE,         /* array analyze procedure */
            elemtypoid,               /* array element type - the rowtype */
            false,                       /* yes, this is an array type */
            InvalidOid,                 /* this has no array type */
            InvalidOid,                 /* domain base type - irrelevant */
            NULL,                       /* default value - none */
            NULL,                       /* default binary representation */
            false,                      /* passed by reference */
            'd',                        /* alignment - must be the largest! */
            'x',                        /* fully TOASTable */
            atttypmod,                  /* typmod */
            0,                          /* array dimensions for typBaseType */
            false,                      /* Type NOT NULL */
            get_typcollation(elemtypoid),
            dependExtend);

        typoid = address.objectId;
        CommandCounterIncrement();
   

        bool anonymous_subprogram = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile &&
                                    u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->in_anonymous;
        /*
         * Types defined within an anonymous block do not have dependencies and are deleted once the execution of 
         * the anonymous block is complete.
         */
        if(!anonymous_subprogram) {
            ObjectAddress myself, referenced;
            myself.classId = ProcedureRelationId;
            myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid;
            myself.objectSubId = 0;
            referenced.classId = TypeRelationId;
            referenced.objectId = typoid;
            referenced.objectSubId = 0;
            recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
            CommandCounterIncrement();
        }  
        pfree_ext(casttypename);

        record_inline_subprogram_type(typoid);
    }
    return;
}

static void plpgsql_build_package_refcursor_type(const char* typname)
{
    CreateSynonymStmt stmt;
    stmt.replace = true;
    stmt.isPublic = false;
    Node* lc = NULL;
    List* synList = NULL;
    List* objList = NULL;
    
    char* casttypename = CastPackageTypeName(typname, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid, true,
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling);
    if (strlen(casttypename) >= NAMEDATALEN ) {
        ereport(errstate,
            (errcode(ERRCODE_NAME_TOO_LONG),
                errmsg("type name too long"),
                errdetail("cursor type name %s with package name %s should be less the %d letters.",
                    typname, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_signature, NAMEDATALEN)));
        u_sess->plsql_cxt.have_error = true;
    }

    Oid pkgNamespaceOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->namespaceOid;
    if (!OidIsValid(pkgNamespaceOid)) {
        pkgNamespaceOid = getCurrentNamespace();
    }

    HeapTuple tuple = NULL;
    tuple = SearchSysCache2(SYNONYMNAMENSP, PointerGetDatum(casttypename), ObjectIdGetDatum(pkgNamespaceOid));
    if (HeapTupleIsValid(tuple)) {
        Oid synOid = HeapTupleGetOid(tuple);
        if(IsPackageDependType(synOid, u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid, true)) {
            ReleaseSysCache(tuple);
            return;
        } else {
            ReleaseSysCache(tuple);
            ereport(errstate,
                (errmodule(MOD_PLSQL),
                    errcode(ERRCODE_DUPLICATE_OBJECT),
                    errmsg("duplicate type name"),
                    errdetail("type name of \"%s\" duplicated with an existed type when build package type.", casttypename),
                    errcause("duplicate type name"),
                    erraction("modify package type name")));
        }
    }

    char* pkgSchemaName = NULL;
    if (OidIsValid(pkgNamespaceOid)) {
        pkgSchemaName = get_namespace_name(pkgNamespaceOid);
        lc = (Node*)makeString(pkgSchemaName);
        synList = lappend(synList, lc);
    }

    lc = (Node*)makeString(casttypename);
    synList = lappend(synList, lc);

    lc = (Node*)makeString("pg_catalog");
    objList = lappend(objList, lc);
    lc = (Node*)makeString("refcursor");
    objList = lappend(objList, lc);

    stmt.synName = synList;
    stmt.objName = objList;

    CreateSynonym(&stmt);

    CommandCounterIncrement();

    tuple = NULL;
    tuple = SearchSysCache2(SYNONYMNAMENSP, PointerGetDatum(casttypename), ObjectIdGetDatum(pkgNamespaceOid));
    if (!HeapTupleIsValid(tuple)) {
        ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("synonym \"%s\" does not exist", casttypename)));
    }
    Oid synOid = HeapTupleGetOid(tuple);
    ReleaseSysCache(tuple);

    /* build dependency on Synonym. */
    ObjectAddress myself, referenced;
    myself.classId = PackageRelationId;
    myself.objectId = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid;
    myself.objectSubId = 0;
    referenced.classId = PgSynonymRelationId;
    referenced.objectId = synOid;
    referenced.objectSubId = 0;
    recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
    CommandCounterIncrement();

    pfree_ext(casttypename);
    pfree_ext(pkgSchemaName);

}

static Node* make_columnDef_from_attr(PLpgSQL_rec_attr* attr)
{
    ColumnDef *n = makeNode(ColumnDef);
    n->colname = pstrdup(attr->attrname);
    n->typname = make_typename_from_datatype(attr->type);
    n->inhcount = 0;
    n->is_local = true;
    n->is_not_null = false;
    n->is_from_type = false;
    n->storage = 0;
    n->raw_default = attr->defaultvalue ? get_default_node_from_plpgsql_expr(attr->defaultvalue) : NULL;
    n->cooked_default = NULL;
    n->collClause = NULL;
    n->clientLogicColumnRef=NULL;
    n->collOid = InvalidOid;
    n->constraints = NIL;

    return (Node*)n;
}

static TypeName* make_typename_from_datatype(PLpgSQL_type* datatype)
{
    return makeTypeNameFromOid(datatype->typoid, datatype->atttypmod, datatype->dependExtend);
}

/*
 * build SUBTYPE in ANONYMOUS_BLOCK
 * typname : name of subtype to be create
 * newp : base type info
 * RangeList : unsuport in opengauss util now, use to supprt type like pls_interge
 * isNotNull : is not null
 */
static Oid plpgsql_build_anonymous_subtype(char* typname, PLpgSQL_type* newp, const List* RangeList, bool isNotNull)
{
    /* some variable */
    Oid NamespaceOid = InvalidOid;
    Oid ownerId = InvalidOid;
    Oid newtypeoid = InvalidOid;
    HeapTuple baseTypeTup;
    Form_pg_type baseType;
    ObjectAddress referenced;
    PLpgSQL_type* res = NULL;
    SubTypeRange* typerange = NULL;

    /* check is typname too long */
    if (strlen(typname) >= NAMEDATALEN) {
        ereport(errstate,
            (errcode(ERRCODE_NAME_TOO_LONG),
                errmsg("type name too long")));
        u_sess->plsql_cxt.have_error = true;
    }

    if (!OidIsValid(NamespaceOid)) {
        NamespaceOid = getCurrentNamespace();
    }

    /* check for duplicate name */
    Oid oldtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(typname), ObjectIdGetDatum(NamespaceOid));
    if (OidIsValid(oldtypeoid)) {
        ereport(errstate,
            (errmodule(MOD_PLSQL),
                errcode(ERRCODE_DUPLICATE_OBJECT),
                errmsg("duplicate type name"),
                errdetail("type name of \"%s\" duplicated with an existed type when build subtype.", typname),
                errcause("duplicate type name"),
                erraction("modify subtype name")));
        u_sess->plsql_cxt.have_error = true;
    }

    ownerId = GetUserIdFromNspId(NamespaceOid);
    if (!OidIsValid(ownerId)) {
        ownerId = GetUserId();
    }

    baseTypeTup = SearchSysCache(TYPEOID, ObjectIdGetDatum(newp->typoid), 0, 0, 0 );
    baseType = (Form_pg_type)GETSTRUCT(baseTypeTup);
    /* refer DOMAIN */
    referenced = TypeCreate(InvalidOid, /* force the type's OID to this */
        typname,                        /* Array type name */
        NamespaceOid,                   /* Same namespace as parent */
        InvalidOid,                     /* Not composite, no relationOid */
        0,                              /* relkind, also N/A here */
        ownerId,                        /* owner's ID */
        baseType->typlen,               /* Internal size (varlena) */
        TYPTYPE_DOMAIN,                 /* Not composite - typelem is */
        baseType->typcategory,          /* type-category (array or table of) */
        false,                          /* array types are never preferred */
        baseType->typdelim,             /* default array delimiter */
        F_SUBTYPE_IN,                   /* input proc */
        baseType->typoutput,            /* output proc */
        F_SUBTYPE_RECV,                 /* recv (bin) proc */
        baseType->typsend,              /* send (bin) proc */
        baseType->typmodin,             /* typmodin procedure - none */
        baseType->typmodout,            /* typmodout procedure - none */
        baseType->typanalyze,           /* array analyze procedure */
        baseType->typelem,              /* array element type - the rowtype */
        false,                          /* yes, this is an array type */
        baseType->typarray,             /* this has no array type */
        newp->typoid,                   /* domain base type - irrelevant */
        NULL,                           /* default value - none */
        NULL,                           /* default binary representation */
        baseType->typbyval,             /* passed by reference */
        baseType->typalign,             /* alignment - must be the largest! */
        baseType->typstorage,           /* fully TOASTable */
        newp->atttypmod,                /* typmod */
        baseType->typndims,             /* array dimensions for typBaseType */
        isNotNull,                      /* Type NOT NULL */
        baseType->typcollation);

    ReleaseSysCache(baseTypeTup);

    CommandCounterIncrement();

    if (RangeList != NIL) {
        ereport(errstate,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("not support range in subtype")));
    }

    /* get the oid of type we built just now, for saving subtype into func. we can delete type when we delete func */
    newtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(typname), ObjectIdGetDatum(NamespaceOid));
    res = plpgsql_build_datatype(newtypeoid, -1, InvalidOid);
    res->dtype = PLPGSQL_DTYPE_SUBTYPE;
    res->ttype = PLPGSQL_TTYPE_SCALAR;
    res->ispkg = false;
    res->typoid = newtypeoid;
    int varno = plpgsql_adddatum((PLpgSQL_datum*)res);
    plpgsql_ns_additem(PLPGSQL_NSTYPE_SUBTYPE, varno, typname);

    return newtypeoid;
}

/*
 * Suild SUBTYPE in FUNKTION or PACKAGE
 * What's difference with anonym block:
 * 1. Subtype has different name, it need concat oid in pkg/func 
 * 2. Subtype isn't needed to delete immediately, constraints will 
 *    be released when obj released
 * typname : name of subtype to be create
 * newp : base type info
 * RangeList : unsuport in opengauss util now, use to supprt type like pls_interge
 * isNotNull : is not null
 */
static Oid 
plpgsql_build_function_package_subtype(char* typname, PLpgSQL_type* newp, const List* RangeList, bool isNotNull)
{
    /* some variable */
    Oid NamespaceOid = InvalidOid;
    Oid ownerId = InvalidOid;
    HeapTuple typeTup;
    Form_pg_type baseType;
    ObjectAddress myself;
    ObjectAddress referenced;
    char* functypname = NULL;
    char* pkgtypname = NULL;
    char* schamaname = NULL;
    Oid pkgoid = InvalidOid;
    SubTypeRange* typerange = NULL;
    char* casttypename = NULL;
    bool sameType = false;

    /* it should be a function if curr_package is null */
    PLpgSQL_package* curr_package = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package;
    PLpgSQL_function* curr_function = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile;

    AssertEreport(curr_package || curr_function, MOD_PLSQL, "Error defining subtype range");

    if (RangeList != NIL) {
        ereport(errstate,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
             errmsg("not support range in subtype")));
    }


    /* exchange name into oid_typname, check length in this function */
    getPkgFuncTypeName(typname, &functypname, &pkgtypname);

    if (curr_package) {
        NamespaceOid = curr_package->namespaceOid;
    }
    if (!OidIsValid(NamespaceOid)) {
        NamespaceOid = getCurrentNamespace();
    }

    Oid oldtypeoid = getOldTypeOidByTypeName(&casttypename, &schamaname, functypname, pkgtypname, &pkgoid);

    HeapTuple old_typtuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(oldtypeoid));
    /* check can old record we use */
    if (HeapTupleIsValid(old_typtuple)) {
        Form_pg_type typform = (Form_pg_type)GETSTRUCT(old_typtuple);
        if (typform->typbasetype != newp->typoid){
            ereport(errstate,
            (errmodule(MOD_PLSQL),
                errcode(ERRCODE_DUPLICATE_OBJECT),
                errmsg("duplicate type name"),
                errdetail("type name of \"%s\" duplicated with an existed type when build subtype.", typname),
                errcause("duplicate type name"),
                erraction("modify subtype name")));
        } else {
            sameType = true;
        }
        ReleaseSysCache(old_typtuple);
    }

    if (OidIsValid(oldtypeoid) && sameType) {
        /* skip same check for we have build it, get the constraint and return */
        if (!IsPackageDependType(oldtypeoid, pkgoid)) {
            /* create dependency if there wasn't any one */
            referenced.classId = TypeRelationId;
            referenced.objectId = oldtypeoid;
            referenced.objectSubId = 0;
            /* create dependency relation with pkg/func */
            if (curr_package) {
                myself.classId = PackageRelationId;
                myself.objectId = curr_package->pkg_oid;
                myself.objectSubId = 0;
            } else {
                myself.classId = ProcedureRelationId;
                myself.objectId = curr_function->fn_oid;
                myself.objectSubId = 0;
            }
            recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);
        }
        CommandCounterIncrement();

        /* save to current ns*/
        PLpgSQL_type* res = plpgsql_build_datatype(oldtypeoid, -1, InvalidOid);
        res->dtype = PLPGSQL_DTYPE_SUBTYPE;
        res->ttype = PLPGSQL_TTYPE_SCALAR;
        res->ispkg = false;
        res->typoid = oldtypeoid;
        int varno = plpgsql_adddatum((PLpgSQL_datum*)res);
        plpgsql_ns_additem(PLPGSQL_NSTYPE_SUBTYPE, varno, typname);

        pfree_ext(functypname);
        pfree_ext(pkgtypname);

        return oldtypeoid;
    }
    ownerId = GetUserIdFromNspId(NamespaceOid);
    if (!OidIsValid(ownerId)) {
        ownerId = GetUserId();
    }

    typeTup = SearchSysCache(TYPEOID, ObjectIdGetDatum(newp->typoid), 0, 0, 0);
    baseType = (Form_pg_type)GETSTRUCT(typeTup);

    /* refer DOMAIN */
    referenced = TypeCreate(InvalidOid, /* force the type's OID to this */
        casttypename,                        /* Array type name */
        NamespaceOid,                   /* Same namespace as parent */
        InvalidOid,                     /* Not composite, no relationOid */
        0,                              /* relkind, also N/A here */
        ownerId,                        /* owner's ID */
        baseType->typlen,               /* Internal size (varlena) */
        TYPTYPE_DOMAIN,                 /* Not composite - typelem is */
        baseType->typcategory,          /* type-category (array or table of) */
        false,                          /* array types are never preferred */
        baseType->typdelim,             /* default array delimiter */
        F_SUBTYPE_IN,                   /* input proc */
        baseType->typoutput,            /* output proc */
        F_SUBTYPE_RECV,                 /* recv (bin) proc */
        baseType->typsend,              /* send (bin) proc */
        baseType->typmodin,             /* typmodin procedure - none */
        baseType->typmodout,            /* typmodout procedure - none */
        baseType->typanalyze,           /* array analyze procedure */
        baseType->typelem,              /* no array element type */
        false,                          /* yes, this is an array type */
        baseType->typarray,                 /* no array for subtype */
        newp->typoid,                   /* domain base type - irrelevant */
        NULL,                           /* default value - none */
        NULL,                           /* default binary representation */
        baseType->typbyval,             /* passed by reference */
        baseType->typalign,             /* alignment - must be the largest! */
        baseType->typstorage,           /* fully TOASTable */
        newp->atttypmod,                /* typmod */
        baseType->typndims,             /* array dimensions for typBaseType */
        isNotNull,                      /* Type NOT NULL */
        baseType->typcollation);        /* type's collation */

    ReleaseSysCache(typeTup);
    CommandCounterIncrement();

    /* build dependcy relation with FUNC/PKG */
    if (curr_package) {
        myself.classId = PackageRelationId;
        myself.objectId = curr_package->pkg_oid;
        myself.objectSubId = 0;
    } else {
        myself.classId = ProcedureRelationId;
        myself.objectId = curr_function->fn_oid;
        myself.objectSubId = 0;
    }
    recordDependencyOn(&referenced, &myself, DEPENDENCY_AUTO);

    CommandCounterIncrement();

    /* save to datum in current function */
    Oid newtypeoid = GetSysCacheOid2(TYPENAMENSP, PointerGetDatum(casttypename), ObjectIdGetDatum(NamespaceOid));
    PLpgSQL_type* res = plpgsql_build_datatype(newtypeoid, -1, InvalidOid);
    res->dtype = PLPGSQL_DTYPE_SUBTYPE;
    res->ttype = PLPGSQL_TTYPE_SCALAR;
    res->ispkg = false;
    res->typoid  = newtypeoid;
    int varno = plpgsql_adddatum((PLpgSQL_datum*)res);
    plpgsql_ns_additem(PLPGSQL_NSTYPE_SUBTYPE, varno, typname);

    pfree_ext(functypname);
    pfree_ext(pkgtypname);
    return newtypeoid;
}

/*
 * Check block starting and ending labels match.
 */
static void
check_labels(const char *start_label, const char *end_label, int end_location)
{
    if (end_label)
    {
        if (u_sess->plsql_cxt.curr_compile_context && u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile) {
            if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->proc_list) {
                char* func_name = get_func_name(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid);
                if (func_name && pg_strcasecmp(end_label, func_name) == 0) {
                    return;
                }
            }
        }

        if (!start_label) {
            const char* message = "end label specified for unlabelled block";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("end label \"%s\" specified for unlabelled block",
                            end_label),
                     parser_errposition(end_location)));
        }

        if (start_label == NULL || end_label == NULL) {
            if (errstate == NOTICE) {
                u_sess->plsql_cxt.have_error = true;
                return;
            } else {
                const char* message = "end label specified for unlabelled block";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("end label \"%s\" differs from block's label \"%s\"",
                            end_label, start_label),
                     parser_errposition(end_location)));
            }
        }
        if (strcmp(start_label, end_label) != 0) {
            const char* message = "end label differs from block's label";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("end label \"%s\" differs from block's label \"%s\"",
                            end_label, start_label),
                     parser_errposition(end_location)));
        }
    }
}

/*
 * Read the arguments (if any) for a cursor, followed by the until token
 *
 * If cursor has no args, just swallow the until token and return NULL.
 * If it does have args, we expect to see "( arg [, arg ...] )" followed
 * by the until token, where arg may be a plain expression, or a named
 * parameter assignment of the form argname := expr. Consume all that and
 * return a SELECT query that evaluates the expression(s) (without the outer
 * parens).
 */
static PLpgSQL_expr *
read_cursor_args(PLpgSQL_var *cursor, int until, const char *expected)
{
    PLpgSQL_expr *expr;
    PLpgSQL_row *row;
    int			tok;
    int			argc;
    char	  **argv;
    StringInfoData ds;
    char	   *sqlstart = "SELECT ";
    bool		any_named = false;

    tok = yylex();
    if (cursor->cursor_explicit_argrow < 0)
    {
        /* No arguments expected */
        if (tok == '(') {
            const char* message = "cursor has no arguments";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("cursor \"%s\" has no arguments",
                            cursor->refname),
                     parser_errposition(yylloc)));
        }

        if (tok != until)
            yyerror("syntax error");

        return NULL;
    }

    /* Else better provide arguments */
    if (tok != '(' && cursor->cursor_implicit_argrow > 0) {
        const char* message = "cursor has arguments";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                 errmsg("cursor \"%s\" has arguments",
                        cursor->refname),
                 parser_errposition(yylloc)));
    }

    bool isPkgCur = cursor->ispkg &&
        (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package == NULL
        || u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->pkg_oid != cursor->pkg->pkg_oid);
    if (isPkgCur) {
        ereport(errstate,
            (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
            errmsg("package cursor with arguments is only supported to be opened in the same package."),
            errdetail("cursor \"%s.%s\" is only supported to be opened in the package \"%s\"",
                cursor->pkg->pkg_signature,
                cursor->varname == NULL ? cursor->refname : cursor->varname,
                cursor->pkg->pkg_signature),
            errcause("feature not supported"),
            erraction("define this cursor without arguments or open this cursor in same package"),
            parser_errposition(yylloc)));
    }

    /*
     * Read the arguments, one by one.
     */
    if (cursor->ispkg) {
        row = (PLpgSQL_row *) cursor->pkg->datums[cursor->cursor_explicit_argrow];
    } else {
        row = (PLpgSQL_row *) u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[cursor->cursor_explicit_argrow];
    }
    argv = (char **) palloc0(row->nfields * sizeof(char *));

    /* No need to process value exprs in case of 'open c;' */
    for (argc = 0; argc < row->nfields && tok == '('; argc++)
    {
        PLpgSQL_expr *item;
        int		endtoken;
        int		argpos;
        int		tok1,
                tok2;
        int		arglocation;

        /* Check if it's a named parameter: "param := value" */
        plpgsql_peek2(&tok1, &tok2, &arglocation, NULL);
        /* Done reading value exprs */
        if (tok1 == ')'){
            /* read ')' */
            yylex();
            break;
        }
        if (tok1 == IDENT && tok2 == COLON_EQUALS)
        {
            char   *argname;
            IdentifierLookup save_IdentifierLookup;

            /* Read the argument name, ignoring any matching variable */
            save_IdentifierLookup = u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup;
            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = IDENTIFIER_LOOKUP_DECLARE;
            yylex();
            argname = yylval.str;
            u_sess->plsql_cxt.curr_compile_context->plpgsql_IdentifierLookup = save_IdentifierLookup;

            /* Match argument name to cursor arguments */
            for (argpos = 0; argpos < row->nfields; argpos++)
            {
                if (strcmp(row->fieldnames[argpos], argname) == 0)
                    break;
            }
            if (argpos == row->nfields) {
                const char* message = "cursor has no argument named";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                        (errcode(ERRCODE_SYNTAX_ERROR),
                         errmsg("cursor \"%s\" has no argument named \"%s\"",
                                cursor->refname, argname),
                         parser_errposition(yylloc)));
            }

            /*
             * Eat the ":=". We already peeked, so the error should never
             * happen.
             */
            tok2 = yylex();
            if (tok2 != COLON_EQUALS)
                yyerror("syntax error");

            any_named = true;
        }
        else
            argpos = argc;

        if (argv[argpos] != NULL) {
            const char* message = "value for parameter of cursor specified more than once";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("value for parameter \"%s\" of cursor \"%s\" specified more than once",
                            row->fieldnames[argpos], cursor->refname),
                     parser_errposition(arglocation)));
        }

        /*
         * Read the value expression. To provide the user with meaningful
         * parse error positions, we check the syntax immediately, instead of
         * checking the final expression that may have the arguments
         * reordered. Trailing whitespace must not be trimmed, because
         * otherwise input of the form (param -- comment\n, param) would be
         * translated into a form where the second parameter is commented
         * out.
         */
        item = read_sql_construct(',', ')', 0,
                                  ",\" or \")",
                                  sqlstart,
                                  true, true,
                                  false, /* do not trim */
                                  NULL, &endtoken);

        argv[argpos] = item->query + strlen(sqlstart);

        if (endtoken == ')') {
            argc++;
            break;
        }

        if (endtoken == ',' && (argc == row->nfields - 1)) {
            const char* message = "too many arguments for cursor";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                     errmsg("too many arguments for cursor \"%s\"",
                            cursor->refname),
                     parser_errposition(yylloc)));
        }
    }

    /* fill rest arguments with default values, if they have any */
    for (argc = 0; argc < row->nfields; argc++)
    {
        if (argv[argc] != NULL)
            continue;
        PLpgSQL_expr* defexpr = row->argDefExpr[argc];
        if(!defexpr){
            const char* message = "not enough values for cursor arguments";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                    (errcode(ERRCODE_SYNTAX_ERROR),
                    errmsg("not enough values for arguments of cursor \"%s\"",
                            cursor->refname),
                    parser_errposition(yylloc)));
        }
        argv[argc] = defexpr->query + strlen(sqlstart);
    }

    /* Make positional argument list */
    initStringInfo(&ds);
    appendStringInfoString(&ds, sqlstart);
    for (argc = 0; argc < row->nfields; argc++)
    {
        AssertEreport(argv[argc] != NULL,
                            MOD_PLSQL,
                            "It should not be null");

        /*
         * Because named notation allows permutated argument lists, include
         * the parameter name for meaningful runtime errors.
         */
        appendStringInfoString(&ds, argv[argc]);
        if (any_named)
            appendStringInfo(&ds, " AS %s",
                             quote_identifier(row->fieldnames[argc]));
        if (argc < row->nfields - 1)
            appendStringInfoString(&ds, ", ");
    }
    appendStringInfoChar(&ds, ';');

    expr = (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype			= PLPGSQL_DTYPE_EXPR;
    expr->query			= pstrdup(ds.data);
    expr->plan			= NULL;
    expr->paramnos		= NULL;
    expr->ns            = plpgsql_ns_top();
    expr->idx			= (uint32)-1;
    expr->out_param_dno = -1;
    pfree_ext(ds.data);

    /* Next we'd better find the until token */
    if(tok == '(')
        tok = yylex();
    if (tok != until)
        yyerror("syntax error");

    return expr;
}

/*
 * Parse RAISE ... USING options
 */
static List *
read_raise_options(void)
{
    List	   *result = NIL;

    for (;;)
    {
        PLpgSQL_raise_option *opt;
        int		tok;

        if ((tok = yylex()) == 0)
            yyerror("unexpected end of function definition");

        opt = (PLpgSQL_raise_option *) palloc(sizeof(PLpgSQL_raise_option));

        if (tok_is_keyword(tok, &yylval,
                           K_ERRCODE, "errcode"))
            opt->opt_type = PLPGSQL_RAISEOPTION_ERRCODE;
        else if (tok_is_keyword(tok, &yylval,
                                K_MESSAGE, "message"))
            opt->opt_type = PLPGSQL_RAISEOPTION_MESSAGE;
        else if (tok_is_keyword(tok, &yylval,
                                K_DETAIL, "detail"))
            opt->opt_type = PLPGSQL_RAISEOPTION_DETAIL;
        else if (tok_is_keyword(tok, &yylval,
                                K_HINT, "hint"))
            opt->opt_type = PLPGSQL_RAISEOPTION_HINT;
        else
            yyerror("unrecognized RAISE statement option");

        tok = yylex();
        if (tok != '=' && tok != COLON_EQUALS)
            yyerror("syntax error, expected \"=\"");

        opt->expr = read_sql_expression2(',', ';', ", or ;", &tok);

        result = lappend(result, opt);

        if (tok == ';')
            break;
    }

    return result;
}

/* 
 * Parse SIGNAL/RESIGNAL ... SQLSTATE
 */
static void read_signal_sqlstate(PLpgSQL_stmt_signal *newp, int tok)
{
    char *sqlstate_value;

    if (tok == 0 || tok == ';') {
        yyerror("unexpected end of function definition");
    }

    if (tok != SCONST && tok != T_WORD) {
        yyerror("syntax error, the expected value is a string.");
    }

    if (tok == T_WORD) {
        if (strcmp(yylval.str, "value") == 0) {
            if (yylex() != SCONST) {
                yyerror("syntax error, the expected value is a string.");
            }
        } else {
            yyerror("syntax error, the expected word is value.");
        }
    }

    sqlstate_value = yylval.str;

    if (strlen(sqlstate_value) != 5 ||
        strspn(sqlstate_value, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5 ||
        strncmp(sqlstate_value, "00", 2) == 0) {
            const char *message = "bad SQLSTATE";
            InsertErrorMessage(message, plpgsql_yylloc);
                ereport(ERROR,
                    (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                        errmsg("bad SQLSTATE '%s'", sqlstate_value)));
    }

    newp->sqlstate = sqlstate_value;
    newp->sqlerrstate = MAKE_SQLSTATE(sqlstate_value[0],
                                      sqlstate_value[1],
                                      sqlstate_value[2],
                                      sqlstate_value[3],
                                      sqlstate_value[4]);
    newp->condname = NULL;
    return;
}

/*
 * Parse SIGNAL/RESIGNAL ... condname
 */
static void read_signal_condname(PLpgSQL_stmt_signal *newp, int tok)
{
    char *condname = NULL;

    if (tok == T_WORD) {
        condname = yylval.word.ident;
    } else if (tok == T_DATUM) {
        condname = NameOfDatum(&yylval.wdatum);
    } else {
        yyerror("syntax error, the condition name is expected.");
    }

    PLpgSQL_condition *newcon = plpgsql_parse_err_condition_b_signal(condname);
    if (newcon->isSqlvalue) {
        const char *message = "SIGNAL/RESIGNAL can only use a CONDITION defined with SQLSTATE";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(ERROR,
            (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                errmsg("SIGNAL/RESIGNAL can only use a CONDITION defined with SQLSTATE")));
    }
    newp->sqlerrstate = newcon->sqlerrstate;
    newp->sqlstate = newcon->sqlstate;
    newp->condname = newcon->condname;
    return;
}

/*
 * Parse SIGNAL/RESIGNAL ... set
 */
static void read_signal_set(PLpgSQL_stmt_signal *newp, int tok)
{
    if (tok == T_WORD) {
        if (pg_strcasecmp(yylval.word.ident, "set") == 0) {
            newp->cond_info_item = read_signal_items();
        } else {
            yyerror("invalid keyword");
        }
    } else if (tok == ';') {
        newp->cond_info_item = NIL;
    } else {
        yyerror("syntax error");
    }
    return;
}

/*
 * Parse SIGNAL/RESIGNAL ... SET items
 */
static List *read_signal_items(void)
{
    List *result = NIL;

    for (;;) {
        PLpgSQL_signal_info_item *item;
        int tok;
        ListCell *lc;

        if ((tok = yylex()) == 0) {
            yyerror("unexpected end of siganl information definition");
        }

        item = (PLpgSQL_signal_info_item *)palloc(sizeof(PLpgSQL_signal_info_item));

        if (tok_is_keyword(tok, &yylval, K_CLASS_ORIGIN, "class_origin")) {
            item->con_info_value = PLPGSQL_CLASS_ORIGIN;
            item->con_name = pstrdup("CLASS_ORIGIN");
        } else if (tok_is_keyword(tok, &yylval, K_SUBCLASS_ORIGIN, "subclass_origin")) {
            item->con_info_value = PLPGSQL_SUBCLASS_ORIGIN;
            item->con_name = pstrdup("SUBCLASS_ORIGIN");
        } else if (tok_is_keyword(tok, &yylval, K_MESSAGE_TEXT, "message_text")) {
            item->con_info_value = PLPGSQL_MESSAGE_TEXT;
            item->con_name = pstrdup("MESSAGE_TEXT");
        } else if (tok_is_keyword(tok, &yylval, K_MYSQL_ERRNO, "mysql_errno")) {
            item->con_info_value = PLPGSQL_MYSQL_ERRNO;
            item->con_name = pstrdup("MYSQL_ERRNO");
        } else if (tok_is_keyword(tok, &yylval, K_CONSTRAINT_CATALOG, "constraint_catalog")) {
            item->con_info_value = PLPGSQL_CONSTRAINT_CATALOG;
            item->con_name = pstrdup("CONSTRAINT_CATALOG");
        } else if (tok_is_keyword(tok, &yylval, K_CONSTRAINT_SCHEMA, "constraint_schema")) {
            item->con_info_value = PLPGSQL_CONSTRAINT_SCHEMA;
            item->con_name = pstrdup("CONSTRAINT_SCHEMA");
        } else if (tok_is_keyword(tok, &yylval, K_CONSTRAINT_NAME, "constraint_name")) {
            item->con_info_value = PLPGSQL_CONSTRAINT_NAME;
            item->con_name = pstrdup("CONSTRAINT_NAME");
        } else if (tok_is_keyword(tok, &yylval, K_CATALOG_NAME, "catalog_name")) {
            item->con_info_value = PLPGSQL_CATALOG_NAME;
            item->con_name = pstrdup("CATALOG_NAME");
        } else if (tok_is_keyword(tok, &yylval, K_SCHEMA_NAME, "schema_name")) {
            item->con_info_value = PLPGSQL_SCHEMA_NAME;
            item->con_name = pstrdup("SCHEMA_NAME");
        } else if (tok_is_keyword(tok, &yylval, K_TABLE_NAME, "table_name")) {
            item->con_info_value = PLPGSQL_TABLE_NAME;
            item->con_name = pstrdup("TABLE_NAME");
        } else if (tok_is_keyword(tok, &yylval, K_COLUMN_NAME, "column_name")) {
            item->con_info_value = PLPGSQL_COLUMN_NAME;
            item->con_name = pstrdup("COLUMN_NAME");
        } else if (tok_is_keyword(tok, &yylval, K_CURSOR_NAME, "corsor_name")) {
            item->con_info_value = PLPGSQL_CURSOR_NAME;
            item->con_name = pstrdup("CURSOR_NAME");
        } else {
            yyerror("syntax error, unrecognized SIGNAL/RESIGNAL statement item");
        }

        tok = yylex();
        if (tok != '=') {
            yyerror("syntax error, expected \"=\"");
        }

        item->expr = read_sql_expression2(',', ';', ", or ;", &tok);

        foreach (lc, result) {
            PLpgSQL_signal_info_item *signal_item = (PLpgSQL_signal_info_item *)lfirst(lc);
            if (signal_item->con_info_value == item->con_info_value) {
                const char *message = "Duplicate condition information item";
                InsertErrorMessage(message, plpgsql_yylloc);
                    ereport(ERROR,
                        (errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
                            errmsg("Duplicate condition information item '%s'", item->con_name)));
            }
        }

        result = lappend(result, item);

        if (tok == ';') {
            break;
        }
    }

    return result;
}

/*
 * Fix up CASE statement
 */
static PLpgSQL_stmt *
make_case(int location, PLpgSQL_expr *t_expr,
          List *case_when_list, List *else_stmts)
{
    PLpgSQL_stmt_case 	*newp;

    newp = (PLpgSQL_stmt_case 	*)palloc(sizeof(PLpgSQL_stmt_case));
    newp->cmd_type = PLPGSQL_STMT_CASE;
    newp->lineno = plpgsql_location_to_lineno(location);
    newp->t_expr = t_expr;
    newp->t_varno = 0;
    newp->case_when_list = case_when_list;
    newp->have_else = (else_stmts != NIL);
    newp->sqlString = plpgsql_get_curline_query();
    /* Get rid of list-with-NULL hack */
    if (list_length(else_stmts) == 1 && linitial(else_stmts) == NULL)
        newp->else_stmts = NIL;
    else
        newp->else_stmts = else_stmts;

    /*
     * When test expression is present, we create a var for it and then
     * convert all the WHEN expressions to "VAR IN (original_expression)".
     * This is a bit klugy, but okay since we haven't yet done more than
     * read the expressions as text.  (Note that previous parsing won't
     * have complained if the WHEN ... THEN expression contained multiple
     * comma-separated values.)
     */
    if (t_expr)
    {
        char	varname[32];
        PLpgSQL_var *t_var;
        ListCell *l;

        /* use a name unlikely to collide with any user names */
        snprintf(varname, sizeof(varname), "__Case__Variable_%d__",
                 u_sess->plsql_cxt.curr_compile_context->plpgsql_nDatums);

        /*
         * We don't yet know the result datatype of t_expr.  Build the
         * variable as if it were INT4; we'll fix this at runtime if needed.
         */
        t_var = (PLpgSQL_var *)
            plpgsql_build_variable(varname, newp->lineno,
                                   plpgsql_build_datatype(INT4OID,
                                                          -1,
                                                          InvalidOid),
                                   true, true);
        newp->t_varno = t_var->dno;

        foreach(l, case_when_list)
        {
            PLpgSQL_case_when *cwt = (PLpgSQL_case_when *) lfirst(l);
            PLpgSQL_expr *expr = cwt->expr;
            StringInfoData	ds;

            /* copy expression query without SELECT keyword (expr->query + 7) */
            AssertEreport(strncmp(expr->query, "SELECT ", 7) == 0,
                                MOD_PLSQL,
                                "copy expression query without SELECT keyword");

            /* And do the string hacking */
            initStringInfo(&ds);

            appendStringInfo(&ds, "SELECT \"%s\" IN (%s)",
                             varname, expr->query + 7);

            pfree_ext(expr->query);
            expr->query = pstrdup(ds.data);
            /* Adjust expr's namespace to include the case variable */
            expr->ns = plpgsql_ns_top();

            pfree_ext(ds.data);
        }
    }

    return (PLpgSQL_stmt *) newp;
}

static PLpgSQL_stmt *
make_callfunc_stmt_no_arg(const char *sqlstart, int location, bool withsemicolon, List* funcNameList)
{
    char *cp[3] = {0};
    HeapTuple proctup = NULL;
    Oid *p_argtypes = NULL;
    char **p_argnames = NULL;
    char *p_argmodes = NULL;
    int narg = 0;
    List *funcname = NIL;
    PLpgSQL_expr* expr = NULL;
    FuncCandidateList clist = NULL;
    StringInfoData func_inparas;
    char *quoted_sqlstart = NULL;
    MemoryContext oldCxt = NULL;

    /*
     * the function make_callfunc_stmt_no_arg is only to assemble a sql statement, 
     * so the context is set to tmp context. 
     */
    oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->compile_tmp_cxt);
    /* get the function's name. */
    if (funcNameList == NULL) {
        plpgsql_parser_funcname(sqlstart, cp, 3);
    } else {
        funcname = funcNameList;
    }
    if (funcNameList == NULL) {
        if (cp[2] != NULL && cp[2][0] != '\0')
            funcname = list_make3(makeString(cp[0]), makeString(cp[1]), makeString(cp[2]));
        else if (cp[1] && cp[1][0] != '\0')
            funcname = list_make2(makeString(cp[0]), makeString(cp[1]));
        else
            funcname = list_make1(makeString(cp[0]));
    }

    /* search the function */
    clist = FuncnameGetCandidates(funcname, -1, NIL, false, false, false);
    if (clist == NULL)
    {
        const char* message = "function doesn't exist";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function \"%s\" doesn't exist ", sqlstart)));
    }

    proctup = SearchSysCache(PROCOID,
                            ObjectIdGetDatum(clist->oid),
                            0, 0, 0);
    /* function may be deleted after clist be searched. */
    if (!HeapTupleIsValid(proctup))
    {
        const char* message = "function doesn't exist";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                errmsg("function \"%s\" doesn't exist ", sqlstart)));
    }
    /* get the all args informations, only "in" parameters if p_argmodes is null */
    narg = get_func_arg_info(proctup,&p_argtypes, &p_argnames, &p_argmodes);
    int default_args = ((Form_pg_proc)GETSTRUCT(proctup))->pronargdefaults;
    ReleaseSysCache(proctup);

    // For function with default values, SQL will handle the arg with default value.
    if (narg != 0 && narg > default_args) {
        const char* message = "function has no enough parameters";
        InsertErrorMessage(message, plpgsql_yylloc);
        ereport(errstate,
                (errcode(ERRCODE_UNDEFINED_FUNCTION),
                 errmsg("function %s has no enough parameters", sqlstart)));
    }

    initStringInfo(&func_inparas);

    appendStringInfoString(&func_inparas, "CALL ");

    quoted_sqlstart = NameListToQuotedString(funcname);
    appendStringInfoString(&func_inparas, quoted_sqlstart);
    pfree_ext(quoted_sqlstart);

    appendStringInfoString(&func_inparas, "(");

    appendStringInfoString(&func_inparas, ")");

    /* read the end token */
    if (!withsemicolon) {
        yylex();
    }
    (void)MemoryContextSwitchTo(oldCxt);

    /* generate the expression */
    expr = (PLpgSQL_expr*)palloc0(sizeof(PLpgSQL_expr));
    expr->dtype = PLPGSQL_DTYPE_EXPR;
    expr->query = pstrdup(func_inparas.data);
    expr->plan = NULL;
    expr->paramnos = NULL;
    expr->ns = plpgsql_ns_top();
    expr->idx = (uint32)-1;
    expr->out_param_dno = -1;

    PLpgSQL_stmt_perform *perform = NULL;
    perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
    perform->cmd_type = PLPGSQL_STMT_PERFORM;
    perform->lineno = plpgsql_location_to_lineno(location);
    perform->expr = expr;
    perform->sqlString = plpgsql_get_curline_query();

    return (PLpgSQL_stmt *)perform;
}

/*
 * Brief		: special handling of dbms_lob.open and dbms_lob.close 
 * Description	: open and close are keyword, which need to be handled separately
 * Notes		: 
 */ 
static PLpgSQL_stmt *
parse_lob_open_close(int location)
{
	StringInfoData func;
	int tok = yylex();
	char *mode = NULL;
	bool is_open = false;
	PLpgSQL_expr *expr = NULL;
	PLpgSQL_stmt_perform *perform = NULL;

	initStringInfo(&func);
	appendStringInfoString(&func, "CALL DBMS_LOB.");
	tok = yylex();
	if (K_OPEN == tok)
	{
		is_open = true;
		appendStringInfoString(&func,"OPEN(");
	}
	else
		appendStringInfoString(&func,"CLOSE(");

	if ('(' == (tok = yylex()))
	{
		tok = yylex();
		if (T_DATUM == tok)
			appendStringInfoString(&func, NameOfDatum(&yylval.wdatum));
		else if (T_PLACEHOLDER == tok)
			appendStringInfoString(&func, yylval.word.ident);
		else
			yyerror("syntax error");

		if (is_open)
		{
			if (',' == (tok = yylex()) && T_CWORD == (tok = yylex()))
			{
				mode = NameListToString(yylval.cword.idents);
				if (strcasecmp(mode, "DBMS_LOB.LOB_READWRITE") != 0 
					&& strcasecmp(mode, "DBMS_LOB.LOB_READWRITE") != 0)
					yyerror("syntax error");
				else if (!(')' == yylex() && ';' == yylex()))
					yyerror("syntax error");

				appendStringInfoChar(&func, ')');
			}
			else
				yyerror("syntax error");
		}
		else if(')' == yylex() && ';' == yylex())
			appendStringInfoChar(&func, ')');
		else
			yyerror("syntax error");

		expr = (PLpgSQL_expr *)palloc0(sizeof(PLpgSQL_expr));
		expr->dtype = PLPGSQL_DTYPE_EXPR;
		expr->query = pstrdup(func.data);
		expr->plan = NULL;
		expr->paramnos = NULL;
		expr->ns = plpgsql_ns_top();
        expr->idx = (uint32)-1;
        expr->out_param_dno = -1;

		perform = (PLpgSQL_stmt_perform*)palloc0(sizeof(PLpgSQL_stmt_perform));
		perform->cmd_type = PLPGSQL_STMT_PERFORM;
		perform->lineno = plpgsql_location_to_lineno(location);
		perform->expr = expr;
        perform->sqlString = plpgsql_get_curline_query();
		return (PLpgSQL_stmt *)perform;
	}
	yyerror("syntax error");
	return NULL;
}

static void raw_parse_package_function_callback(void *arg)
{
    sql_error_callback_arg *cbarg = (sql_error_callback_arg*)arg;
    int cur_pos = geterrposition();

    if (cur_pos > cbarg->leaderlen)
    {
        cur_pos += cbarg->location - cbarg->leaderlen;
        errposition(cur_pos);
    }
}

static void raw_parse_package_function(char* proc_str, int location, int leaderlen)
{
    sql_error_callback_arg cbarg;
    ErrorContextCallback  syntax_errcontext;
    List* raw_parsetree_list = NULL;
    u_sess->plsql_cxt.plpgsql_yylloc = plpgsql_yylloc;
    u_sess->plsql_cxt.rawParsePackageFunction = true;

    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL)
    {
        cbarg.location = location;
        cbarg.leaderlen = leaderlen;

        syntax_errcontext.callback = raw_parse_package_function_callback;
        syntax_errcontext.arg = &cbarg;
        syntax_errcontext.previous = t_thrd.log_cxt.error_context_stack;
        t_thrd.log_cxt.error_context_stack = &syntax_errcontext;
    }
    raw_parsetree_list = raw_parser(proc_str);
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL) {
        /* Restore former ereport callback */
        t_thrd.log_cxt.error_context_stack = syntax_errcontext.previous;
    }

    CreateFunctionStmt* stmt;
    u_sess->plsql_cxt.rawParsePackageFunction = false;
    int rc = 0;
    if (raw_parsetree_list == NULL) {
        return;
    }
    stmt = (CreateFunctionStmt *)linitial(raw_parsetree_list);
    stmt->queryStr = proc_str;
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package == NULL)
        stmt->isPrivate = false;
    else {
        if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL && 
            u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->is_spec_compiling) {
            stmt->isPrivate = false;
        } else {
            stmt->isPrivate = true;
        }
    }
    if (u_sess->parser_cxt.isFunctionDeclare) {
        stmt->isFunctionDeclare = true;
    } else {
        stmt->isFunctionDeclare = false;
    }

    rc = CompileWhich();
    if (rc == PLPGSQL_COMPILE_PACKAGE) {
        stmt->startLineNumber = u_sess->plsql_cxt.procedure_start_line;
        stmt->firstLineNumber = u_sess->plsql_cxt.procedure_first_line;
    }
    /* check function name */
    CheckDuplicateFunctionName(stmt->funcname);
        
    if(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL){
        List *proc_list = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->proc_list;
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package->proc_list = lappend(proc_list,stmt);

    } else if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL) {
        List *proc_list = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->proc_list;
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->proc_list = lappend(proc_list,stmt);
    }
}

static void CheckDuplicateFunctionName(List* funcNameList)
{
    char* schemaname = NULL;
    char* pkgname = NULL;
    char* funcname = NULL;
    DeconstructQualifiedName(funcNameList, &schemaname, &funcname, &pkgname);
    if (plpgsql_ns_lookup(plpgsql_ns_top(), true, funcname, NULL, NULL, NULL) != NULL)
        yyerror("duplicate declaration");
}

static void IsInPublicNamespace(char* varname) {
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile == NULL && 
        u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package != NULL) {
        PLpgSQL_package* pkg = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package;
        if (plpgsql_ns_lookup(pkg->public_ns, true, varname, NULL, NULL, NULL) != NULL) {
            const char* message = "duplicate declaration";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errmodule(MOD_PLSQL), errcode(ERRCODE_SYNTAX_ERROR),
                    errmsg("duplicate declaration"),
                    errdetail("name \"%s\" already defined", varname),
                    errcause("name maybe defined in public variable or type"),
                    erraction("rename variable or type")));
        }
    }
}

static void CheckDuplicateCondition (char* name) {
    if (u_sess->plsql_cxt.curr_compile_context->plpgsql_conditions != NULL) {
        PLpgSQL_condition* cond = u_sess->plsql_cxt.curr_compile_context->plpgsql_conditions;
        while(cond) {
            if (strcmp(cond->condname, name) == 0) {
                const char* message = "duplicate declaration";
                InsertErrorMessage(message, plpgsql_yylloc);
                ereport(errstate,
                    (errmodule(MOD_PLSQL), errcode(ERRCODE_SYNTAX_ERROR),
                        errmsg("duplicate declaration"),
                        errdetail("condition \"%s\" already defined", name)));
                break;
            }
            cond = cond->next;
        }
    }
}
static void AddNamespaceIfNeed(int dno, char* ident)
{
    if (getCompileStatus() != COMPILIE_PKG_FUNC) {
        return;
    }

    if (ident == NULL) {
        yyerror("null string when add package type to procedure namespace");
    }

    if (plpgsql_ns_lookup(plpgsql_ns_top(), true, ident, NULL, NULL, NULL) != NULL) {
        return;
    }

    if (dno == -1) {
        plpgsql_ns_additem(PLPGSQL_NSTYPE_REFCURSOR, 0, ident);
        return;
    }

    PLpgSQL_datum* datum = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[dno];
    switch(datum->dtype) {
        case PLPGSQL_DTYPE_RECORD_TYPE : {
            plpgsql_ns_additem(PLPGSQL_NSTYPE_RECORD, dno, ident);
        } break;
        case PLPGSQL_DTYPE_VARRAY : {
            plpgsql_ns_additem(PLPGSQL_NSTYPE_VARRAY, dno, ident);
        } break;
        case PLPGSQL_DTYPE_TABLE : {
            plpgsql_ns_additem(PLPGSQL_NSTYPE_TABLE, dno, ident);
        } break;
        default : {
            yyerror("not recognized type when add package type to procedure namespace");
        } break;
    }

    return;
}

static void AddNamespaceIfPkgVar(const char* ident, IdentifierLookup save_IdentifierLookup)
{
    if (getCompileStatus() != COMPILIE_PKG_FUNC) {
        return;
    }

    /* only declare session need to add */
    if (save_IdentifierLookup != IDENTIFIER_LOOKUP_DECLARE) {
        return;
    }

    if (ident == NULL) {
        yyerror("null string when add package variable to procedure namespace");
    }

    if (plpgsql_ns_lookup(plpgsql_ns_top(), true, ident, NULL, NULL, NULL) != NULL) {
        return;
    }

    PLpgSQL_nsitem* ns = NULL;
    PLpgSQL_compile_context* curr_compile = u_sess->plsql_cxt.curr_compile_context;
    if (curr_compile->plpgsql_curr_compile_package != NULL) {
        PLpgSQL_package* pkg = curr_compile->plpgsql_curr_compile_package;
        ns = plpgsql_ns_lookup(pkg->public_ns, false, ident, NULL, NULL, NULL);
        if (ns == NULL) {
            ns = plpgsql_ns_lookup(pkg->private_ns, false, ident, NULL, NULL, NULL);
        }
    }
    if (ns != NULL && (ns->itemtype == PLPGSQL_NSTYPE_VAR || ns->itemtype == PLPGSQL_NSTYPE_ROW)) {
        plpgsql_ns_additem(ns->itemtype, ns->itemno, ident);
    }

    return;
}

static void get_datum_tok_type(PLpgSQL_datum* target, int* tok_flag)
{
    if (target->dtype == PLPGSQL_DTYPE_VAR) {
        PLpgSQL_var* var = (PLpgSQL_var*)(target);
        if (var != NULL && var->datatype != NULL &&
            var->datatype->typinput.fn_oid == F_ARRAY_IN) {
            if (var->datatype->collectionType == PLPGSQL_COLLECTION_TABLE) {
                *tok_flag = PLPGSQL_TOK_TABLE_VAR;
            } else {
                *tok_flag = PLPGSQL_TOK_VARRAY_VAR;
            }
        } else if (var != NULL && var->datatype &&
            var->datatype->typinput.fn_oid == F_DOMAIN_IN) {
            HeapTuple type_tuple =
                SearchSysCache1(TYPEOID, ObjectIdGetDatum(var->datatype->typoid));
            if (HeapTupleIsValid(type_tuple)) {
                Form_pg_type type_form = (Form_pg_type)GETSTRUCT(type_tuple);
                if (F_ARRAY_OUT == type_form->typoutput) {
                    *tok_flag = PLPGSQL_TOK_VARRAY_VAR;
                }
            }
            ReleaseSysCache(type_tuple);
        }
    }
}

static void plpgsql_cast_reference_list(List* idents, StringInfoData* ds, bool isPkgVar)
{
    char* word = NULL;
    if (isPkgVar) {
        word = strVal(linitial(idents));
        appendStringInfoString(ds, "(");
        appendStringInfoString(ds, word);
        appendStringInfoString(ds, ".");
        word = strVal(lsecond(idents));
        appendStringInfoString(ds, word);
        word = strVal(lthird(idents));
        appendStringInfoString(ds, ".");
        appendStringInfoString(ds, word);
        appendStringInfoString(ds, ")");
        word = strVal(lfourth(idents));
        appendStringInfoString(ds, ".");
        appendStringInfoString(ds, word);
        appendStringInfoString(ds, " ");
    } else {
        word = strVal(linitial(idents));
        appendStringInfoString(ds, "(");
        appendStringInfoString(ds, word);
        appendStringInfoString(ds, ".");
        word = strVal(lsecond(idents));
        appendStringInfoString(ds, word);
        appendStringInfoString(ds, ")");
        word = strVal(lthird(idents));
        appendStringInfoString(ds, ".");
        appendStringInfoString(ds, word);
        if (list_length(idents) == 4) {
            word = strVal(lfourth(idents));
            appendStringInfoString(ds, ".");
            appendStringInfoString(ds, word);
        }
        appendStringInfoString(ds, " ");
    }
}

static void CastArrayNameToArrayFunc(StringInfoData* ds, List* idents, bool needDot)
{
    char* arrayName1 = NULL;
    char* arrayName2 = NULL;
    char* arrayName3 = NULL;

    switch (list_length(idents)) {
        case 2: {
            arrayName1 = strVal(linitial(idents));
            appendStringInfo(ds, "\"%s\"", arrayName1);
        } break;
        case 3: {
            arrayName1 = strVal(linitial(idents));
            arrayName2 = strVal(lsecond(idents));
            appendStringInfo(ds, "\"%s\".\"%s\"", arrayName1, arrayName2);
        } break;
        case 4: {
            arrayName1 = strVal(linitial(idents));
            arrayName2 = strVal(lsecond(idents));
            arrayName3 = strVal(lthird(idents));
            appendStringInfo(ds, "\"%s\".\"%s\".\"%s\"", arrayName1, arrayName2, arrayName3);
        } break;
        default: {
            yyerror("syntax error of array functions");
        } break;
    }

    if (needDot) {
        appendStringInfo(ds, ", ");
    } else {
        appendStringInfo(ds, " ");
    }

    return;
}

static bool PkgVarNeedCast(List* idents)
{
    /* only pkg.row.col1.col2 need cast */
    if (list_length(idents) != 4) {
        return false;
    }

    char* word1 = strVal(linitial(idents));
    char* word2 = strVal(lsecond(idents));
    char* word3 = strVal(lthird(idents));
    PLpgSQL_nsitem* ns = NULL;
    int nnames = 0;

    ns = plpgsql_ns_lookup(plpgsql_ns_top(), false, word1, word2, word3, &nnames);
    /* nnames = 2, means pkg.row.col2.col2 form */
    if (nnames == 2) {
        return true;
    }

    return false;

}

static void check_autonomous_nest_tablevar(PLpgSQL_var* var)
{
    if (unlikely(var->ispkg && var->nest_table != NULL &&
                 u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL &&
                 u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->is_autonomous)) {
        ereport(errstate, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                           errmsg("Un-support feature: nest tableof variable \"%s\" not support pass through autonm function",
                                  var->refname)));
    }
}

static void SetErrorState() 
{
#ifndef ENABLE_MULTIPLE_NODES
        if (u_sess->attr.attr_common.plsql_show_all_error) {
            errstate = NOTICE;
        } else {
            errstate = ERROR;
        }
#else
        errstate = ERROR;
#endif
}

static bool need_build_row_for_func_arg(PLpgSQL_rec **rec, PLpgSQL_row **row, int out_arg_num, int all_arg, int *varnos, char *p_argmodes)
{
    /* out arg number > 1 should build a row */
    if (out_arg_num > 1) {
        return true;
    }

    /* no row or rec, should build a row */
    if (*rec == NULL && *row == NULL) {
        return true;
    }

    /* no out arg, need not build */
    if (out_arg_num == 0) {
        return false;
    }
  
    PLpgSQL_compile_context* curr_compile = u_sess->plsql_cxt.curr_compile_context;
    PLpgSQL_datum *tempDatum = NULL;
    for (int i = 0; i < all_arg; i++) {
        if (p_argmodes[i] == 'i') {
            continue;
        }

        /* out param no destination */
        if (varnos[i] == -1) {
            *rec = NULL;
            *row = NULL;
            return false;
        }

        tempDatum = curr_compile->plpgsql_Datums[varnos[i]];
        /* the out param is the rec or row */
        if (tempDatum == (PLpgSQL_datum*)(*rec) || tempDatum == (PLpgSQL_datum*)(*row)) {
            return false;
        }
        /* the out param is scalar, need build row */
        if (tempDatum->dtype == PLPGSQL_DTYPE_VAR
            || tempDatum->dtype == PLPGSQL_DTYPE_RECFIELD
            || tempDatum->dtype == PLPGSQL_DTYPE_ASSIGNLIST) {
            return true;
        }

        /* need not build a new row, but need to replace the correct row */
        if (tempDatum->dtype == PLPGSQL_DTYPE_ROW || tempDatum->dtype == PLPGSQL_DTYPE_RECORD) {
            *row = (PLpgSQL_row *)tempDatum;
            return false;
        }
        if (tempDatum->dtype == PLPGSQL_DTYPE_REC) {
            *rec = (PLpgSQL_rec *)tempDatum;
            return false;
        }

        /* arrive here, means out param invalid, set row and rec to null*/
        *rec = NULL;
        *row = NULL;
        return false;
    }

    /* should not arrive here */
    *rec = NULL;
    *row = NULL;
    return false;
}

#ifndef ENABLE_MULTIPLE_NODES
static void BuildForQueryVariable(PLpgSQL_expr* expr, PLpgSQL_row **row, PLpgSQL_rec **rec,
    const char* refname, int lineno)
{
    TupleDesc desc = getCursorTupleDesc(expr, true);
    if (desc == NULL || desc->natts == 0 || checkAllAttrName(desc)) {
        PLpgSQL_type dtype;
        dtype.ttype = PLPGSQL_TTYPE_REC;
        *rec = (PLpgSQL_rec *)
        plpgsql_build_variable(refname, lineno, &dtype, true);
    } else {
        *row = build_row_from_tuple_desc(refname, lineno, desc);
    }
}
#endif

static PLpgSQL_stmt *
funcname_is_call(const char* name, int location)
{
    PLpgSQL_stmt *stmt = NULL;
    bool isCallFunc = false;
    bool FuncNoarg = false;
    if(pltsql_is_token_match(';'))
    {
        FuncNoarg = true;
    }
    isCallFunc = is_function(name, false, FuncNoarg);
    if(isCallFunc)
    {
        if(FuncNoarg)
        {
            stmt = make_callfunc_stmt_no_arg(name, location);
        }
        else
        {
            stmt = make_callfunc_stmt(name, location, false, false);
            if (stmt->cmd_type == PLPGSQL_STMT_PERFORM)
            {
                ((PLpgSQL_stmt_perform *)stmt)->expr->is_funccall = true;
            }
            else if (stmt->cmd_type == PLPGSQL_STMT_EXECSQL)
            {
                ((PLpgSQL_stmt_execsql *)stmt)->sqlstmt->is_funccall = true;
            }
        }
    }

    return stmt;
}

static void processFunctionRecordOutParam(int varno, Oid funcoid, int* outparam)
{
    if (varno != -1 && is_function_with_plpgsql_language_and_outparam(funcoid)) {
        int dtype = u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[varno]->dtype;
        if (dtype == PLPGSQL_DTYPE_ROW) {
            *outparam = yylval.wdatum.dno;
        }
    }
}

/*
 * If the cursor is specified by PARALLEL_ENABLE PARTITION BY,
 * only FETCH CURSOR support in function body.
 */
static void CheckParallelCursorOpr(PLpgSQL_stmt_fetch* fetch)
{
    AssertEreport(u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[fetch->curvar]->dtype == PLPGSQL_DTYPE_VAR,
                  MOD_PLSQL,
                  "Cursor which would be fetched should be var");

    PLpgSQL_var* var = (PLpgSQL_var*)u_sess->plsql_cxt.curr_compile_context->plpgsql_Datums[fetch->curvar];
    if (u_sess->plsql_cxt.parallel_cursor_arg_name == NULL ||
        strcmp(var->varname, u_sess->plsql_cxt.parallel_cursor_arg_name) != 0) {
        return;
    }

    if (fetch->bulk_collect) {
        return;
    }

    if (fetch->direction != FETCH_FORWARD || fetch->expr != NULL || fetch->is_move) {
        ereport(ERROR, (errmsg("only support FETCH CURSOR for parallel cursor \"%s\"", var->varname)));
    }
}

static void
yylex_object_type_selfparam(char ** fieldnames,
                int *varnos,
                int nfields,
                PLpgSQL_row **row,
                PLpgSQL_rec **rec,
                int *retvarno,
                PLwdatum *objtypwdatum,
                bool overload)
{
    StringInfoData ds;
    initStringInfo(&ds);
    if (NULL != objtypwdatum && NULL != objtypwdatum->idents)
        CastTypeVariableNameToString(&ds, objtypwdatum->idents, false);
    if (!overload) {
        if (PLPGSQL_DTYPE_ROW == objtypwdatum->datum->dtype) {
            fieldnames[nfields] = pstrdup(ds.data);
            int varno = objtypwdatum->dno;
            varnos[nfields] = varno;
            *row = (PLpgSQL_row *)objtypwdatum->datum;
            *retvarno = varno;
        } else if (PLPGSQL_DTYPE_RECORD == objtypwdatum->datum->dtype) {
            check_assignable(objtypwdatum->datum, yylloc);
            fieldnames[nfields] = pstrdup(ds.data);
            varnos[nfields] = objtypwdatum->dno;
            *row = (PLpgSQL_row *)objtypwdatum->datum;
        } else if(PLPGSQL_DTYPE_REC == objtypwdatum->datum->dtype) {
            check_assignable(objtypwdatum->datum, yylloc);
            fieldnames[nfields] = pstrdup(ds.data);
            varnos[nfields] = objtypwdatum->dno;
            *rec = (PLpgSQL_rec *)objtypwdatum->datum;            
        } else {
            const char *message = "It is not a object type variable";
            InsertErrorMessage(message, plpgsql_yylloc);
            ereport(errstate,
                (errcode(ERRCODE_SYNTAX_ERROR),
                errmsg("\"%s\" is not a object variable", NameOfDatum(&yylval.wdatum))));
        }
    } else {
        fieldnames[nfields] = NULL;
        varnos[nfields] = -1;
    }
    pfree_ext(ds.data);
}

static void HandleSubprogram(){
    if(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile != NULL 
       && u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->proc_list != NULL){
        if(u_sess->attr.attr_sql.sql_compatibility == A_FORMAT) {
            Oid parentFuncOid = u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_oid;
            MemoryContext oldCxt = MemoryContextSwitchTo(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->fn_cxt);
            u_sess->plsql_cxt.curr_compile_context->plpgsql_subprogram_nDatums = u_sess->plsql_cxt.curr_compile_context->plpgsql_nDatums;
            ProcessSubprograms(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile->proc_list, parentFuncOid);
            (void)MemoryContextSwitchTo(oldCxt);
        } else {
            ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                           errmsg("'subprogram:' is only supported in database which dbcompatibility='A'.")));
        }
    }
}

static void HandleBlockLevel() {
    if(u_sess->plsql_cxt.curr_compile_context->plpgsql_curr_compile_package == NULL)
        u_sess->plsql_cxt.block_level++;
}
