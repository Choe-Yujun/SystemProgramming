package SP20_simulator;

import java.io.*;
import java.util.ArrayList;
import java.util.*;



/**
 * ResourceManager�� ��ǻ���� ���� ���ҽ����� �����ϰ� �����ϴ� Ŭ�����̴�.
 * ũ�� �װ����� ���� �ڿ� ������ �����ϰ�, �̸� ������ �� �ִ� �Լ����� �����Ѵ�.<br><br>
 * 
 * 1) ������� ���� �ܺ� ��ġ �Ǵ� device<br>
 * 2) ���α׷� �ε� �� ������ ���� �޸� ����. ���⼭�� 64KB�� �ִ밪���� ��´�.<br>
 * 3) ������ �����ϴµ� ����ϴ� �������� ����.<br>
 * 4) SYMTAB �� simulator�� ���� �������� ���Ǵ� �����͵��� ���� ������. 
 * <br><br>
 * 2���� simulator������ ����Ǵ� ���α׷��� ���� �޸𸮰����� �ݸ�,
 * 4���� simulator�� ������ ���� �޸� �����̶�� ������ ���̰� �ִ�.
 */
public class ResourceManager{
	/**
	 * ����̽��� ���� ����� ��ġ���� �ǹ� ������ ���⼭�� ���Ϸ� ����̽��� ��ü�Ѵ�.<br>
	 * ��, 'F1'�̶�� ����̽��� 'F1'�̶�� �̸��� ������ �ǹ��Ѵ�. <br>
	 * deviceManager�� ����̽��� �̸��� �Է¹޾��� �� �ش� �̸��� ���� ����� ���� Ŭ������ �����ϴ� ������ �Ѵ�.
	 * ���� ���, 'A1'�̶�� ����̽����� ������ read���� ������ ���, hashMap�� <"A1", scanner(A1)> ���� �������μ� �̸� ������ �� �ִ�.
	 * <br><br>
	 * ������ ���·� ����ϴ� �� ���� ����Ѵ�.<br>
	 * ���� ��� key������ String��� Integer�� ����� �� �ִ�.
	 * ���� ������� ���� ����ϴ� stream ���� �������� ����, �����Ѵ�.
	 * <br><br>
	 * �̰͵� �����ϸ� �˾Ƽ� �����ؼ� ����ص� �������ϴ�.
	 */
	HashMap<String,Object> deviceManager = new HashMap<String,Object>();
	char[] memory = new char[65536]; // String���� �����ؼ� ����Ͽ��� ������.
	int[] register = new int[10];
	double register_F;
	
	SymbolTable symtabList = new SymbolTable();
	private int currentSection;
	private int readPointer = 0;
	SymbolTable extabList = new SymbolTable();

	private ArrayList<String> progNameList = new ArrayList<String>();
	private ArrayList<Integer> progLengthList = new ArrayList<Integer>();
	private ArrayList<Integer> progStartAddrList = new ArrayList<Integer>();
	// �̿ܿ��� �ʿ��� ���� �����ؼ� ����� ��.

	/**
	 * �޸�, �������͵� ���� ���ҽ����� �ʱ�ȭ�Ѵ�.
	 */
	public void initializeResource(){
		for (int i = 0; i < register.length; i++)
			register[i] = 0;

		register_F = 0;
		currentSection = 0;
		register[SicSimulator.X_REGISTER] = progStartAddrList.get(currentSection);
		register[SicSimulator.L_REGISTER] = SicSimulator.INIT_RETADR;
	}
	
	/**
	 * deviceManager�� �����ϰ� �ִ� ���� ����� stream���� ���� �����Ű�� ����.
	 * ���α׷��� �����ϰų� ������ ���� �� ȣ���Ѵ�.
	 */
	public void closeDevice() {
		Iterator<String> it = deviceManager.keySet().iterator();

		while (it.hasNext())
		{
			String key = it.next();
			Object stream = deviceManager.get(key);

			try
			{
				if (stream instanceof FileReader)
				{
					((FileReader) stream).close();
				}
				else if (stream instanceof FileWriter)

				{
					((FileWriter) stream).close();
				}
			}
			catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		}
	}
	
	/**
	 * ����̽��� ����� �� �ִ� ��Ȳ���� üũ. TD���ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * ����� stream�� ���� deviceManager�� ���� ������Ų��.
	 * @param devName Ȯ���ϰ��� �ϴ� ����̽��� ��ȣ,�Ǵ� �̸�
	 */
	public void testDevice(String devName) {

		String str = devName+".txt";
		File file = new File(str);
		if(file != null){
			deviceManager.put(devName,(File)file);
			register[SicSimulator.SW_REGISTER] = 1;
			setRegister(SicSimulator.PC_REGISTER, getRegister(SicSimulator.PC_REGISTER) + 3);
		}
	}

	/**
	 * ����̽��κ��� ���ϴ� ������ŭ�� ���ڸ� �о���δ�. RD���ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * @param devName ����̽��� �̸�
	 * @param num �������� ������ ����
	 * @return ������ ������
	 */
	public char readDevice(String devName){
		char input = ' ';
		try
		{
			File file = (File)deviceManager.get(devName);
			FileReader fileReader = new FileReader(file);
			int inputChar = 0;
			int index = 0;
			
			while(index <= readPointer)
			{
				inputChar = fileReader.read();
				index++;
			}
			
			if (inputChar != -1)
			{
				input = (char) inputChar;
			}
			else
				input = 0;
			
			readPointer++;
			fileReader.close();
		}
		catch (FileNotFoundException e)
		{
		}
		catch (IOException e)
		{
			System.out.println(e);
		}

		return input;
		
	}

	/**
	 * ����̽��� ���ϴ� ���� ��ŭ�� ���ڸ� ����Ѵ�. WD���ɾ ������� �� ȣ��Ǵ� �Լ�.
	 * @param devName ����̽��� �̸�
	 * @param data ������ ������
	 * @param num ������ ������ ����
	 */
	public void writeDevice(String devName){
		try
		{
			File file = (File)deviceManager.get(devName);
			FileWriter fileWriter = new FileWriter(file, true);
			//for(int i = 0; i < num; i++) {
				fileWriter.write((char)(register[SicSimulator.A_REGISTER] & 255));
				fileWriter.close();
			//}
			//, char[] data, int num

		}
		catch (FileNotFoundException e)
		{
			// ������ ã�� ������ ���� ���� �ڵ鸵
		}
		catch (IOException e)
		{
			System.out.println(e);
		}
	}
	
	/**
	 * �޸��� Ư�� ��ġ���� ���ϴ� ������ŭ�� ���ڸ� �����´�.
	 * @param location �޸� ���� ��ġ �ε���
	 * @param num ������ ����
	 * @return �������� ������
	 */
	public char[] getMemory(int location, int num){
		char[] result = new char[num];

		for (int i = location; i < location + num; i++)
		{
			result[i-location] = memory[i];
		}

		return result;
		
	}

	/**
	 * �޸��� Ư�� ��ġ�� ���ϴ� ������ŭ�� �����͸� �����Ѵ�. 
	 * @param locate ���� ��ġ �ε���
	 * @param data �����Ϸ��� ������
	 * @param num �����ϴ� �������� ����
	 */
	public void setMemory(int locate, char[] data, int num){
		for (int i = locate; i < locate + num; i++)
		{

			memory[i] = data[i - locate];

			System.out.print(data[i - locate] >> 8);
			System.out.print(" ");
			System.out.print(data[i - locate] & 255);
		}
	}

	/**
	 * ��ȣ�� �ش��ϴ� �������Ͱ� ���� ��� �ִ� ���� �����Ѵ�. �������Ͱ� ��� �ִ� ���� ���ڿ��� �ƴԿ� �����Ѵ�.
	 * @param regNum �������� �з���ȣ
	 * @return �������Ͱ� ������ ��
	 */
	public int getRegister(int regNum){
		return register[regNum];
		
	}

	/**
	 * ��ȣ�� �ش��ϴ� �������Ϳ� ���ο� ���� �Է��Ѵ�. �������Ͱ� ��� �ִ� ���� ���ڿ��� �ƴԿ� �����Ѵ�.
	 * @param regNum ���������� �з���ȣ
	 * @param value �������Ϳ� ����ִ� ��
	 */
	public void setRegister(int regNum, int value){

		register[regNum] = value;
	}

	/**
	 * �ַ� �������Ϳ� �޸𸮰��� ������ ��ȯ���� ���ȴ�. int���� char[]���·� �����Ѵ�.
	 * @param data
	 * @return
	 */
	public char[] intToChar(int data){
		char[] inputData = String.format("%X", data).toCharArray();
		int length = (inputData.length / 2) + (inputData.length % 2);
		char[] outputData = new char[length];

		int upByte = 0;
		int downByte = 0;

		if (inputData.length % 2 == 0)
		{

			for (int i = 0; i < length; i++)
			{
				upByte = inputData[i * 2] - '0';
				downByte = inputData[i * 2 + 1] - '0';
				if (upByte >= 10)
					upByte -= 7;
				if (downByte >= 10)
					downByte -= 7;
				
				

				outputData[i] = (char) ((upByte << 8) + downByte);
			}
		}
		else
		{
			downByte = (inputData[0] - '0');
			if(downByte >= 10)
				downByte -= 7;
			outputData[0] = (char) downByte;
			
			for (int i = 1; i < length; i++)
			{
				upByte = inputData[i * 2 - 1] - '0';
				downByte = inputData[i * 2] - '0';
				if (upByte >= 10)
					upByte -= 7;
				if (downByte >= 10)
					downByte -= 7;

				outputData[i] = (char) ((upByte << 8) + downByte);
			}
		}
		return outputData;
	}

	/**
	 * �ַ� �������Ϳ� �޸𸮰��� ������ ��ȯ���� ���ȴ�. char[]���� int���·� �����Ѵ�.
	 * @param data
	 * @return
	 */
	public int byteToInt(char[] data){
		int result = 0;
		for(int i = 0; i < data.length; i++)
		{
			result = result << 4;
			result += (data[i] >> 8);
			result = result << 4;
			result += (data[i] & 255);
		}
		return result;
	}

	//프로그램이름 저장
	public void setProgName(String progName, int sectionNum)
	{
		progNameList.add(sectionNum, progName);
	}


	//프로그램 시작주소 저장
	public void setProgStartAddr(String startAddr, int sectionNum)
	{
		int addr = Integer.parseInt(startAddr, 16);

		if (sectionNum > 0)
		{
			addr += progStartAddrList.get(sectionNum - 1) + progLengthList.get(sectionNum - 1);
		}

		progStartAddrList.add(sectionNum, addr);
	}

	//프로그램 길이 저장
	public void setProgLength(String length, int sectionNum)
	{
		progLengthList.add(sectionNum, Integer.parseInt(length, 16));
	}

	//프로그램 시작주소 반환
	public int getProgStartAddr(int sectionNum)
	{
		return progStartAddrList.get(sectionNum);
	}

	//프로그램 이름 반환
	public String getProgName(int sectionNum)
	{
		return progNameList.get(sectionNum);
	}

	//프로그램 길이 반환
	public int getProgLength(int section)
	{
		return progLengthList.get(section);
	}

	//이름크기 반환
	public int getProgCount()
	{
		return progNameList.size();
	}

	//현재 섹션
	public int getCurrentSection()
	{
		return currentSection;
	}

	//현재 섹션 저장
	public void setCurrentSection()
	{
		int  i = 0;
		for(i = 0; i < getProgCount(); i++)
		{
			if(getProgStartAddr(i) <= getRegister(SicSimulator.PC_REGISTER) && getRegister(SicSimulator.PC_REGISTER) < getProgStartAddr(i) + getProgLength(i))
			{
				currentSection = i;
				break;
			}
		}
		
		if(i == getProgCount())
			currentSection = 0;
	}

	//수정데이터 메모리
	public void modifMemory(int locate, char[] data, int num, char modifMode)
	{
		if (modifMode == '+')
		{
			for (int i = locate; i < locate + num; i++)
			{
				memory[i] += data[i - locate];
			}
		}
		else if (modifMode == '-')
		{
			for (int i = locate; i < locate + num; i++)
			{
				memory[i] -= data[i - locate];
			}
		}
	}
}