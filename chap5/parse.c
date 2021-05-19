/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"
#include "semant.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname) 
{EM_reset(fname);
 if (yyparse() == 0) /* parsing worked */
   return absyn_root;
 else return NULL;
}

int main(int argc, char **argv) {
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
 A_exp root = parse(argv[1]);

 SEM_transProg(root);
 
 char *out_file_name = malloc(100);
 strcpy(out_file_name, argv[1]);
 strcat(out_file_name, "_parse.txt");

 FILE *fp = fopen (out_file_name, "w");
 pr_exp(fp, root ,2);
 return 0;
}