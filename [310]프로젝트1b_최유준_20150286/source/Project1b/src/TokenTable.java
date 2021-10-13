import java.util.ArrayList;
import java.util.HashMap;

/**
 * 사용자가 작성한 프로그램 코드를 단어별로 분할 한 후, 의미를 분석하고, 최종 코드로 변환하는 과정을 총괄하는 클래스이다. <br>
 * pass2에서 object code로 변환하는 과정은 혼자 해결할 수 없고 symbolTable과 instTable의 정보가 필요하므로 이를 링크시킨다.<br>
 * section 마다 인스턴스가 하나씩 할당된다.
 *
 */
public class TokenTable {
	public static final int MAX_OPERAND=3;
	
	/* bit 조작의 가독성을 위한 선언 */
	public static final int nFlag=32;
	public static final int iFlag=16;
	public static final int xFlag=8;
	public static final int bFlag=4;
	public static final int pFlag=2;
	public static final int eFlag=1;
	
	/* Token을 다룰 때 필요한 테이블들을 링크시킨다. */
	SymbolTable symTab;
	LiteralTable literalTab;
	InstTable instTab;
	int Listlength;
	
	
	/** 각 line을 의미별로 분할하고 분석하는 공간. */
	ArrayList<Token> tokenList;
	
	/**
	 * 초기화하면서 symTable과 instTable을 링크시킨다.
	 * @param symTab : 해당 section과 연결되어있는 symbol table
	 * @param instTab : instruction 명세가 정의된 instTable
	 */
	public TokenTable(SymbolTable symTab, LiteralTable literalTab, InstTable instTab) {
		this.symTab = symTab;
		this.literalTab = literalTab;
		this.instTab = instTab;
		this.tokenList = new ArrayList<Token>();
	}
	
	/**
	 * 일반 문자열을 받아서 Token단위로 분리시켜 tokenList에 추가한다.
	 * @param line : 분리되지 않은 일반 문자열
	 */
	public void putToken(String line) {
		tokenList.add(new Token(line));
	}
	
	/**
	 * tokenList에서 index에 해당하는 Token을 리턴한다.
	 * @param index
	 * @return : index번호에 해당하는 코드를 분석한 Token 클래스
	 */
	public Token getToken(int index) {
		return tokenList.get(index);
	}
	
	/**
	 * Pass2 과정에서 사용한다.
	 * instruction table, symbol table literal table 등을 참조하여 objectcode를 생성하고, 이를 저장한다.
	 * @param index
	 */
	public void makeObjectCode(int index){
		//...
		int objectcode = 0;//임시 int형 objectcode
		int opcode = 0;//임시 int형 opcode
		int address = 0;
		if(instTab.hash_get(tokenList.get(index).operator) > 0) {//instruction이면
			if(tokenList.get(index).operator.charAt(0) == '+') {
				String newline;
				newline = tokenList.get(index).operator.substring(1);
				opcode = Integer.parseInt(instTab.instMap.get(newline).opcode, 16);
			}
			else {
				opcode = Integer.parseInt(instTab.instMap.get(tokenList.get(index).operator).opcode, 16);
			}
			if(instTab.hash_get(tokenList.get(index).operator) == 3) {//format 3,4
				tokenList.get(index).setFlag(nFlag, 1);//기본적으로 3형식으로 간주, byteSize=3, n=1, i=1, PC relative
				tokenList.get(index).setFlag(iFlag, 1);
				tokenList.get(index).setFlag(pFlag, 1);
				tokenList.get(index).byteSize = 3;
				if(tokenList.get(index).operator.charAt(0) == '+') {//format4 p, e는 0, byteSize = 4
					tokenList.get(index).setFlag(pFlag, 0);
					tokenList.get(index).setFlag(eFlag, 1);
					tokenList.get(index).byteSize = 4;
				}
				if(tokenList.get(index).operator.equals("RSUB")) {//RSUB은 예외처리
					tokenList.get(index).setFlag(pFlag, 0);
					objectcode = opcode << 16 | tokenList.get(index).nixbpe << 12;
					tokenList.get(index).objectCode = Integer.toString(objectcode, 16).toUpperCase();
					return;
				}
				if(tokenList.get(index).operand[0].charAt(0) == '#') {//immediate
					tokenList.get(index).setFlag(pFlag, 0);
					tokenList.get(index).setFlag(nFlag, 0);
				}
				else if(tokenList.get(index).operand[0].charAt(0) == '@') {//indirect
					tokenList.get(index).setFlag(iFlag, 0);
				}
				if(tokenList.get(index).operand.length == 2 && tokenList.get(index).operand[1].equals("X")) {//index
					tokenList.get(index).setFlag(xFlag, 1);
				}
				//find address
				String str;
				if(tokenList.get(index).operand[0].charAt(0) == '#' || tokenList.get(index).operand[0].charAt(0) == '@') {//immediate or indirect의 경우 기호 빼고 operand 저장
					str = tokenList.get(index).operand[0].substring(1);
				}
				else {
					str = tokenList.get(index).operand[0];
				}
				int i;
				int check = -1;
				for(i = 0; i < this.symTab.symbolList.size(); i++) {
					if(str.equals(this.symTab.symbolList.get(i))) {//같은 section symbol이면 check >= 0
						check = i;
						break;
					}
				}
				if(check != -1){//같은 section symbol
					address = this.symTab.locationList.get(i) - tokenList.get(index+1).location;
					if(address < 0) {
						address = 4096 + (this.symTab.locationList.get(i) - tokenList.get(index+1).location);
					}
				}
				else {//같은 section symbol이 아님
					if(str.charAt(0) >= '0' && str.charAt(0) <= '9') {//immediate #number
						address = Integer.parseInt(str);
					}
					else {
						if(tokenList.get(index).operand[0].charAt(0) == '=') {//literal
							address = this.literalTab.locationList.get(0) - tokenList.get(index+1).location;
						}
						else {//another section symbol
							address = 0;
						}
					}
				}
				if(tokenList.get(index).byteSize == 3) {//3형식의 경우 비트연산
					objectcode = opcode << 16 | tokenList.get(index).nixbpe << 12 | address;
				}
				else if(tokenList.get(index).byteSize == 4) {//format4 비트연산
					objectcode = opcode << 24 | tokenList.get(index).nixbpe << 20 | address;
				}
			}
			else {//2형식
				tokenList.get(index).byteSize = 2;
				if(tokenList.get(index).operand.length >= 1){
					if(tokenList.get(index).operand[0].equals("X")) {//operand[0]일 경우...->비트연산으로 4비트 밀어준다.
						address = 1;
					}
					else if(tokenList.get(index).operand[0].equals("A")) {
						address = 0;
					}
					else if(tokenList.get(index).operand[0].equals("S")) {
						address = 4;
					}
					else if(tokenList.get(index).operand[0].equals("T")) {
						address = 5;
					}
					objectcode = opcode << 8 | address << 4;
				}
				if(tokenList.get(index).operand.length == 2) {
					if (tokenList.get(index).operand[1].equals("X")) {//operand[1]의 경우...->바로 저장
						address = 1;
					}
					else if(tokenList.get(index).operand[1].equals("A")) {
						address = 0;
					}
					else if(tokenList.get(index).operand[1].equals("S")) {
						address = 4;
					}
					else if(tokenList.get(index).operand[1].equals("T")) {
						address = 5;
					}

					objectcode = objectcode | address;
				}
			}
			tokenList.get(index).objectCode = Integer.toString(objectcode, 16).toUpperCase();
			if(tokenList.get(index).objectCode.length() == 5) {//제일 앞자리가 0이어서 생략된 경우 0 추가
				tokenList.get(index).objectCode = "0"+tokenList.get(index).objectCode;
			}
		}
		else {//not Instruction
			if(tokenList.get(index).operator.equals("LTORG")) {
				tokenList.get(index).objectCode = this.literalTab.literalList.get(0);
				tokenList.get(index).byteSize = 3;
			}
			else if(tokenList.get(index).operator.equals("BYTE")){
				String[] arr = tokenList.get(index).operand[0].split("'");
				tokenList.get(index).objectCode = arr[1];
				tokenList.get(index).byteSize = 1;
			}
			else if(tokenList.get(index).operator.equals("WORD")) {
				tokenList.get(index).objectCode = "000000";
				tokenList.get(index).byteSize = 3;
			}
			else if(tokenList.get(index).operator.equals("END")) {
				tokenList.get(index).objectCode = this.literalTab.literalList.get(0);
				tokenList.get(index).byteSize = 1;
			}
		}
	}
	
	/** 
	 * index번호에 해당하는 object code를 리턴한다.
	 * @param index
	 * @return : object code
	 */
	public String getObjectCode(int index) {
		return tokenList.get(index).objectCode;
	}
	
}

/**
 * 각 라인별로 저장된 코드를 단어 단위로 분할한 후  의미를 해석하는 데에 사용되는 변수와 연산을 정의한다. 
 * 의미 해석이 끝나면 pass2에서 object code로 변형되었을 때의 바이트 코드 역시 저장한다.
 */
class Token{
	//의미 분석 단계에서 사용되는 변수들
	int location;
	String label;
	String operator;
	String[] operand;
	String comment;
	char nixbpe;

	// object code 생성 단계에서 사용되는 변수들 
	String objectCode;
	int byteSize;
	
	/**
	 * 클래스를 초기화 하면서 바로 line의 의미 분석을 수행한다. 
	 * @param line 문장단위로 저장된 프로그램 코드
	 */
	public Token(String line) {
		//initialize 추가
		this.nixbpe = 0;
		this.location = 0;
		parsing(line);
	}
	
	/**
	 * line의 실질적인 분석을 수행하는 함수. Token의 각 변수에 분석한 결과를 저장한다.
	 * @param line 문장단위로 저장된 프로그램 코드.
	 */
	public void parsing(String line) {
		String[] arr = line.split("\t", 4);
		this.label = arr[0];
		this.operator = arr[1];
		if(arr.length > 3)
			this.comment = arr[3];
		if(arr.length > 2) {
			String op[] = arr[2].split(",", 3);
			this.operand = new String[op.length];
			for(int i = 0; i < op.length; i++) {
				this.operand[i] = op[i];
			}
		}
		
	}
	
	/** 
	 * n,i,x,b,p,e flag를 설정한다. 
	 * 
	 * 사용 예 : setFlag(nFlag, 1); 
	 *   또는     setFlag(TokenTable.nFlag, 1);
	 * 
	 * @param flag : 원하는 비트 위치
	 * @param value : 집어넣고자 하는 값. 1또는 0으로 선언한다.
	 */
	public void setFlag(int flag, int value) {
		if(value == 1) {
			this.nixbpe += (char)flag;
		}
		else if(value == 0) {
			this.nixbpe -= (char)flag;
		}
	}
	
	/**
	 * 원하는 flag들의 값을 얻어올 수 있다. flag의 조합을 통해 동시에 여러개의 플래그를 얻는 것 역시 가능하다 
	 * 
	 * 사용 예 : getFlag(nFlag)
	 *   또는     getFlag(nFlag|iFlag)
	 * 
	 * @param flags : 값을 확인하고자 하는 비트 위치
	 * @return : 비트위치에 들어가 있는 값. 플래그별로 각각 32, 16, 8, 4, 2, 1의 값을 리턴할 것임.
	 */
	public int getFlag(int flags) {
		return nixbpe & flags;
	}
}
