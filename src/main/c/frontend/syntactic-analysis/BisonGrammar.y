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
	char * alphanum;
	char * string;
	TokenLabel token;

	/** Non-terminals. */

	BlockDeclaration * blockDeclaration;
	ClassDeclaration * classDeclaration;
	ClassBody * classBody;
	MemberDeclaration * memberDeclaration;
	MethodDeclaration * methodDeclaration;
	FieldDeclaration * fieldDeclaration;
	Statement * statement;
	Expression * expression;
	TypeSpecifier * typeSpecifier;
	AccessSpecifier * accessSpecifier;
	Parameter * parameter;
	ArgumentList * argumentList;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyBlockDeclaration($$); } <blockDeclaration>
%destructor { destroyClassDeclaration($$); } <classDeclaration>
%destructor { destroyClassBody($$); } <classBody>
%destructor { destroyMemberDeclaration($$); } <memberDeclaration>
%destructor { destroyMethodDeclaration($$); } <methodDeclaration>
%destructor { destroyFieldDeclaration($$); } <fieldDeclaration>
%destructor { destroyStatement($$); } <statement>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyTypeSpecifier($$); } <typeSpecifier>
%destructor { destroyParameter($$); } <parameter>
%destructor { destroyArgumentList($$); } <argumentList>

/** Terminals. */
%token <integer> INTEGER
%token <alphanum> IDENTIFIER
%token <string> STRING
%token <token> ADD
%token <token> SUB
%token <token> MUL
%token <token> DIV
%token <token> GREATER_THAN
%token <token> LOWER_THAN
%token <token> GREATER_OR_EQUAL_THAN
%token <token> LOWER_OR_EQUAL_THAN
%token <token> EQUAL
%token <token> NOT_EQUAL
%token <token> LOGICAL_AND
%token <token> LOGICAL_OR
%token <token> ASSIGN
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> SEMICOLON
%token <token> COMMA
%token <token> ARROW
%token <token> DOT
%token <token> CLASS
%token <token> IF
%token <token> ELSE
%token <token> FOR
%token <token> WHILE
%token <token> DO
%token <token> NEW
%token <token> RETURN
%token <token> PRIVATE
%token <token> PUBLIC
%token <token> THIS
%token <token> STATIC
%token <token> CONSTRUCTOR
%token <token> DESTRUCTOR
%token <token> INT
%token <token> VOID
%token <token> CHAR

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <blockDeclaration> blockDeclaration blockDeclarationList
%type <classDeclaration> classDeclaration
%type <classBody> classBody
%type <memberDeclaration> member memberDeclaration memberList
%type <methodDeclaration> methodDeclaration constructorDeclaration destructorDeclaration
%type <fieldDeclaration> fieldDeclaration
%type <statement> statement statementList compoundStatement declarationStatement expressionStatement returnStatement ifStatement forStatement whileStatement doWhileStatement
%type <expression> expression assignmentExpression additiveExpression multiplicativeExpression unaryExpression postfixExpression primaryExpression
%type <typeSpecifier> typeSpecifier 
%type <accessSpecifier> accessSpecifier
%type <parameter> parameter parameterList parameters
%type <argumentList> argumentList arguments

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%right ASSIGN
%left ADD SUB
%left MUL DIV
%left ARROW DOT

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: blockDeclarationList									{ $$ = BlockProgramSemanticAction($1); }
	;

blockDeclarationList: blockDeclaration						{ $$ = SingleBlockDeclarationSemanticAction($1); }
	| blockDeclarationList blockDeclaration					{ $$ = MultipleBlockDeclarationSemanticAction($1, $2); }
	;

blockDeclaration: classDeclaration							{ $$ = ClassBlockDeclarationSemanticAction($1); }
	| methodDeclaration										{ $$ = MethodBlockDeclarationSemanticAction($1); }
	;

classDeclaration: CLASS IDENTIFIER OPEN_BRACE classBody CLOSE_BRACE	{ $$ = ClassDeclarationSemanticAction($2, $4); }
	;

classBody: %empty												{ $$ = EmptyClassBodySemanticAction(); }
	| memberList												{ $$ = MemberListClassBodySemanticAction($1); }
	;

memberList: member												{ $$ = SingleMemberSemanticAction($1); }
	| memberList member										{ $$ = MultipleMemberSemanticAction($1, $2); }
	;

member: accessSpecifier memberDeclaration						{ $$ = AccessSpecifiedMemberSemanticAction($1, $2); }
	| memberDeclaration										{ $$ = DefaultAccessMemberSemanticAction($1); }
	;

accessSpecifier: PRIVATE										{ $$ = PrivateAccessSemanticAction(); }
	| PUBLIC													{ $$ = PublicAccessSemanticAction(); }
	;

memberDeclaration: fieldDeclaration							{ $$ = FieldMemberSemanticAction($1); }
	| methodDeclaration										{ $$ = MethodMemberSemanticAction($1); }
	| constructorDeclaration									{ $$ = ConstructorMemberSemanticAction($1); }
	| destructorDeclaration									{ $$ = DestructorMemberSemanticAction($1); }
	;

fieldDeclaration: typeSpecifier IDENTIFIER ASSIGN expression SEMICOLON	{ $$ = InitializedFieldDeclarationSemanticAction($1, $2, $4); }
	| typeSpecifier IDENTIFIER SEMICOLON			{ $$ = FieldDeclarationSemanticAction($1, $2); }
	| STATIC typeSpecifier IDENTIFIER ASSIGN expression SEMICOLON	{ $$ = InitializedStaticFieldDeclarationSemanticAction($2, $3, $5); }
	| STATIC typeSpecifier IDENTIFIER SEMICOLON				{ $$ = StaticFieldDeclarationSemanticAction($2, $3); }
	;

methodDeclaration: typeSpecifier IDENTIFIER OPEN_PARENTHESIS parameterList CLOSE_PARENTHESIS OPEN_BRACE statementList CLOSE_BRACE
																{ $$ = MethodDeclarationSemanticAction($1, $2, $4, $7); }
	| STATIC typeSpecifier IDENTIFIER OPEN_PARENTHESIS parameterList CLOSE_PARENTHESIS OPEN_BRACE statementList CLOSE_BRACE
																{ $$ = StaticMethodDeclarationSemanticAction($2, $3, $5, $8); }
	;

constructorDeclaration: CONSTRUCTOR OPEN_PARENTHESIS parameterList CLOSE_PARENTHESIS OPEN_BRACE statementList CLOSE_BRACE
																{ $$ = ConstructorDeclarationSemanticAction($3, $6); }
	;

destructorDeclaration: DESTRUCTOR OPEN_PARENTHESIS CLOSE_PARENTHESIS OPEN_BRACE statementList CLOSE_BRACE
																{ $$ = DestructorDeclarationSemanticAction($5); }
	;

parameterList: %empty											{ $$ = EmptyParameterListSemanticAction(); }
	| parameters												{ $$ = ParametersSemanticAction($1); }
	;

parameters: parameter											{ $$ = SingleParameterSemanticAction($1); }
	| parameters COMMA parameter								{ $$ = MultipleParameterSemanticAction($1, $3); }
	;

parameter: typeSpecifier IDENTIFIER								{ $$ = ParameterSemanticAction($1, $2); }
	;

typeSpecifier: INT												{ $$ = IntTypeSemanticAction(); }
	| VOID														{ $$ = VoidTypeSemanticAction(); }
	| CHAR														{ $$ = CharTypeSemanticAction(); }
	| STRING													{ $$ = StringTypeSemanticAction(); }															
	| IDENTIFIER												{ $$ = IdentifierTypeSemanticAction($1); }
	;

statementList: %empty											{ $$ = EmptyStatementListSemanticAction(); }
	| statementList statement									{ $$ = StatementListSemanticAction($1, $2); }
	;

statement: expressionStatement									{ $$ = ExpressionStatementSemanticAction($1); }
	| declarationStatement										{ $$ = DeclarationStatementSemanticAction($1); }
	| returnStatement											{ $$ = ReturnStatementSemanticAction($1); }
	| compoundStatement											{ $$ = CompoundStatementSemanticAction($1); }
	| ifStatement												{ $$ = IfStatementSemanticAction($1); }
	| forStatement												{ $$ = ForStatementSemanticAction($1); }
	| whileStatement											{ $$ = WhileStatementSemanticAction($1); }
	| doWhileStatement											{ $$ = DoWhileStatementSemanticAction($1); }
;

ifStatement: IF OPEN_PARENTHESIS expression CLOSE_PARENTHESIS statement 	{ $$ = IfStatementBodySemanticAction($3, $5); }
	| ifStatement ELSE statement							{ $$ = IfElseStatementSemanticAction($1, $3); }
	;

forStatement: FOR OPEN_PARENTHESIS declarationStatement[initialization] expression[condition] SEMICOLON expression[increment] CLOSE_PARENTHESIS statement[statement]
		{ $$ = ForStatementBodySemanticAction($initialization, $condition, $increment, $statement); }
	;

whileStatement: WHILE OPEN_PARENTHESIS expression CLOSE_PARENTHESIS statement
		{ $$ = WhileStatementBodySemanticAction($3, $5); }
	;

doWhileStatement: DO statement WHILE OPEN_PARENTHESIS expression CLOSE_PARENTHESIS SEMICOLON
		{ $$ = DoWhileStatementBodySemanticAction($2, $5); };
	;

compoundStatement: OPEN_BRACE statementList CLOSE_BRACE		{ $$ = CompoundStatementBodySemanticAction($2); }
	;

declarationStatement: typeSpecifier IDENTIFIER SEMICOLON		{ $$ = VariableDeclarationSemanticAction($1, $2); }
	| typeSpecifier IDENTIFIER ASSIGN expression SEMICOLON		{ $$ = InitializedVariableDeclarationSemanticAction($1, $2, $4); }
	;

expressionStatement: expression SEMICOLON						{ $$ = ExpressionStatementBodySemanticAction($1); }
	;

returnStatement: RETURN expression SEMICOLON					{ $$ = ReturnExpressionSemanticAction($2); }
	| RETURN SEMICOLON											{ $$ = ReturnVoidSemanticAction(); }
	;

expression: assignmentExpression								{ $$ = AssignmentExpressionSemanticAction($1); }
	;

assignmentExpression: additiveExpression						{ $$ = AdditiveExpressionSemanticAction($1); }
	| unaryExpression ASSIGN assignmentExpression				{ $$ = AssignmentSemanticAction($1, $3); }
	| additiveExpression GREATER_THAN assignmentExpression			{ $$ = GreaterThanSemanticAction($1, $3); }
	| additiveExpression LOWER_THAN assignmentExpression			{ $$ = LowerThanSemanticAction($1, $3); }
	| additiveExpression GREATER_OR_EQUAL_THAN assignmentExpression	{ $$ = GreaterEqualThanSemanticAction($1, $3); }
	| additiveExpression LOWER_OR_EQUAL_THAN assignmentExpression	{ $$ = LowerEqualThanSemanticAction($1, $3); }
	| additiveExpression EQUAL assignmentExpression				{ $$ = EqualSemanticAction($1, $3); }
	| additiveExpression NOT_EQUAL assignmentExpression			{ $$ = NotEqualSemanticAction($1, $3); }
	| additiveExpression LOGICAL_AND assignmentExpression			{ $$ = LogicalAndSemanticAction($1, $3); }
	| additiveExpression LOGICAL_OR assignmentExpression			{ $$ = LogicalOrSemanticAction($1, $3); }
	;

additiveExpression: multiplicativeExpression					{ $$ = MultiplicativeExpressionSemanticAction($1); }
	| additiveExpression[left] ADD multiplicativeExpression[right]	{ $$ = AdditionSemanticAction($left, $right); }
	| additiveExpression[left] SUB multiplicativeExpression[right]	{ $$ = SubtractionSemanticAction($left, $right); }
	;

multiplicativeExpression: unaryExpression						{ $$ = UnaryExpressionSemanticAction($1); }
	| multiplicativeExpression[left] MUL unaryExpression[right]		{ $$ = MultiplicationSemanticAction($left, $right); }
	| multiplicativeExpression[left] DIV unaryExpression[right]		{ $$ = DivisionSemanticAction($left, $right); }
	;

unaryExpression: postfixExpression							{ $$ = PostfixExpressionSemanticAction($1); }
	| SUB unaryExpression										{ $$ = NegationSemanticAction($2); }
	;

postfixExpression: primaryExpression							{ $$ = PrimaryExpressionSemanticAction($1); }
	| postfixExpression ARROW IDENTIFIER						{ $$ = MemberAccessSemanticAction($1, $3); }
	| postfixExpression DOT IDENTIFIER							{ $$ = MemberAccessSemanticAction($1, $3); }
	| postfixExpression OPEN_PARENTHESIS argumentList CLOSE_PARENTHESIS	{ $$ = FunctionCallSemanticAction($1, $3); }
	;

primaryExpression: IDENTIFIER									{ $$ = IdentifierExpressionSemanticAction($1); }
	| INTEGER													{ $$ = IntegerExpressionSemanticAction($1); }
	| THIS														{ $$ = ThisExpressionSemanticAction(); }
	| OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ParenthesizedExpressionSemanticAction($2); }
	| NEW IDENTIFIER OPEN_PARENTHESIS argumentList CLOSE_PARENTHESIS	{ $$ = NewExpressionSemanticAction($2, $4); }
	;

argumentList: %empty											{ $$ = EmptyArgumentListSemanticAction(); }
	| arguments													{ $$ = ArgumentsSemanticAction($1); }
	;

arguments: expression											{ $$ = SingleArgumentSemanticAction($1); }
	| arguments COMMA expression								{ $$ = MultipleArgumentSemanticAction($1, $3); }
	;

%%