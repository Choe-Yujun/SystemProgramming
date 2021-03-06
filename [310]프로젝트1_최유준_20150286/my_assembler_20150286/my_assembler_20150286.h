/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
    char opname[6];
    int format;
    char opcode[2];
    int odnum;
    /* add your code here */
};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
    char *label;                //명령어 라인 중 label
    char *operator;             //명령어 라인 중 operator
    char *operand[MAX_OPERAND]; //명령어 라인 중 operand
    char *comment;              //명령어 라인 중 comment
    char nixbpe; // 추후 프로젝트에서 사용된다.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

static int tok_adr[MAX_LINES];//각 line마다 주소를 저장하는 변수
static int objectcode[MAX_LINES];//명령어를 저장하는 변수
static int csect_len[3];//각각의 section마다 길이를 저장하는 변수
static int start = 1;//각 section에서 반복문 시작할 인덱스를 저장하는 함수
static int print[MAX_LINES];//출력배열
static int mod[3] = { 0 };//Modify를 위한 배열

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 프로젝트에서 사용된다.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int sym_line;

/*
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
*/
struct literal_unit
{
    char literal[10];
    int addr;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];

static int locctr;//현재위치(tok_adr찾기위해)
static int lit_line;//literal line수

//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* 추후 프로젝트에서 사용하게 되는 함수*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
