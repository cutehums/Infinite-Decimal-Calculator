#include "calculator.h"


void push(Stack* stack, char data) { // 스텍에 새로운 노드 입력
    Node *tmp = (Node*) malloc(sizeof(Node));
    if (tmp == NULL){
        printf("memory allocation error.\n");
        return ;
    }
    tmp -> next = stack -> Top; // tmp의 next를 top의 주소로 대체
    tmp -> Data = data; // 데이터 저장
    stack -> Top = tmp; // top을 temp의 주소로 변경해 top이 top의 역할을 하지 않음.(가장 마지막에 저장된 데이터의 위치 가지고 있음)
}


// void push_back(Stack* stack, char data){ // 스텍에 새로운 노드 입력
//     Node* tmp = (Node*) malloc(sizeof(Node));
//     if (tmp == NULL){
//         printf("memory allocation error.\n");
//         return ;
//     }
//     tmp -> pre = stack -> Bottom;// tmp의 pre를 bottom의 주소로 대체
//     tmp -> Data = data;// 데이터 저장
//     stack -> Bottom = tmp;// bottom을 temp의 주소로 변경해 bottom이 bottom의 역할을 하지 않음.(가장 마지막에 저장된 데이터의 위치 가지고 있음)
// }


char pop(Stack* stack) { // 스텍 최상위 노드 팝
    if (isEmpty(stack)) {
        printf("stack empty.\n");
        exit(1);
    }
    char data;
    Node* node = stack -> Top;
    data = node->Data;
    stack->Top=node->next;// 비어있는 next의 주소를 top에 전달
    free(node);//node헤체해 메모리 확보
    return data;//데이터값 받아옴
}


char pop_back(Stack* stack) { // 스텍 최하위 노드 팝
    if (isEmpty(stack)) {
        printf("stack empty.\n");
        exit(1);
    }
    char data;
    Node* node = stack -> Bottom;
    data = node->Data;
    stack->Bottom=node->pre; // 비어있는 pre의 주소를 bottom에 전달
    free(node); //node헤체해 메모리 확보
    return data;//데이터값 받아옴
}


// char read_back(Stack* stack) {
//     if (isEmpty(stack)) {
//         printf("stack empty.\n");
//         exit(1);
//     }
//     char data;
//     Node* extract = sta
// }


void init(Stack* stack) { // 스텍 초기화
	stack->Top = NULL;
    stack->Bottom = NULL;
}


int isEmpty(Stack* stack) { // 스텍이 비었는지 확인
	return (stack->Top == NULL /* && stack->Bottom == NULL */);
}

int LSisEmpty(LStack* lsstack ) { //LS 스택이 비었는지 확인 
    return (lsstack->top == NULL /* && stack->Bottom == NULL */);
}
int peek(Stack *stack) { // 스텍의 최상위 노드의 데이터 리턴
	if (isEmpty(stack))	// 노드가 비었으면 실행시키지 않는다
	{
		printf("stack empty.\n");
		exit(1);
	}
	else	// 최상위 노드의 값을 리턴함
		return stack->Top->Data;
}


// int bottom(Stack *stack)
// {
// 	if (isEmpty(stack))	// 노드가 비었으면 실행시키지 않는다
// 	{
// 		printf("stack empty.\n");
// 		exit(1);
// 	}
// 	else	// 최상위 노드의 값을 리턴함
// 		return stack->Bottom->Data;
// }


char priority(char d) { // 연산자 우선순위 계산 
    switch (d) {
        case '(': case ')':
            return 0;
        case '+': case '-':
            return 1;
        case '*': case '/':
            return 2;
    }
    return -1;
}


char* exprealloc(char* exp, int *cap) { // 내부함수 : 동적할당을 원활하게 해주기 위한 함수
    (*cap) *= 2;
    char *tmp = (char*)malloc(sizeof(char) * (*cap));
    strcpy(tmp, exp);
    free(exp);
    return tmp;
}


char* fileRead(int *cap) { // 파일 리더 메소드
    FILE* input = fopen("input.txt", "r");
    
    char f;
    (*cap) = 16;
    char *exp = (char*)malloc(sizeof(char) * (*cap));
    while((f = fgetc(input)) != EOF) {
        if(size == *cap) {
            exp = exprealloc(exp, cap);
        }
		// printf("%c", f);
        exp[size++] = f;
    }
    fclose(input);
    
    // 예외처리 : 괄호완성형 아닐때, .., 영어, 특수문자
    char ch;
    char ch2;
    char* ms = (char*)'*';
    bool chkdot = 0;
    bool chkbraket = 0;
    Stack* s = (Stack*)malloc(sizeof(Stack*));
    init(s);
    for(int i = 0; i<size; i++) {
        ch = exp[i];
        if (!((ch >= '0' && ch <= '9') || ch =='.' || ch == '+' || ch == '-' || ch == '*' || ch == '(' || ch == ')' || ch == '\n')) {
            printf("%c INPUT ERROR! Not included num or operator.\n", ch);
            exit(1);
        }

        if (ch == '-') {
            char fch = exp[i-1];
            if (!(fch >= '0' && fch <= '9')) {
                // printf("Call\n");
                if(*cap == size)
                    exp = exprealloc(exp, cap);
                size++;
                for (int j = size; j >= i; j--) {
                    exp[j] = exp[j - 1];
                }
                exp[i]='0';
            }
            i++;
        }
        
        if(ch == '.') {
            if(!chkdot) chkdot = 1;
            else  {
                printf("INPUT ERROR! Dot has been writen more then two times.\n");
                exit(1);
            }
        }

        if(ch == '+' || ch == '-' || ch == '*' || ch == '(')
            chkdot = 0;

        if (ch == '(') {
            chkbraket = 1;
            push(s,ch);
            
            if (i!=0) {
                char fch = exp[i-1];
                if (fch != '+' && fch != '-' && fch != '*' && fch != '(') {
                    // printf("Call\n");
                    if(*cap == size)
                        exp = exprealloc(exp, cap);
                    size++;
                    for (int j = size; j >= i; j--) {
                        exp[j] = exp[j - 1];
                    }
                    exp[i]='*';
                }
            }
        }
        else if(ch == ')') {
            if (isEmpty(s)) {
                printf("INPUT ERROR! Maybe '('is missing.\n");
                exit(1);
            }
            else {
                pop(s);
            }
        }
    }

    if(isEmpty(s) == 0) {
        printf("INPUT ERROR! Maybe ')'is missing.\n");
        exit(1);
    }
    return exp;
}

char* infix2postfix(char exp[], int cap) { // 내부함수 : 후위표기 처리함수
	int i = 0, k = 0;	// i : loop문 제어 변수, k : 배열 위치에 관한 변수
	char* p_exp = (char*)malloc(sizeof(char) * cap);
	char d, op;	// d : 배열문자 저장될t 임시변수, op : 스택값 저장될 임시변수
	// int len = size;	// 배열의 길이를 저장 변수
	// printf(" %d ", len);
	Stack *n = (Stack*)malloc(sizeof(Stack) * cap);
    int origin = size;
	init(n);	// 스택 초기화
    // exprealloc(p_exp, &cap);
	for (i = 0; i < origin; i++)
	{
        // if(size == cap)  exprealloc(p_exp, &cap);
		d = exp[i];	// 입력받은 배열을 1칸씩 전진시키면서 문자를 확인하고
		switch (d)		// 아래의 case문대로 조건실행, 혹은 default에 따라 처리함.
		{
		case '+': case '-': case '*': case '/':	// 연산자일때
            p_exp[k++] = ' ';
			while (!isEmpty(n) && (priority(d) <= priority(peek(n)))) {
				p_exp[k++] = pop(n); // 스택에 있는 연산자의 우선순위가 더 크거나 같으면 출력
                p_exp[k++] = ' ';
                size++;
            }
			push(n, d);
            size++;
			break;
		case '(':
			if (exp[i-1])
			    push(n, d);
            size--;
			break;
		case ')':
			op = pop(n);	// 왼쪽 괄호를 만날 때까지 출력
            p_exp[k++] = ' ';
			while (op != '(')
			{
				p_exp[k++] = op; // 괄호안의 연산자 출력
				op = pop(n);
                p_exp[k++] = ' ';
                size++;
			}
            // size--;
			break;
		default:
            p_exp[k++] = d;
            // p_exp[k++] = ' '; 
            // size++; // 숫자 출력
            break;
		}
        
	}
	while (!isEmpty(n)) {
        p_exp[k++] = ' ';
		p_exp[k++] = pop(n);
        size++;
    }
    
    return p_exp;
}


DList* makeList() { // 리스트 만들기
    DList *DL = (DList*)malloc(sizeof(DList));
    DL->head = NULL; DL->tail = NULL;
    DL->next = NULL;
    return DL;
}


void insertNode(DList* DL, char data) { // 노드 뒤에 뉴노드 삽입
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode -> Data = data;
    if (DL -> head == NULL) {
        newNode -> next = NULL;
        newNode -> pre = NULL;
        DL -> head = newNode;
        DL -> tail = newNode;
    }
    else {
        newNode -> next = DL -> tail -> next;
        DL -> tail -> next = newNode;
        newNode -> pre = DL -> tail;
        DL -> tail = newNode;
        if (newNode -> next != NULL)
            newNode -> next -> pre = newNode;
    }
}


void insertNodeMiddle(DList* DL, char data, Node* idx) { // Ư�� ��� �տ� ����� ����
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode->Data = data;
    if (DL->head == NULL) {
        newNode->next = NULL;
        newNode->pre = NULL;
        DL->head = newNode;
        DL->tail = newNode;
    }
    else {
        newNode->pre = idx->pre;
        newNode->next = idx;
        idx->pre = newNode;
        if (newNode->pre != NULL)
            newNode->pre->next = newNode;
        else
            DL->head = newNode;
    }
}


void insertNodeHead(DList* DL, char data) { // 노드 앞에 뉴노드 삽입
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode -> Data = data;
    if (DL -> head == NULL) {
        newNode -> next = NULL;
        newNode -> pre = NULL;
        DL -> head = newNode;
        DL -> tail = newNode;
    }
    else {
        newNode -> pre = DL -> head -> pre;
        DL -> head -> pre = newNode;
        newNode -> next = DL -> head;
        DL -> head = newNode;
        if (newNode -> pre != NULL)
            newNode -> pre -> next = newNode;
    }
}


// void deleteNode(DList* DL, Node* Dnode) { // 리스트의 원하는 Dnode 삭제
//     if (DL -> head == NULL) return;
//     else if (Dnode == NULL) return;
//     else {
//         Dnode -> pre -> next = Dnode -> next;
//         Dnode -> next -> pre = Dnode -> pre;
//         free(Dnode);
//     }
// }


// char* getList(DList* DL) { // 리스트에 저장된 값 스트링(주소)으로 가져옴
//     Node* tmp;
//     char* num;
//     while (tmp != NULL) {
//         strcat(num, tmp->Data);
//         tmp = tmp -> next;
//     }
//     return num;
// }


// Node* searchNode(DList* DL, char* data) {
//     Node *tmp;
//     tmp = DL -> head;
//     while (tmp != NULL) {
//         if (strcmp(tmp -> Data, data) == 0) return tmp;
//         else tmp = tmp -> next;
//     }
//     return tmp;
// }


LStack* makeLS() { // 새로운 리스트스텍과 그 초기값 지정
    LStack* ls = (LStack*)malloc(sizeof(LStack*));
    ls->top = NULL;
}


void LSpush(LStack* LS, DList* DL) { // 리스트스텍에 푸시
    DL -> next = LS -> top;
    LS -> top = DL;
}


DList* LSpop(LStack* LS) { // 리스트스텍에서 팝
    DList* DL = LS -> top;
    LS -> top = DL-> next;
    return DL; // !메모리 누수 있음!
}


DList* LSref(LStack* LS) { // 리스트스텍에서 탑의 데이터만 가져옴
    return LS -> top;
}


void Dlist_maker(char* exp, LStack* fac, Stack* op) { 
    int num_len = 0;
    int i = 0;
    init(op);
    for (; num_len <= size; num_len++){
        if (exp[num_len] == ' '){ // 빈칸이면
            if ((exp[num_len-1] >= '0' && exp[num_len-1] <= '9') || exp[num_len-1] == '.'){ //빈칸 앞이 숫자이면
                DList* list = makeList();
                for (; i < num_len; i++) {
                    insertNode(list, exp[i]);
                    // printf("%c", exp[i]); // 인자 확인용
                }
                LSpush(fac, list);
                // printf(" is factor.\n"); // 인자 확인용 코드
                i++;
            }
            else if (exp[num_len-1] == '-' || exp[num_len-1] == '+' || exp[num_len-1] == '*'){ //빈칸 앞이 연산자이면
                push(op,exp[num_len-1]);
                // printf("operator is %c.\n", exp[num_len-1]); // 연산자 확인용
                i+=2;
            }
            else if (exp[num_len-1] == ' ') {
                i+=2;
            }
        }
        else if(num_len+1 == size) {
            // printf("CALL"); // 마지막 연산자 스텍여부 확인용
            push(op, exp[num_len]);
        }
    }
}

//곱셈을 계산하는 함수
DList* multiply(DList* a, DList* b) {
    //포인터 초기화
    DList* result = makeList();

    Node* result_cur = result->tail;
    Node* a_cur = a->tail;
    Node* b_cur = b->tail;
    Node* temp_cur = NULL;

    bool isminus = false;

    int temp = 0;
    int upper = 0;
    int real_num = 0;


    //두 번째 연결 리스트 요소 이동
    while (a_cur!=NULL) {
        // '-' 부호가 있는지 검사
        // '.'일 때 한 칸 더 움직이기
        DList* temp_list = makeList();
        if (a_cur->Data == '.') {
            a_cur = a_cur->pre;
        }
        else if (a_cur->Data == '-') {
            isminus = true;
            a_cur = a_cur->pre;
        }
        temp_list = getTempMultiple(a, b, a_cur);
        temp_cur = temp_list->head;

        //while (temp_cur != NULL) {
        //    printf("%c ", temp_cur->Data);
        //    temp_cur = temp_cur->next;
        //}
        //printf("\n");
        result = list_add(result, temp_list);
        a_cur = a_cur->pre;
    }
    //점 찾기 위해 초기화
    a_cur = a->tail;
    b_cur = b->tail;
    result_cur = result->tail;
    //두 연결 리스트의 '.'위치 검사
    while (a_cur->Data == '.') {
        if (a_cur == NULL) {
            temp_cur = result_cur = result->tail;
            break;
        }
        a_cur = a_cur->pre;
        result_cur = result_cur->pre;
    }
    while (b_cur->Data == '.') {
        if (b_cur == NULL) {
            result_cur = temp_cur;
            break;
        }
        a_cur = a_cur->pre;
        result_cur = result_cur->pre;
    }
    if (result_cur != result->tail) {
        insertNodeMiddle(result, '.', result_cur);
    }
    if (isminus) {
        insertNodeHead(result, '-');
    }
    free(result_cur);
    free(a_cur);
    free(b_cur);
    return result;
}

//임시 DList를 만들어서 쪼개서 계산한다.
DList* getTempMultiple(DList* a, DList* b, Node* a_cur_origin) {
    Node* a_cur = a->tail;
    Node* b_cur = b->tail;
    DList* temp_list = makeList();
    Node* temp_cur = temp_list->tail;

    //앞에 0 추가
    while (a_cur != a_cur_origin) {
        if (a_cur == NULL) return NULL;
        a_cur = a_cur->pre;
        insertNodeHead(temp_list, '0');
        temp_cur = temp_list->head;
    }
    //한 바퀴만 반복
    if (a_cur->Data == '.'|| a_cur->Data == '-') {
        a_cur = a_cur->pre;
    }
    //첫 번째 연결 리스트 요소 이동
    while (b_cur != NULL) {
        if (b_cur->Data == '.'|| b_cur->Data == '-') {
            b_cur = b_cur->pre;
        }
        //곱셈 후 한자리씩 분할
        int temp = (a_cur->Data - '0') * (b_cur->Data - '0');
        int real_num = temp % 10;
        int upper = (int)(temp / 10);

        //노드가 없을 때 생성 후 값 입력
        if (temp_cur == NULL||temp_cur->Data=='0') {
            insertNodeHead(temp_list, real_num + '0');
            temp_cur = temp_list->head;
        }
        //노드가 있을 때 기존 노드의 값에서 1의 자리 수 더하기
        else {
            int inner_temp = ((temp_cur->Data - '0') + real_num);
            if (inner_temp >= 10) {
                upper += 1;
                temp_cur->Data = (inner_temp - 10) + '0';
            }
            else {
                temp_cur->Data = ((temp_cur->Data - '0') + real_num) + '0';
            }
        }
        if (upper != 0) {
            insertNodeHead(temp_list, upper+'0');
        }
        b_cur = b_cur->pre;
        temp_cur = temp_cur->pre;
    }
    return temp_list;
}

void Printer(DList* DL) { 
    Node* num = DL -> head;
    while (num != NULL) {
        char data = num -> Data;
        num = num->next;
        printf("%c", data);
    }
} 

int Dot(DList* dlist){
    int Dot = 0;
    Node *cur = dlist->head;
    while (cur != NULL){
        if (cur->Data == 46){
            Dot = 1;
            break;
        }
        cur = cur->next;
    }
    return Dot;
}


int integer_size(DList* dlist){//정수부분의 길이를 리턴
    int size = 0;
    Node *cur = dlist->head;
    while (cur->Data != 46){//점 이라면
        cur = cur->next;
        size++;
    }
    return size;
}


int decimal_size(DList* dlist){//소수부분의 길이를 리턴
    int size = 0;
    Node *cur = dlist->tail;
    while (cur->Data != 46){//점 이라면
        cur = cur->pre;
        size++;
    }
    return size;
}


// int main() { // 테스트용 메인 함수 - 추후 삭제 요망
//     int cap;
//     char *exp = fileRead(&cap);
//     for(int i = 0; i < size; i++)
//         printf("%c", exp[i]);
//     printf("\n");
//     char* pexp = infix2postfix(exp, cap);
    
//     for(int i = 0; i < size; i++)
//         printf("%c", pexp[i]);
//     printf("|\n");
//     Stack* op = (Stack*)malloc(sizeof(Stack*)); // 계산에 사용될 연산자들을 저장하는 스텍(계산순서대로 쌓임)
//     LStack* fac = makeLS(); // 계산에 사용될 인수들을 저장하는 스텍(계산순서대로 쌓임)
//     Dlist_maker(pexp, fac, op);

//     // DList* a = LSpop(fac);

//     // ListNum(a);

//     // printf("\n");

//     // DList* b = LSpop(fac);

//     // ListNum(b);

//     // printf("\n");

//     DList* c = LSpop(fac);

//     Printer(c);

//     printf("\n");

//     DList* d = LSpop(fac);

//     Printer(d);

//     printf("\n");

//     DList* multi_result = multiply(c, d);

//     Printer(multi_result);
// }
