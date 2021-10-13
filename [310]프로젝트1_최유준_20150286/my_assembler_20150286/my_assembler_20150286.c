/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20150286.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	//make_opcode_output("output_20150286.txt");

	
	
	
	make_symtab_output("symtab_20150286");
    make_literaltab_output("literaltab_20150286");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_20150286") ; 
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;
	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE *file;
	int errno;

	/* add your code here */
    file = fopen(inst_file, "r");
    int i = 0;
    if(file != NULL) {
        errno = 0;
        while (!feof(file)) {
             inst_table[i] = (inst*)malloc(sizeof(inst));//동적할당
             if (4 != fscanf(file, "%s %d %s %d", inst_table[i]->opname, &inst_table[i]->format, &inst_table[i]->opcode, &inst_table[i]->odnum)) {
                 errno = -1;
                 break;
             }
             else {//
                 i++;
                 inst_index++;
             }
            
		}
	 }
	
    
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE *file;
	int errno;

	/* add your code here */
    int i = 0;
    char str[500];
    int length;

    file = fopen(input_file, "r");
    if (file != NULL) {
        while (!feof(file)) {
            fgets(str, sizeof(str), file);
            str[strlen(str) - 1] = '\0';
            length = strlen(str);
            input_data[i] = (char *)malloc(sizeof(char)*length+1);
            if (str[0] == '.')
                continue;
            strcpy(input_data[i], str);
            i++;
            ++line_num;

        }
        errno = 0;
    }
    else
        errno = 1;
   
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	/* add your code here */
    char check;
    int first;

    //label이 없으면 first = 0, 있으면 first = 1
    sscanf(str, "%c", &check);

    if ((check == '\t') || (check == ' '))
        first = 0;
    else
        first = 1;
      
    token_table[token_line] = (token*)malloc(sizeof(token));//동적할당

    if (check == '.') {
        return 0;
    }
    token_table[token_line]->operand[0] = "";
    token_table[token_line]->operand[1] = "";
    token_table[token_line]->operand[2] = ""; //operand 초기화

    if (first == 0) { //label이 없을 때
        
        token_table[token_line]->label = "";
        token_table[token_line]->operator = strtok(str, "\t");
        token_table[token_line]->operand[0] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");
        
        
        
    }
    else if (first == 1) { //label이 있을 때
        token_table[token_line]->label = strtok(str, "\t");
        token_table[token_line]->operator = strtok(NULL, "\t");
        token_table[token_line]->operand[0] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");

    }
    //operand 분할
    token_table[token_line]->operand[0] = strtok(token_table[token_line]->operand[0], ",");
    token_table[token_line]->operand[1] = strtok(NULL, ",");
    token_table[token_line]->operand[2] = strtok(NULL, ",");

    
    token_line++;
    return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
    int e = 0;//4형식인지
   

    if (str == NULL)
        return 0;
    
    if (str[0] == '+') {
        e = 1;//4형식
        str = str + 1;
    }

    for (int i = 0; i < inst_index; i++) {//inst_table에서 명령어 검색
        if (strcmp(inst_table[i]->opname, str) == 0) {
            return i;
        }
    }
    return -1;

	/* add your code here */

}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

    //char* str;
    int i;
    int len;
    int temaddr;
    for (i = 0; i < line_num; i++) {
        token_parsing(input_data[i]);
    }
    
    sym_line = 0;
    i = 0;
    locctr = 0;
    len = 0;
    while (strcmp(token_table[i]->label, "RDREC") != 0) {//첫번째 CSECT
        if (i == 0)//첫번째 주소는 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//증가된 locctr 저장

        if(token_table[i]->operand[0] == NULL) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //리터럴이면
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//리터럴테이블에 복사
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label이 있으면 symbol에 저장
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator일 경우
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1형식
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2형식
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3형식
                    locctr += 3;
                }
                else//4형식
                    locctr += 4;
            }
        }

        else if (strcmp(token_table[i]->operator, "RESW") == 0) {//RESW
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += (3 * temaddr);
        }
        else if (strcmp(token_table[i]->operator, "RESB") == 0) {
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += temaddr;
        }
        else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
            locctr += 1;
            }
        else if (strcmp(token_table[i]->operator, "EQU") == 0) {
            if (strcmp(token_table[i]->label, "MAXLEN") == 0) {
                int adr1 = 0, adr2 = 0;
                char* ptr = strtok(token_table[i]->operand[0], "-");
                int j;
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr1= sym_table[j].addr;
                            break;
                        }
                    }
                }
                ptr = strtok(NULL, "-");
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr2 = sym_table[j].addr;
                            break;
                        }
                    }
                }
                tok_adr[i] = adr1 - adr2;

            }
        }
        if (sym_table[sym_line].symbol[0] != NULL) {
            sym_table[sym_line].addr = tok_adr[i];
        }
        i++;
        sym_line++;
        len++;
    }
    csect_len[0] = len - 1;


    int k = 0;
    locctr = 0;
    len = 0;
    while (strcmp(token_table[i]->label, "WRREC") != 0) {//두 번째 CSECT
        if (k == 0)//첫번째 주소는 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//증가된 locctr 저장

        if (token_table[i]->operand[0] == NULL) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //리터럴이면
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//리터럴테이블에 복사
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label이 있으면 symbol에 저장
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator일 경우
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1형식
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2형식
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3형식
                    locctr += 3;
                }
                else//4형식
                    locctr += 4;
            }
        }

        else if (strcmp(token_table[i]->operator, "RESW") == 0) {//RESW
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += (3 * temaddr);
        }
        else if (strcmp(token_table[i]->operator, "RESB") == 0) {
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += temaddr;
        }
        else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
            locctr += 1;
        }
        else if (strcmp(token_table[i]->operator, "WORD") == 0) {
            locctr += 3;
        }
        else if (strcmp(token_table[i]->operator, "EQU") == 0) {
            if (strcmp(token_table[i]->label, "MAXLEN") == 0) {
                int adr1 = 0, adr2 = 0;
                char* ptr = strtok(token_table[i]->operand[0], "-");
                int j;
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr1 = sym_table[j].addr;
                            break;
                        }
                    }
                }
                ptr = strtok(NULL, "-");
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr2 = sym_table[j].addr;
                            break;
                        }
                    }
                }
                tok_adr[i] = adr1 - adr2;

            }
        }
        if (sym_table[sym_line].symbol[0] != NULL) {
            sym_table[sym_line].addr = tok_adr[i];
        }
        i++;
        sym_line++;
        k++;
        len++;
    }
    csect_len[1] = len - 1;

    
    locctr = 0;
    k = 0;
    len = 0;
    while (i >= 0) {//세 번째 CSECT
        if (k == 0)//첫번째 주소는 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//증가된 locctr 저장

        if ((token_table[i]->operand[0] == NULL) || strcmp(token_table[i]->operand[0], "FIRST") == 0) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //리터럴이면
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//리터럴테이블에 복사
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label이 있으면 symbol에 저장
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator일 경우
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1형식
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2형식
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3형식
                    locctr += 3;
                }
                else//4형식
                    locctr += 4;
            }
        }

        else if (strcmp(token_table[i]->operator, "RESW") == 0) {//RESW
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += (3 * temaddr);
        }
        else if (strcmp(token_table[i]->operator, "RESB") == 0) {
            temaddr = atoi(token_table[i]->operand[0]);
            locctr += temaddr;
        }
        else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
            locctr += 1;
        }
        else if (strcmp(token_table[i]->operator, "EQU") == 0) {
            if (strcmp(token_table[i]->label, "MAXLEN") == 0) {
                int adr1 = 0, adr2 = 0;
                char* ptr = strtok(token_table[i]->operand[0], "-");
                int j;
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr1 = sym_table[j].addr;
                            break;
                        }
                    }
                }
                ptr = strtok(NULL, "-");
                if (ptr != NULL) {
                    for (j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, ptr) == 0) {
                            adr2 = sym_table[j].addr;
                            break;
                        }
                    }
                }
                tok_adr[i] = adr1 - adr2;

            }
        }
        if (sym_table[sym_line].symbol[0] != NULL) {
            sym_table[sym_line].addr = tok_adr[i];
        }
        if (strcmp(token_table[i]->operator, "END") == 0) {
            break;
        }
        i++;
        sym_line++;
        len++;
    }
    csect_len[2] = len - 1;
	/* input_data의 문자열을 한줄씩 입력 받아서 
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
    return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
/*
void make_opcode_output(char *file_name)
{
	add your code here
    FILE* file;
    file = fopen(file_name, "w");

    if (file_name != NULL) {
        for (int i = 0; i < token_line-2; i++) {

            //case1: not instruction
            if (search_opcode(token_table[i]->operator) < 0) {

                //case1-1: not instruction, no label
                if ((input_data[i][0] == '\t') || (input_data[i][0] == ' ')) {
                    if (token_table[i]->operand[0] == NULL) {//operand가 없음
                        fprintf(file, "\t%s\n", token_table[i]->operator);
                    }
                    else if(token_table[i]->operand[1] == NULL)//operand 1개
                        fprintf(file, "\t%s\t%s\n", token_table[i]->operator, token_table[i]->operand[0]);
                    
                    else if(token_table[i]->operand[2] == NULL)//operand 2개
                        fprintf(file, "\t%s\t%s,%s\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1]);
                    
                    else if((token_table[i]->operand[0] != NULL) &&(token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] != NULL))//operand 3개
                        fprintf(file, "\t%s\t%s,%s,%s\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->operand[2]);
                }
     
                //case1-2: not instruction, label
                else {
                    if (token_table[i]->operand[0] == NULL) {//operand 0개
                        fprintf(file, "%s\t%s\n", token_table[i]->label, token_table[i]->operator);
                    }
                    else if (token_table[i]->operand[1] == NULL)
                        fprintf(file, "%s\t%s\t%s\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0]);
                    
                    else if (token_table[i]->operand[2] == NULL)
                        fprintf(file, "%s\t%s\t%s,%s\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1]);
                    
                    else if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] != NULL))
                        fprintf(file, "%s\t%s\t%s,%s,%s\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->operand[2]);
                }
            }
            //case2: instruction
            else {
                //case2-1: instruction, no label
                if ((input_data[i][0] == '\t') || (input_data[i][0] == ' ')) {
                    if (inst_table[search_opcode(token_table[i]->operator)]->odnum == 0) {
                        fprintf(file, "\t%s\t\t%c%c\n", token_table[i]->operator, inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                    }
                    else if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] == NULL))//op 1개
                        fprintf(file, "\t%s\t%s\t%c%c\n", token_table[i]->operator, token_table[i]->operand[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                    
                    else if ((token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] == NULL))//op 2개
                        fprintf(file, "\t%s\t%s,%s\t%c%c\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                    
                    else if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] != NULL))
                        fprintf(file, "\t%s\t%s,%s,%s\t%c%c\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->operand[2], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                }

                //case2-2: instruction, label
                else {
                    if (token_table[i]->operand[0] == NULL)
                        fprintf(file, "%s\t%s\t\t%c%c\n", token_table[i]->label, token_table[i]->operator, inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);

                    else if (token_table[i]->operand[1] == NULL)
                        fprintf(file, "%s\t%s\t%s\t%c%c\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);

                    else if (token_table[i]->operand[2] == NULL)
                        fprintf(file, "%s\t%s\t%s,%s\t%c%c\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                    
                    else if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] != NULL))
                        fprintf(file, "%s\t%s\t%s,%s,%s\t%c%c\n", token_table[i]->label, token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->operand[2], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                }
            }
        }
    }
}
*/

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
    FILE* file;


    if (file_name != NULL) {
        file = fopen(file_name, "w");
        for (int i = 0; i < sym_line; i++) {
            if (sym_table[i].symbol[0] != 't') {
                if(strcmp(sym_table[i].symbol, "RDREC") == 0 || (strcmp(sym_table[i].symbol, "WRREC") == 0))
                    fprintf(file, "\n");
                fprintf(file, "%s\t\t%X\n", sym_table[i].symbol, sym_table[i].addr);
            }
        }
    }
    else {
        for (int i = 0; i < sym_line; i++) {
            if (sym_table[i].symbol[0] != 't') {
                if (strcmp(sym_table[i].symbol, "RDREC") == 0 || (strcmp(sym_table[i].symbol, "WRREC") == 0))
                    printf("\n");
                printf("%s\t\t%X\n", sym_table[i].symbol, sym_table[i].addr);
            }
        }
        printf("\n");
    }
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *file_name)
{
	/* add your code here */
    FILE* file;

    char* str;
    char str1[10];

    if (file_name != NULL) {
        file = fopen(file_name, "w");
        for (int i = 0; i < lit_line; i++) {
            strcpy(str1, literal_table[i].literal);
            str = strtok(str1, "\'");
            str = strtok(NULL, "\'");
            fprintf(file, "%s\t\t%X\n", str, literal_table[i].addr);
        }
    }
    else {
        for (int i = 0; i < lit_line; i++) {
            strcpy(str1, literal_table[i].literal);
            str = strtok(str1, "\'");
            str = strtok(NULL, "\'");
            printf("%s\t\t%X\n", str, literal_table[i].addr);
        }
        printf("\n");
    }
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
    int opcode;
    int address;
    int i = 0;
    while (strcmp(token_table[i]->label, "RDREC") != 0) {//첫번째 section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int형 opcode에 코드 저장
            if (inst_table[search_opcode(token_table[i]->operator)]->format != 2){//format3,4
                if (strncmp(token_table[i]->operand[0], "#", 1) == 0) //immediate
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 4;
                else if(strncmp(token_table[i]->operand[0], "@", 1) == 0) //indirect
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 5;
                else
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 3 << 4;

                if ((token_table[i]->operand[1] != NULL) && (strcmp(token_table[i]->operand[1], "X") == 0))//index
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 3;
                token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 1;//PC relative

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB의 경우는 예외처리
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //addr 구하기
                char* str;
                int check = -1;
                int a = 0;
                int e = 0;
                if (token_table[i]->operand[0][0] == '#' || token_table[i]->operand[0][0] == '@') {
                    str = token_table[i]->operand[0] + 1;
                    if (token_table[i]->operand[0][0] == '#')
                        token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                }
                else
                    str = token_table[i]->operand[0];
                while(strcmp(sym_table[a].symbol, "RDREC") != 0){
                    if (strcmp(str, sym_table[a].symbol) == 0)//같은 CSECT symbol이면 check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol이 있으면
                    address = sym_table[check].addr - tok_adr[i+1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol이 없으면
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//다른 CSECT symbol이면 check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//다른 CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//리터럴이면
                                    address = literal_table[a].addr - tok_adr[i + 1];
                                    break;
                                }
                                else
                                    return -1;//에러
                            }
                        }
                    }
                }
                if(strncmp(token_table[i]->operator, "+", 1) != 0)
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12 | address;
                else {
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1;
                    objectcode[i] = opcode << 24 | token_table[i]->nixbpe << 20 | address;
                }

            }
            else {//2형식
                if (strcmp(token_table[i]->operand[0], "X") == 0)
                    address = 16;
                else if (strcmp(token_table[i]->operand[0], "A") == 0)
                    address = 0;
                else if (strcmp(token_table[i]->operand[0], "S") == 0)
                    address = 64;
                else if (strcmp(token_table[i]->operand[0], "T") == 0)
                    address = 80;

                objectcode[i] = opcode << 8 | address;
            }
        }
        i++;
    }
    int k = i;
    while (strcmp(token_table[i]->label, "WRREC") != 0) {//2번째 section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int형 opcode에 코드 저장
            if (inst_table[search_opcode(token_table[i]->operator)]->format != 2) {//format3,4
                if (strncmp(token_table[i]->operand[0], "#", 1) == 0) //immediate
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 4;
                else if (strncmp(token_table[i]->operand[0], "@", 1) == 0) //indirect
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 5;
                else
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 3 << 4;

                if ((token_table[i]->operand[1] != NULL) && (strcmp(token_table[i]->operand[1], "X") == 0))//index
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 3;
                token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 1;//PC relative

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB의 경우는 예외처리
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //addr 구하기
                char* str;
                int check = -1;
                int a = k;
                if (token_table[i]->operand[0][0] == '#' || token_table[i]->operand[0][0] == '@'){
                    str = token_table[i]->operand[0] + 1;
                    if (token_table[i]->operand[0][0] == '#')
                        token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                }

                else
                    str = token_table[i]->operand[0];
                while (strcmp(sym_table[a].symbol, "WRREC") != 0) {
                    if (strcmp(str, sym_table[a].symbol) == 0)//같은 CSECT symbol이면 check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol이 있으면
                    address = sym_table[check].addr - tok_adr[i + 1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol이 없으면
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//다른 CSECT symbol이면 check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//다른 CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//리터럴이면
                                    address = literal_table[a].addr - tok_adr[i + 1];
                                    break;
                                }
                            }
                        }
                    }
                }
                if (strncmp(token_table[i]->operator, "+", 1) != 0)
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12 | address;
                else {
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1;
                    objectcode[i] = opcode << 24 | token_table[i]->nixbpe << 20 | address;
                }

            }
            else {//2형식
                if (token_table[i]->operand[0] != NULL) {
                    if (strcmp(token_table[i]->operand[0], "X") == 0)
                        address = 1;
                    else if (strcmp(token_table[i]->operand[0], "A") == 0)
                        address = 0;
                    else if (strcmp(token_table[i]->operand[0], "S") == 0)
                        address = 4;
                    else if (strcmp(token_table[i]->operand[0], "T") == 0)
                        address = 5;

                    objectcode[i] = opcode << 8 | address << 4;
                }
                if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] != NULL)) {
                    if (strcmp(token_table[i]->operand[1], "X") == 0)
                        address = 1;
                    else if (strcmp(token_table[i]->operand[1], "A") == 0)
                        address = 0;
                    else if (strcmp(token_table[i]->operand[1], "S") == 0)
                        address = 4;
                    else if (strcmp(token_table[i]->operand[1], "T") == 0)
                        address = 5;

                    objectcode[i] = objectcode[i] | address;
                }
            }
        }
        i++;
    }
    k = i;
    while (i >= 0) {//3번째 section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int형 opcode에 코드 저장
            if (inst_table[search_opcode(token_table[i]->operator)]->format != 2) {//format3,4
                if (strncmp(token_table[i]->operand[0], "#", 1) == 0) //immediate
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 4;
                else if (strncmp(token_table[i]->operand[0], "@", 1) == 0) //indirect
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 5;
                else
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 3 << 4;

                if ((token_table[i]->operand[1] != NULL) && (strcmp(token_table[i]->operand[1], "X") == 0))//index
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 3;
                token_table[i]->nixbpe = token_table[i]->nixbpe | 1 << 1;//PC relative

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB의 경우는 예외처리
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //address 구하기
                char* str;
                int check = -1;
                int a = k;
                if (token_table[i]->operand[0][0] == '#' || token_table[i]->operand[0][0] == '@'){
                    str = token_table[i]->operand[0] + 1;
                    if (token_table[i]->operand[0][0] == '#')
                        token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                }

                else
                    str = token_table[i]->operand[0];
                while (a <= sym_line) {
                    if (strcmp(str, sym_table[a].symbol) == 0)//같은 CSECT symbol이면 check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol이 있으면
                    address = sym_table[check].addr - tok_adr[i + 1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol이 없으면
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//다른 CSECT symbol이면 check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//다른 CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//리터럴이면
                                    address = literal_table[a].addr - tok_adr[i + 1];
                                    break;
                                }
                            }
                        }
                    }
                }
                if (strncmp(token_table[i]->operator, "+", 1) != 0)
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12 | address;
                else {
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    token_table[i]->nixbpe = token_table[i]->nixbpe | 1;
                    objectcode[i] = opcode << 24 | token_table[i]->nixbpe << 20 | address;
                }

            }
            else {//2형식
                if (token_table[i]->operand[0] != NULL) {
                    if (strcmp(token_table[i]->operand[0], "X") == 0)
                        address = 1;
                    else if (strcmp(token_table[i]->operand[0], "A") == 0)
                        address = 0;
                    else if (strcmp(token_table[i]->operand[0], "S") == 0)
                        address = 4;
                    else if (strcmp(token_table[i]->operand[0], "T") == 0)
                        address = 5;

                    objectcode[i] = opcode << 8 | address << 4;
                }
                if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] != NULL)) {
                    if (strcmp(token_table[i]->operand[1], "X") == 0)
                        address = 1;
                    else if (strcmp(token_table[i]->operand[1], "A") == 0)
                        address = 0;
                    else if (strcmp(token_table[i]->operand[1], "S") == 0)
                        address = 4;
                    else if (strcmp(token_table[i]->operand[1], "T") == 0)
                        address = 5;

                    objectcode[i] = objectcode[i] | address;
                }

                
            }
        }
        if (strcmp(token_table[i]->operator, "END") == 0) {
            break;
        }
        i++;
    }

    return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
    /* add your code here */
    FILE* file;

    if (file_name != NULL) {
        file = fopen(file_name, "w"); 
        int i = 0;
        int txtlen1 = 0;//txt길이변수
        int txtlen2 = 0;
        int start = 0;//명령어 시작위치
        int second = 0;//두번째 T 시작위치
        while (strcmp(token_table[i]->label, "RDREC") != 0) {//section 1
            fprintf(file, "H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0] - 1]);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    fputc('D', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);

                        fprintf(file, "%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    fputc('\n', file);
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    fputc('R', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);
                    }
                    fputc('\n', file);
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정(BYTE단위)
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {//2형식 +2
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {//4형식 +4
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {//3형식 +3
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    fprintf(file, "%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        fprintf(file, "%08X", objectcode[j]);
                    else
                        fprintf(file, "%06X", objectcode[j]);
                }
            }
            second = i;
            fputc('\n', file);
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//두번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen2 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen2 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen2 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 4;
                    }
                    else {
                        if (txtlen2 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//두번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    fprintf(file, "%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        fprintf(file, "%08X", objectcode[j]);
                    else
                        fprintf(file, "%06X", objectcode[j]);
                }
            }
            fputc('\n', file);
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "RDREC") == 0) {
                    fprintf(file, "M%06X05+RDREC\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "WRREC") == 0) {
                    fprintf(file, "M%06X05+WRREC\n", mod[cnt1] - 3);
                    cnt1++;
                }
            }
            fprintf(file, "E000000\n\n");
            break;
        }

        //section 2
        txtlen1 = 0;
        txtlen2 = 0;
        start = 0;
        second = 0;
        while (strcmp(token_table[i]->label, "WRREC") != 0) {
            i = csect_len[0] + 1;
            fprintf(file, "H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0]+csect_len[1] + 1]+3);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    fputc('D', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);

                        fprintf(file, "%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    fputc('\n', file);
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    fputc('R', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);
                    }
                    fputc('\n', file);
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    fprintf(file, "%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        fprintf(file, "%08X", objectcode[j]);
                    else
                        fprintf(file, "%06X", objectcode[j]);
                }
            }
            second = i;
            fputc('\n', file);
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//두번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen2 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen2 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen2 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 4;
                    }
                    else {
                        if (txtlen2 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            if (strcmp(token_table[i]->operator, "BYTE") == 0) {//X'F1' 처리
                char* str1;
                char str[10];
                strcpy(str, token_table[i]->operand[0]);
                str1 = strtok(str, "\'");
                str1 = strtok(NULL, "\'");
                print[i] = strtol(str1, NULL, 16);
                txtlen2 += 1;
                i++;
            }
            if (strcmp(token_table[i]->operator, "WORD") == 0) {//MAXLEN 처리
                print[i] = 0;
                txtlen2 += 3;
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//두번째T 출력
                if (strcmp(token_table[j]->operator, "BYTE") == 0)
                    fprintf(file, "%02X", print[j]);
                else if (strcmp(token_table[j]->operator, "WORD") == 0)
                    fprintf(file, "%06X", print[j]);
                else if (search_opcode(token_table[j]->operator) >= 0 && inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    fprintf(file, "%04X", print[j]);
                else if (search_opcode(token_table[j]->operator) >= 0 && inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        fprintf(file, "%08X", objectcode[j]);
                    else
                        fprintf(file, "%06X", objectcode[j]);
                }


            }
            fputc('\n', file);
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "BUFFER") == 0) {
                    fprintf(file, "M%06X05+BUFFER\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "LENGTH") == 0) {
                    fprintf(file, "M%06X05+LENGTH\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "BUFEND-BUFFER") == 0) {
                    char* str2;
                    char str3[15];
                    strcpy(str3, token_table[j]->operand[0]);
                    str2 = strtok(str3, "\-");
                    fprintf(file, "M%06X06+%s\n", mod[cnt1] - 3, str3);
                    str2 = strtok(NULL, "\-");
                    fprintf(file, "M%06X06+%s\n", mod[cnt1] - 3, str2);
                    cnt1++;
                }
            }
            fprintf(file, "E\n\n");
            break;
        }
        //section 3
        txtlen1 = 0;
        txtlen2 = 0;
        start = 0;
        second = 0;
        while (strcmp(token_table[i]->operator, "END") != 0) {
            i = csect_len[0] + csect_len[1]+ 2;
            fprintf(file, "H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[i+csect_len[2]+1] + 1);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    fputc('D', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);

                        fprintf(file, "%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    fputc('\n', file);
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    fputc('R', file);
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        fprintf(file, "%s", token_table[i]->operand[j]);
                    }
                    fputc('\n', file);
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i+1]->operator, "END") == 0) 
                    txtlen1 += 1;
                
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    fprintf(file, "%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        fprintf(file, "%08X", objectcode[j]);
                    else
                        fprintf(file, "%06X", objectcode[j]);
                }
                if (strcmp(token_table[j + 1]->operator, "END") == 0) {
                    char* str;
                    char str1[10];
                    strcpy(str1, literal_table[1].literal);
                    str = strtok(str1, "\'");
                    str = strtok(NULL, "\'");
                    fprintf(file, "%s", str);
                }
            }
            fputc('\n', file);
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "BUFFER") == 0) {
                    fprintf(file, "M%06X05+BUFFER\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "LENGTH") == 0) {
                    fprintf(file, "M%06X05+LENGTH\n", mod[cnt1] - 3);
                    cnt1++;
                }
            }
            fprintf(file, "E\n\n");
            break;
        }
    }
    else {//file_name == NULL일때, 이하 내용 출력방식(printf, putchar)외에 같음
        int i = 0;
        int txtlen1 = 0;//txt길이변수
        int txtlen2 = 0;
        int start = 0;//명령어 시작위치
        int second = 0;//두번째 T 시작위치
        while (strcmp(token_table[i]->label, "RDREC") != 0) {//section 1
            printf("H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0] - 1]);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    putchar('D');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);

                        printf("%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    putchar('\n');
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    putchar('R');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);
                    }
                    putchar('\n');
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    printf("%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        printf("%08X", objectcode[j]);
                    else
                        printf("%06X", objectcode[j]);
                }
            }
            second = i;
            putchar('\n');
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//두번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen2 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen2 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen2 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 4;
                    }
                    else {
                        if (txtlen2 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//두번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    printf("%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        printf("%08X", objectcode[j]);
                    else
                        printf("%06X", objectcode[j]);
                }
            }
            putchar('\n');
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "RDREC") == 0) {
                    printf("M%06X05+RDREC\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "WRREC") == 0) {
                    printf("M%06X05+WRREC\n", mod[cnt1] - 3);
                    cnt1++;
                }
            }
            printf("E000000\n\n");
            break;
        }

        //section 2
        txtlen1 = 0;
        txtlen2 = 0;
        start = 0;
        second = 0;
        while (strcmp(token_table[i]->label, "WRREC") != 0) {
            i = csect_len[0] + 1;
            printf("H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0] + csect_len[1] + 1] + 3);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    putchar('D');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);

                        printf("%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    putchar('\n');
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    putchar('R');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);
                    }
                    putchar('\n');
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    printf("%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        printf("%08X", objectcode[j]);
                    else
                        printf("%06X", objectcode[j]);
                }
            }
            second = i;
            putchar('\n');
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//두번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen2 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen2 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen2 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 4;
                    }
                    else {
                        if (txtlen2 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen2 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            if (strcmp(token_table[i]->operator, "BYTE") == 0) {
                char* str1;
                char str[10];
                strcpy(str, token_table[i]->operand[0]);
                str1 = strtok(str, "\'");
                str1 = strtok(NULL, "\'");
                print[i] = strtol(str1, NULL, 16);
                txtlen2 += 1;
                i++;
            }
            if (strcmp(token_table[i]->operator, "WORD") == 0) {
                print[i] = 0;
                txtlen2 += 3;
                i++;
            }
            printf("T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//두번째T 출력
                if (strcmp(token_table[j]->operator, "BYTE") == 0)
                    printf("%02X", print[j]);
                else if (strcmp(token_table[j]->operator, "WORD") == 0)
                    printf("%06X", print[j]);
                else if (search_opcode(token_table[j]->operator) >= 0 && inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    printf("%04X", print[j]);
                else if (search_opcode(token_table[j]->operator) >= 0 && inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        printf("%08X", objectcode[j]);
                    else
                        printf("%06X", objectcode[j]);
                }


            }
            putchar('\n');
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "BUFFER") == 0) {
                    printf("M%06X05+BUFFER\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "LENGTH") == 0) {
                    printf("M%06X05+LENGTH\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "BUFEND-BUFFER") == 0) {
                    char* str2;
                    char str3[15];
                    strcpy(str3, token_table[j]->operand[0]);
                    str2 = strtok(str3, "\-");
                    printf("M%06X06+%s\n", mod[cnt1] - 3, str3);
                    str2 = strtok(NULL, "\-");
                    printf("M%06X06+%s\n", mod[cnt1] - 3, str2);
                    cnt1++;
                }
            }
            printf("E\n\n");
            break;
        }
        //section 3
        txtlen1 = 0;
        txtlen2 = 0;
        start = 0;
        second = 0;
        while (strcmp(token_table[i]->operator, "END") != 0) {
            i = csect_len[0] + csect_len[1] + 2;
            printf("H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[i + csect_len[2] + 1] + 1);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF 처리
                if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                    putchar('D');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);

                        printf("%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
                    }
                    putchar('\n');
                }
                else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                    putchar('R');
                    for (int j = 0; j < MAX_OPERAND; j++) {
                        if (token_table[i]->operand[j] == NULL)
                            break;

                        printf("%s", token_table[i]->operand[j]);
                    }
                    putchar('\n');
                    start = i + 1;
                    break;
                }
            }
            //instruction시작
            int cnt1 = 0;//mod배열 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//첫번째 T length측정
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i + 1]->operator, "END") == 0)
                    txtlen1 += 1;

                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify위치 저장
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//첫번째T 출력
                if (inst_table[search_opcode(token_table[j]->operator)]->format == 2)
                    printf("%04X", print[j]);
                else if (inst_table[search_opcode(token_table[j]->operator)]->format == 3) {
                    if (strncmp(token_table[j]->operator, "+", 1) == 0)
                        printf("%08X", objectcode[j]);
                    else
                        printf("%06X", objectcode[j]);
                }
                if (strcmp(token_table[j + 1]->operator, "END") == 0) {
                    char* str;
                    char str1[10];
                    strcpy(str1, literal_table[1].literal);
                    str = strtok(str1, "\'");
                    str = strtok(NULL, "\'");
                    printf("%s", str);
                }
            }
            putchar('\n');
            cnt1 = 0;
            for (j = start; j < i; j++) {//modify출력
                if (strcmp(token_table[j]->operand[0], "BUFFER") == 0) {
                    printf("M%06X05+BUFFER\n", mod[cnt1] - 3);
                    cnt1++;
                }
                else if (strcmp(token_table[j]->operand[0], "LENGTH") == 0) {
                    printf("M%06X05+LENGTH\n", mod[cnt1] - 3);
                    cnt1++;
                }
            }
            printf("E\n\n");
            break;
        }
    }
}

int search_symbol(char* str)//입력받은 string이 sym_table에 있는지 확인, 있다면 해당 line number 반환
{
    int i;
    for (i = 0; i < sym_line; i++) {
        if (strcmp(str, sym_table[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}