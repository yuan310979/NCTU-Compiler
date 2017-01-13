%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
int yyerror(char* msg);

%}

%token SEMICOLON    /* ; */
%token COMMA        /* , */
%token LEFT_PARENTHESES RIGHT_PARENTHESES        /* () */
%token LEFT_SQUARE RIGHT_SQUARE        /* [] */
%token LEFT_BRACE RIGHT_BRACE        /* {} */

%token ID           /* identifier */

%token INT          /* keyword */
%token WHILE
%token DO
%token IF
%token TRUE
%token FALSE
%token FOR
%token PRINT
%token CONST
%token READ
%token BOOL
%token VOID
%token FLOAT
%token DOUBLE
%token STRING
%token CONTINUE
%token BREAK
%token RETURN

%nonassoc ELSE

%token STRING_LIT
%token INT_LIT
%token FLOAT_LIT
%token SCIENTIFIC

%right '='
%left OR
%left AND
%right '!'
%left EQ NE GT GE LT LE
%left '+' '-'
%left '*' '/' '%'


%%

program : declaration_list funct_defi decl_and_def_list
	;

decl_and_def_list	: decl_and_def_list funct_decl
			| decl_and_def_list var_decl
			| decl_and_def_list const_decl
			| decl_and_def_list funct_defi
			|
			;

declaration_list : declaration_list const_decl
                 | declaration_list var_decl
                 | declaration_list funct_decl
                 | 
                 ;

/*
definition_list : definition_list funct_defi
                | funct_defi
                ;
*/

funct_defi : type ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES compound
           | procedure_definition
           ;

procedure_definition : VOID ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES compound
                     ;

statement : compound 
          | simple
          | conditional
          | while
          | for
          | jump
          ;

/*statement_list : statement_list statement
               | statement
               ;
*/
compound : LEFT_BRACE compound_content RIGHT_BRACE
         ;

simple : simple_content SEMICOLON
       ;


simple_content : variable_reference '=' expression
               | PRINT expression
               | READ variable_reference
               | expression
               ;

expression : expression OR expression
           | expression AND expression
           | '!' expression
           | expression LE expression
           | expression LT expression
           | expression GT expression
           | expression GE expression
           | expression EQ expression
           | expression NE expression
           | expression '+' expression
           | expression '-' expression
           | expression '%' expression
           | expression '*' expression
           | expression '/' expression
           | '-' expression %prec '*'
           | LEFT_PARENTHESES expression RIGHT_PARENTHESES %prec '*'
           | literal_constant
           | variable_reference
           | function_invocation
           ;

function_invocation : ID LEFT_PARENTHESES expression_list RIGHT_PARENTHESES
                    ;

expression_list : nonEmptyExpressionList
                |
                ;

nonEmptyExpressionList : nonEmptyExpressionList COMMA expression
                       | expression
                       ;

variable_reference : ID 
                   | array_reference
                   ;

array_reference : ID arr_reference_square
                ;

arr_reference_square : arr_reference_square square_expression
                     | square_expression
                     ;
square_expression : LEFT_SQUARE expression RIGHT_SQUARE
                  ;

conditional : IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound ELSE compound 
            | IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound
            ;

boolean_expression : expression
                   ;

while : WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound
      | DO compound WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES SEMICOLON
      ;

for : FOR LEFT_PARENTHESES initial_expression SEMICOLON control_expression SEMICOLON increment_expression RIGHT_PARENTHESES compound
    ;

jump : RETURN expression SEMICOLON
     | BREAK SEMICOLON
     | CONTINUE SEMICOLON
     ;

initial_expression : ID '=' expression
                   | expression
                   ;

control_expression : ID '=' expression
                   | expression
                   ;

increment_expression : ID '=' expression
                     | expression
                     ;

compound_content : compound_content const_decl
                 | compound_content var_decl
                 | compound_content statement
                 | 
                 ;

/*local_declaration_list : local_declaration_list const_decl
                       | local_declaration_list var_decl
                       | const_decl
                       | var_decl
                       ;*/


const_decl : CONST type const_list SEMICOLON
           ;

const_list : const_list COMMA const
           | const
           ;

const : ID '=' literal_constant
      ;

literal_constant : INT_LIT
                 | STRING_LIT
                 | FLOAT_LIT
                 | SCIENTIFIC
                 | TRUE
                 | FALSE
                 ;

var_decl : type identifier_list SEMICOLON
         ;

type : INT
     | DOUBLE
     | FLOAT
     | STRING
     | BOOL
     ; 

identifier_list : identifier_list COMMA identifier
                | identifier
                ;

identifier : identifier_no_initial
           | identifier_with_initial
           ;

identifier_no_initial : ID
                      | ID array
                      ;
identifier_with_initial : ID '=' expression
                        | ID array '=' initial_array
                        ;

initial_array : LEFT_BRACE expression_list RIGHT_BRACE
              ;

array : array LEFT_SQUARE INT_LIT RIGHT_SQUARE
      | LEFT_SQUARE INT_LIT RIGHT_SQUARE
      ;

funct_decl : type ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES SEMICOLON
           | procedure_decl
           ;

procedure_decl : VOID ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES SEMICOLON
               ;

argument_list : nonEmptyArgumentList
              |
              ;
nonEmptyArgumentList : nonEmptyArgumentList COMMA argument
                     | argument
                     ;
argument : type identifier_no_initial
         ;

%%

int yyerror( char *msg )
{
  fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
  fprintf( stderr, "|--------------------------------------------------------------------------\n" );
  exit(-1);
}

int  main( int argc, char **argv )
{
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

	FILE *fp = fopen( argv[1], "r" );
	
	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}
	
	yyin = fp;
	yyparse();

	fprintf( stdout, "\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	exit(0);
}
