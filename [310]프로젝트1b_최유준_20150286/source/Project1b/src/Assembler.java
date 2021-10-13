import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.io.FileWriter;

/**
 * Assembler : 
 * �� ���α׷��� SIC/XE �ӽ��� ���� Assembler ���α׷��� ���� ��ƾ�̴�.
 * ���α׷��� ���� �۾��� ������ ����. 
 * 1) ó�� �����ϸ� Instruction ���� �о�鿩�� assembler�� �����Ѵ�. 
 * 2) ����ڰ� �ۼ��� input ������ �о���� �� �����Ѵ�. 
 * 3) input ������ ������� �ܾ�� �����ϰ� �ǹ̸� �ľ��ؼ� �����Ѵ�. (pass1) 
 * 4) �м��� ������ �������� ��ǻ�Ͱ� ����� �� �ִ� object code�� �����Ѵ�. (pass2) 
 * 
 * 
 * �ۼ����� ���ǻ��� : 
 *  1) ���ο� Ŭ����, ���ο� ����, ���ο� �Լ� ������ �󸶵��� ����. ��, ������ ������ �Լ����� �����ϰų� ������ ��ü�ϴ� ���� �ȵȴ�.
 *  2) ���������� �ۼ��� �ڵ带 �������� ������ �ʿ信 ���� ����ó��, �������̽� �Ǵ� ��� ��� ���� ����.
 *  3) ��� void Ÿ���� ���ϰ��� ������ �ʿ信 ���� �ٸ� ���� Ÿ������ ���� ����.
 *  4) ����, �Ǵ� �ܼ�â�� �ѱ��� ��½�Ű�� �� ��. (ä������ ����. �ּ��� ���Ե� �ѱ��� ��� ����)
 * 
 *     
 *  + �����ϴ� ���α׷� ������ ��������� �����ϰ� ���� �е��� ������ ��� �޺κп� ÷�� �ٶ��ϴ�. ���뿡 ���� �������� ���� �� �ֽ��ϴ�.
 */
public class Assembler {
	/** instruction ���� ������ ���� */
	InstTable instTable;
	/** �о���� input ������ ������ �� �� �� �����ϴ� ����. */
	ArrayList<String> lineList;
	/** ���α׷��� section���� symbol table�� �����ϴ� ����*/
	ArrayList<SymbolTable> symtabList;
	/** ���α׷��� section���� literal table�� �����ϴ� ����*/
	ArrayList<LiteralTable> literaltabList;
	/** ���α׷��� section���� ���α׷��� �����ϴ� ����*/
	ArrayList<TokenTable> TokenList;
	/** 
	 * Token, �Ǵ� ���þ ���� ������� ������Ʈ �ڵ���� ��� ���·� �����ϴ� ����.   
	 * �ʿ��� ��� String ��� ������ Ŭ������ �����Ͽ� ArrayList�� ��ü�ص� ������.
	 */
	ArrayList<String> codeList;
	
	/**
	 * Ŭ���� �ʱ�ȭ. instruction Table�� �ʱ�ȭ�� ���ÿ� �����Ѵ�.
	 * 
	 * @param instFile : instruction ���� �ۼ��� ���� �̸�. 
	 */
	public Assembler(String instFile) {
		instTable = new InstTable(instFile);
		lineList = new ArrayList<String>();
		symtabList = new ArrayList<SymbolTable>();
		literaltabList = new ArrayList<LiteralTable>();
		TokenList = new ArrayList<TokenTable>();
		codeList = new ArrayList<String>();
	}

	/** 
	 * ������� ���� ��ƾ
	 */
	public static void main(String[] args) {
		Assembler assembler = new Assembler("inst.data");
		assembler.loadInputFile("input.txt");
		assembler.pass1();

		assembler.printSymbolTable("symtab_20150286.txt");
		assembler.printLiteralTable("literaltab_20150286.txt");
		assembler.pass2();
		assembler.printObjectCode("output_20150286.txt");
		
	}

	/**
	 * inputFile�� �о�鿩�� lineList�� �����Ѵ�.
	 * @param inputFile : input ���� �̸�.
	 */
	private void loadInputFile(String inputFile) {
		try {
			File filename = new File(inputFile);
			FileReader fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while ((line = br.readLine()) != null) {//���پ� �б�
				lineList.add(line);
			}
			br.close();
		} catch (FileNotFoundException e) {
			System.out.println(e);
		} catch( IOException e) {
			System.out.println(e);
		}
	}
		

	/** 
	 * pass1 ������ �����Ѵ�.
	 *   1) ���α׷� �ҽ��� ��ĵ�Ͽ� ��ū������ �и��� �� ��ū���̺� ����
	 *   2) label�� symbolTable�� ����
	 *   
	 *    ���ǻ��� : SymbolTable�� TokenTable�� ���α׷��� section���� �ϳ��� ����Ǿ�� �Ѵ�.
	 */
	private void pass1() {
		// TODO Auto-generated method stub
		int i = 0;
		int j;
		int start = 0;
		int addr = 0;
		String lit = null;
		TokenList.add(new TokenTable(new SymbolTable(), new LiteralTable(), instTable));//section 1
		while(i < lineList.size()) {//token parsing
			TokenList.get(0).putToken(lineList.get(i));
			TokenList.get(0).tokenList.get(i).location = addr;
			if(i > 1) {
				if(TokenList.get(0).tokenList.get(i).location - TokenList.get(0).tokenList.get(i-1).location >= 0) {//TokenTable�� length ���ϱ�
					TokenList.get(0).Listlength = TokenList.get(0).tokenList.get(i).location;
				}
			}
			//location find
			if(instTable.hash_get(TokenList.get(0).tokenList.get(i).operator) > 0) {//Instruction
				if(instTable.hash_get(TokenList.get(0).tokenList.get(i).operator) == 3) {
					if(TokenList.get(0).tokenList.get(i).operator.charAt(0) == '+') {//format 4
						addr += 4;
					}
					else {//format 3
						addr += 3;
					}
				}
				else if(instTable.hash_get(TokenList.get(0).tokenList.get(i).operator) == 2) {
					TokenList.get(0).tokenList.get(i).location += 2;//format2
				}
			}
			else {//not Instruction
				if(TokenList.get(0).tokenList.get(i).operator.equals("RESW")) {
					int tmpadr = Integer.parseInt(TokenList.get(0).tokenList.get(i).operand[0]);
					addr += (3*tmpadr);
				}
				else if(TokenList.get(0).tokenList.get(i).operator.equals("RESB")) {
					int tmpadr = Integer.parseInt(TokenList.get(0).tokenList.get(i).operand[0]);
					addr += tmpadr;
				}
				else if(TokenList.get(0).tokenList.get(i).operator.equals("BYTE")) {
					addr += 1;
				}
				else if(TokenList.get(0).tokenList.get(i).operator.equals("WORD")) {
					addr += 3;
				}
				else if(TokenList.get(0).tokenList.get(i).operator.equals("EQU")) {//equation�� ���
					if(!TokenList.get(0).tokenList.get(i).operand[0].equals("*")) {
						String[] arr = TokenList.get(0).tokenList.get(i).operand[0].split("-");//ex) BUFEND-BUFFER
						int tmpadr1=0, tmpadr2=0;
						for(int k = 0; k < TokenList.get(0).tokenList.size(); k++) {
							if(TokenList.get(0).tokenList.get(k).label.equals(arr[0])) {
								tmpadr1 = TokenList.get(0).tokenList.get(k).location;
							}
							if(TokenList.get(0).tokenList.get(k).label.equals(arr[1])) {
								tmpadr2 = TokenList.get(0).tokenList.get(k).location;
							}
						}
						TokenList.get(0).tokenList.get(i).location = tmpadr1-tmpadr2;
					}
	
				}
				else if(TokenList.get(0).tokenList.get(i).operator.equals("LTORG")) {
					addr += 3;
				}
			}

			if(TokenList.get(0).getToken(i).operator.equals("CSECT")){//���� section�� ���� Table�� ���ԵǾ��� ��� remove���ش�.
				TokenList.get(0).tokenList.remove(i);
				break;
			}
			i++;
		}
		for(j = 0; j < i; j++) { //literal, symbol input
			if(!TokenList.get(0).getToken(j).label.equals("")) {//label�� ������ symboltable�� ����
				TokenList.get(0).symTab.putSymbol(TokenList.get(0).getToken(j).label, TokenList.get(0).getToken(j).location);
			}
			if(TokenList.get(0).getToken(j).operand == null) {//lit�� ����� �� literaltable�� ����
				if((TokenList.get(0).getToken(j).operator.equals("LTORG")) || (TokenList.get(0).getToken(j).operator.equals("END"))) {
					TokenList.get(0).literalTab.putLiteral(lit, TokenList.get(0).getToken(j).location);
				}
			}
			else {
				if (TokenList.get(0).getToken(j).operand[0].charAt(0) == '=' ) {//operand�� literal�� ��� lit�� ����
					String[] arr1 = TokenList.get(0).getToken(j).operand[0].split("'");
					lit = arr1[1];
				}
			}
		}
		symtabList.add(TokenList.get(0).symTab);//symtabList�� ���� section ����
		literaltabList.add(TokenList.get(0).literalTab);
		
		j = i;
		start = i;
		addr = 0;
		i = 0;
		TokenList.add(new TokenTable(new SymbolTable(), new LiteralTable(), instTable));//section 2 ���� �ݺ�
		while(j < lineList.size()) {
			if(lineList.get(j).charAt(0) == '.') {
				j++;
				continue;
			}
			TokenList.get(1).putToken(lineList.get(j));
			

			TokenList.get(1).tokenList.get(i).location = addr;
			if(i > 1) {
				if(TokenList.get(1).tokenList.get(i).location - TokenList.get(1).tokenList.get(i-1).location >= 0) {
					TokenList.get(1).Listlength = TokenList.get(1).tokenList.get(i).location;
				}
			}
			if(instTable.hash_get(TokenList.get(1).tokenList.get(i).operator) > 0) {
				if(instTable.hash_get(TokenList.get(1).tokenList.get(i).operator) == 3) {
					if(TokenList.get(1).tokenList.get(i).operator.charAt(0) == '+') {
						addr += 4;
					}
					else {
						addr += 3;
					}
				}
				else if(instTable.hash_get(TokenList.get(1).tokenList.get(i).operator) == 2) {
					addr += 2;
				}
			}
			else {
				if(TokenList.get(1).tokenList.get(i).operator.equals("RESW")) {
					int tmpadr = Integer.parseInt(TokenList.get(1).tokenList.get(i).operand[0]);
					addr += (3*tmpadr);
				}
				else if(TokenList.get(1).tokenList.get(i).operator.equals("RESB")) {
					int tmpadr = Integer.parseInt(TokenList.get(1).tokenList.get(i).operand[0]);
					addr += tmpadr;
				}
				else if(TokenList.get(1).tokenList.get(i).operator.equals("BYTE")) {
					addr += 1;
				}
				else if(TokenList.get(1).tokenList.get(i).operator.equals("WORD")) {
					addr += 3;
				}
				else if(TokenList.get(1).tokenList.get(i).operator.equals("EQU")) {
					if(!TokenList.get(1).tokenList.get(i).operand[0].equals("*")) {
						String[] arr = TokenList.get(1).tokenList.get(i).operand[0].split("-");
						int tmpadr1=0, tmpadr2=0;
						for(int k = 0; k < TokenList.get(1).tokenList.size(); k++) {
							if(TokenList.get(1).tokenList.get(k).label.equals(arr[0])) {
								tmpadr1 = TokenList.get(1).tokenList.get(k).location;
							}
							if(TokenList.get(1).tokenList.get(k).label.equals(arr[1])) {
								tmpadr2 = TokenList.get(1).tokenList.get(k).location;
							}
						}
						addr = tmpadr1-tmpadr2;
					}
	
				}
				else if(TokenList.get(1).tokenList.get(i).operator.equals("LTORG")) {
					addr += 3;
				}
			}
			
			if(TokenList.get(1).getToken(i).label.equals("WRREC")) {
				TokenList.get(1).tokenList.remove(i);
				break;
			}
			j++;
			i++;
		}
		start = j;
		for(j = 0; j < i; j++) {	
			if(!TokenList.get(1).getToken(j).label.equals("")) {
				TokenList.get(1).symTab.putSymbol(TokenList.get(1).getToken(j).label, TokenList.get(1).getToken(j).location);
			}
			if(TokenList.get(1).getToken(j).operand == null) {
				if(TokenList.get(1).getToken(j).operator.equals("LTORG") || (TokenList.get(1).getToken(j).operator.equals("END"))) {
					continue;
				}
			}
			else {
				if(!TokenList.get(1).getToken(j).operator.equals("RSUB")) {
					if(TokenList.get(1).getToken(j).operand[0].charAt(0) == '=' ) {
						String[] arr1 = TokenList.get(1).getToken(j).operand[0].split("'");
						TokenList.get(1).literalTab.putLiteral(arr1[1], TokenList.get(1).getToken(j).location);
					}
				}
			}
		}
		symtabList.add(TokenList.get(1).symTab);
		literaltabList.add(TokenList.get(1).literalTab);
		
		j = start;
		addr = 0;
		i = 0;
		TokenList.add(new TokenTable(new SymbolTable(), new LiteralTable(), instTable));//section3 ���� �ݺ�
		while(j < lineList.size()) {
			if(lineList.get(j).charAt(0) == '.') {
				j++;
				continue;
			}
			TokenList.get(2).putToken(lineList.get(j));
			
			TokenList.get(2).tokenList.get(i).location = addr;
			if(i > 1) {
				if(TokenList.get(2).tokenList.get(i).location - TokenList.get(2).tokenList.get(i-1).location >= 0) {
					TokenList.get(2).Listlength = TokenList.get(2).tokenList.get(i).location;
				}
			}
			if(instTable.hash_get(TokenList.get(2).tokenList.get(i).operator) > 0) {
				if(instTable.hash_get(TokenList.get(2).tokenList.get(i).operator) == 3) {
					if(TokenList.get(2).tokenList.get(i).operator.charAt(0) == '+') {
						addr += 4;
					}
					else {
						addr += 3;
					}
				}
				else if(instTable.hash_get(TokenList.get(2).tokenList.get(i).operator) == 2) {
					addr += 2;
				}
			}
			else {
				if(TokenList.get(2).tokenList.get(i).operator.equals("RESW")) {
					int tmpadr = Integer.parseInt(TokenList.get(2).tokenList.get(i).operand[0]);
					addr += (3*tmpadr);
				}
				else if(TokenList.get(2).tokenList.get(i).operator.equals("RESB")) {
					int tmpadr = Integer.parseInt(TokenList.get(2).tokenList.get(i).operand[0]);
					addr += tmpadr;
				}
				else if(TokenList.get(2).tokenList.get(i).operator.equals("BYTE")) {
					addr += 1;
				}
				else if(TokenList.get(2).tokenList.get(i).operator.equals("WORD")) {
					addr += 3;
				}
				else if(TokenList.get(2).tokenList.get(i).operator.equals("EQU")) {
					if(!TokenList.get(2).tokenList.get(i).operand[0].equals("*")) {
						String[] arr = TokenList.get(2).tokenList.get(i).operand[0].split("-");
						int tmpadr1=0, tmpadr2=0;
						for(int k = 0; k < TokenList.get(2).tokenList.size(); k++) {
							if(TokenList.get(2).tokenList.get(k).label.equals(arr[0])) {
								tmpadr1 = TokenList.get(2).tokenList.get(k).location;
							}
							if(TokenList.get(2).tokenList.get(k).label.equals(arr[1])) {
								tmpadr2 = TokenList.get(2).tokenList.get(k).location;
							}
						}
						addr = tmpadr1-tmpadr2;
					}
	
				}
				else if(TokenList.get(2).tokenList.get(i).operator.equals("LTORG")) {
					addr += 3;
				}
			}
			
			if(TokenList.get(2).getToken(i).operator.equals("END")) {
				TokenList.get(2).Listlength += 1;
				break;
			}
			j++;
			i++;
		}
		for(j = 0; j <i+1; j++) {
			if(!TokenList.get(2).getToken(j).label.equals("")) {
				TokenList.get(2).symTab.putSymbol(TokenList.get(2).getToken(j).label, TokenList.get(2).getToken(j).location);
			}
			if(TokenList.get(2).getToken(j).operand == null) {
				if(TokenList.get(2).getToken(j).operator.equals("LTORG")) {
					TokenList.get(2).literalTab.putLiteral(lit, TokenList.get(2).getToken(j).location);
				}
			}
			else {
				if(!TokenList.get(2).getToken(j).operator.equals("RSUB")) {
					if (TokenList.get(2).getToken(j).operand[0].charAt(0) == '=' ) {
						String[] arr1 = TokenList.get(2).getToken(j).operand[0].split("'");
						lit = arr1[1];
					}
				}
				if(TokenList.get(2).getToken(j).operator.equals("END")) {
					TokenList.get(2).literalTab.putLiteral(lit, TokenList.get(2).getToken(j).location);
				}
			}
		}
		symtabList.add(TokenList.get(2).symTab);
		literaltabList.add(TokenList.get(2).literalTab);
	}
	
	/**
	 * �ۼ��� SymbolTable���� ������¿� �°� ����Ѵ�.
	 * @param fileName : ����Ǵ� ���� �̸�
	 */
	private void printSymbolTable(String fileName) {
		File filename = new File(fileName);
		FileWriter fw = null;
		try{
			fw = new FileWriter(filename, true);
			for(int i = 0; i < symtabList.get(0).symbolList.size(); i++){
				fw.write(TokenList.get(0).symTab.symbolList.get(i) +"\t"+ Integer.toHexString(TokenList.get(0).symTab.locationList.get(i)).toUpperCase()+"\n");//16���� �빮�ڷ� ��� toHexString, toUpperCase �̿�
			}
			fw.write("\n\n");
			for(int i = 0; i < symtabList.get(1).symbolList.size(); i++) {
				fw.write(TokenList.get(1).symTab.symbolList.get(i) +"\t"+ Integer.toHexString(TokenList.get(1).symTab.locationList.get(i)).toUpperCase()+"\n");
			}
			fw.write("\n\n");
			for(int i = 0; i < symtabList.get(2).symbolList.size(); i++) {
				fw.write(TokenList.get(2).symTab.symbolList.get(i) +"\t"+ Integer.toHexString(TokenList.get(2).symTab.locationList.get(i)).toUpperCase()+"\n");
			}
			fw.close();
		} catch (FileNotFoundException e) {
			System.out.println(e);
		} catch( IOException e) {
			System.out.println(e);
		}
		//TokenList.get(0).symTab.symbolList(i)
	}

	/**
	 * �ۼ��� LiteralTable���� ������¿� �°� ����Ѵ�.
	 * @param fileName : ����Ǵ� ���� �̸�
	 */
	private void printLiteralTable(String fileName) {
		// TODO Auto-generated method stub
		File filename = new File(fileName);
		FileWriter fw = null;
		try{
			fw = new FileWriter(filename, true);
			for(int i = 0; i < literaltabList.get(0).literalList.size(); i++){
				fw.write(TokenList.get(0).literalTab.literalList.get(i) +"\t"+ Integer.toHexString(TokenList.get(0).literalTab.locationList.get(i)).toUpperCase()+"\n");
			}
			fw.write("\n\n");
			for(int i = 0; i < literaltabList.get(2).literalList.size(); i++) {
				fw.write(TokenList.get(2).literalTab.literalList.get(i) +"\t"+ Integer.toHexString(TokenList.get(2).literalTab.locationList.get(i)).toUpperCase()+"\n");
			}
			fw.close();
		} catch (FileNotFoundException e) {
			System.out.println(e);
		} catch( IOException e) {
			System.out.println(e);
		}
	}

	/**
	 * pass2 ������ �����Ѵ�.
	 *   1) �м��� ������ �������� object code�� �����Ͽ� codeList�� ����.
	 */
	private void pass2() {
		// TODO Auto-generated method stub
		for(int i = 0; i < TokenList.get(0).tokenList.size(); i++) {//section 1
			TokenList.get(0).makeObjectCode(i);
			codeList.add(TokenList.get(0).getObjectCode(i));
		}
		for(int i = 0; i < TokenList.get(1).tokenList.size(); i++) {//section2
			TokenList.get(1).makeObjectCode(i);
			codeList.add(TokenList.get(1).getObjectCode(i));
		}
		for(int i = 0; i < TokenList.get(2).tokenList.size(); i++) {
			TokenList.get(2).makeObjectCode(i);//section3
			codeList.add(TokenList.get(2).getObjectCode(i));
		}
	}
	
	/**
	 * �ۼ��� codeList�� ������¿� �°� ����Ѵ�.
	 * @param fileName : ����Ǵ� ���� �̸�
	 */
	private void printObjectCode(String fileName) {
		// TODO Auto-generated method stub
		File filename = new File(fileName);
		FileWriter fw = null;
		int i;
		int start = 0;
		try{
			fw = new FileWriter(filename, true);
			for(int k = 0; k < TokenList.size(); k ++) {
				for(i = 0; i < TokenList.get(k).tokenList.size(); i++){
					if(codeList.get(start+i) == null) {
						if(TokenList.get(k).tokenList.get(i).operator.equals("START") || TokenList.get(k).tokenList.get(i).operator.equals("CSECT")) {
							String str = "H"+TokenList.get(k).tokenList.get(i).label+"\t000000";
							fw.write(str+""+String.format("%06X", TokenList.get(k).Listlength, 16));
						}
						else if(TokenList.get(k).tokenList.get(i).operator.equals("EXTDEF")) {
							fw.write("D");
							for(int j = 0; j < TokenList.get(k).tokenList.get(i).operand.length; j ++) {
								String str = TokenList.get(k).tokenList.get(i).operand[j];
								fw.write(str+String.format("%06X", TokenList.get(k).symTab.search(str)));
							}
						}
						else if(TokenList.get(k).tokenList.get(i).operator.equals("EXTREF")) {
							fw.write("R");
							for(int j = 0; j < TokenList.get(k).tokenList.get(i).operand.length; j ++) {
								fw.write(TokenList.get(k).tokenList.get(i).operand[j]+" ");
							}
						}
						fw.write("\n");
					}
					else {
						break;
					}	
				}
				int startadr = 0;
				int txtlen = 0;
				int j = i;
				int txt = 0;
				while(txtlen <= 30) {
					if(txtlen + TokenList.get(k).tokenList.get(j).byteSize <= 30){
						if(codeList.get(start+j).equals("05")) {
							txtlen += 1;
							break;
						}
						txtlen += TokenList.get(k).tokenList.get(j).byteSize;
					}
					else {
						break;
					}
					j++;
				}
				fw.write("T"+String.format("%06X%X", startadr, txtlen));
				for(; i < TokenList.get(k).tokenList.size(); i++) {
					fw.write(codeList.get(start+i));
					txt += TokenList.get(k).tokenList.get(i).byteSize;
					if(txt == txtlen) {
						break;
					}
				}
				i = j;
				startadr = txtlen;
				txtlen = 0;
				txt = 0;
				while(txtlen <= 30) {
					if(k == 2) {
						break;
					}
					if(txtlen + TokenList.get(k).tokenList.get(j).byteSize <= 30){
						if(codeList.get(start+j+1) == null) {
							break;
						}
						txtlen += TokenList.get(k).tokenList.get(j).byteSize;
					}
					else {
						break;
					}
					j++;
				}
				if(k != 2){
					fw.write("\nT"+String.format("%06X%X", startadr, txtlen));
				}
				for(; i < TokenList.get(k).tokenList.size(); i++) {
					fw.write(codeList.get(start+i));
					txt += TokenList.get(k).tokenList.get(i).byteSize;
					if(i == j) {
						break;
					}
				}
				for(; i < TokenList.get(k).tokenList.size(); i++) {
					if(TokenList.get(k).tokenList.get(i).operator.equals("LTORG")) {
						char c1, c2, c3;
						int i1, i2, i3;
						c1 = TokenList.get(k).literalTab.literalList.get(0).charAt(0);
						c2 = TokenList.get(k).literalTab.literalList.get(0).charAt(1);
						c3 = TokenList.get(k).literalTab.literalList.get(0).charAt(2);
						i1 = (int)c1; i2 = (int)c2; i3 = (int)c3;
						fw.write("\nT"+String.format("%06X", TokenList.get(k).literalTab.locationList.get(0))+String.format("%02X", TokenList.get(k).tokenList.get(i).byteSize)+String.format("%X%X%X", i1, i2, i3));
					}
				}
				for(int cnt = 3; cnt < TokenList.get(k).tokenList.size(); cnt++) {
					if(TokenList.get(k).tokenList.get(cnt).operand == null) {
						cnt++;
						continue;
					}
					if(k == 0) {
						if(TokenList.get(k).tokenList.get(cnt).operand[0].contains("RDREC") || TokenList.get(k).tokenList.get(cnt).operand[0].contains("WRREC")) {
							fw.write("\nM"+String.format("%06X", TokenList.get(k).tokenList.get(cnt).location+1)+"05"+"+"+TokenList.get(k).tokenList.get(cnt).operand[0]);
						}
					}
					else if (k == 1) {
						if(TokenList.get(k).tokenList.get(cnt).operand[0].contains("BUFEND") || TokenList.get(k).tokenList.get(cnt).operand[0].contains("LENGTH") || TokenList.get(k).tokenList.get(cnt).operand[0].contains("BUFFER")) {
							if(TokenList.get(k).tokenList.get(cnt).operand[0].equals("BUFEND-BUFFER")) {
								fw.write("\nM"+String.format("%06X", TokenList.get(k).tokenList.get(cnt).location+1)+"06"+"+BUFEND");
								fw.write("\nM"+String.format("%06X", TokenList.get(k).tokenList.get(cnt).location+1)+"06"+"+BUFFER");
							}
							else {
								fw.write("\nM"+String.format("%06X", TokenList.get(k).tokenList.get(cnt).location+1)+"05"+"+"+TokenList.get(k).tokenList.get(cnt).operand[0]);
							}
							
						}
					}
					else if(k == 2) {
						if(TokenList.get(k).tokenList.get(cnt).operand[0].contains("BUFFER") || TokenList.get(k).tokenList.get(cnt).operand[0].contains("LENGTH")) {
							fw.write("\nM"+String.format("%06X", TokenList.get(k).tokenList.get(cnt).location+1)+"05"+"+"+TokenList.get(k).tokenList.get(cnt).operand[0]);
						}
					}
				}
				fw.write("\nE");
				if(k == 0) {
					fw.write("000000");
				}
				start += i;
				fw.write("\n\n");
			}
		fw.close();
		} catch (FileNotFoundException e) {
			System.out.println(e);
		} catch( IOException e) {
			System.out.println(e);
		}
	}
	
}
