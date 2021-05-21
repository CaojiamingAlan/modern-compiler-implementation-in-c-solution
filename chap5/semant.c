/*
 * util.c - commonly used utility functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "errormsg.h"
#include "semant.h"

expty expTy(Tr_exp exp, Ty_ty ty) {
  struct expty e;
  e.exp = exp;
  e.ty = ty;
  return e;
}

Ty_ty actual_ty(Ty_ty ty){
  while(ty->kind == Ty_name){
    ty = ty->u.name.ty;
  }
  return ty;
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
    case A_fieldVar: {
      //find the variable first
      expty e = transVar(venv, tenv, v->u.field.var);
      //then search in the field list
      Ty_ty ty = actual_ty(e.ty);
      if(ty->kind == Ty_record){
        Ty_fieldList l = ty->u.record;
        while(l && l->head){
          return expTy(NULL, actual_ty(l->head->ty));
        }
      }
      EM_error(v->pos, "undefined field varaible %s", S_name(v->u.simple));
    }
    case A_subscriptVar: {
      expty e = transVar(venv, tenv, v->u.subscript.var);
      Ty_ty ty = actual_ty(e.ty);
      if(ty->kind == Ty_array){
        return expTy(NULL, actual_ty(ty->u.array));
      }
      EM_error(v->pos, "undefined subscript varaible %s", S_name(v->u.simple));
    }
  }
  assert(0);
  return expTy(NULL, NULL);
}

expty transExp(S_table venv, S_table tenv, A_exp a) {
  switch(a->kind) {
    case A_varExp: {
      return transVar(venv, tenv, a->u.var);
    }
    case A_nilExp: {
      return expTy(NULL, Ty_Nil());
    }
    case A_intExp: {
      return expTy(NULL, Ty_Int());
    }
    case A_stringExp: {
      return expTy(NULL, Ty_String());
    }
    case A_callExp: {
      //should check each parameter
      //todo
      E_enventry x = S_look(venv, a->u.call.func);
      return expTy(NULL, x->u.fun.result);
    }
    case A_opExp: {
      A_oper oper = a->u.op.oper;
      expty left = transExp(venv, tenv, a->u.op.left);
      expty right = transExp(venv, tenv, a->u.op.right);
      if(oper == A_plusOp || oper == A_minusOp || oper == A_timesOp) {
        if(left.ty->kind != Ty_int) EM_error(a->u.op.left->pos, "integer required");
        if(right.ty->kind != Ty_int) EM_error(a->u.op.right->pos, "integer required");
        return expTy(NULL, Ty_Int());
      }
    }
    case A_recordExp: {
      //check each field
      //todo
      E_enventry x = S_look(venv, a->u.record.typ);
      return expTy(NULL, x->u.var.ty);
    }
    case A_seqExp: {
      //iterate over each exp
      A_expList l = a->u.seq;
      while (l->tail) {
        transExp(tenv, venv, l->head);
        l = l->tail;
      }
      //return the type of the last expression
      return transExp(tenv, venv, l->head);
    }
    case A_assignExp: {
      //add to env
      Ty_ty ty = transExp(venv, tenv, a->u.assign.exp).ty;
      if(a->u.assign.var->kind == A_simpleVar) {
        S_enter(venv, a->u.assign.var->u.simple, ty);
      } else if (a->u.assign.var->kind == A_subscriptVar) {
        //check the type of the array
        //todo
      } else {
        S_enter(venv, a->u.assign.var->u.field.sym, ty);
      }

      //assign exp does not have a value
      return expTy(NULL, Ty_Nil());
    }
    case A_ifExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      transExp(venv, tenv, a->u.iff.test);
      transExp(venv, tenv, a->u.iff.then);
      if(a->u.iff.then) transExp(venv, tenv, a->u.iff.then);
      S_endScope(tenv);
      S_endScope(venv);
      return expTy(NULL, Ty_Nil());
    }
    case A_whileExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      transExp(venv, tenv, a->u.whilee.test);
      transExp(venv, tenv, a->u.whilee.body);
      S_endScope(tenv);
      S_endScope(venv);
      return expTy(NULL, Ty_Nil());
    }
    case A_forExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      //todo
      S_endScope(tenv);
      S_endScope(venv);
      return expTy(NULL, Ty_Nil());
    }
    case A_breakExp: {
      return expTy(NULL, Ty_Nil());
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
    case A_arrayExp: {
      //should check the type of the init value
      //todo
      return expTy(NULL, Ty_Array(transExp(venv, tenv, a->u.array.init).ty));
    }
  }
  //should never go here
  assert(0);
  return expTy(NULL, NULL);
}

//todo
Ty_ty transTy(S_table tenv, A_ty a) {
  return NULL;
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList l){
  Ty_tyList tyl = NULL;
  while(l && l->head){
    tyl = Ty_TyList(S_look(tenv, l->head->typ), tyl);
    l = l->tail;
  }
  return tyl;
}

void transDec(S_table venv, S_table tenv, A_dec d) {
  switch (d->kind){
    case A_varDec: {
      expty e = transExp(venv, tenv, d->u.var.init);
      S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
      return;
    }
    case A_typeDec: {
      //todo
      S_enter(tenv, d->u.type->head->name, transTy(tenv, d->u.type->head->ty));
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
      transExp(venv, tenv, f->body);
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