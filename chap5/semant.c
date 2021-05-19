/*
 * util.c - commonly used utility functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "semant.h"
#include "env.h"
#include "errormsg.h"

typedef void * Tr_exp;

typedef struct expty {Tr_exp exp; Ty_ty ty;} expty;

expty expTy(Tr_exp exp, Ty_ty) {
  struct expty e;
  e.exp = exp;
  e.ty = ty;
  return e;
}

//todo
Ty_ty actual_ty(){

}

expty transVar(S_table venv, S_table tenv, A_var v) {
  switch(v->kind) {
    case A_simpleVar: {
      E_enventry x = S_look(venv, v->u.simple);
      if(x && x->kind == E_varEntry) return expTy(NULL, actual_ty(x->u.var.ty));
      else {
        EM_error(v->pos, "undefined varaible %s", S_name(v->u.simple));
      }
    }
    case A_fieldVar:
  }
}

expty transExp(S_table venv, S_table tenv, A_exp a) {
  switch(a->kind) {
    case A_opExp: {
      A_oper oper = a->u.op.oper;
      expty left = transExp(venv, tenv, a->u.op.left);
      expty right = transExp(venv, tenv, a->u.op.right);
      if(oper == A_plusOp) {
        if(left.ty->kind != Ty_int) EM_error(a->u.op.left->pos, "integer required");
        if(right.ty->kind != Ty_int) EM_error(a->u.op.right->pos, "integer required");
        return expTy(NULL, Ty_Int());
      }
    }
    case A_letExp: {
      expty exp;
      A_decList d;
      S_beginScope(venv);
      S_beginScope(tenv);
      for (d = a->u.let.decs; d; d=d->tail) transDec(venv, tenv, d->head);
      exp = transExp(venv, tenv, a->u.let.body);
      S_endScope(tenv);
      S_endScope(venv);
      return exp;
    }
  }
}

//todo
Ty_ty transTy(S_tables tenv, A_ty a) {

}

//todo
Ty_tyList makeFormalTyList(S_tables tenv, A_fieldList){

}

void transDec(S_table venv, S_table tenv, A_dec d) {
  switch (d->kind){
    case A_varDec: {
      expty e = transExp(venv, tenv, d->u.var.init);
      S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
      return;
    }
    case A_typeDec: {
      S_enter(tenv, d->u.type->head->name, transTy(d->u.type->head->ty));
      return;
    }
    case A_functionDec: {
      A_fundec f = d->u.function->head;
      Ty_ty resultTy = S_look(tenv, f->result);
      Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
      S_enter(venv, f->name, E_FunEntry(formalTys, resultTy));
      S_beginScope(venv);
      {
        A_fieldList l;
        Ty_tyList t;
        for(l=f->params, t=formalTys; l; l=l->tail, t=t->tail)
          S_enter(venv, l->head->name, E_VarEntry(t->head));
      }
      transExp(venv, tenv, d->u.function->body);
      S_endScope(venv);
      return;
    default:
      assert(0);
    }
  }
}



void SEM_transProg(A_exp exp){ 
  S_table venv = E_base_venv();
  S_table tenv = E_base_tenv();
  transExp(venv, tenv, exp);
}