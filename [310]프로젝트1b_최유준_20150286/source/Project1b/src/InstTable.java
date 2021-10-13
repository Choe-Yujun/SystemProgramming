import java.util.HashMap;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;

/**
 * 모든 instruction의 정보를 관리하는 클래스. instruction data들을 저장한다.
 * 또한 instruction 관련 연산, 예를 들면 목록을 구축하는 함수, 관련 정보를 제공하는 함수 등을 제공 한다.
 */
public class InstTable {
	/** 
	 * inst.data 파일을 불러와 저장하는 공간.
	 *  명령어의 이름을 집어넣으면 해당하는 Instruction의 정보들을 리턴할 수 있다.
	 */
	HashMap<String, Instruction> instMap;
	
	/**
	 * 클래스 초기화. 파싱을 동시에 처리한다.
	 * @param instFile : instuction에 대한 명세가 저장된 파일 이름
	 */
	public InstTable(String instFile) {
		instMap = new HashMap<String, Instruction>();
		openFile(instFile);
	}
	
	/**
	 * 입력받은 이름의 파일을 열고 해당 내용을 파싱하여 instMap에 저장한다.
	 */
	public void openFile(String fileName) {
		File filename = new File(fileName);
		FileReader fr = null;
		try {
			fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while ((line = br.readLine()) != null) {
				String[] arr = line.split("\t");//tab으로 구분하여 앞은 Instruction(key) 뒤는 format,opcode,number of operand
				Instruction inst = new Instruction(arr[1]);
				instMap.put(arr[0], inst);
			}
			br.close();
		} catch (FileNotFoundException e) {
			System.out.println(e);
		} catch( IOException e) {
			System.out.println(e);
		}
	}
	public int hash_get(String line) {
		if(line.charAt(0) == '+') {//4형식일 경우
			String newline = line.substring(1);//+를 떼준다
			return instMap.get(newline).format;
		}
		else {
			if(instMap.containsKey(line) == true) {
				return (instMap.get(line).format);
			}
			else {
				return -1;
			}
		}
	}
}
/**
 * 명령어 하나하나의 구체적인 정보는 Instruction클래스에 담긴다.
 * instruction과 관련된 정보들을 저장하고 기초적인 연산을 수행한다.
 */
class Instruction {

	 int format;
	 String opcode;
	 int numberOfOperand;
	
	/** instruction이 몇 바이트 명령어인지 저장. 이후 편의성을 위함 */

	
	/**
	 * 클래스를 선언하면서 일반문자열을 즉시 구조에 맞게 파싱한다.
	 * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
	 */
	public Instruction(String line) {
		parsing(line);
	}
	
	/**
	 * 일반 문자열을 파싱하여 instruction 정보를 파악하고 저장한다.
	 * @param line : instruction 명세파일로부터 한줄씩 가져온 문자열
	 */
	public void parsing(String line) {
		//Instruction inst = new Instruction(line);
		String[] arr = line.split(",");//format,opcode,numberOfOperand 분리
		this.format = Integer.parseInt(arr[0]);
		this.opcode = arr[1];
		this.numberOfOperand = Integer.parseInt(arr[2]);
	}
}
