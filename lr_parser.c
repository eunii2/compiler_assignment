// hw01 - LR Parser 구현
// 소프트웨어학부 2022078054 최가은

#include <stdio.h>
#define MAX 100

typedef struct { // 파서 스택에 사용될 구조체 정의
    int state; // 파서의 현재 상태
    char symbol; // 현재 심볼(토큰)
} Component;

Component stack[MAX]; // 파서 스택 선언
int sp; // 스택 포인터

int action_tbl[12][6] = { // 액션 테이블 정의 (상태 x 입력 심볼)
        {5, 0, 0, 4, 0, 0},
        {0, 6, 0, 0, 0, 999},
        {0, -2, 7, 0, -2, -2},
        {0, -4, -4, 0, -4, -4},
        {5, 0, 0, 4, 0, 0},
        {0, -6, -6, 0, -6, -6},
        {5, 0, 0, 4, 0, 0},
        {5, 0, 0, 4, 0, 0},
        {0, 6, 0, 0, 11, 0},
        {0, -1, 7, 0, -1, -1},
        {0, -3, -3, 0, -3, -3},
        {0, -5, -5, 0, -5, -5}};

int goto_tbl[12][4] = { // goto 테이블 정의 (상태 x 비터미널 심볼)
        {0, 1, 2, 3},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 8, 2, 3},
        {0, 0, 0, 0},
        {0, 0, 9, 3},
        {0, 0, 0, 10},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

// left hand-side 심볼과 규칙 길이 정의
char lhs[] = {' ', 'E', 'E', 'T', 'T', 'F', 'F'};
int rhs_len[] = {0, 3, 1, 3, 1, 3, 1};

// 입력 심볼과 비터미널 심볼 정의
char token[] = {'d', '+', '*', '(', ')', '$'};
char NT[] = {' ', 'E', 'T', 'F'};

// 현재 상태와 입력 심볼에 따라 액션 테이블에서의 액션 결정
int get_action(int state, char symbol){
    int symbol_idx = -1; // 심볼 인덱스 초기화
    // 입력 심볼 배열을 순회하며 입력 심볼의 인덱스 탐색
    for (int i = 0; i < sizeof(token) / sizeof(token[0]); ++i){
        if (token[i] == symbol){
            symbol_idx = i; // 인덱스 찾음
            break;
        }
    }
    // 인덱스가 유효하면 액선 테이블에서 해당 액션 반환, 아닐 경우 -100 반환
    return symbol_idx != -1 ? action_tbl[state][symbol_idx] : -100;
}

// 현재 상태와 비터미널 심볼에 따라 goto 테이블에서 상태 결정
int get_goto(int state, char nt_symbol){
    int nt_index = -1; // 비터미널 심볼 인덱스 초기화
    // 비터미널 심볼 배열을 순회하며 비터미널 심볼의 인덱스 탐색
    for (int i = 0; i < sizeof(NT) / sizeof(NT[0]); ++i){
        if (NT[i] == nt_symbol){
            nt_index = i; // 인덱스 찾음
            break;
        }
    }
    // 인덱스 유효하면 goto 테이블에서 해당 상태 반환
    return goto_tbl[state][nt_index];
}

// 스택의 현재 상태 출력
void print_stack() {
    // 스택의 처음부터 스택 포인터까지 모든 요소 출력
    for (int i = 0; i <= sp; i++) {
        // 심볼이 공백이 아니면 심볼 출력
        if (stack[i].symbol != ' ') printf("%c", stack[i].symbol);
        // 상태 출력
        printf("%d", stack[i].state);
    }
}

// LR_Parser 구현부
void LR_Parser(char *input) {
    sp = -1;
    stack[++sp] = (Component){0, ' '}; // 초기 스택 상태 정의
    int idx = 0; // 입력 문자열의 현재 인덱스
    int accept = 0; // 파싱 성공 여부
    int step = 1; // 파싱 단계

    // 초기 스택 상태 출력
    printf("(%d) initial : ", step++);
    print_stack();
    printf("       %s\n", input);

    // 입력 문자열 끝까지 처리하거나, 파싱이 완료될 때까지 반복
    while (!accept && input[idx] != '\0') {
        int state = stack[sp].state; // 현재 스택 상태
        char symbol = input[idx]; // 현재 입력 심볼
        int action = get_action(state, symbol); //액션 결정

        // 액션에 따른 처리들
        if (action == -100) { // Invalid token
            printf("(%d) invalid token (%c) error\n", step++, symbol);
            return;
        }
        else if (action == 999) { // Accept
            printf("(%d) accept\n", step++);
            accept = 1; // 완료 플래그 설정
        }
        else if (action > 0) { // Shift
            printf("(%d) shift %d : ", step++, action);
            stack[++sp] = (Component){action, symbol}; // 스택에 상태와 심볼 푸시
            print_stack();
            printf("\t%s\n", input + idx + 1);
            idx++;
        }
        else if (action < 0) { // Reduce
            int rule = -action;
            printf("(%d) reduce %d : ", step++, rule);
            sp -= rhs_len[rule]; // 규칙에 따라 스택에서 팝
            int goto_state = get_goto(stack[sp].state, lhs[rule]); // 새로운 상태 결정
            stack[++sp] = (Component){goto_state, lhs[rule]}; // 스택에 상태와 비터미널 심볼 푸시
            print_stack();
            printf("\t%s\n", input + idx);
        }
        else { // Error
            printf("(%d) error\n", step++);
            return;
        }
    }
}

int main(){
    char input[MAX];
    while (1){
        printf("\nInput: ");
        scanf("%s", input);
        if (input[0] == '$') // '$' 입력 시 종료
            break;
        LR_Parser(input);
    }
    return 0;
}