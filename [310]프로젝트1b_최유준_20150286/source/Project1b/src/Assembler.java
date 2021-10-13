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
 * 이 프로그램은 SIC/XE 머신을 위한 Assembler 프로그램의 메인 루틴이다.
 * 프로그램의 수행 작업은 다음과 같다. 
 * 1) 처음 시작하면 Instruction 명세를 읽어들여서 assembler를 세팅한다. 
 * 2) 사용자가 작성한 input 파일을 읽어들인 후 저장한다. 
 * 3) input 파일의 문장들을 단어별로 분할하고 의미를 파악해서 정리한다. (pass1) 
 * 4) 분석된 내용을 바탕으로 컴퓨터가 사용할 수 있는 object code를 생성한다. (pass2) 
 * 
 * 
 * 작성중의 유의사항 : 
 *  1) 새로운 클래스, 새로운 변수, 새로운 함수 선언은 얼마든지 허용됨. 단, 기존의 변수와 함수들을 삭제하거나 완전히 대체하는 것은 안된다.
 *  2) 마찬가지로 작성된 코드를 삭제하지 않으면 필요에 따라 예외처리, 인터페이스 또는 상속 사용 또한 허용됨.
 *  3) 모든 void 타입의 리턴값은 유저의 필요에 따라 다른 리턴 타입으로 변경 가능.
 *  4) 파일, 또는 콘솔창에 한글을 출력시키지 말 것. (채점상의 이유. 주석에 포함된 한글은 상관 없음)
 * 
 *     
 *  + 제공하는 프로그램 구조의 개선방법을 제안하고 싶은 분들은 보고서의 결론 뒷부분에 첨부 바랍니다. 내용에 따라 가산점이 있을 수 있습니다.
 */
public class Assembler {
	/** instruction 명세를 저장한 공간 */
	InstTable instTable;
	/** 읽어들인 input 파일의 내용을 한 줄 씩 저장하는 공간. */
	ArrayList<String> lineList;
	/** 프로그램의 section별로 symbol table을 저장하는 공간*/
	ArrayList<SymbolTable> symtabList;
	/** 프로그램의 section별로 literal table을 저장하는 공간*/
	ArrayList<LiteralTable> literaltabList;
	/** 프로그램의 section별로 프로그램을 저장하는 공간*/
	ArrayList<TokenTable> TokenList;
	/** 
	 * Token, 또는 지시어에 따라 만들어진 오브젝트 코드들을 출력 형태로 저장하는 공간.   
	 * 필요한 경우 String 대신 별도의 클래스를 선언하여 ArrayList를 교체해도 무방함.
	 */
	ArrayList<String> codeList;
	
	/**
	 * 클래스 초기화. instruction Table을 초기화와 동시에 세팅한다.
	 * 
	 * @param instFile : instruction 명세를 작성한 파일 이름. 
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
	 * 어셈블러의 메인 루틴
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
	 * inputFile을 읽어들여서 lineList에 저장한다.
	 * @param inputFile : input 파일 이름.
	 */
	private void loadInputFile(String inputFile) {
		try {
			File filename = new File(inputFile);
			FileReader fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while ((line = br.readLine()) != null) {//한줄씩 읽기
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
	 * pass1 과정을 수행한다.
	 *   1) 프로그램 소스를 스캔하여 토큰단위로 분리한 뒤 토큰테이블 생성
	 *   2) label을 symbolTable에 정리
	 *   
	 *    주의사항 : SymbolTable과 TokenTable은 프로그램의 section별로 하나씩 선언되어야 한다.
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
				if(TokenList.get(0).tokenList.get(i).location - TokenList.get(0).tokenList.get(i-1).location >= 0) {//TokenTable의 length 구하기
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
				else if(TokenList.get(0).tokenList.get(i).operator.equals("EQU")) {//equation일 경우
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

			if(TokenList.get(0).getToken(i).operator.equals("CSECT")){//다음 section이 현재 Table에 포함되었을 경우 remove해준다.
				TokenList.get(0).tokenList.remove(i);
				break;
			}
			i++;
		}
		for(j = 0; j < i; j++) { //literal, symbol input
			if(!TokenList.get(0).getToken(j).label.equals("")) {//label이 있으면 symboltable에 저장
				TokenList.get(0).symTab.putSymbol(TokenList.get(0).getToken(j).label, TokenList.get(0).getToken(j).location);
			}
			if(TokenList.get(0).getToken(j).operand == null) {//lit에 저장된 값 literaltable에 저장
				if((TokenList.get(0).getToken(j).operator.equals("LTORG")) || (TokenList.get(0).getToken(j).operator.equals("END"))) {
					TokenList.get(0).literalTab.putLiteral(lit, TokenList.get(0).getToken(j).location);
				}
			}
			else {
				if (TokenList.get(0).getToken(j).operand[0].charAt(0) == '=' ) {//operand가 literal일 경우 lit에 저장
					String[] arr1 = TokenList.get(0).getToken(j).operand[0].split("'");
					lit = arr1[1];
				}
			}
		}
		symtabList.add(TokenList.get(0).symTab);//symtabList에 현재 section 저장
		literaltabList.add(TokenList.get(0).literalTab);
		
		j = i;
		start = i;
		addr = 0;
		i = 0;
		TokenList.add(new TokenTable(new SymbolTable(), new LiteralTable(), instTable));//section 2 이하 반복
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
		TokenList.add(new TokenTable(new SymbolTable(), new LiteralTable(), instTable));//section3 이하 반복
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
	 * 작성된 SymbolTable들을 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
	 */
	private void printSymbolTable(String fileName) {
		File filename = new File(fileName);
		FileWriter fw = null;
		try{
			fw = new FileWriter(filename, true);
			for(int i = 0; i < symtabList.get(0).symbolList.size(); i++){
				fw.write(TokenList.get(0).symTab.symbolList.get(i) +"\t"+ Integer.toHexString(TokenList.get(0).symTab.locationList.get(i)).toUpperCase()+"\n");//16진수 대문자로 출력 toHexString, toUpperCase 이용
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
	 * 작성된 LiteralTable들을 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
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
	 * pass2 과정을 수행한다.
	 *   1) 분석된 내용을 바탕으로 object code를 생성하여 codeList에 저장.
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
	 * 작성된 codeList를 출력형태에 맞게 출력한다.
	 * @param fileName : 저장되는 파일 이름
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
