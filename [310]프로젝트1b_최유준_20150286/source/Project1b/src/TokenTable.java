import java.util.ArrayList;
import java.util.HashMap;

/**
 * ����ڰ� �ۼ��� ���α׷� �ڵ带 �ܾ�� ���� �� ��, �ǹ̸� �м��ϰ�, ���� �ڵ�� ��ȯ�ϴ� ������ �Ѱ��ϴ� Ŭ�����̴�. <br>
 * pass2���� object code�� ��ȯ�ϴ� ������ ȥ�� �ذ��� �� ���� symbolTable�� instTable�� ������ �ʿ��ϹǷ� �̸� ��ũ��Ų��.<br>
 * section ���� �ν��Ͻ��� �ϳ��� �Ҵ�ȴ�.
 *
 */
public class TokenTable {
	public static final int MAX_OPERAND=3;
	
	/* bit ������ �������� ���� ���� */
	public static final int nFlag=32;
	public static final int iFlag=16;
	public static final int xFlag=8;
	public static final int bFlag=4;
	public static final int pFlag=2;
	public static final int eFlag=1;
	
	/* Token�� �ٷ� �� �ʿ��� ���̺���� ��ũ��Ų��. */
	SymbolTable symTab;
	LiteralTable literalTab;
	InstTable instTab;
	int Listlength;
	
	
	/** �� line�� �ǹ̺��� �����ϰ� �м��ϴ� ����. */
	ArrayList<Token> tokenList;
	
	/**
	 * �ʱ�ȭ�ϸ鼭 symTable�� instTable�� ��ũ��Ų��.
	 * @param symTab : �ش� section�� ����Ǿ��ִ� symbol table
	 * @param instTab : instruction ���� ���ǵ� instTable
	 */
	public TokenTable(SymbolTable symTab, LiteralTable literalTab, InstTable instTab) {
		this.symTab = symTab;
		this.literalTab = literalTab;
		this.instTab = instTab;
		this.tokenList = new ArrayList<Token>();
	}
	
	/**
	 * �Ϲ� ���ڿ��� �޾Ƽ� Token������ �и����� tokenList�� �߰��Ѵ�.
	 * @param line : �и����� ���� �Ϲ� ���ڿ�
	 */
	public void putToken(String line) {
		tokenList.add(new Token(line));
	}
	
	/**
	 * tokenList���� index�� �ش��ϴ� Token�� �����Ѵ�.
	 * @param index
	 * @return : index��ȣ�� �ش��ϴ� �ڵ带 �м��� Token Ŭ����
	 */
	public Token getToken(int index) {
		return tokenList.get(index);
	}
	
	/**
	 * Pass2 �������� ����Ѵ�.
	 * instruction table, symbol table literal table ���� �����Ͽ� objectcode�� �����ϰ�, �̸� �����Ѵ�.
	 * @param index
	 */
	public void makeObjectCode(int index){
		//...
		int objectcode = 0;//�ӽ� int�� objectcode
		int opcode = 0;//�ӽ� int�� opcode
		int address = 0;
		if(instTab.hash_get(tokenList.get(index).operator) > 0) {//instruction�̸�
			if(tokenList.get(index).operator.charAt(0) == '+') {
				String newline;
				newline = tokenList.get(index).operator.substring(1);
				opcode = Integer.parseInt(instTab.instMap.get(newline).opcode, 16);
			}
			else {
				opcode = Integer.parseInt(instTab.instMap.get(tokenList.get(index).operator).opcode, 16);
			}
			if(instTab.hash_get(tokenList.get(index).operator) == 3) {//format 3,4
				tokenList.get(index).setFlag(nFlag, 1);//�⺻������ 3�������� ����, byteSize=3, n=1, i=1, PC relative
				tokenList.get(index).setFlag(iFlag, 1);
				tokenList.get(index).setFlag(pFlag, 1);
				tokenList.get(index).byteSize = 3;
				if(tokenList.get(index).operator.charAt(0) == '+') {//format4 p, e�� 0, byteSize = 4
					tokenList.get(index).setFlag(pFlag, 0);
					tokenList.get(index).setFlag(eFlag, 1);
					tokenList.get(index).byteSize = 4;
				}
				if(tokenList.get(index).operator.equals("RSUB")) {//RSUB�� ����ó��
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
				if(tokenList.get(index).operand[0].charAt(0) == '#' || tokenList.get(index).operand[0].charAt(0) == '@') {//immediate or indirect�� ��� ��ȣ ���� operand ����
					str = tokenList.get(index).operand[0].substring(1);
				}
				else {
					str = tokenList.get(index).operand[0];
				}
				int i;
				int check = -1;
				for(i = 0; i < this.symTab.symbolList.size(); i++) {
					if(str.equals(this.symTab.symbolList.get(i))) {//���� section symbol�̸� check >= 0
						check = i;
						break;
					}
				}
				if(check != -1){//���� section symbol
					address = this.symTab.locationList.get(i) - tokenList.get(index+1).location;
					if(address < 0) {
						address = 4096 + (this.symTab.locationList.get(i) - tokenList.get(index+1).location);
					}
				}
				else {//���� section symbol�� �ƴ�
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
				if(tokenList.get(index).byteSize == 3) {//3������ ��� ��Ʈ����
					objectcode = opcode << 16 | tokenList.get(index).nixbpe << 12 | address;
				}
				else if(tokenList.get(index).byteSize == 4) {//format4 ��Ʈ����
					objectcode = opcode << 24 | tokenList.get(index).nixbpe << 20 | address;
				}
			}
			else {//2����
				tokenList.get(index).byteSize = 2;
				if(tokenList.get(index).operand.length >= 1){
					if(tokenList.get(index).operand[0].equals("X")) {//operand[0]�� ���...->��Ʈ�������� 4��Ʈ �о��ش�.
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
					if (tokenList.get(index).operand[1].equals("X")) {//operand[1]�� ���...->�ٷ� ����
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
			if(tokenList.get(index).objectCode.length() == 5) {//���� ���ڸ��� 0�̾ ������ ��� 0 �߰�
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
	 * index��ȣ�� �ش��ϴ� object code�� �����Ѵ�.
	 * @param index
	 * @return : object code
	 */
	public String getObjectCode(int index) {
		return tokenList.get(index).objectCode;
	}
	
}

/**
 * �� ���κ��� ����� �ڵ带 �ܾ� ������ ������ ��  �ǹ̸� �ؼ��ϴ� ���� ���Ǵ� ������ ������ �����Ѵ�. 
 * �ǹ� �ؼ��� ������ pass2���� object code�� �����Ǿ��� ���� ����Ʈ �ڵ� ���� �����Ѵ�.
 */
class Token{
	//�ǹ� �м� �ܰ迡�� ���Ǵ� ������
	int location;
	String label;
	String operator;
	String[] operand;
	String comment;
	char nixbpe;

	// object code ���� �ܰ迡�� ���Ǵ� ������ 
	String objectCode;
	int byteSize;
	
	/**
	 * Ŭ������ �ʱ�ȭ �ϸ鼭 �ٷ� line�� �ǹ� �м��� �����Ѵ�. 
	 * @param line ��������� ����� ���α׷� �ڵ�
	 */
	public Token(String line) {
		//initialize �߰�
		this.nixbpe = 0;
		this.location = 0;
		parsing(line);
	}
	
	/**
	 * line�� �������� �м��� �����ϴ� �Լ�. Token�� �� ������ �м��� ����� �����Ѵ�.
	 * @param line ��������� ����� ���α׷� �ڵ�.
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
	 * n,i,x,b,p,e flag�� �����Ѵ�. 
	 * 
	 * ��� �� : setFlag(nFlag, 1); 
	 *   �Ǵ�     setFlag(TokenTable.nFlag, 1);
	 * 
	 * @param flag : ���ϴ� ��Ʈ ��ġ
	 * @param value : ����ְ��� �ϴ� ��. 1�Ǵ� 0���� �����Ѵ�.
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
	 * ���ϴ� flag���� ���� ���� �� �ִ�. flag�� ������ ���� ���ÿ� �������� �÷��׸� ��� �� ���� �����ϴ� 
	 * 
	 * ��� �� : getFlag(nFlag)
	 *   �Ǵ�     getFlag(nFlag|iFlag)
	 * 
	 * @param flags : ���� Ȯ���ϰ��� �ϴ� ��Ʈ ��ġ
	 * @return : ��Ʈ��ġ�� �� �ִ� ��. �÷��׺��� ���� 32, 16, 8, 4, 2, 1�� ���� ������ ����.
	 */
	public int getFlag(int flags) {
		return nixbpe & flags;
	}
}
