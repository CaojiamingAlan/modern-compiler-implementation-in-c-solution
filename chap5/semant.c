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

S_symbol getParentVarName(A_var v) {
  switch (v->kind)
  {
    case A_simpleVar:
      return v->u.simple;
    case A_fieldVar:
      return getParentVarName(v->u.field.var);
    case A_subscriptVar:
      return getParentVarName(v->u.subscript.var);
    default:
      return NULL;
  }
}

expty transVar(S_table venv, S_table tenv, A_var v) {
  switch(v->kind) {
    case A_simpleVar: {
      E_enventry x = S_look(venv, v->u.simple);
      if(x && x->kind == E_varEntry) return expTy(NULL, actual_ty(x->u.var.ty));
      else {
        EM_error(v->pos, "undefined varaible %s", S_name(v->u.simple));
      }
      break;
    }
    case A_fieldVar: {
      //find the variable first
      expty e = transVar(venv, tenv, v->u.field.var);
      //then search in the field list
      Ty_ty ty = actual_ty(e.ty);
      string recName = S_name(getParentVarName(v->u.field.var));
      if(!ty) {
        EM_error(v->pos, "cannot find the record %s", recName);
      }
      if(ty->kind == Ty_record){
        Ty_fieldList l = ty->u.record;
        while(l && l->head){
          // fprintf(stdout, "%s\n", S_name(l->head->name));
          // fprintf(stdout, "%s\n", S_name(v->u.field.sym));
          if(!strcmp(S_name(l->head->name), S_name(v->u.field.sym))) return expTy(NULL, actual_ty(l->head->ty));
          l = l->tail;
        }
        //cannot find the field
        EM_error(v->pos, "cannot find the field %s in %s", S_name(v->u.field.sym), recName);
      } else {
        EM_error(v->pos, "%s is not a record", recName);
      }
      break;
    }
    case A_subscriptVar: {
      expty e = transVar(venv, tenv, v->u.subscript.var);
      Ty_ty ty = actual_ty(e.ty);
      if(!ty){
        EM_error(v->pos, "undefined subscript varaible %s", S_name(getParentVarName(v->u.subscript.var)));
        exit(0);
      }
      if(ty->kind == Ty_array){
        return expTy(NULL, actual_ty(ty->u.array));
      } else {
        EM_error(v->pos, "varaible %s is not subsciptable", S_name(getParentVarName(v->u.subscript.var)));
        exit(0);
      }
      break;
    }
  }
  exit(0);
  return expTy(NULL, NULL);
}

expty transExp(S_table venv, S_table tenv, A_exp a) {
  switch(a->kind) {
    case A_varExp: {
      //printf("before varexp:");
      //printVenv(venv);
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
      E_enventry x = S_look(venv, a->u.call.func);
      if(!x || x->kind != E_funEntry){ 
        EM_error(a->pos, "Cannot find the function named %s", S_name(a->u.call.func));
        exit(0);
      }
      //should check each parameter
      A_expList l = a->u.call.args;
      Ty_tyList tyl = x->u.fun.formals;
      //should inverse the explist first

      while(l && l->head){
        if(!tyl){
          EM_error(a->pos, "func %s has less parameters than used", S_name(a->u.call.func));
          exit(0);
        }
        if(transExp(venv, tenv, l->head).ty != tyl->head){
          //Ty_print(transExp(venv, tenv, l->head).ty);
          //Ty_print(tyl->head);
          EM_error(a->pos, "arg type imcompatible in %s", S_name(a->u.call.func));
          exit(0);
        }
        tyl = tyl->tail;
        l = l->tail;
      }
      if(tyl){
          EM_error(a->pos, "func %s has more parameters than used", S_name(a->u.call.func));
          exit(0);
      }
      return expTy(NULL, x->u.fun.result);
    }
    case A_opExp: {
      A_oper oper = a->u.op.oper;
      expty left = transExp(venv, tenv, a->u.op.left);
      expty right = transExp(venv, tenv, a->u.op.right);
      if(oper == A_plusOp || oper == A_minusOp || oper == A_timesOp || oper == A_divideOp) {
        if(left.ty->kind != Ty_int) EM_error(a->u.op.left->pos, "integer required");
        if(right.ty->kind != Ty_int) EM_error(a->u.op.right->pos, "integer required");
        return expTy(NULL, Ty_Int());
      }
      else {
        if(left.ty != right.ty) EM_error(a->u.op.left->pos, "comparison of incompatible types");
        return expTy(NULL, Ty_Int());
      }
      return expTy(NULL, Ty_Int());
    }
    case A_recordExp: {
      //check each field
      //todo
      Ty_ty x = S_look(tenv, a->u.record.typ);
      if(!x) {
        EM_error(a->pos, "Cannot find record type %s", S_name(a->u.record.typ));
      }
      return expTy(NULL, x);
    }
    case A_seqExp: {
      //iterate over each exp
      A_expList l = a->u.seq;
      while (l->tail) {
        transExp(venv, tenv, l->head);
        l = l->tail;
      }
      //return the type of the last expression
      return transExp(venv, tenv, l->head);
    }
    case A_assignExp: {
      Ty_ty ty_left = transVar(venv, tenv, a->u.assign.var).ty;
      Ty_ty ty_right = transExp(venv, tenv, a->u.assign.exp).ty;
      if(ty_left->kind != ty_right->kind){
        EM_error(a->pos, "assignment type imcompatible");
        exit(0);
      }
      //assign exp does not have a value
      return expTy(NULL, Ty_Nil());
    }
    case A_ifExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      transExp(venv, tenv, a->u.iff.test);
      expty then_e = transExp(venv, tenv, a->u.iff.then);
      if(a->u.iff.elsee){ 
        expty else_e =transExp(venv, tenv, a->u.iff.elsee);
        if(else_e.ty != then_e.ty){
          EM_error(a->pos, "types of then - else differ");
          exit(0);
        }
      } else if(then_e.ty->kind != Ty_nil){
        EM_error(a->pos, "if-then returns non unit");
        exit(0);
      }

      S_endScope(tenv);
      S_endScope(venv);
      return expTy(NULL, Ty_Nil());
    }
    case A_whileExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      transExp(venv, tenv, a->u.whilee.test);
      if(transExp(venv, tenv, a->u.whilee.body).ty->kind != Ty_nil){
        EM_error(a->pos, "body of while not unit");
        exit(0);
      }
      S_endScope(tenv);
      S_endScope(venv);
      return expTy(NULL, Ty_Nil());
    }
    case A_forExp: {
      S_beginScope(venv);
      S_beginScope(tenv);
      //todo
      if(transExp(venv, tenv, a->u.forr.lo).ty->kind != Ty_int){
        EM_error(a->pos, "lo expr is not int");
        exit(0);
      }
      if(transExp(venv, tenv, a->u.forr.hi).ty->kind != Ty_int){
        EM_error(a->pos, "hi expr is not int");
        exit(0);
      }
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
      if(transExp(venv, tenv, a->u.array.init).ty != S_look(tenv, a->u.array.typ)){
        EM_error(a->pos, "Init value type wrong");
        exit(0);
      }
      return expTy(NULL, Ty_Array(transExp(venv, tenv, a->u.array.init).ty));
    }
  }
  //should never go here
  exit(0);
  return expTy(NULL, NULL);
}

//todo
Ty_ty transTy(S_table tenv, A_ty a) {
  switch (a->kind) {
    case A_nameTy: {
      Ty_ty ty = S_look(tenv, a->u.name);
      if(!ty){
        EM_error(a->pos, "undefined type %s", S_name(a->u.name));
        exit(0);
      }
    }
    case A_recordTy: {
      A_fieldList l = a->u.record;
      Ty_fieldList fl = NULL;
      while(l && l->head){
        Ty_ty ty = S_look(tenv, l->head->typ);
        if(!ty){
          EM_error(a->pos, "undefined type %s", S_name(l->head->typ));
          exit(0);
        }
        fl = Ty_FieldList(Ty_Field(l->head->name, ty), fl);
        l = l->tail;
      }
      return Ty_Record(fl);
    }
    case A_arrayTy: {
      Ty_ty ty = S_look(tenv, a->u.array);
      if(!ty){
        EM_error(a->pos, "undefined type %s", S_name(a->u.array));
        exit(0);
      }
      return Ty_Array(ty);
    }
  }
  assert(0);
  return NULL;
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList l){
  Ty_tyList tyl = NULL;
  //inverse the list first
  A_fieldList il = NULL;
  while(l){
    il = A_FieldList(l->head, il);
    l = l->tail;
  }
  l = il;

  while(l){
    tyl = Ty_TyList(S_look(tenv, l->head->typ), tyl);
    l = l->tail;
  }
  return tyl;
}

void transDec(S_table venv, S_table tenv, A_dec d) {
  switch (d->kind){
    case A_varDec: {
      expty e = transExp(venv, tenv, d->u.var.init);
      if(d->u.var.typ != NULL){
        Ty_ty ty = S_look(tenv, d->u.var.typ);
        if(!ty){
          EM_error(d->pos, "Cannot find type %s", d->u.var.typ);
          exit(0);
        } 
        if(ty != e.ty){
          EM_error(d->pos, "Different types");         
          exit(0);
        }
      }
      if(e.ty == Ty_Nil()){
        EM_error(d->pos, "Initializing nil expressions not constrained by record type");
        exit(0);
      }

      E_enventry v = S_look(venv, d->u.var.var);
      if(v && v->u.var.ty != e.ty){
        EM_error(d->pos, "Variable assigned different types");
      }

      S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
      //printf("after vardec: \n");
      //printVenv(venv);
      return;
    }
    case A_typeDec: {
      //todo
      Ty_ty ty = S_look(tenv, d->u.type->head->name);
      if(ty){
        EM_error(d->pos, "Type already assigned");
      }

      S_enter(tenv, d->u.type->head->name, transTy(tenv, d->u.type->head->ty));
      // printf("after typedec:");
      // printTenv(tenv);
      return;
    }
    case A_functionDec: {
      A_fundec f = d->u.function->head;

      E_enventry v = S_look(venv, f->name);
      if(v){
        EM_error(d->pos, "Function already defined");
      }


      Ty_ty resultTy = f->result?S_look(tenv, f->result):Ty_Nil();
      Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
      S_enter(venv, f->name, E_FunEntry(formalTys, resultTy));
      S_beginScope(venv);
      {
        A_fieldList l;
        Ty_tyList t;
        for(l=f->params, t=formalTys; l; l=l->tail, t=t->tail)
          S_enter(venv, l->head->name, E_VarEntry(t->head));
      }
      // printf("after funcdec:");
      // printVenv(venv);
      if(transExp(venv, tenv, f->body).ty != resultTy) {
        EM_error(d->pos, "procedure return type different from declared", S_name(d->u.function->head->name));
      }
      S_endScope(venv);

      return;
    default:
      exit(0);
    }
  }
}

void SEM_transProg(A_exp exp){ 
  S_table venv = E_base_venv();
  S_table tenv = E_base_tenv();
  transExp(venv, tenv, exp);
}