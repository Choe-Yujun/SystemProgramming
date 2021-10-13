from LiteralTable import LiteralTable
from SymbolTable import SymbolTable
from InstTable import InstTable

class TokenTable:
    def __init__(self, symTab, literalTab, instTab):
        self.nFlag = 32
        self.iFlag = 16
        self.xFlag = 8
        self.bFlag = 4
        self.pFlag = 2
        self.eFlag = 1
        self.tokenList = []
        self.symTab = symTab
        self.literalTab = literalTab
        self.instTab = instTab
        self.Listlength = 0

    def putToken(self, line):
        self.tokenList.append(Token(line))

    def getToken(self, index):
        return self.tokenList[index]

    def makeObjectCode(self, index):
        objectcode = 0#type int objectcode
        opcode = 0#type int opcode
        address = 0
        if self.instTab.hash_get(self.tokenList[index].operator) > 0:#Instruction
            if self.tokenList[index].operator[0] == '+':#format 4
                newline = self.tokenList[index].operator[1:]
                opcode = int(self.instTab.instMap[newline].opcode, 16)
            else:#format 3
                opcode = int(self.instTab.instMap[self.tokenList[index].operator].opcode, 16)
            if self.instTab.hash_get(self.tokenList[index].operator) == 3:#format 3, 4
                self.tokenList[index].setFlag(self.nFlag, 1)#basically n, i, p = 1
                self.tokenList[index].setFlag(self.iFlag, 1)
                self.tokenList[index].setFlag(self.pFlag, 1)
                self.tokenList[index].byteSize = 3
                if self.tokenList[index].operator[0] == '+':#format4 p = 0, e = 1
                    self.tokenList[index].setFlag(self.pFlag, 0)
                    self.tokenList[index].setFlag(self.eFlag, 1)
                    self.tokenList[index].byteSize = 4
                if self.tokenList[index].operator == "RSUB":#RSUB exception
                    self.tokenList[index].setFlag(self.pFlag, 0)
                    objectcode = opcode * 65536 + self.tokenList[index].nixbpe * 4096
                    self.tokenList[index].objectCode = hex(objectcode)[2:].upper()
                    return
                if self.tokenList[index].operand[0][0] == '#':#immediate
                    self.tokenList[index].setFlag(self.pFlag, 0)
                    self.tokenList[index].setFlag(self.nFlag, 0)
                elif self.tokenList[index].operand[0][0] == '@':#indirect
                    self.tokenList[index].setFlag(self.iFlag, 0)
                if len(self.tokenList[index].operand) == 2 and self.tokenList[index].operand[1] == "X":
                    self.tokenList[index].setFlag(self.xFlag, 1)

                if self.tokenList[index].operand[0][0] == '#' or self.tokenList[index].operand[0][0] == '@':
                    str1 = self.tokenList[index].operand[0][1:]
                else:
                    str1 = self.tokenList[index].operand[0]

                check = -1
                for i in range(len(self.symTab.symbolList)):
                    if str1 == self.symTab.symbolList[i]:
                        check = i#if it is same section symbol, check = i else, check = -1
                        break
                if check != -1:#symbol
                    address = self.symTab.locationList[i] - self.tokenList[index+1].location
                    if address < 0:
                        address = 4096 + self.symTab.locationList[i] - self.tokenList[index+1].location
                else:#immediate number
                    if str1[0] >= '0' and str1[0] <= '9':
                        address = int(str1, 16)
                    else:
                        if self.tokenList[index].operand[0][0] == '=':#literal
                            address = self.literalTab.locationList[0] - self.tokenList[index+1].location
                        else:#another section symbol
                            address = 0
                if self.tokenList[index].byteSize == 3:
                    objectcode = opcode * 65536 + self.tokenList[index].nixbpe * 4096 + address
                elif self.tokenList[index].byteSize == 4:
                    objectcode = opcode * 16777216 + self.tokenList[index].nixbpe * 1048576 + address
            else:#format 2
                self.tokenList[index].byteSize = 2
                if len(self.tokenList[index].operand) >= 1:
                    if self.tokenList[index].operand[0] == "X":
                        address = 1
                    elif self.tokenList[index].operand[0] == "A":
                        address = 0
                    elif self.tokenList[index].operand[0] == "S":
                        address = 4
                    elif self.tokenList[index].operand[0] == "T":
                        address = 5
                    objectcode = opcode * 256 + address * 16
                if len(self.tokenList[index].operand) == 2:
                    if self.tokenList[index].operand[1] == "X":
                        address = 1
                    elif self.tokenList[index].operand[1] == "A":
                        address = 0
                    elif self.tokenList[index].operand[1] == "S":
                        address = 4
                    elif self.tokenList[index].operand[1] == "T":
                        address = 5
                    objectcode = objectcode + address

            self.tokenList[index].objectCode = hex(objectcode)[2:].upper()
            if len(self.tokenList[index].objectCode)==5:#if first number is 0, put it in
                self.tokenList[index].objectCode = "0"+self.tokenList[index].objectCode

        else:#not instruction
            if self.tokenList[index].operator == "LTORG":
                self.tokenList[index].objectCode = self.literalTab.literalList[0]
                self.tokenList[index].byteSize = 3
            elif self.tokenList[index].operator == "BYTE":
                arr = self.tokenList[index].operand[0].split('\'')
                self.tokenList[index].objectCode = arr[1]
                self.tokenList[index].byteSize = 1
            elif self.tokenList[index].operator == "WORD":
                self.tokenList[index].objectCode = "000000"
                self.tokenList[index].byteSize = 3
            elif self.tokenList[index].operator == "END":
                self.tokenList[index].objectCode = self.literalTab.literalList[0]
                self.tokenList[index].byteSize = 1


    def getObjectCode(self, index):
        return self.tokenList[index].objectCode


class Token:
    def __init__(self, line):
        self.location = 0
        self.label = ""
        self.operator = ""
        self.operand = []
        self.comment = ""
        self.nixbpe = 0
        self.objectCode = ""
        self.byteSize = 0
        self.parsing(line)

    def parsing(self, line):
        arr = line.split('\t')
        self.label = arr[0]
        self.operator = arr[1]
        if len(arr) > 3:
            self.comment = arr[3]
        if len(arr) > 2:
            op = arr[2].split(",")
            for i in range(len(op)):
                self.operand.append(op[i])

    def setFlag(self, flag, value):
        if value == 1:
            self.nixbpe += flag
        elif value == 0:
            self.nixbpe -= flag

    '''def getFlag(self, flag):
        return self.nixbpe & flag'''