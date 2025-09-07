#include "calculator.h"

int main() { 
    printf("Welcome to the Infinite Number Calculator!\nTh0e expression in input file is ");
    int cap;
    DList* ans;
    char *exp = fileRead(&cap);
    char response;
    for(int i = 0; i < size; i++)
    printf("%c", exp[i]);
    printf(" is this what you wrote?\n");
    printf("'Y' to calculate, 'N' to stop and rewrite expression.\n");
    scanf("%c", &response);
    if(response == 'Y') {
        char* pexp = infix2postfix(exp, cap);
        for(int i = 0; i < size; i++)
            printf("%c", pexp[i]);
        printf("\n");

        Stack* op = (Stack*)malloc(sizeof(Stack*)); // 계산에 사용될 연산자들을 저장하는 스텍(계산순서대로 쌓임)
        LStack* fac = makeLS(); // 계산에 사용될 인수들을 저장하는 스텍(계산순서대로 쌓임)
        Dlist_maker(pexp, fac, op);
        while ( !(LSisEmpty(fac)== 1 && isEmpty(op) == 1) ) {
            char oper = pop(op);
            printf("starter\n");
            if (oper == '*') {
                printf("0P000000000\n");
                DList* a = LSpop(fac);
                DList* b = LSpop(fac);
                DList* result = multiply(a, b);
                LSpush(fac,result);
                ans = result;
            }

            else if (oper == '+') {
                printf("11111111\n");
                DList* a = LSpop(fac);
                DList* b = LSpop(fac);
                DList* result = list_add(a, b);
                LSpush(fac,result);
                ans = result;
            } 

            else if (oper == '-') {
                printf("22222222\n");
                DList* a = LSpop(fac);
                DList* b = LSpop(fac);
                if ( a->head->Data=='+'){ // a가 양수
                    if  ( b->head->Data=='+'){ //b가 양수 
                            DList* result = minus(a, b);
                            LSpush(fac,result);
                            ans = result;
                    }
                    else{  //b가 음수 
                            DList* result = list_add(b,a);
                            LSpush(fac,result);
                            ans = result;
                    }
                }
                else{  // a 가 음수  
                    if  ( b->head->Data=='+'){ // b가 양수 
                        DList* result = minus(a, b);
                        LSpush(fac,result);
                        ans = result;
                    }
                    else{ //b가 음수
                        DList* result = list_add(a, b);
                        LSpush(fac,result);
                        ans = result;
                    }
                }
            

            }
        Printer(ans);
        }
       
    }
}