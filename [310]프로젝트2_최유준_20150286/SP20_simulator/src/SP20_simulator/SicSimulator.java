package SP20_simulator;

import java.io.*;
import java.util.*;

import org.omg.CORBA.ARG_IN;

/**
 * �ùķ����ͷμ��� �۾��� ����Ѵ�. VisualSimulator���� ������� ��û�� ������ �̿� ����
 * ResourceManager�� �����Ͽ� �۾��� �����Ѵ�.  
 * 
 * �ۼ����� ���ǻ��� : <br>
 *  1) ���ο� Ŭ����, ���ο� ����, ���ο� �Լ� ������ �󸶵��� ����. ��, ������ ������ �Լ����� �����ϰų� ������ ��ü�ϴ� ���� ������ ��.<br>
 *  2) �ʿ信 ���� ����ó��, �������̽� �Ǵ� ��� ��� ���� ����.<br>
 *  3) ��� void Ÿ���� ���ϰ��� ������ �ʿ信 ���� �ٸ� ���� Ÿ������ ���� ����.<br>
 *  4) ����, �Ǵ� �ܼ�â�� �ѱ��� ��½�Ű�� �� ��. (ä������ ����. �ּ��� ���Ե� �ѱ��� ��� ����)<br>
 * 
 * <br><br>
 *  + �����ϴ� ���α׷� ������ ��������� �����ϰ� ���� �е��� ������ ��� �޺κп� ÷�� �ٶ��ϴ�. ���뿡 ���� �������� ���� �� �ֽ��ϴ�.
 */
public class SicSimulator {
	ResourceManager rMgr;
	public static final int A_REGISTER = 0;
	public static final int X_REGISTER = 1;
	public static final int L_REGISTER = 2;
	public static final int B_REGISTER = 3;
	public static final int S_REGISTER = 4;
	public static final int T_REGISTER = 5;
	public static final int F_REGISTER = 6;
	public static final int PC_REGISTER = 8;
	public static final int SW_REGISTER = 9;

	public static final int INIT_RETADR = 0x4000;

	private int targetAddr;
	private String currentDevice;
	private List<String> instList = new ArrayList<>();
	private List<String> logList = new ArrayList<>();
	InstLuncher inst;

	public SicSimulator(ResourceManager resourceManager) {
		// �ʿ��ϴٸ� �ʱ�ȭ ���� �߰�
		this.rMgr = resourceManager;
		targetAddr = 0;
		currentDevice = "";
		this.inst = new InstLuncher(rMgr);
	}

	/**
	 * ��������, �޸� �ʱ�ȭ �� ���α׷� load�� ���õ� �۾� ����.
	 * ��, object code�� �޸� ���� �� �ؼ��� SicLoader���� �����ϵ��� �Ѵ�. 
	 */
	public void load(File program) {
		/* �޸� �ʱ�ȭ, �������� �ʱ�ȭ ��*/
		rMgr.initializeResource();
	}

	/**
	 * 1���� instruction�� ����� ����� ���δ�. 
	 */
	public void oneStep() {
		char [] bytes = rMgr.getMemory(rMgr.getRegister(PC_REGISTER), 2);
		int temp = (bytes[0] >>> 4) + (bytes[0] & 15);
		int opcode = temp;
		boolean extForm = false;
		boolean pcRelative = false;
		boolean immediate = false;
		boolean indirect = false;
		int registerNum = 0;
		int difference = 0;
		char [] instruction = new char[1];
		
		// ���� 1����Ʈ�� ������
		// indirect�� immediate addressing ǥ��
		if((temp & 2) == 2)
		{
			opcode -= 2;
			indirect = true;
		}
		
		if((temp & 1) == 1)
		{
			opcode -= 1;
			immediate = true;
		}
		
		// �ι�° ����Ʈ�� ������
		// Ȯ��� 4���� ��ɾ ����Ͽ�����,
		// PC relative�� ����Ͽ����� ǥ��
		temp = (bytes[1] >>> 8);
		extForm = (temp & 1) == 1;
		pcRelative = (temp & 2) == 2;
		// target address �ʱ�ȭ
		targetAddr = 0;
		
		// �м��� opcode�� ���� ���� ������ �޸���
		switch(opcode)
		{
			case 0x14:  // STL ��ɾ�: L �������� ���� �ش� �ּҿ� �����ϴ� ��ɾ�
				addLog("STL");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.STL(opcode, extForm, pcRelative);
				}
				else  // 3���� ��ɾ��� ���
				{
					instruction = inst.STL(opcode, extForm, pcRelative);
						
				}
				break;
				
			case 0x48: // JSUB ��ɾ�: �ּҰ����� ���� ������ �̵�
				addLog("JSUB");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.JSUB(opcode, extForm, pcRelative);

				}
				else   // 3���� ��ɾ��� ���,
				{
					instruction = inst.JSUB(opcode, extForm, pcRelative);

				}
				break;
				
			case 0x00:  // LDA ��ɾ�: �ش� �ǿ����� �ּҿ� ����� ���� A �������ͷ� ������
				addLog("LDA");
				if(extForm)  // 4������ ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.LDA(opcode, extForm, pcRelative, immediate);

				}
				else  // 3���� ��ɾ��� ���,
				{
					instruction = inst.LDA(opcode, extForm, pcRelative, immediate);

				}
				break;
				
			case 0x28: // COMP ��ɾ�: A�������� ���� ��ɾ �־��� ���� ���Ѵ�.
				addLog("COMP");
				if(extForm)  // 4���� ��ɾ ����ϴ� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.COMP(opcode, extForm, immediate, difference);

				}
				else  // 3���� ��ɾ ����ϴ� ���,
				{
					instruction = inst.COMP(opcode, extForm, immediate, difference);
				}
				break;
		
			case 0x4c: // RSUB ��ɾ�: L �������Ϳ� ����Ǿ��ִ� �ּҷ� �̵�(ȣ�� ���� ���� ��ɾ�� ���ư�)
				addLog("RSUB");
				instruction = inst.RSUB(opcode);
				
				currentDevice = "";
				rMgr.setCurrentSection();
				break;
			
			case 0x50:  // LDCH ��ɾ�: �ش� �ּ��� ���� A�������� ���� 1����Ʈ�� �ҷ��´�.
				logList.add("LDCH");
				if(extForm)  // 4���� ��ɾ ����ϴ� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.LDCH(opcode, extForm, pcRelative);
					
				}
				else // 3���� ��ɾ ����ϴ� ���,
				{
					instruction = inst.LDCH(opcode, extForm, pcRelative);
				}
				break;
			
			case 0xdc: // WD ��ɾ�: ������ ���(�Ǵ� ����)�� A �������� ���� 1����Ʈ�� ���� ����Ѵ�.
				addLog("WD");
				if(extForm)  // 4���� ��ɾ ����ϴ� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.WD(opcode, extForm, pcRelative);
				}//, Integer.toHexString(rMgr.getRegister[A_REGISTER] & 255), 1
				else  // 3���� ��ɾ ����ϴ� ���,
				{
					instruction = inst.WD(opcode, extForm, pcRelative);
				}//, Integer.toHexString(rMgr.getRegister[A_REGISTER] & 255), 1
				
				break;
				
			case 0x3c: // �ǿ����ڷ� ���� �ּҷ� ���α׷� �帧�� �̵��Ѵ�.
				addLog("J");
				if(extForm)  // 4������ ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.J(opcode, extForm, pcRelative, indirect, immediate);
				}
				else  // 3������ ���,
				{
					instruction = inst.J(opcode, extForm, pcRelative, indirect, immediate);
				}
				break;
			
			case 0x0c:
				addLog("STA"); //STA ��ɾ�: A �������Ϳ� ����� ���� ������ �ּҷ� �����Ѵ�.
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.STA(opcode, extForm, pcRelative);
				}
				else  // 3���� ��ɾ��� ���,
				{
					instruction = inst.STA(opcode, extForm, pcRelative);
				}
				break;
				
			case 0xb4:  // CLEAR ��ɾ�: �ش� ���������� ���� 0���� �ʱ�ȭ ��Ű�� ��ɾ�
				logList.add("CLEAR");
				instruction = inst.CLEAR(opcode);
				
				registerNum = instruction[1] >>> 8;
				rMgr.setRegister(registerNum, 0);
				break;
			
			case 0x74:  // LDT ��ɾ�: �ش� �ǿ������� ���� T �������Ϳ� �����Ѵ�.
				addLog("LDT"); 
				if(extForm)
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.LDT(opcode, extForm, pcRelative);			
				}
				else
				{
					instruction = inst.LDT(opcode, extForm, pcRelative);
				}
				break;
			
			case 0xe0: // TD ��ɾ�: �ش� �̸��� ���(�Ǵ� ����)�� ����� ��Ʈ���� Ȯ���Ѵ�.
				addLog("TD");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.TD(opcode, extForm);
					
					char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
					String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
					currentDevice = deviceName;
					rMgr.testDevice(deviceName);
				}
				else  // 3���� ��ɾ��� ���,
				{
					//instruction = inst.TD(opcode, extForm);
					instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            		targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            		rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
					
					if(pcRelative)
						targetAddr += rMgr.getRegister(PC_REGISTER);
					char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
					String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
					currentDevice = deviceName;
					rMgr.testDevice(deviceName);
				}
				
				break;
			
			case 0xd8:  // RD ��ɾ�: �ش� ���(�Ǵ� ����)���� ���� �ϳ��� �о� A�������Ϳ� �����Ѵ�.
				addLog("RD");
				if(extForm)
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = rMgr.getMemory(rMgr.getRegister(PC_REGISTER), 4);
					targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
					rMgr.setRegister(PC_REGISTER, rMgr.getRegister(PC_REGISTER) + 4);
					//instruction = inst.RD(opcode, extForm);
					
					char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
					String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
					rMgr.setRegister(A_REGISTER, rMgr.readDevice(deviceName));
				}
				else
				{
					//instruction = inst.RD(opcode, extForm);
					instruction = rMgr.getMemory(rMgr.getRegister(PC_REGISTER), 3);
					targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
					rMgr.setRegister(PC_REGISTER, rMgr.getRegister(PC_REGISTER) + 3);
					
					if(pcRelative)
						targetAddr += rMgr.getRegister(PC_REGISTER);
					char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
					String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
					rMgr.setRegister(A_REGISTER, rMgr.readDevice(deviceName));
				}				
				break;
				
			case 0xa0:  // COMPR ��ɾ�: �� �������� ���� ���Ѵ�.
				addLog("COMPR");
				//instruction = inst.COMPR(opcode);
				instruction = rMgr.getMemory(rMgr.getRegister(PC_REGISTER), 2);
        		rMgr.setRegister(PC_REGISTER, rMgr.getRegister(PC_REGISTER) + 2);
				
				registerNum = instruction[1] >>> 8;
				int compareRegister = instruction[1] & 15;
				difference = rMgr.getRegister(registerNum) - rMgr.getRegister(compareRegister);
				rMgr.setRegister(SW_REGISTER, difference);
				break;
			
			case 0x54:  // STCH ��ɾ�: A�������� ���� 1����Ʈ�� ����� ���ڸ� ������ �ּҿ� �����Ѵ�.
				addLog("STCH");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.STCH(opcode, extForm, pcRelative);
				}
				else  // 3���� ��ɾ��� ���,
				{
					instruction = inst.STCH(opcode, extForm, pcRelative);
				}
				break;
			
			case 0xb8:  //TIXR ��ɾ�: X �������� ���� 1 �ø��� �ǿ����ڷ� ���� ���������� ���� ���Ѵ�.
				addLog("TIXR");
				instruction = inst.TIXR();
				
				registerNum = instruction[1] >>> 8;
				rMgr.setRegister(X_REGISTER, rMgr.getRegister(X_REGISTER)+1);
				difference = rMgr.getRegister(X_REGISTER) - rMgr.getRegister(registerNum);
				rMgr.setRegister(SW_REGISTER, difference);
				break;
			
			case 0x38:  // JLT ��ɾ�: �� �� �۴ٸ� ��õ� �ּҷ� �̵��Ѵ�.
				addLog("JLT");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.JLT(extForm, pcRelative);
				}
				else  // 3���� ��ɾ��� ���,
				{
					instruction = inst.JLT(extForm, pcRelative);
				}
				break;
			
			case 0x10:  // STX ��ɾ�: X���������� ���� ������ �ּҿ� �����Ѵ�.
				addLog("STX");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.STX(extForm, pcRelative);
				}
				else  // 3���� ��ɾ��� ���,
				{
					instruction = inst.STX(extForm, pcRelative);
				}
				break;
				
			case 0x30:  // JEQ ��ɾ�: ���� �� ���� ���� ��� ������ �ּҷ� �̵��Ѵ�.
				addLog("JEQ");
				if(extForm)  // 4���� ��ɾ��� ���,
				{
					logList.set(logList.size()-1, "+" + logList.get(logList.size()-1));
					instruction = inst.JEQ(extForm, pcRelative);
				}
				else // 3���� ��ɾ��� ���,
				{
					instruction = inst.JEQ(extForm, pcRelative);
				}
				break;
		}
		
		// instruction ����� ���� 4��Ʈ�� �ش��ϴ� ���� ���ڷ� �ٽ� ��ȯ
		// ��ȯ�� instruction�� ����Ʈ�� �߰��Ѵ�.
		char[] outputInst = new char[instruction.length*2];
		for(int  i = 0; i < instruction.length; i++)
		{
			outputInst[i * 2] = (char)((instruction[i] >> 8) + '0');
			outputInst[i * 2 + 1] = (char)((instruction[i] & 255) + '0');
			
			if((instruction[i] >> 8) >= 10)
				outputInst[i * 2] += 7;
			
			if((instruction[i] & 255) >= 10)
				outputInst[i * 2 + 1] += 7;
		}
		instList.add(new String(outputInst, 0, outputInst.length));
	}
	
	/**
	 * ���� ��� instruction�� ����� ����� ���δ�.
	 */
	public void allStep() {
		while(true)
		{
			oneStep();
			
			// �ʱ� ������ L �������� ���� PC �������� ���� �������� �۵��� �����.
			if(rMgr.getRegister(PC_REGISTER) == INIT_RETADR)
				break;
		}
	}
	
	/**
	 * �� �ܰ踦 ������ �� ���� ���õ� ����� ���⵵�� �Ѵ�.
	 */
	public void addLog(String log) {
		logList.add(log);
	}	
//log��ȯ
	public List<String> getLogList()
	{
		return logList;
	}
//inst��ȯ
	public List<String> getInstList()
	{
		return instList;
	}
//Ÿ���ּ� ��ȯ(�������ּ�)
	public int getTargetAddr()
	{
		return targetAddr; 
	}
//���� ����̽�
	public String getDevice()
	{
		return currentDevice;
	}

}
