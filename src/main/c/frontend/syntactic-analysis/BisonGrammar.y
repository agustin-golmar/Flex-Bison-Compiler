%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {

    signed int integer;
	TokenLabel token;

	char* strVal;
    int intVal;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 *
 * Por ahora no tengo destructores.
 */


/** Terminals. */

%token <token> ADD
%token <token> CLOSE_BRACE
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_BRACE
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
%token <token> SUB

%token <token> IGNORED

%token <strVal> IDENTIFIER
%token <strVal> HOUR
%token <strVal> STRING
%token YEAR MONTH TIMEZONE
%token EVENT OVERRIDE
%token ON AND FROM TO AT SINCE TILL
%token EVERY STARTING ONLY IF
%token COLOR DESCRIPTION
%token DAY_OF_MONTH DAY_OF_WEEK
%token <strVal> WEEKDAY MONTH_NAME TIMEZONE_NAME
%token <strVal> URL
%token <intVal> INTEGER
%token LBRACE RBRACE
%token UNKNOWN


/** Non-terminals. */
%type <strVal> timezone_decl year_decl month_decl event_decl override_decl
%type <strVal> event_body recurrence
%type <strVal> expr_list expr time_range time_expr day_list
%type <strVal> event_spec
%type <strVal> event_prop


%%
program:
      timezone_decl year_block
    | year_block
    ;

timezone_decl:
      TIMEZONE TIMEZONE_NAME                  { printf("Definición de zona horaria: %s\n", $2); free($2); }
    ;

year_block:
      YEAR INTEGER LBRACE month_blocks RBRACE { printf("Año %d definido\n", $2); }
    ;

month_blocks:
      month_blocks month_block
    | month_block
    ;

month_block:
      MONTH MONTH_NAME LBRACE statements RBRACE
        {
            printf("Mes definido: %s\n", $2);
        }
    | MONTH INTEGER LBRACE statements RBRACE
        {
            printf("Mes numérico definido: %d\n", $2);
        }
    ;

statements:
      statements statement
    | statement
    ;

statement:
      event_decl
    | override_decl
    ;

event_decl:
      EVENT IDENTIFIER event_spec LBRACE event_body RBRACE
        { printf("Evento '%s' definido\n", $2); free($2); }
    ;

override_decl:
      OVERRIDE IDENTIFIER LBRACE event_body RBRACE
        { printf("Override del evento '%s'\n", $2); free($2); }
    ;

event_spec:
      ON day_list FROM HOUR TO HOUR           { printf("Días y rango horario definidos: %s - %s\n", $4, $6); free($4); free($6); }
    | ON day_list AT HOUR                     { printf("Evento puntual a las %s\n", $4); free($4); }
    | EVERY INTEGER DAY_OF_WEEK STARTING FROM INTEGER { printf("Evento recurrente cada %d semanas\n", $2); }
    | /* vacío */
    ;

day_list:
      WEEKDAY
    | day_list AND WEEKDAY                    { /* ejemplo: Monday and Friday */ }
    ;

event_body:
      event_body event_prop
    | event_prop
    ;

event_prop:
      COLOR IDENTIFIER                        { printf("Color: %s\n", $2); free($2); }
    | DESCRIPTION STRING                      { printf("Descripción: %s\n", $2); free($2); }
    | URL                                     { printf("Link detectado: %s\n", $1); free($1); }
    ;

%%

