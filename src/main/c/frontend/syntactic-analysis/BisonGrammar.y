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
	char * strVal;



	/** Non-terminals. */

	Program * program;
	Header * header;
	TimezoneDecl * timezone_decl;
	ColorList * color_list;
	ColorDef * color_def;
	YearBlock * year_block;
	MonthBlocks * month_blocks;
	MonthBlock * month_block;
	Statements * statements;
 	Statement * statement;
	EventDecl * event_decl;
	OverrideDecl * override_decl;
	EventSpec * event_spec ;
	DayList * day_list;
	EventBody * event_body;
	EventProp * event_prop;
	Time * time;
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
%destructor { destroyEventBody($$); } <event_body>
%destructor { destroyDayList($$); } <day_list>
%destructor { destroyTime($$); } <time>
%destructor { destroyEventSpec($$); } <event_spec>
%destructor { destroyEventDecl($$); } <event_decl>
%destructor { destroyOverrideDecl($$); } <override_decl>
%destructor { destroyStatements($$); } <statements>
%destructor { destroyStatement($$); } <statement>
%destructor { destroyMonthBlocks($$); } <month_blocks>
%destructor { destroyMonthBlock($$); } <month_block>
%destructor { destroyYearBlock($$); } <year_block>
%destructor { destroyColorList($$); } <color_list>
%destructor { destroyColorDef($$); } <color_def>
%destructor { destroyTimezoneDecl($$); } <timezone_decl>
%destructor { destroyHeader($$); } <header>
%destructor { destroyString($$); } <strVal>


/** Terminals. */

%token <integer> INTEGER
%token <token> LBRACE
%token <token> RBRACE

%token <token> TEST
%token <token> YEAR
%token <token> MONTH
%token <token> TIMEZONE
%token <token> EVENT
%token <token> OVERRIDE
%token <token> ON
%token <token> AND
%token <token> FROM
%token <token> TO
%token <token> AT
%token <token> SINCE
%token <token> TILL
%token <token> EVERY
%token <token> STARTING
%token <token> ONLY
%token <token> IF
%token <strVal> DAY_OF_WEEK
%token <token> DEFINE
%token <token> COLOR
%token <strVal> HEXVALUE
%token <token> DESCRIPTION
%token <token> URL
%token <strVal> STRING
%token <strVal> DATE
%token <strVal> TIME
%token <strVal> TIMEZONE_NAME
%token <strVal> MONTH_NAME
%token <strVal> IDENTIFIER
%token <strVal> URL_PATTERN

%token <token> IGNORED

/** Non-terminals. */

%type <program> program
%type <header> header
%type <timezone_decl> timezone_decl
%type <color_list> color_list
%type <color_def> color_def
%type <year_block> year_block
%type <month_blocks> month_blocks
%type <month_block> month_block
%type <statements> statements
%type <statement> statement
%type <event_decl> event_decl
%type <override_decl> override_decl
%type <event_spec> event_spec
%type <day_list> day_list
%type <event_body> event_body
%type <event_prop> event_prop
%type <time> time

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */


%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: header year_block									{ $$ = CreateProgramSemanticAction($1, $2); }
	| year_block											{ $$ = CreateProgramSemanticAction(NULL, $1); }
	;

header: timezone_decl color_list							{ $$ = createHeaderSemanticAction($1, $2); }
	| timezone_decl											{ $$ = createHeaderSemanticAction($1, NULL); }
	| color_list											{ $$ = createHeaderSemanticAction(NULL, $1); }
	;

timezone_decl: TIMEZONE TIMEZONE_NAME						{ $$ = CreateTimezoneSemanticAction($2); }
	;

color_list: color_list color_def							{ $$ = AppendColorDefSemanticAction($1, $2); }
	| color_def												{ $$ =  SingleColorDefSemanticAction($1); }
	;

color_def: DEFINE COLOR IDENTIFIER HEXVALUE					{ $$ = DefineColorSemanticAction($3, $4); }
	;

year_block: YEAR INTEGER LBRACE month_blocks RBRACE  		{ $$ = YearBlockSemanticAction($2, $4); }
	;

month_blocks: month_blocks month_block						{ $$ = AppendMonthBlockSemanticAction($1, $2); }
	| month_block											{ $$ = SingleMonthBlockSemanticAction($1); }
	;

month_block: MONTH MONTH_NAME LBRACE statements RBRACE 		{ $$ = StatementsMonthNameSemanticAction($2, $4); }
	| MONTH INTEGER LBRACE statements RBRACE				{ $$ = StatementsMonthIntegerSemanticAction($2, $4); }
	;

statements: statements statement							{ $$ = AppendStatementSemanticAction($1, $2); }
	| statement												{ $$ = SingleStatementListSemanticAction($1); }
	;

statement: event_decl										{ $$ = EventStatementSemanticAction($1); }
	| override_decl											{ $$ = OverrideStatementSemanticAction($1); }
	;

event_decl: 
	EVENT IDENTIFIER event_spec LBRACE event_body RBRACE 	{ $$ = CreateEventSemanticAction($2, $3, $5); }
	;

override_decl: OVERRIDE IDENTIFIER LBRACE event_body RBRACE		{ $$ = CreateOverrideSemanticAction($2, $4); }
	;

event_spec: ON day_list FROM time TO time					{ $$ = DayListSpecSemanticAction($2, $4, $6); }
	| ON INTEGER FROM time TO time						{ $$ = DayOfMonthSpecSemanticAction($2, $4, $6); }
	;

time:
    TIME 													{ $$ = TimeSemanticAction($1); }
    ;


day_list: DAY_OF_WEEK										{ $$ = SingleDayStringSemanticAction($1); }
	| day_list AND DAY_OF_WEEK								{ $$ = AppendDayStringSemanticAction($1, $3); }
	;

event_body: event_body event_prop							{ $$ = EventBodyAppendSemanticAction($1, $2); }
	| event_prop											{ $$ = EventBodySingleSemanticAction($1); }
	;

event_prop: COLOR IDENTIFIER								{ $$ = ColorSemanticAction($2); }
	| DESCRIPTION STRING									{ $$ = DescriptionSemanticAction($2); }
	| URL STRING											{ $$ = UrlSemanticAction($2); }
	;



%%

