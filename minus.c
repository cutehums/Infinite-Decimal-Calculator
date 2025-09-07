
#include "calculator.h"


DList* minus(DList* a, DList* b){
    DList* result = makeList();
    int type = 0;
    int aisDot = Dot(a);
    int bisDot = Dot(b);
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
    while(front_cur_a->Data ){
        if (front_cur_a->Data > front_cur_b->Data){
            //a가 큼 따라서 그냥 계산하면 됨
            type = 0;
            break;
        }
        else if (front_cur_a->Data < front_cur_b->Data){
            //b가 큼 따라서 바꿔서 계산해서 -를 붙여야함.
            type = 1;
            break;
        }
        else{
            front_cur_a = front_cur_a->next;
            front_cur_b = front_cur_b->next;
        }
    }
    if (front_cur_a == NULL){ //a와b가 같음.
        type = 3;
        insertNodeHead(result, '0');
    }
    int i = 0;
    int subtract = 0;
    // printf("%c\n",back_cur_a->Data);
    while(back_cur_a != NULL){
        // printf("sdfssdf");
        if (type == 0){ // |a| > |b|
            subtract = back_cur_a->Data - back_cur_b->Data -i;
        }
        else if (type == 1){ //|a| < |b|
            subtract = back_cur_b->Data - back_cur_a->Data -i;
        }
        else { // a = b
            break;
        }
        if (back_cur_a->Data == 46){//back_cur_a가 .일때
            insertNodeHead(result,back_cur_a->Data);
            back_cur_a = back_cur_a->pre;
            back_cur_b = back_cur_b->pre;
        }
        else{
            // printf("\n%c\n",back_cur_a->Data);
            



            // printf("%c\n",back_cur_a->Data);
            // printf("%c\n",back_cur_b->Data);
            // printf("%d\n",sum);
            if (subtract < 0){//받아내림 해야함
                i = 1;
                char value = subtract + 10 + 48;
                insertNodeHead(result,value);
                back_cur_a = back_cur_a->pre;
                back_cur_b = back_cur_b->pre;
            }
            else {//받아내림 안함
                i = 0;
                char value = subtract + 48;
                insertNodeHead(result,value);
                back_cur_a = back_cur_a->pre;
                back_cur_b = back_cur_b->pre;
            }
        }
    }
 
   if (type == 1){
    insertNodeHead(result,'-');
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
void move_dot(Node* cur_a, Node* cur_b,int a_int_size, int b_int_size){
    while (a_int_size != 0){ // 점 노드로 이동
            cur_a = cur_a->next;
            a_int_size--;
        }
        while (b_int_size != 0){//점 노드로 이동
            cur_b = cur_b->next;
            b_int_size--;
        }
}
*/