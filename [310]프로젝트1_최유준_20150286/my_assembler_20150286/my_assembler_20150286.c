/*
 * ȭ�ϸ� : my_assembler_00000000.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20150286.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	//make_opcode_output("output_20150286.txt");

	
	
	
	make_symtab_output("symtab_20150286");
    make_literaltab_output("literaltab_20150286");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_20150286") ; 
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
             inst_table[i] = (inst*)malloc(sizeof(inst));//�����Ҵ�
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
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
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	/* add your code here */
    char check;
    int first;

    //label�� ������ first = 0, ������ first = 1
    sscanf(str, "%c", &check);

    if ((check == '\t') || (check == ' '))
        first = 0;
    else
        first = 1;
      
    token_table[token_line] = (token*)malloc(sizeof(token));//�����Ҵ�

    if (check == '.') {
        return 0;
    }
    token_table[token_line]->operand[0] = "";
    token_table[token_line]->operand[1] = "";
    token_table[token_line]->operand[2] = ""; //operand �ʱ�ȭ

    if (first == 0) { //label�� ���� ��
        
        token_table[token_line]->label = "";
        token_table[token_line]->operator = strtok(str, "\t");
        token_table[token_line]->operand[0] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");
        
        
        
    }
    else if (first == 1) { //label�� ���� ��
        token_table[token_line]->label = strtok(str, "\t");
        token_table[token_line]->operator = strtok(NULL, "\t");
        token_table[token_line]->operand[0] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");

    }
    //operand ����
    token_table[token_line]->operand[0] = strtok(token_table[token_line]->operand[0], ",");
    token_table[token_line]->operand[1] = strtok(NULL, ",");
    token_table[token_line]->operand[2] = strtok(NULL, ",");

    
    token_line++;
    return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
    int e = 0;//4��������
   

    if (str == NULL)
        return 0;
    
    if (str[0] == '+') {
        e = 1;//4����
        str = str + 1;
    }

    for (int i = 0; i < inst_index; i++) {//inst_table���� ��ɾ� �˻�
        if (strcmp(inst_table[i]->opname, str) == 0) {
            return i;
        }
    }
    return -1;

	/* add your code here */

}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
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
    while (strcmp(token_table[i]->label, "RDREC") != 0) {//ù��° CSECT
        if (i == 0)//ù��° �ּҴ� 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//������ locctr ����

        if(token_table[i]->operand[0] == NULL) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //���ͷ��̸�
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//���ͷ����̺� ����
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label�� ������ symbol�� ����
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator�� ���
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1����
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2����
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3����
                    locctr += 3;
                }
                else//4����
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
    while (strcmp(token_table[i]->label, "WRREC") != 0) {//�� ��° CSECT
        if (k == 0)//ù��° �ּҴ� 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//������ locctr ����

        if (token_table[i]->operand[0] == NULL) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //���ͷ��̸�
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//���ͷ����̺� ����
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label�� ������ symbol�� ����
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator�� ���
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1����
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2����
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3����
                    locctr += 3;
                }
                else//4����
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
    while (i >= 0) {//�� ��° CSECT
        if (k == 0)//ù��° �ּҴ� 0
            tok_adr[i] = 0;
        tok_adr[i] = locctr;//������ locctr ����

        if ((token_table[i]->operand[0] == NULL) || strcmp(token_table[i]->operand[0], "FIRST") == 0) {
            if ((strcmp(token_table[i]->operator, "LTORG")) == 0 || (strcmp(token_table[i]->operator, "END")) == 0) {
                literal_table[lit_line].addr = locctr;
                locctr += 3;
                lit_line++;
            }
        }
        else if (strncmp(token_table[i]->operand[0], "=", 1) == 0) { //���ͷ��̸�
            strcpy(literal_table[lit_line].literal, token_table[i]->operand[0]);//���ͷ����̺� ����
        }
        sym_table[sym_line].symbol[0] = "";
        if (token_table[i]->label != "") {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);//label�� ������ symbol�� ����
        }
        else
            strcpy(sym_table[sym_line].symbol, "t");
        if (search_opcode(token_table[i]->operator) >= 0) {//operator�� ���
            if (inst_table[search_opcode(token_table[i]->operator)]->format == 1)//1����
                locctr += 1;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 2)//2����
                locctr += 2;
            else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {//3or4
                if (token_table[i]->operator[0] != '+') {//3����
                    locctr += 3;
                }
                else//4����
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
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ� 
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */
    return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
                    if (token_table[i]->operand[0] == NULL) {//operand�� ����
                        fprintf(file, "\t%s\n", token_table[i]->operator);
                    }
                    else if(token_table[i]->operand[1] == NULL)//operand 1��
                        fprintf(file, "\t%s\t%s\n", token_table[i]->operator, token_table[i]->operand[0]);
                    
                    else if(token_table[i]->operand[2] == NULL)//operand 2��
                        fprintf(file, "\t%s\t%s,%s\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1]);
                    
                    else if((token_table[i]->operand[0] != NULL) &&(token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] != NULL))//operand 3��
                        fprintf(file, "\t%s\t%s,%s,%s\n", token_table[i]->operator, token_table[i]->operand[0], token_table[i]->operand[1], token_table[i]->operand[2]);
                }
     
                //case1-2: not instruction, label
                else {
                    if (token_table[i]->operand[0] == NULL) {//operand 0��
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
                    else if ((token_table[i]->operand[0] != NULL) && (token_table[i]->operand[1] == NULL))//op 1��
                        fprintf(file, "\t%s\t%s\t%c%c\n", token_table[i]->operator, token_table[i]->operand[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[0], inst_table[search_opcode(token_table[i]->operator)]->opcode[1]);
                    
                    else if ((token_table[i]->operand[1] != NULL) && (token_table[i]->operand[2] == NULL))//op 2��
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
    int opcode;
    int address;
    int i = 0;
    while (strcmp(token_table[i]->label, "RDREC") != 0) {//ù��° section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int�� opcode�� �ڵ� ����
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

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB�� ���� ����ó��
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //addr ���ϱ�
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
                    if (strcmp(str, sym_table[a].symbol) == 0)//���� CSECT symbol�̸� check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol�� ������
                    address = sym_table[check].addr - tok_adr[i+1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol�� ������
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//�ٸ� CSECT symbol�̸� check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//�ٸ� CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//���ͷ��̸�
                                    address = literal_table[a].addr - tok_adr[i + 1];
                                    break;
                                }
                                else
                                    return -1;//����
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
            else {//2����
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
    while (strcmp(token_table[i]->label, "WRREC") != 0) {//2��° section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int�� opcode�� �ڵ� ����
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

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB�� ���� ����ó��
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //addr ���ϱ�
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
                    if (strcmp(str, sym_table[a].symbol) == 0)//���� CSECT symbol�̸� check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol�� ������
                    address = sym_table[check].addr - tok_adr[i + 1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol�� ������
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//�ٸ� CSECT symbol�̸� check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//�ٸ� CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//���ͷ��̸�
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
            else {//2����
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
    while (i >= 0) {//3��° section
        objectcode[i] = 0;
        if (search_opcode(token_table[i]->operator) >= 0) {//instruction
            token_table[i]->nixbpe = 0;
            sscanf(inst_table[(search_opcode(token_table[i]->operator))]->opcode, "%X", &opcode);//int�� opcode�� �ڵ� ����
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

                if (strcmp(token_table[i]->operator, "RSUB") == 0) {//RUSB�� ���� ����ó��
                    token_table[i]->nixbpe = token_table[i]->nixbpe & (~(1 << 1));
                    objectcode[i] = opcode << 16 | token_table[i]->nixbpe << 12;
                    i++;
                    continue;
                }

                //address ���ϱ�
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
                    if (strcmp(str, sym_table[a].symbol) == 0)//���� CSECT symbol�̸� check == a;
                        check = a;
                    a++;
                }
                if (check != -1) {//symbol�� ������
                    address = sym_table[check].addr - tok_adr[i + 1];
                    if (address < 0)
                        address = 4096 - tok_adr[i + 1] + sym_table[check].addr;
                }
                else {//symbol�� ������
                    if (str[0] >= '0' && str[0] <= '9') {//immediate #number 
                        sscanf(str, "%d", &address);
                    }
                    else {
                        for (a = 0; a < sym_line; a++) {
                            if (strcmp(str, sym_table[a].symbol) == 0) {//�ٸ� CSECT symbol�̸� check == a;
                                check = a;
                                break;
                            }
                        }
                        if (check != -1) {//�ٸ� CSECT symbol
                            address = 0;
                        }
                        else {
                            for (a = 0; a < lit_line; a++) {
                                if (strcmp(str, literal_table[a].literal) == 0) {//���ͷ��̸�
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
            else {//2����
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
        int txtlen1 = 0;//txt���̺���
        int txtlen2 = 0;
        int start = 0;//��ɾ� ������ġ
        int second = 0;//�ι�° T ������ġ
        while (strcmp(token_table[i]->label, "RDREC") != 0) {//section 1
            fprintf(file, "H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0] - 1]);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����(BYTE����)
                if (inst_table[search_opcode(token_table[i]->operator)]->format == 2) {//2���� +2
                    if (txtlen1 + 2 > 30)
                        break;
                    print[i] = objectcode[i];
                    txtlen1 += 2;
                }
                else if (inst_table[search_opcode(token_table[i]->operator)]->format == 3) {
                    if (strncmp(token_table[i]->operator, "+", 1) == 0) {//4���� +4
                        if (txtlen1 + 4 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 4;
                    }
                    else {//3���� +3
                        if (txtlen1 + 3 > 30)
                            break;
                        print[i] = objectcode[i];
                        txtlen1 += 3;
                    }
                }
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//�ι�° T length����
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
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//�ι�°T ���
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
            for (j = start; j < i; j++) {//modify���
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
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����
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
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//�ι�° T length����
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
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            if (strcmp(token_table[i]->operator, "BYTE") == 0) {//X'F1' ó��
                char* str1;
                char str[10];
                strcpy(str, token_table[i]->operand[0]);
                str1 = strtok(str, "\'");
                str1 = strtok(NULL, "\'");
                print[i] = strtol(str1, NULL, 16);
                txtlen2 += 1;
                i++;
            }
            if (strcmp(token_table[i]->operator, "WORD") == 0) {//MAXLEN ó��
                print[i] = 0;
                txtlen2 += 3;
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//�ι�°T ���
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
            for (j = start; j < i; j++) {//modify���
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
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����
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
                
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            fprintf(file, "T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            for (j = start; j < i; j++) {//modify���
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
    else {//file_name == NULL�϶�, ���� ���� ��¹��(printf, putchar)�ܿ� ����
        int i = 0;
        int txtlen1 = 0;//txt���̺���
        int txtlen2 = 0;
        int start = 0;//��ɾ� ������ġ
        int second = 0;//�ι�° T ������ġ
        while (strcmp(token_table[i]->label, "RDREC") != 0) {//section 1
            printf("H%s\t%6s%06X\n", token_table[i]->label, "000000", tok_adr[csect_len[0] - 1]);
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����
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
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//�ι�° T length����
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
                if (strcmp(token_table[i]->operand[0], "RDREC") == 0 || strcmp(token_table[i]->operand[0], "WRREC") == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[j], txtlen2);
            for (j = second; j < i; j++) {//�ι�°T ���
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
            for (j = start; j < i; j++) {//modify���
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
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����
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
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1 + txtlen2;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            while ((txtlen2 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//�ι�° T length����
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
                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
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
            for (j = second; j < i; j++) {//�ι�°T ���
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
            for (j = start; j < i; j++) {//modify���
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
            for (i = i + 1; i < i + 3; i++) { //EXTDEF, EXTREF ó��
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
            //instruction����
            int cnt1 = 0;//mod�迭 count
            int j;
            i = start;
            while ((txtlen1 <= 30) && search_opcode(token_table[i]->operator) >= 0) {//ù��° T length����
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

                if (strcmp(token_table[i]->operand[0], "BUFFER") == 0 || strcmp(token_table[i]->operand[0], "LENGTH") == 0 || strncmp(token_table[i]->operand[0], "BUFEND", 6) == 0) {//modify��ġ ����
                    mod[cnt1] = txtlen1;
                    cnt1++;
                }
                i++;
            }
            printf("T%06X%02X", tok_adr[start], txtlen1);
            for (j = start; j < i; j++) {//ù��°T ���
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
            for (j = start; j < i; j++) {//modify���
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

int search_symbol(char* str)//�Է¹��� string�� sym_table�� �ִ��� Ȯ��, �ִٸ� �ش� line number ��ȯ
{
    int i;
    for (i = 0; i < sym_line; i++) {
        if (strcmp(str, sym_table[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}