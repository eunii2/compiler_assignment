// hw01 - LR Parser 구현
// 소프트웨어학부 2022078054 최가은

#include <stdio.h>
#define MAX 100

typedef struct {
    int state;
    char symbol;
} Component;

Component stack[MAX];
int sp;

int action_tbl[12][6] = {
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

int goto_tbl[12][4] = {
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

char lhs[] = {' ', 'E', 'E', 'T', 'T', 'F', 'F'};
int rhs_len[] = {0, 3, 1, 3, 1, 3, 1};

char token[] = {'d', '+', '*', '(', ')', '$'};
char NT[] = {' ', 'E', 'T', 'F'};

int get_action(int state, char symbol){
    int symbol_idx = -1;
    for (int i = 0; i < sizeof(token) / sizeof(token[0]); ++i){
        if (token[i] == symbol){
            symbol_idx = i;
            break;
        }
    }
    return symbol_idx != -1 ? action_tbl[state][symbol_idx] : -100;
}

int get_goto(int state, char nt_symbol){
    int nt_index = -1;
    for (int i = 0; i < sizeof(NT) / sizeof(NT[0]); ++i){
        if (NT[i] == nt_symbol){
            nt_index = i;
            break;
        }
    }
    return goto_tbl[state][nt_index];
}

void print_stack() {
    for (int i = 0; i <= sp; i++) {
        if (stack[i].symbol != ' ') printf("%c", stack[i].symbol);
        printf("%d", stack[i].state);
    }
}

void LR_Parser(char *input) {
    sp = -1;
    stack[++sp] = (Component){0, ' '};
    int idx = 0;
    int accept = 0;
    int step = 1;

    printf("(%d) initial : ", step++);
    print_stack();
    printf("       %s\n", input);

    while (!accept && input[idx] != '\0') {
        int state = stack[sp].state;
        char symbol = input[idx];
        int action = get_action(state, symbol);

        if (action == -100) {
            printf("(%d) invalid token (%c) error\n", step++, symbol);
            return;
        }
        else if (action == 999) { // Accept
            printf("(%d) accept\n", step++);
            accept = 1;
        }
        else if (action > 0) { // Shift
            printf("(%d) shift %d : ", step++, action);
            stack[++sp] = (Component){action, symbol};
            print_stack();
            printf("\t%s\n", input + idx + 1);
            idx++;
        }
        else if (action < 0) { // Reduce
            int rule = -action;
            printf("(%d) reduce %d : ", step++, rule);
            sp -= rhs_len[rule];
            int goto_state = get_goto(stack[sp].state, lhs[rule]);
            stack[++sp] = (Component){goto_state, lhs[rule]};
            print_stack();
            printf("\t%s\n", input + idx);
        }
        else {
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
        if (input[0] == '$')
            break;
        LR_Parser(input);
    }
    return 0;
}