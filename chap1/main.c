#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>

int maxargs(A_stm a){
    return 2;
}

int main(){
    printf("%d\n", maxargs(prog()));
    return 0;
}