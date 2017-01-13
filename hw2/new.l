%{
#define LIST { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t) { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s) { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
#include "y.tab.h"
int id_size = 0;
int i;
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];
char str_buf[MAX_LINE_LENGTH];
struct id{
  char name[MAX_ID_LENGTH];
  int  times;
  struct id* next;
};
struct id* first = NULL;
struct id* current = NULL;

void create_id(char*);
void find_id(char*);
void print_id();

%}
digit [0-9]
letter [_a-zA-Z]
keyword ("while"|"do"|"if"|"else"|"true"|"false"|"for"|"int"|"print"|"const"|"read"|"boolean"|"bool"|"void"|"float"|"double"|"string"|"continue"|"break"|"return")
id {letter}({letter}|{digit})*
delimiter [,;\(\)\[\]\{\}]
integer (0|([1-9]{digit}*))
float {digit}+\.{digit}+
scientific ({float}|{integer})[eE][\+\-]?{digit}+
space [ \t]+

%x _STRING
%x COMMENT
%x PRAGMA
%x PRAGMA_COMMENT
%x SOURCE
%x TOKEN
%x STATISTIC
%%

^"#pragma" {
  LIST;
  BEGIN PRAGMA;
}

"/*" {
  LIST;
  BEGIN COMMENT;
}

while { tokenString("KW",yytext); return WHILE;}
do { tokenString("KW",yytext); return DO;}
if { tokenString("KW",yytext); return IF;}
else { tokenString("KW",yytext); return ELSE;}
true { tokenString("KW",yytext); return TRUE;}
false { tokenString("KW",yytext); return FALSE;}
for { tokenString("KW",yytext); return FOR;}
int { tokenString("KW",yytext); return INT;}
print { tokenString("KW",yytext); return PRINT;}
const { tokenString("KW",yytext); return CONST;}
read { tokenString("KW",yytext); return READ;}
boolean { tokenString("KW",yytext); return BOOL;}
bool { tokenString("KW",yytext); return BOOL;}
void { tokenString("KW",yytext); return VOID;}
float { tokenString("KW",yytext); return FLOAT;}
double { tokenString("KW",yytext); return DOUBLE;}
string { tokenString("KW",yytext); return STRING;}
continue { tokenString("KW",yytext); return CONTINUE;}
break { tokenString("KW",yytext); return BREAK;}
return { tokenString("KW",yytext); return RETURN;}

{id} { tokenString("id",yytext); find_id(yytext); return ID;}
{float} {  tokenString("float",yytext); return FLOAT_LIT;}
{integer} { tokenString("integer",yytext); return INT_LIT;}

, { tokenString("delim",yytext); return COMMA;}
; { tokenString("delim",yytext); return SEMICOLON;}
\( { tokenString("delim",yytext); return LEFT_PARENTHESES;}
\) { tokenString("delim",yytext); return RIGHT_PARENTHESES;}
\[ { tokenString("delim",yytext); return LEFT_SQUARE;}
\] { tokenString("delim",yytext); return RIGHT_SQUARE;}
\{ { tokenString("delim",yytext); return LEFT_BRACE;}
\} { tokenString("delim",yytext); return RIGHT_BRACE;}

{scientific} {  tokenString("scientific",yytext);  return SCIENTIFIC;}

"+" { token("+"); return '+';}
"-" { token("-"); return '-';}
"*" { token("*"); return '*';}
"/" { token("/"); return '/';}
"%" { token("%"); return '%';}
"=" { token("="); return '=';}
"<" { token("<"); return LT;}
"<="  { token("<="); return LE;}
"!=" {  token("!="); return NE;}
">=" {  token(">="); return GE;}
">" {  token(">"); return GT;}
"==" { token("=="); return EQ;}
"&&" { token("&&"); return AND;}
"||" { token("||"); return OR;}
"!" { token("!"); return '!';}

\" { str_buf[0] = '\0' ;LIST; BEGIN _STRING; }
"//"(.)*(\n) { //C++-Style Comment
  LIST;
  if (Opt_Source)
    printf("%d:%s\n", linenum, buf);
  ++linenum;
  buf[0] = '\0';
}
<_STRING>\\\" { LIST;  strncat(str_buf, "\"", sizeof(buf) - strlen(buf) - 1);  }
<_STRING>\\\\ { LIST;  strncat(str_buf, "\\", sizeof(buf) - strlen(buf) - 1);  }
<_STRING>\\ { LIST;  strncat(str_buf, "\\", sizeof(buf) - strlen(buf) - 1);  }
<_STRING>\" { tokenString("string",str_buf); BEGIN 0; return STRING_LIT;}
<_STRING>. { LIST;  strncat(str_buf, yytext, sizeof(buf) - strlen(buf) - 1); }
<COMMENT>. {
  LIST;
}
<COMMENT>"*/" {
  LIST;
  BEGIN 0;
}
<COMMENT>"\n" {
  LIST;
  /*if (Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
}

<PRAGMA>{space}"source" {
  LIST;
  BEGIN SOURCE;
}
<PRAGMA>{space}"token" {
  LIST;
  BEGIN TOKEN;
}
<PRAGMA>{space}"statistic" {
  LIST;
  BEGIN STATISTIC;
}

<PRAGMA_COMMENT>"\n" {
  LIST;
  /*if (Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
}
<PRAGMA_COMMENT>"*/" {
  LIST;
  BEGIN 0;
}
<PRAGMA_COMMENT>. {
  LIST;
}

<SOURCE>{space}"on"({space})*("/*") {
  LIST;
  Opt_Source = 1;
  BEGIN PRAGMA_COMMENT;
}
<SOURCE>{space}"off"({space})*("/*") {
  LIST;
  Opt_Source = 0;
  BEGIN PRAGMA_COMMENT;
}
<SOURCE>{space}"on"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Source = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<SOURCE>{space}"off"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Source = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;

}
<SOURCE>{space}"on"({space})*"\n" {
  LIST;
  Opt_Source = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;

}
<SOURCE>{space}"off"({space})*"\n" {
  LIST;
  Opt_Source = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<SOURCE>. {
  fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
  exit(1);
}

<TOKEN>{space}"on"({space})*("/*") {
  LIST;
  Opt_Token = 1;
  BEGIN PRAGMA_COMMENT;
}
<TOKEN>{space}"off"({space})*("/*") {
  LIST;
  Opt_Token = 0;
  BEGIN PRAGMA_COMMENT;
}
<TOKEN>{space}"on"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Token = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<TOKEN>{space}"off"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Token = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;

}
<TOKEN>{space}"on"({space})*"\n" {
  LIST;
  Opt_Token = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;

}
<TOKEN>{space}"off"({space})*"\n" {
  LIST;
  Opt_Token = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<TOKEN>. {
  fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
  exit(1);
}


<STATISTIC>{space}"on"({space})*("/*") {
  LIST;
  Opt_Statistic = 1;
  BEGIN PRAGMA_COMMENT;
}
<STATISTIC>{space}"off"({space})*("/*") {
  LIST;
  Opt_Statistic = 0;
  BEGIN PRAGMA_COMMENT;
}
<STATISTIC>{space}"on"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Statistic = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<STATISTIC>{space}"off"({space})*("//")(.)*"\n" {
  LIST;
  Opt_Statistic = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<STATISTIC>{space}"on"({space})*"\n" {
  LIST;
  Opt_Statistic = 1;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;

}
<STATISTIC>{space}"off"({space})*"\n" {
  LIST;
  Opt_Statistic = 0;
  /*if(Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
  BEGIN 0;
}
<STATISTIC>. {
  fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
  exit(1);
}

\n {
  LIST;
/*  if (Opt_Source)
    printf("%d:%s\n", linenum, buf);*/
  ++linenum;
  buf[0] = '\0';
}
[ \t] {
  LIST;
}
. { // Unrecognized character
  fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
  exit(1);
}

%%

void create_id(char* name){
  struct id* i = (struct id*) malloc(sizeof(struct id));
  strcpy(i->name,name);
  i->times = 1;
  i->next = NULL;
  if(current == NULL){
    current = i;
    first = i;
  }
  else{
    current->next = i;
    current = i;
  }
}

void find_id(char* name){
  struct id* i2 = first;
  if(i2 == NULL){
    create_id(name);
    return;
  }
  while(i2->next != NULL){
    if(!strcmp(i2->name, name)){
      (i2->times)++;
      return;
    }
    else{
      i2 = i2->next;
    }
  }
  if(!strcmp(i2->name, name)){
    (i2->times)++;
    return;
  }
  create_id(name);
  return;
}

void print_id(){
  struct id* i2 = first;
  if(first==NULL)
    return;
  if(first->next == NULL){
    printf("%s\t%d\n", first->name, first->times);
    return;
  }
  while(i2->next!=NULL){
    printf("%s\t%d\n",i2->name,i2->times);
    i2 = i2->next;
  }
  printf("%s\t%d\n",i2->name,i2->times);
  return;
}

/*int main( int argc, char **argv )
{
  if (argc >= 2){
    yyin = fopen( argv[1], "r" );
    if ( NULL == yyin ) {
      fprintf( stderr, "Cannot open: %s\n", argv[1] );
      exit(-1);
    }
  }
  else{
    yyin = stdin;
  }
  yylex();
  if (Opt_Statistic){
    // Print frequency table
    printf("frequencies of identifiers:\n");
    print_id();
  }
  exit(0);
}*/
