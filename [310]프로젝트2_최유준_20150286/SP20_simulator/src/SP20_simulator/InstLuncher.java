package SP20_simulator;

// instruction에 따라 동작을 수행하는 메소드를 정의하는 클래스

public class InstLuncher {
    ResourceManager rMgr;
    int targetAddr;
    SicSimulator sim;
    char [] instruction;
    public InstLuncher(ResourceManager resourceManager) {
        this.rMgr = resourceManager;
        instruction = new char[1];
        
    }


    // instruction 별로 동작을 수행하는 메소드를 정의
    // ex) public void add(){...}

    public char[] STL(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
			
			char[] data = new char[3];
			rMgr.setMemory(targetAddr, data, 3);
			data = rMgr.intToChar(rMgr.getRegister(SicSimulator.L_REGISTER));
			rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
			targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            				
            if(pcRelative)
            	targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] data = new char[3];
    		rMgr.setMemory(targetAddr, data, 3);
			data = rMgr.intToChar(rMgr.getRegister(SicSimulator.L_REGISTER));
			rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        return instruction;
    }

    public char[] JSUB(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
					
			rMgr.setRegister(SicSimulator.L_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER));
			rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
			rMgr.setCurrentSection();
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
			targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
					
			rMgr.setRegister(SicSimulator.L_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER));
					
			if(pcRelative)
				targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
			rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
			rMgr.setCurrentSection();
        }
        return instruction;
    }

    public char[] LDA(int opcode, boolean extForm, boolean pcRelative, boolean immediate) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);

			char[] data = rMgr.getMemory(targetAddr, 3);
			rMgr.setRegister(SicSimulator.A_REGISTER, rMgr.byteToInt(data));	
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);

            if(pcRelative) // PC relative를 사용하는 경우,
            {
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            
                char[] data = rMgr.getMemory(targetAddr, 3);
                rMgr.setRegister(SicSimulator.A_REGISTER, rMgr.byteToInt(data));
            }
            else if(immediate)  // immediate를 사용하는 경우,
			{
				rMgr.setRegister(SicSimulator.A_REGISTER, targetAddr);
			}
        }
        return instruction;
    }

    public char[] COMP(int opcode, boolean extForm, boolean immediate, int difference) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
					
			if(immediate) {
				difference = rMgr.getRegister(SicSimulator.A_REGISTER) - targetAddr;
				rMgr.setRegister(SicSimulator.SW_REGISTER, difference);
				System.out.println("SW: " + rMgr.getRegister(SicSimulator.SW_REGISTER));
			}
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
			targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);

			if(immediate)
			{
				difference = rMgr.getRegister(SicSimulator.A_REGISTER) - targetAddr;
				rMgr.setRegister(SicSimulator.SW_REGISTER, difference);
				System.out.println("SW: " + rMgr.getRegister(SicSimulator.SW_REGISTER));
			}
        }
        return instruction;
    }

    public char[] RSUB(int opcode) {
        instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
		rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.L_REGISTER));
                
        return instruction;
    }

    public char[] LDCH(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
					
			char[] data = rMgr.getMemory(targetAddr + rMgr.getRegister(SicSimulator.X_REGISTER), 1);
			rMgr.setRegister(SicSimulator.A_REGISTER, rMgr.byteToInt(data));
			System.out.println((char)rMgr.getRegister(SicSimulator.A_REGISTER));
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if(pcRelative)  // PC relative를 사용하는 경우,
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] data = rMgr.getMemory(targetAddr + rMgr.getRegister(SicSimulator.X_REGISTER), 1);
            rMgr.setRegister(SicSimulator.A_REGISTER, rMgr.byteToInt(data));
            System.out.println((char)rMgr.getRegister(SicSimulator.A_REGISTER));
        }
        return instruction;
    }
    
    public char[] WD(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
			targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
			rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
					
			char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
			String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
			System.out.println(deviceName);
			rMgr.writeDevice(deviceName);
			System.out.print(Integer.toBinaryString(rMgr.getRegister(SicSimulator.A_REGISTER)));
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] deviceInfo = rMgr.getMemory(targetAddr, 1);
            String deviceName = String.format("%X%X", deviceInfo[0] >> 8, deviceInfo[0] & 15);
            System.out.println(deviceName);
            rMgr.writeDevice(deviceName);
            System.out.print(Integer.toBinaryString(rMgr.getRegister(SicSimulator.A_REGISTER)));
        }
        return instruction;
    }

    public char[] J(int opcode, boolean extForm, boolean pcRelative, boolean indirect, boolean immediate) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            if((instruction[1] & 15) == 15)  // 음수인 경우(상위 8비트가 F인 경우)
                targetAddr += (0xFFF << 20); // 상위 나머지 비트들도 F로 채워 음수값을 만든다.
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
            
            rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            rMgr.setCurrentSection();
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
            if((instruction[1] & 15) == 15)   // 음수인 경우(상위 8비트가 F인 경우)
                targetAddr += (0xFFFFF << 12);  // 상위 나머지 비트들도 F로 채워 음수값을 만든다.
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);

            if(pcRelative)
            {
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
                
                if(indirect & !immediate)
                {
                    targetAddr = rMgr.byteToInt(rMgr.getMemory(targetAddr, 3));
                }
                
                rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            }
            rMgr.setCurrentSection();
        }
        return instruction;
    }

    public char[] STA(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
                
            char[] data = new char[3];
            rMgr.setMemory(targetAddr, data, 3);
            data = rMgr.intToChar(rMgr.getRegister(SicSimulator.A_REGISTER));
            rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
                
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] data = new char[3];
            rMgr.setMemory(targetAddr, data, 3);
            data = rMgr.intToChar(rMgr.getRegister(SicSimulator.A_REGISTER));
            rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        return instruction;
    }
    
    public char[] CLEAR(int opcode) {
        instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 2);
        rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 2);

        return instruction;
    }

    public char[] LDT(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);

            char[] data = rMgr.getMemory(targetAddr, 3);
            rMgr.setRegister(SicSimulator.T_REGISTER, rMgr.byteToInt(data));		
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);

            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] data = rMgr.getMemory(targetAddr, 3);
            rMgr.setRegister(SicSimulator.T_REGISTER, rMgr.byteToInt(data));
        }
        return instruction;
    }

    public char[] TD(int opcode, boolean extForm) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
        }
            
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
        }
        return instruction;
    }
    
   /* public char[] RD(int opcode, boolean extForm) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
        }   
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
        }
        return instruction;
    }*/

   /* public char[] COMPR(int opcode) {
        instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 2);
        rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 2);
        
        return instruction;
    }*/

    public char[] STCH(int opcode, boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
            
            char [] data = rMgr.intToChar(rMgr.getRegister(SicSimulator.A_REGISTER) & 255);
            rMgr.setMemory(targetAddr + rMgr.getRegister(SicSimulator.X_REGISTER), data, 1);
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char [] data = rMgr.intToChar(rMgr.getRegister(SicSimulator.A_REGISTER) & 255);
            rMgr.setMemory(targetAddr + rMgr.getRegister(SicSimulator.X_REGISTER), data, 1);
        }
        return instruction;
    }

    public char[] TIXR() {
        instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 2);
        rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 2);

        return instruction;
    }

    public char[] JLT(boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
            
            if((instruction[1] & 15) == 15)
                targetAddr += (0xFFF << 20);
            
            if(rMgr.getRegister(SicSimulator.SW_REGISTER) < 0)
            {
                rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            }
            rMgr.setCurrentSection();
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if((instruction[1] & 15) == 15)
                targetAddr += (0xFFFFF << 12);
            
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            
            if(rMgr.getRegister(SicSimulator.SW_REGISTER) < 0)
            {
                rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            }
            rMgr.setCurrentSection();
        }
        return instruction;
    }

    public char[] STX(boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);

            char[] data = new char[3];
            rMgr.setMemory(targetAddr, data, 3);
            data = rMgr.intToChar(rMgr.getRegister(SicSimulator.X_REGISTER));
            rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            char[] data = new char[3];
            rMgr.setMemory(targetAddr, data, 3);
            data = rMgr.intToChar(rMgr.getRegister(SicSimulator.X_REGISTER));
            rMgr.modifMemory(targetAddr + (3 - data.length), data, data.length, '+');
        }
        return instruction;
    }

    public char[] JEQ(boolean extForm, boolean pcRelative) {
        if(extForm) {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 4);
            targetAddr = ((instruction[1] & 15) << 16) + ((instruction[2] >>> 8) << 12) + ((instruction[2] & 15) << 8) + ((instruction[3] >>> 8) << 4) + (instruction[3] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 4);
            
            if((instruction[1] & 15) == 15)
                targetAddr += (0xFFF << 20);
            
            if(rMgr.getRegister(SicSimulator.SW_REGISTER) == 0)
            {
                rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            }
            rMgr.setCurrentSection();
        }
        else {
            instruction = rMgr.getMemory(rMgr.getRegister(SicSimulator.PC_REGISTER), 3);
            targetAddr = ((instruction[1] & 15) << 8) + ((instruction[2] >>> 8) << 4) + (instruction[2] & 15);
            rMgr.setRegister(SicSimulator.PC_REGISTER, rMgr.getRegister(SicSimulator.PC_REGISTER) + 3);
            
            if((instruction[1] & 15) == 15)
                targetAddr += (0xFFFFF << 12);
            if(pcRelative)
                targetAddr += rMgr.getRegister(SicSimulator.PC_REGISTER);
            if(rMgr.getRegister(SicSimulator.SW_REGISTER) == 0)
            {
                rMgr.setRegister(SicSimulator.PC_REGISTER, targetAddr);
            }
            rMgr.setCurrentSection();
        }
        return instruction;
    }



}