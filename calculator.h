#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int size;

typedef enum {
    false,
    true
} bool;

typedef struct NODE{
    char Data;
    struct NODE* pre;
    struct NODE* next;
} Node;

typedef struct STACK{
    struct NODE* Top;
    struct NODE* Bottom;
} Stack;

void push(Stack* stack, char data); // 스텍 삽입

//void push_back(Stack* stack, char data); // 스텍 바닥삽입

char pop(Stack* stack); // 스텍 데이터 꺼내기

char pop_back(Stack* stack); // 스텍 바닥에서 데이터 꺼내기

void init(Stack* stack); // 스텍 초기화

int isEmpty(Stack* stack); // 빈 스텍인지 확인

int peek(Stack* stack); // 스텍 꼭대기의 값

// int bottom(Stack* stack); // 스텍 바닥의 값

char priority(char d); // 연산자 우선순위 (곱셈 1순위, 덧셈뺄셈 2순위, 괄호 3순위)


char* fileRead(int *cap); // 파일에서 읽어온 후 예외를 발견하면 정지 혹은 자동 변환


char* infix2postfix(char exp[], int cap); // 중위표기법에서 후위표기법으로 바꾸는 함수


typedef struct DLIST{
    struct NODE* head;
    struct NODE* tail;
    struct DLIST* next;
} DList;

typedef struct LSTACK{
    DList* top;
} LStack;

DList* makeList(); // 인수열리스트 생성후 리턴

void insertNode(DList* DL, char data); // 인수열리스트에 수 하나씩 삽입을 위한 함수

void insertNodeMiddle(DList* DL, char data, Node* idx);

void insertNodeHead(DList* DL, char data); // 인수열리스트에 수 하나씩 삽입을 위한 함수

// void deleteNode(DList* DL, Node* Dnode);

// char* getList(DList* DL);

// Node* searchNode(DList* DL, char* data);

LStack* makeLS(); // 인수열리스트 배열스텍 생성후 리턴

void LSpush(LStack* LS, DList* DL);

DList* LSpop(LStack* LS);

DList* LSref(LStack* LS);

void Dlist_maker(char* exp, LStack* fac, Stack* op);

DList* multiply(DList* a, DList* b);

DList* getTempMultiple(DList* a, DList* b, Node* a_cur_origin);

void Printer(DList* DL);

DList* list_add(DList* a, DList* b);

int isDot(DList* DL);

DList* plus(DList* a, DList* b);

int Dot(DList* dlist);

int integer_size(DList* dlist);

int decimal_size(DList* dlist);

DList* minus(DList* a, DList* b);

int decimal_size(DList* dlist);
int LSisEmpty(LStack* lsstack );