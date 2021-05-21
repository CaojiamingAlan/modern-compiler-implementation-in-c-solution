void SEM_transProg(A_exp exp);

typedef void * Tr_exp;
typedef struct expty {Tr_exp exp; Ty_ty ty;} expty;

expty transVar(S_table venv, S_table tenv, A_var v);
expty transExp(S_table venv, S_table tenv, A_exp a);
Ty_ty transTy(S_table tenv, A_ty a);
void transDec(S_table venv, S_table tenv, A_dec d);