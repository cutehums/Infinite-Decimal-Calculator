#include "calculator.h"


DList* list_add(DList* a, DList* b){
    DList* result = makeList();
    bool aisDot = Dot(a);
    bool bisDot = Dot(b);
    if (!aisDot){//소수점이 없으면 맨 뒤에 점 만들기
        insertNode(a,'.');
    }
    if (!bisDot){//소수점이 없으면 맨 뒤에 점 만들기
        insertNode(b, '.');
    }
    int a_int_size = integer_size(a);
    int a_dec_size = decimal_size(a);
    int b_int_size = integer_size(b);
    int b_dec_size = decimal_size(b);
    int zero_int = a_int_size - b_int_size;
    int zero_dec = a_dec_size - b_dec_size;


    while (zero_int != 0){
        if (zero_int > 0){//a가 큼
            insertNodeHead(b,'0');
            zero_int--;
        }
        else if (zero_int < 0){
            insertNodeHead(a,'0');
            zero_int++;
        }
        
    }
    while (zero_dec != 0){
        if (zero_dec > 0){//a가 큼
            insertNode(b,'0');
            zero_dec--;
        }
        else if (zero_dec < 0){//b가 큼
            insertNode(a,'0');
            zero_dec++;
        }
        
    }


    Node* front_cur_a = a->head;
    Node* front_cur_b = b->head;
    Node* back_cur_a = a->tail;
    Node* back_cur_b = b->tail;
    
    // while(front_cur_a != NULL && front_cur_b != NULL){
    //     front_cur_a = front_cur_a->next;
    //     front_cur_b = front_cur_b->next;
    //     //  printf("%c\n", front_cur_a->Data);
    //     // printf("%c\n", front_cur_b->Data);
    // }
    // // printf("ss");
    // // printf("a: %c\n",front_cur_a->Data);
    // // printf("b: %c\n",front_cur_b->Data);
    // if (front_cur_a == NULL && front_cur_b != NULL){//b가 더 길때
    //     while (front_cur_b->Data != back_cur_b->Data){
    //         insertNodeHead(result,back_cur_a->Data);
    //         back_cur_b = back_cur_b->pre;
    //     }
    //     insertNodeHead(result,back_cur_b->Data);
    //     back_cur_b = back_cur_b->pre;
    //     // printf("b: %c\n",back_cur_b->Data);
        
    // }
    // else if (front_cur_a != NULL && front_cur_b == NULL){ //a가 더 길때

    //     while (front_cur_a->Data != back_cur_a->Data){
    //         insertNodeHead(result,back_cur_a->Data);
    //         //printf("%c",back_cur_a->Data);
    //         back_cur_a = back_cur_a->pre;
    //     }
    //     insertNodeHead(result,back_cur_a->Data);
    //     back_cur_a = back_cur_a->pre;
    // }
    // else if (front_cur_a == NULL && front_cur_b == NULL){ //a와 b의 길이가 같을 때
        
    // }
    // back_cur_a = back_cur_a->pre;
    // back_cur_b = back_cur_b->pre;

    int i = 0;
    int sum = 0;
    // printf("%c\n",back_cur_a->Data);
    while(back_cur_a != NULL){
        // printf("sdfssdf");
        if (back_cur_a->Data == 46){//back_cur_a가 .일때
            insertNodeHead(result,back_cur_a->Data);
            back_cur_a = back_cur_a->pre;
            back_cur_b = back_cur_b->pre;
        }
        else{
            // printf("\n%c\n",back_cur_a->Data);
            
            sum = (back_cur_a->Data-48) + (back_cur_b->Data-48) + i;
            // printf("%c\n",back_cur_a->Data);
            // printf("%c\n",back_cur_b->Data);
            // printf("%d\n",sum);
            if (sum / 10 > 0){//받아올림 해야함
                i = 1;
                char value = (sum % 10) + 48;
                insertNodeHead(result,value);
                back_cur_a = back_cur_a->pre;
                back_cur_b = back_cur_b->pre;
            }
            else {//받아올림 안함
                i = 0;
                char value = sum + 48;
                insertNodeHead(result,value);
                back_cur_a = back_cur_a->pre;
                back_cur_b = back_cur_b->pre;
            }
        }
    }
    if (sum / 10 > 0){
        char value = (sum / 10) + 48;
        insertNodeHead(result,value);
    }

    if (result->tail->Data == 46){
        insertNode(result,'0');
    }

    return result;
}


// int size(DList* list){
//     int cnt = 0;
//     Node* cur_node = list->head;
//      while(cur_node != NULL){
//         cur_node = list->next;
//         cnt++;
//     }
//     return cnt;
// }
/*
void ListNum(DList* DL) { // 리스트에 데이터가 잘 들어가 있는지 확인용 테스트 함수 - 추후 삭제 요망
    Node* num = DL -> head;
    while (num != NULL) {
        char data = num -> Data;
        num = num->next;
        printf("%c", data);
    }
} 
int main() { // 테스트용 메인 함수 - 추후 삭제 요망
    int cap;
    char *exp = fileRead(&cap);
    for(int i = 0; i < size; i++)
        printf("%c", exp[i]);
    printf("\n");
    char* pexp = infix2postfix(exp, cap);
    
    for(int i = 0; i < size; i++)
        printf("%c", pexp[i]);
    printf("\n");
    Stack* op = (Stack*)malloc(sizeof(Stack*)); // 계산에 사용될 연산자들을 저장하는 스텍(계산순서대로 쌓임)
    LStack* fac = makeLS(); // 계산에 사용될 인수들을 저장하는 스텍(계산순서대로 쌓임)
    Dlist_maker(pexp, fac, op);

    DList* a = LSpop(fac);
    
    ListNum(a);
    printf("\n");
    printf("a is operating");
    printf("\n");

    DList* b = LSpop(fac);
    ListNum(b);
    printf("\n");
    printf("b is operating");
    printf("\n");

    DList* s = plus(a,b);
    ListNum(s);
}
*/

