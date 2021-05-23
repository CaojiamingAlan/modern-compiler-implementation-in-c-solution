#include "util.h"
#include "symbol.h"
#include "types.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>

E_enventry E_VarEntry(Ty_ty ty){
    E_enventry p = checked_malloc(sizeof(*p));
    p->kind=E_varEntry;
    p->u.var.ty=ty;
    return p;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result){
    E_enventry p = checked_malloc(sizeof(*p));
    p->kind=E_funEntry;
    p->u.fun.formals=formals;
    p->u.fun.result=result;
    return p;
}

//base environment
S_table E_base_tenv(void){
    S_table env = S_empty();
    S_enter(env, S_Symbol("int"), Ty_Int());
    S_enter(env, S_Symbol("string"), Ty_String());
    S_enter(env, S_Symbol("nil"), Ty_Nil());
    return env;
}

S_table E_base_venv(void){
    S_table env = S_empty();
    return env;
}

void showTenv(S_symbol sym, Ty_ty ty) {
    fprintf(stdout, "%s\n", S_name(sym));
    Ty_print(ty);
    printf("\n");
}

void showVenv(S_symbol sym, E_enventry e) {
    fprintf(stdout, "%s\n", S_name(sym));
    if(e){
        if(e->kind == E_varEntry){    
            Ty_print(e->u.var.ty);
        } else {
            fprintf(stdout, "func\n");
            printf("\n");
            Ty_print(e->u.fun.result);
            printf("\n");
            TyList_print(e->u.fun.formals);
        }
    }
    printf("\n");
}

//print env for debug
void printVenv(S_table venv){
    S_dump(venv, showVenv);
    printf("\n");
    fflush(stdout);
}

//print env for debug
void printTenv(S_table tenv){
    S_dump(tenv, showTenv);
    printf("\n");
    fflush(stdout);
}