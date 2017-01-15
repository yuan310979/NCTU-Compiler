%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"
#include "codeGenerate.h"

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */

extern int Opt_Statistic;           /* declared in lex.l */
extern void create_id(char*);
extern void find_id(char*);
extern void print_id();
extern int yylex(void);

int yyerror(char* msg);

IdList* tmp_idlist;
TypeList* tmp_typelist;
ArraySig* tmp_arraysig;
SymbolTable* symbol_table;
Type* return_buf;
Type* globaldecl_buf;

int arraysig_isnew;
int current_dim;
int has_return=0;
int error = 0;
int is_forwhile = 0;
int is_entryfunc = 1;

FILE *fpout;

%}

%token <str> SEMICOLON    /* ; */
%token <str> COMMA        /* , */
%token <str> LEFT_PARENTHESES RIGHT_PARENTHESES        /* () */
%token <str> LEFT_SQUARE RIGHT_SQUARE        /* [] */
%token <str> LEFT_BRACE RIGHT_BRACE        /* {} */

%token <str> ID           /* identifier */

%token <str> INT          /* keyword */
%token <str> WHILE
%token <str> DO
%token <str> IF
%token <str> TRUE
%token <str> FALSE
%token <str> FOR
%token <str> PRINT
%token <str> CONST
%token <str> READ
%token <str> BOOL
%token <str> VOID
%token <str> FLOAT
%token <str> DOUBLE
%token <str> STRING
%token <str> CONTINUE
%token <str> BREAK
%token <str> RETURN

%nonassoc <str> ELSE

%token <str> STRING_LIT
%token <num> INT_LIT
%token <dnum> FLOAT_LIT
%token <str> SCIENTIFIC

%right <str> ASSIGN
%left <str> OR
%left <str> AND
%right <str> NOT
%left <str> EQ NE GT GE LT LE
%left <str> PLUS MINUS
%left <str> MUL DIV MOD

%type <type> type 
%type <value> literal_constant
%type <typelist> argument
%type <typelist> nonEmptyArgumentList
%type <typelist> argument_list
%type <expr> expression
%type <expr> variable_reference
%type <expr> array_reference
%type <exprlist> nonEmptyExpressionList
%type <exprlist> expression_list
%type <expr> function_invocation
%type <exprlist> initial_array
%type <expr> print
/*      types       */
%union  {
  int num;
  double dnum;
  char* str;
  struct Type* type;
  struct Value* value;
  struct Attribute* attribute;
  struct TypeList* typelist;
  struct Expr* expr;
  struct ExprList* exprlist;
}


%%

program : declaration_list funct_defi decl_and_def_list             { PrintSymbolTable(symbol_table);}
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

funct_defi : type ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES LEFT_BRACE
            {
              return_buf = $1;
              if(FindEntryInGlobal(symbol_table, $2) == NULL){
                TableEntry* new_entry = BuildTableEntry($2, "function", symbol_table->current_level, $1, BuildFunctAttribute($4));
                InsertTableEntry(symbol_table, new_entry);
              } 
              TableEntry* tmp = FindEntryInGlobal(symbol_table, $2);
              CheckHasDef(tmp->has_def);
              if(tmp->has_def == 0){
                tmp->has_def = 1;
                symbol_table->current_level++;
                InsertTableEntryFromTypeList(symbol_table, tmp_idlist, "parameter", $4);
              }
              GenFuncInitialization($2);
              ResetIdList(tmp_idlist);
            }
            compound_content RIGHT_BRACE
            {
              CheckReturnLastLine(has_return);
              return_buf = NULL;
              has_return = 0;
              PrintSymbolTable(symbol_table); 
              PopTableEntry(symbol_table);
              symbol_table->current_level--;
              is_entryfunc = 0;
              GenFuncEnd($1);
            }
           | procedure_definition
           ;

procedure_definition : VOID ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES LEFT_BRACE
            {
              return_buf = BuildType($1);
              if(FindEntryInGlobal(symbol_table, $2) == NULL){
                TableEntry* new_entry = BuildTableEntry($2, "function", symbol_table->current_level, BuildType($1), BuildFunctAttribute($4));
                InsertTableEntry(symbol_table, new_entry);
              } 
              TableEntry* tmp = FindEntryInGlobal(symbol_table, $2);
              CheckHasDef(tmp->has_def);
              if(tmp->has_def == 0){
                tmp->has_def = 1;
                symbol_table->current_level++; 
                InsertTableEntryFromTypeList(symbol_table, tmp_idlist, "parameter", $4);
              }
              GenFuncInitialization($2);
              ResetIdList(tmp_idlist);
            }
            compound_content RIGHT_BRACE
            {
              has_return = 0;
              PrintSymbolTable(symbol_table); 
              PopTableEntry(symbol_table);
              symbol_table->current_level--;
              is_entryfunc = 0;
              GenFuncEnd("void");
            }
            ;

statement : compound              {has_return = 0;}
          | simple                {has_return = 0;}
          | conditional           {has_return = 0;}
          | while                 {has_return = 0;}
          | for                   {has_return = 0;}
          | jump
          ;

/*statement_list : statement_list statement
               | statement
               ;
*/
compound : LEFT_BRACE                       { symbol_table->current_level++; }
           compound_content RIGHT_BRACE     { PrintSymbolTable(symbol_table); PopTableEntry(symbol_table); symbol_table->current_level--;}
         ;

simple : simple_content SEMICOLON
       ;


simple_content : variable_reference ASSIGN expression     {CheckType($1, $3); GenVarStore($1);}
               | print expression                         {CheckRW($2); GenPrintInvoke($2);}
               | read variable_reference                  {CheckRW($2); GenReadInvoke($2);}
               | expression
               ;

print : PRINT { GenPrintInitialization(); }
      ;

read : READ
     ;

expression : expression OR expression                {$$ = LogicOp($1, $3, $2); GenLogicOp($2); }        //both sides are booleans => bool
           | expression AND expression               {$$ = LogicOp($1, $3, $2); GenLogicOp($2);}
           | NOT expression                          {$$ = NotOp($2); GenNotOp();}
           | expression LE expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}          //both sides are int float double => bool
           | expression LT expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}
           | expression GT expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}
           | expression GE expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}
           | expression EQ expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}
           | expression NE expression                {$$ = RelOp($1, $3, $2); GenRelOp($1, $3, $2);}
           | expression PLUS expression              {$$ = ArithOp($1, $3, '+'); GenArithOp($1, $3, '+'); }     //both sides are int float double => int float double
           | expression MINUS expression             {$$ = ArithOp($1, $3, '-'); GenArithOp($1, $3, '-');}
           | expression MOD expression               {$$ = ModOp($1, $3); GenModOp();}              // both sides are int => int
           | expression MUL expression               {$$ = ArithOp($1, $3, '*'); GenArithOp($1, $3, '*');}
           | expression DIV expression               {$$ = ArithOp($1, $3, '/'); GenArithOp($1, $3, '/');}
           | MINUS expression %prec MUL              {$$ = NegativeOp($2); GenNegativeOp($2);}           //should be int float double => int float double
           | LEFT_PARENTHESES expression RIGHT_PARENTHESES %prec MUL    { $$ = $2; }
           | literal_constant                 { $$ = ConstExpr($1); GenLiteralConstant($1);}
           | variable_reference               { $$ = $1; GenVarRef($1); }
           | function_invocation              { $$ = $1; }
           ;

function_invocation : ID LEFT_PARENTHESES expression_list RIGHT_PARENTHESES       {$$ = FunctionCall($1, $3, symbol_table);}
                    ;

expression_list : nonEmptyExpressionList  { $$ = $1; }
                |                         { $$ = NULL; }
                ;

nonEmptyExpressionList : nonEmptyExpressionList COMMA expression  { $$ = BuildExprList($1, $3); }
                       | expression                               { $$ = BuildExprList(NULL, $1); }
                       ;

variable_reference : ID                 { $$ = FindVarRef(symbol_table, $1);}
                   | array_reference    { $$ = $1;}
                   ;

array_reference : ID arr_reference_square   { $$ = FindVarRef(symbol_table, $1); $$->current_dimension = current_dim; current_dim = 0;}
                ;

arr_reference_square : arr_reference_square square_expression   { current_dim++; }
                     | square_expression                        { current_dim++; }
                     ; 
square_expression : LEFT_SQUARE expression RIGHT_SQUARE
                  ;

conditional : IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound ELSE compound 
            | IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound
            ;

boolean_expression : expression     {CheckBool($1);}
                   ;

while : WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES
        {
          is_forwhile = 1;
        }
        compound
        {
          is_forwhile = 0;
        }
      | DO
        {
          is_forwhile = 1;
        } 
        compound WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES SEMICOLON
        {
          is_forwhile = 0;
        }
      ;

for : FOR LEFT_PARENTHESES initial_expression SEMICOLON control_expression SEMICOLON increment_expression RIGHT_PARENTHESES
      {
        is_forwhile = 1;
      }
      compound
      {
        is_forwhile = 0;
      }
    ;

jump : RETURN expression SEMICOLON        {CheckFuncReturn(return_buf, $2); has_return = 1; GenReturn($2);}
     | BREAK SEMICOLON                    {CheckForWhile(is_forwhile);}
     | CONTINUE SEMICOLON                 {CheckForWhile(is_forwhile);}
     ;

initial_expression : ID ASSIGN expression
                   | expression
                   ;

control_expression : ID ASSIGN expression       {CheckBool($3);}
                   | expression                 {CheckBool($1);}
                   ;

increment_expression : ID ASSIGN expression
                     | expression
                     ;

compound_content : compound_content const_decl            {has_return = 0;}
                 | compound_content var_decl              {has_return = 0;}
                 | compound_content statement
                 | 
                 ;

/*local_declaration_list : local_declaration_list const_decl
                       | local_declaration_list var_decl
                       | const_decl
                       | var_decl
                       ;*/


const_decl : CONST type const_list SEMICOLON
{
  InsertTableEntryFromList(symbol_table, tmp_idlist, "constant", $2);
  //printf("%d\n", tmp_idlist->attributes[0]->val->ival);
  ResetIdList(tmp_idlist);
}
;

const_list : const_list COMMA const
           | const
           ;

const : ID ASSIGN literal_constant  
{
  //printf("%s: %f\n", $3->type->name, $3->dval);
  Attribute* tmp = BuildConstAttribute($3);
  InsertIdList(tmp_idlist, $1, NULL, tmp);
}
;

literal_constant : INT_LIT        {$$=BuildValue("int",yytext);}
                 | STRING_LIT     {$$=BuildValue("string",$1);}
                 | FLOAT_LIT      {$$=BuildValue("float",yytext);}
                 | SCIENTIFIC     {$$=BuildValue("scientific",yytext);}
                 | TRUE           {$$=BuildValue("bool",yytext);}
                 | FALSE          {$$=BuildValue("bool",yytext);}
                 | MINUS INT_LIT  {$$=BuildNegValue("int",yytext);}
                 | MINUS FLOAT_LIT {$$=BuildNegValue("float",yytext);}
                 | MINUS SCIENTIFIC   {$$=BuildNegValue("scientific",yytext);}
                 ;

var_decl : type identifier_list SEMICOLON {
                                              InsertTableEntryFromList(symbol_table, tmp_idlist, "variable", $1);
                                              GenGlobalVarFromList(tmp_idlist);
                                              ResetIdList(tmp_idlist);
                                          }
                                          ;

type : INT        {$$ = BuildType("int"); globaldecl_buf=$$;}
     | DOUBLE     {$$ = BuildType("double"); globaldecl_buf=$$;}
     | FLOAT      {$$ = BuildType("float"); globaldecl_buf=$$;}
     | STRING     {$$ = BuildType("string"); globaldecl_buf=$$;}
     | BOOL       {$$ = BuildType("bool"); globaldecl_buf=$$;}
     ; 

identifier_list : identifier_list COMMA identifier
                | identifier        //
                ;

identifier : identifier_no_initial
           | identifier_with_initial
           ;

identifier_no_initial : ID            {InsertIdList(tmp_idlist, $1, NULL, NULL);}
                      | ID array      {InsertIdList(tmp_idlist, $1, tmp_arraysig, NULL); tmp_arraysig = BuildTmpArraySig(); arraysig_isnew = 1;}
                      ;
identifier_with_initial : ID ASSIGN expression            {InsertIdList(tmp_idlist, $1, NULL, BuildExprAttribute($3)); GenInitialStore($1,globaldecl_buf);}
                        | ID array ASSIGN initial_array   {InsertIdList(tmp_idlist, $1, tmp_arraysig, BuildExprListAttribute($4));  tmp_arraysig = BuildTmpArraySig(); arraysig_isnew = 1;}
                        ;

initial_array : LEFT_BRACE expression_list RIGHT_BRACE    {$$ = $2;}
              ;

array : array LEFT_SQUARE INT_LIT RIGHT_SQUARE     {ExtendTmpArraySig(tmp_arraysig, $3, &arraysig_isnew);}
      | LEFT_SQUARE INT_LIT RIGHT_SQUARE           {ExtendTmpArraySig(tmp_arraysig, $2, &arraysig_isnew);}
      ;

funct_decl : type ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES SEMICOLON   
            {
              /*InsertIdList(tmp_idlist, $2, NULL, $4);
              InsertTableEntryFromList(symbol_table, tmp_idlist, "function", $1);
              ResetIdList(tmp_idlist);*/
              TableEntry* new_entry = BuildTableEntry($2, "function", symbol_table->current_level, $1, BuildFunctAttribute($4));
              InsertTableEntry(symbol_table, new_entry);
              ResetIdList(tmp_idlist);
            }
           | procedure_decl
           ;

procedure_decl : VOID ID LEFT_PARENTHESES argument_list RIGHT_PARENTHESES SEMICOLON
                {
                  Type* tmp = BuildType($1);
                  TableEntry* new_entry = BuildTableEntry($2, "function", symbol_table->current_level, tmp, BuildFunctAttribute($4));
                  InsertTableEntry(symbol_table, new_entry);
                  ResetIdList(tmp_idlist);
                }
               ;

argument_list : nonEmptyArgumentList      {$$ = $1;}
              | /*  epsilon */            {$$ = NULL;}
              ;
nonEmptyArgumentList : nonEmptyArgumentList COMMA argument    {$$ = ExtendTypeList($1, $3);}
                     | argument                               {$$ = $1;}
                     ;
argument : type identifier_no_initial
          {
            $$ = AddTypeToList(NULL, $1, tmp_idlist);
            //printf("%d\n", tmp_idlist->current_size);
            //ResetIdList(tmp_idlist);
          }
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

  char* path;
  path = strdup(argv[1]);
  //printf("%s", path);
	
	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

  fpout = fopen("output.j", "w");
  if( fpout == NULL )  {
    fprintf( stdout, "Open  file  error\n" );
    exit(-1);
  }

  symbol_table = BuildSymbolTable();
  tmp_arraysig = BuildTmpArraySig();
  tmp_idlist = BuildIdList();

  arraysig_isnew = 1;
  current_dim = 0;

  //tmp_arraysig = NULL;
	
	yyin = fp;
  Initialization();
	yyparse();

  if (Opt_Statistic){
    // Print frequency table
    printf("\nfrequencies of identifiers:\n");
    print_id();
  }

  if(!error){
    fprintf( stdout, "\n" );
    fprintf( stdout, "|--------------------------------|\n" );
    fprintf( stdout, "|  There is no syntactic error!  |\n" );
    fprintf( stdout, "|--------------------------------|\n" );
  }

	exit(0);
}
