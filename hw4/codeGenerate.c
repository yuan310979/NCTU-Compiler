#include "symbolTable.h"
#include "codeGenerate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Initialization(){
	fprintf(fpout, ".class public output\n");
	fprintf(fpout, ".super java/lang/Object\n");
}