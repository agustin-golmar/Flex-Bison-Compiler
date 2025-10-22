%{

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
	/** Terminals. */

	signed int integer;
	TokenLabel token;
	char * strVal;


	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;

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
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

%destructor { destroyEvent($$); } <event_decl>
%destructor { destroyStatement($$); } <statement>

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
%token <token> DAY_OF_MONTH
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
%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor

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

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: header year_block									{ $$ = HeaderProgramSemanticAction($1, $2); }
	| year_block											{ $$ = YearProgramSemanticAction(); }
	;

header: timezone_decl color_list							{ $$ = TODO; }
	| timezone_decl											{ $$ = TODO; }
	| color_list											{ $$ = TODO; }
	;

timezone_decl: TIMEZONE TIMEZONE_NAME						{ $$ = CreateTimezoneSemanticAction($2); }
	;

color_list: color_list color_def							{ $$ = TODO; }
	| color_def												{ $$ = TODO; }
	;

color_def: DEFINE COLOR IDENTIFIER HEXVALUE					{ $$ = DefineColorSemanticAction($3, $4); }
	;

year_block: YEAR INTEGER LBRACE month_blocks RBRACE  		{ $$ = YearBlockSemanticAction($2, $4); }
	;

month_blocks: month_blocks month_block						{ $$ = TODO; }
	| month_block											{ $$ = TODO; }
	;

month_block: MONTH MONTH_NAME LBRACE statements RBRACE 		{ $$ = StatementsMonthNameSemanticAction($2, $4); }
	| MONTH INTEGER LBRACE statements RBRACE				{ $$ = StatementsMonthIntegerSemanticAction($2, $4); }
	;

statements: statements statement							{ $$ = TODO; }
	| statement												{ $$ = TODO; }
	;

statement: event_decl										{ $$ = EventStatementSemanticAction($1); }
	| override_decl											{ $$ = OverrideStatementSemanticAction($1); }
	;

event_decl: 
	EVENT IDENTIFIER event_spec LBRACE event_body RBRACE 	{ $$ = CreateEventSemanticAction($2, $3, $5); }
	;

override_decl: OVERRIDE IDENTIFIER LBRACE event_body RBRACE		{ $$ = CreateOverrideSemanticAction($2, $4); }
	;

event_spec: ON day_list FROM TIME TO TIME					{ $$ = EventSpecificsSemanticAction($2, $4, $6); }
	| ON day_list AT TIME 									{ $$ = PuntualEventSemanticAction($2, $4); }
	| EVERY INTEGER day_list STARTING FROM INTEGER		{ $$ = RecurrentEventSemanticAction($2, $3, $6); }
	| /* vacío (mmmmmm) */
	;

day_list: DAY_OF_WEEK										{ $$ = CreateDayListSemanticAction($1); }
	| day_list AND DAY_OF_WEEK								{ $$ = CreateDayListRecSemanticAction($1, $3); }
	;

event_body: event_body event_prop							{ $$ = TODO }
	| event_prop											{ $$ = TODO }
	;

event_prop: COLOR IDENTIFIER								{ $$ = ColorSemanticAction($2); }
	| DESCRIPTION STRING									{ $$ = DescriptionSemanticAction($2); }
	| URL STRING											{ $$ = UrlSemanticAction($2); }
	;



//--------------------VIEJO-----------------------
expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
  |  TEST                           { $$ = TestConstantSemanticAction(); }
	;

%%
