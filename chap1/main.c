#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>

int maxargs(A_stm a){
    if(a->kind != A_printStm) return 0;
    int count = 1;
    A_expList e = a->u.print.exps;
    while (e->kind != A_lastExpList){
        count++;
        e = e->u.pair.tail;
    }
    return count;
}

int main(){
    printf("%d\n", maxargs(prog()));
    printf("%d\n", maxargs(
    A_PrintStm(A_PairExpList(A_IdExp("a"),
                 A_LastExpList(A_OpExp(A_IdExp("a"), A_minus, 
                                       A_NumExp(1)))))));
    return 0;
}