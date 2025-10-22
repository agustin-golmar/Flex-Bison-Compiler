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
	Statement * statement;
	Event * event_decl;
	ColorDeclaration * color_decl;
	Replace * replace_statement;
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
%token <integer> INTEGER
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
%token <token> DAY_OF_WEEK
%token <token> COLOR
%token <token> DESCRIPTION
%token <strVal> STRING
%token <strVal> DATE
%token <strVal> DAYTIME

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

%type <statement> statement
%type <color_decl> color_decl
%type <event_decl> event_decl
%type <replace_statement> replace_statement

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

program: statement											{ $$ = StatementProgramSemanticAction($1); }
	;

statement: color_decl										{ $$ = TestConstantSemanticAction(); }
	| event_decl											{ $$ = EventStatementSemanticAction($1); }
	| replace_statement										{ $$ = TestConstantSemanticAction(); }
	;

event_decl: 
	EVENT STRING 											{ $$ = CreateEventSemanticAction($2); }
	;

color_decl: COLOR											{ $$ = TestConstantSemanticAction(); }
	;

replace_statement: COLOR									{ $$ = TestConstantSemanticAction(); }
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
