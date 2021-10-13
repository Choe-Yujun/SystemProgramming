import java.util.HashMap;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;

/**
 * ��� instruction�� ������ �����ϴ� Ŭ����. instruction data���� �����Ѵ�.
 * ���� instruction ���� ����, ���� ��� ����� �����ϴ� �Լ�, ���� ������ �����ϴ� �Լ� ���� ���� �Ѵ�.
 */
public class InstTable {
	/** 
	 * inst.data ������ �ҷ��� �����ϴ� ����.
	 *  ��ɾ��� �̸��� ��������� �ش��ϴ� Instruction�� �������� ������ �� �ִ�.
	 */
	HashMap<String, Instruction> instMap;
	
	/**
	 * Ŭ���� �ʱ�ȭ. �Ľ��� ���ÿ� ó���Ѵ�.
	 * @param instFile : instuction�� ���� ���� ����� ���� �̸�
	 */
	public InstTable(String instFile) {
		instMap = new HashMap<String, Instruction>();
		openFile(instFile);
	}
	
	/**
	 * �Է¹��� �̸��� ������ ���� �ش� ������ �Ľ��Ͽ� instMap�� �����Ѵ�.
	 */
	public void openFile(String fileName) {
		File filename = new File(fileName);
		FileReader fr = null;
		try {
			fr = new FileReader(filename);
			BufferedReader br = new BufferedReader(fr);
			String line = "";
			while ((line = br.readLine()) != null) {
				String[] arr = line.split("\t");//tab���� �����Ͽ� ���� Instruction(key) �ڴ� format,opcode,number of operand
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
		if(line.charAt(0) == '+') {//4������ ���
			String newline = line.substring(1);//+�� ���ش�
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
 * ��ɾ� �ϳ��ϳ��� ��ü���� ������ InstructionŬ������ ����.
 * instruction�� ���õ� �������� �����ϰ� �������� ������ �����Ѵ�.
 */
class Instruction {

	 int format;
	 String opcode;
	 int numberOfOperand;
	
	/** instruction�� �� ����Ʈ ��ɾ����� ����. ���� ���Ǽ��� ���� */

	
	/**
	 * Ŭ������ �����ϸ鼭 �Ϲݹ��ڿ��� ��� ������ �°� �Ľ��Ѵ�.
	 * @param line : instruction �����Ϸκ��� ���پ� ������ ���ڿ�
	 */
	public Instruction(String line) {
		parsing(line);
	}
	
	/**
	 * �Ϲ� ���ڿ��� �Ľ��Ͽ� instruction ������ �ľ��ϰ� �����Ѵ�.
	 * @param line : instruction �����Ϸκ��� ���پ� ������ ���ڿ�
	 */
	public void parsing(String line) {
		//Instruction inst = new Instruction(line);
		String[] arr = line.split(",");//format,opcode,numberOfOperand �и�
		this.format = Integer.parseInt(arr[0]);
		this.opcode = arr[1];
		this.numberOfOperand = Integer.parseInt(arr[2]);
	}
}
