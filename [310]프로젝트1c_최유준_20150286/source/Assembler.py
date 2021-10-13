from InstTable import InstTable
from LiteralTable import LiteralTable
from SymbolTable import SymbolTable
from TokenTable import TokenTable

class Assembler:
    def __init__(self, filename):
        self.instTable = InstTable(filename)
        self.lineList = []
        self.symtabList = []
        self. literaltabList = []
        self.TokenList = []
        self.codeList = []

    def loadInputFile(self, inputFile):
        f = open(inputFile, 'r')
        i = 0
        while True:
            line = f.readline()
            if not line:
                break
            self.lineList.append(line[:-1])
        f.close()

    def pass1(self):
        i = 0
        j = 0
        start = 0
        addr = 0
        lit = ""
        self.TokenList.append(TokenTable(SymbolTable(), LiteralTable(), self.instTable))#section 1
        while i < len(self.lineList):#token parsing
            self.TokenList[0].putToken(self.lineList[i])
            self.TokenList[0].tokenList[i].location = addr
            if i > 1:
                if self.TokenList[0].tokenList[i].location - self.TokenList[0].tokenList[i-1].location >= 0:
                    self.TokenList[0].Listlength = self.TokenList[0].tokenList[i].location
            #location find
            if self.instTable.hash_get(self.TokenList[0].tokenList[i].operator) > 0:#Instruction
                if self.instTable.hash_get(self.TokenList[0].tokenList[i].operator) == 3:
                    if self.TokenList[0].tokenList[i].operator[0] == '+':#format 4
                        addr += 4
                    else:#format 3
                        addr += 3
                elif self.instTable.hash_get(self.TokenList[0].tokenList[i].operator) == 2:#format2
                    addr += 2
            else:#not Instruction
                if self.TokenList[0].tokenList[i].operator == "RESW":
                    tmpadr = int(self.TokenList[0].tokenList[i].operand[0])
                    addr += 3*tmpadr
                elif self.TokenList[0].tokenList[i].operator == "RESB":
                    tmpadr =int( self.TokenList[0].tokenList[i].operand[0])
                    addr += tmpadr
                elif self.TokenList[0].tokenList[i].operator == "BYTE":
                    addr += 1
                elif self.TokenList[0].tokenList[i].operator == "WORD":
                    addr += 3
                elif self.TokenList[0].tokenList[i].operator == "EQU":#equation 처리
                    if self.TokenList[0].tokenList[i].operand[0] != "*":
                        arr = self.TokenList[0].tokenList[i].operand[0].split('-')#ex) BUFEND-BUFFER
                        tmpadr1 = 0
                        tmpadr2 = 0
                        for k in range(len(self.TokenList[0].tokenList)):
                            if self.TokenList[0].tokenList[k].label == arr[0]:
                                tmpadr1 = self.TokenList[0].tokenList[k].location
                            if self.TokenList[0].tokenList[k].label == arr[1]:
                                tmpadr2 = self.TokenList[0].tokenList[k].location
                        self.TokenList[0].tokenList[i].location = tmpadr1-tmpadr2

                elif self.TokenList[0].tokenList[i].operator == "LTORG":
                    addr += 3

            if self.TokenList[0].tokenList[i].operator == "CSECT":#next section stop
                self.TokenList[0].tokenList.remove(self.TokenList[0].tokenList[i])
                break

            i += 1

        for j in range(i):#literal, symbol input
            if self.TokenList[0].tokenList[j].label != "":
                self.TokenList[0].symTab.putSymbol(self.TokenList[0].tokenList[j].label, self.TokenList[0].tokenList[j].location)
            if len(self.TokenList[0].tokenList[j].operand) == 0:
                if self.TokenList[0].tokenList[j].operator == "LTORG":
                    self.TokenList[0].literalTab.putLiteral(lit, self.TokenList[0].tokenList[j].location)
            else:
                if self.TokenList[0].tokenList[j].operand[0][0] == "=":#if operand is literal, save in lit
                    arr1 = self.TokenList[0].tokenList[j].operand[0].split('\'')
                    lit = arr1[1]

        self.symtabList.append(self.TokenList[0].symTab)
        self.literaltabList.append(self.TokenList[0].literalTab)

        j = i
        start = i
        addr = 0
        i = 0
        self.TokenList.append(TokenTable(SymbolTable(), LiteralTable(), self.instTable))
        while j < len(self.lineList):
            if self.lineList[j][0] == '.':
                j += 1
                continue
            self.TokenList[1].putToken(self.lineList[j])

            self.TokenList[1].tokenList[i].location = addr
            if i>1:
                if self.TokenList[1].tokenList[i].location - self.TokenList[1].tokenList[i-1].location >= 0:#section 2
                    self.TokenList[1].Listlength = self.TokenList[1].tokenList[i].location
            if self.instTable.hash_get(self.TokenList[1].tokenList[i].operator) > 0:
                if self.instTable.hash_get(self.TokenList[1].tokenList[i].operator) == 3:
                    if self.TokenList[1].tokenList[i].operator[0] == "+":
                        addr += 4
                    else:
                        addr += 3
                elif self.instTable.hash_get(self.TokenList[1].tokenList[i].operator) == 2:
                    addr += 2
            else:
                if self.TokenList[1].tokenList[i].operator == "RESW":
                    tmpadr = int(self.TokenList[1].tokenList[i].operand[0])
                    addr += 3*tmpadr
                elif self.TokenList[1].tokenList[i].operator == "RESB":
                    tmpadr = int(self.TokenList[1].tokenList[i].operand[0])
                    addr += tmpadr
                elif self.TokenList[1].tokenList[i].operator == "BYTE":
                    addr += 1
                elif self.TokenList[1].tokenList[i].operator == "WORD":
                    addr += 3
                elif self.TokenList[1].tokenList[i].operator == "EQU":
                    if self.TokenList[1].tokenList[i].operand != "*":
                        arr = self.TokenList[1].tokenList[i].operand[0].split("-")
                        tmpadr1 = 0
                        tmpadr2 = 0
                        for k in range(len(self.TokenList[1].tokenList)):
                            if self.TokenList[1].tokenList[k].label == arr[0]:
                                tmpadr1 = self.TokenList[1].tokenList[k].location
                            if self.TokenList[1].tokenList[k].label == arr[1]:
                                tmpadr2 = self.TokenList[1].tokenList[k].location
                        addr = tmpadr1 - tmpadr2

                elif self.TokenList[1].tokenList[i].operator == "LTORG":
                    addr += 3

            if self.TokenList[1].tokenList[i].label == "WRREC":
                self.TokenList[1].tokenList.remove(self.TokenList[1].tokenList[i])
                break
            j += 1
            i += 1

        start = j
        for j in range(i):
            if self.TokenList[1].tokenList[j].label != "":
                self.TokenList[1].symTab.putSymbol(self.TokenList[1].tokenList[j].label, self.TokenList[1].tokenList[j].location)

        self.symtabList.append(self.TokenList[1].symTab)
        self.literaltabList.append(self.TokenList[1].literalTab)

        j = start
        addr = 0
        i = 0
        self.TokenList.append(TokenTable(SymbolTable(), LiteralTable(), self.instTable))#section 3
        while(j < len(self.lineList)):
            if self.lineList[j][0] == '.':
                j += 1
                continue
            self.TokenList[2].putToken(self.lineList[j])

            self.TokenList[2].tokenList[i].location = addr
            if i>1:
                if self.TokenList[2].tokenList[i].location - self.TokenList[2].tokenList[i-1].location >= 0:
                    self.TokenList[2].Listlength = self.TokenList[2].tokenList[i].location
            if self.instTable.hash_get(self.TokenList[2].tokenList[i].operator) > 0:
                if self.instTable.hash_get(self.TokenList[2].tokenList[i].operator) == 3:
                    if self.TokenList[2].tokenList[i].operator[0] == "+":
                        addr += 4
                    else:
                        addr += 3
                elif self.instTable.hash_get(self.TokenList[2].tokenList[i].operator) == 2:
                    addr += 2
            else:
                if self.TokenList[2].tokenList[i].operator == "RESW":
                    tmpadr = int(self.TokenList[2].tokenList[i].operand[0])
                    addr += 3 * tmpadr
                elif self.TokenList[2].tokenList[i].operator == "RESB":
                    tmpadr = int(self.TokenList[2].tokenList[i].operand[0])
                    addr += tmpadr
                elif self.TokenList[2].tokenList[i].operator == "BYTE":
                    addr += 1
                elif self.TokenList[2].tokenList[i].operator == "WORD":
                    addr += 3
                elif self.TokenList[2].tokenList[i].operator == "EQU":
                    if self.TokenList[2].tokenList[i].operand != "*":
                        arr = self.TokenList[2].tokenList[i].operand[0].split("-")
                        tmpadr1 = 0
                        tmpadr2 = 0
                        for k in range(len(self.TokenList[2].tokenList)):
                            if self.TokenList[2].tokenList[k].label == arr[0]:
                                tmpadr1 = self.TokenList[2].tokenList[k].location
                            if self.TokenList[2].tokenList[k].label == arr[1]:
                                tmpadr2 = self.TokenList[2].tokenList[k].location
                                addr = tmpadr1 - tmpadr2

                elif self.TokenList[2].tokenList[i].operator == "LTORG":
                    addr += 3
            if self.TokenList[2].tokenList[i].operator == "END":
                self.TokenList[2].Listlength += 1
                break
            j += 1
            i += 1

        for j in range(i+1):
            if self.TokenList[2].tokenList[j].label != "":
                self.TokenList[2].symTab.putSymbol(self.TokenList[2].tokenList[j].label, self.TokenList[2].tokenList[j].location)
            if len(self.TokenList[2].tokenList[j].operand) > 0:
                if self.TokenList[2].tokenList[j].operator != "RSUB":
                    if self.TokenList[2].tokenList[j].operand[0][0] == '=':
                        arr = self.TokenList[2].tokenList[j].operand[0].split("'")
                        lit = arr[1]
                if self.TokenList[2].tokenList[j].operator == "END":
                    self.TokenList[2].literalTab.putLiteral(lit, self.TokenList[2].tokenList[j].location)

        self.symtabList.append(self.TokenList[2].symTab)
        self.literaltabList.append(self.TokenList[2].literalTab)

    def printSymbolTable(self, filename):
        f = open(filename, 'w')
        for i in range(len(self.symtabList[0].symbolList)):
            line = str.format("%s\t%X\n" % (self.symtabList[0].symbolList[i], self.symtabList[0].locationList[i]))
            f.write(line)
        f.write("\n")
        for i in range(len(self.symtabList[1].symbolList)):
            line = str.format("%s\t%X\n" % (self.symtabList[1].symbolList[i], self.symtabList[1].locationList[i]))
            f.write(line)
        f.write("\n")
        for i in range(len(self.symtabList[2].symbolList)):
            line = str.format("%s\t%X\n" % (self.symtabList[2].symbolList[i], self.symtabList[2].locationList[i]))
            f.write(line)
        f.close()

    def printLiteralTable(self, filename):
        f = open(filename, 'w')
        for i in range(len(self.literaltabList[0].literalList)):
            line = str.format("%s\t%X\n" % (self.literaltabList[0].literalList[i], self.literaltabList[0].locationList[i]))
            f.write(line)
        f.write("\n")
        for i in range(len(self.literaltabList[2].literalList)):
            line = str.format("%s\t%X\n" % (self.literaltabList[2].literalList[i], self.literaltabList[2].locationList[i]))
            f.write(line)

    def pass2(self):
        for i in range(len(self.TokenList[0].tokenList)):#section 1
            self.TokenList[0].makeObjectCode(i)
            self.codeList.append(self.TokenList[0].getObjectCode(i))
        for i in range(len(self.TokenList[1].tokenList)):#section 2
            self.TokenList[1].makeObjectCode(i)
            self.codeList.append(self.TokenList[1].getObjectCode(i))
        for i in range(len(self.TokenList[2].tokenList)):#section 3
            self.TokenList[2].makeObjectCode(i)
            self.codeList.append(self.TokenList[2].getObjectCode(i))

    def printObjectCode(self, filename):
        f = open(filename, 'w')
        start = 0
        i = 0
        for k in range(len(self.TokenList)):
            for i in range(len(self.TokenList[k].tokenList)):#for Starting point, EXTDEF, EXTREF
                if self.codeList[start+i] == '':
                    if self.TokenList[k].tokenList[i].operator == "START" or self.TokenList[k].tokenList[i].operator == "CSECT":
                        str1 = "H"+self.TokenList[k].tokenList[i].label+"\t000000"
                        str2 = str.format("%06X" % self.TokenList[k].Listlength)
                        f.write(str1+str2)
                    elif self.TokenList[k].tokenList[i].operator == "EXTDEF":
                        f.write("D")
                        for j in range(len(self.TokenList[k].tokenList[i].operand)):
                            str1 = self.TokenList[k].tokenList[i].operand[j]
                            f.write(str1+str.format("%06X" % self.TokenList[k].symTab.search(str1)))
                    elif self.TokenList[k].tokenList[i].operator == "EXTREF":
                        f.write("R")
                        for j in range(len(self.TokenList[k].tokenList[i].operand)):
                            f.write(self.TokenList[k].tokenList[i].operand[j]+ " ")
                    f.write("\n")
                else:
                    break

            startadr = 0
            txtlen = 0
            j = i
            txt = 0
            while txtlen <= 30:#first T line
                if txtlen+self.TokenList[k].tokenList[j].byteSize <= 30:
                    if self.codeList[start+j] == "05":
                        txtlen += 1
                        break
                    txtlen += self.TokenList[k].tokenList[j].byteSize
                else:
                    break
                j += 1

            f.write("T"+str.format("%06X%02X" % (startadr, txtlen)))
            for i in range(i, len(self.TokenList[k].tokenList)):#write T codeList
                f.write(self.codeList[start+i])
                txt += self.TokenList[k].tokenList[i].byteSize
                if txt == txtlen:
                    break
            i = j
            startadr = txtlen
            txtlen = 0
            txt = 0
            while txtlen <= 30:#second T line
                if k == 2:
                    break
                if txtlen + self.TokenList[k].tokenList[j].byteSize <= 30:
                    if self.codeList[start+j+1] == '':
                        break
                    txtlen += self.TokenList[k].tokenList[j].byteSize
                else:
                    break
                j += 1
            if k != 2:
                f.write("\nT"+str.format("%06X%02X" % (startadr, txtlen)))#write second T codeList
            for i in range(i, len(self.TokenList[k].tokenList)):
                f.write(self.codeList[start+i])
                txt += self.TokenList[k].tokenList[i].byteSize
                if i == j:
                    break
            for i in range(i, len(self.TokenList[k].tokenList)):#for literal
                if self.TokenList[k].tokenList[i].operator == "LTORG":
                    f.write("\nT"+str.format("%06X" % self.TokenList[k].literalTab.locationList[0])+str.format("%02X" % self.TokenList[k].tokenList[i].byteSize)+str.format("%X%X%X" % (ord(self.TokenList[k].literalTab.literalList[0][0]), ord(self.TokenList[k].literalTab.literalList[0][1]), ord(self.TokenList[k].literalTab.literalList[0][2]))))

            for cnt in range(3, len(self.TokenList[k].tokenList)):#for modify
                if len(self.TokenList[k].tokenList[cnt].operand) == 0:
                    cnt += 1
                    continue
                if k == 0:
                    if "RDREC" in self.TokenList[k].tokenList[cnt].operand[0] or "WRREC" in self.TokenList[k].tokenList[cnt].operand[0]:
                        str1 = str.format("%06X" % (self.TokenList[k].tokenList[cnt].location+1))
                        f.write("\nM"+str1+"05+"+self.TokenList[k].tokenList[cnt].operand[0])
                elif k == 1:
                    if "BUFEND" in self.TokenList[k].tokenList[cnt].operand[0] or "BUFFER" in self.TokenList[k].tokenList[cnt].operand[0] or "LENGTH" in self.TokenList[k].tokenList[cnt].operand[0]:
                        if self.TokenList[k].tokenList[cnt].operand[0] == "BUFEND-BUFFER":
                            f.write("\nM"+str.format("%06X" % (self.TokenList[k].tokenList[cnt].location))+"06+BUFEND")
                            f.write("\nM"+str.format("%06X" % (self.TokenList[k].tokenList[cnt].location))+"06+BUFFER")
                        else:
                            f.write("\nM"+str.format("%06X" % (self.TokenList[k].tokenList[cnt].location+1))+"05+"+self.TokenList[k].tokenList[cnt].operand[0])

                elif k == 2:
                    if "BUFEND" in self.TokenList[k].tokenList[cnt].operand[0] or "BUFFER" in self.TokenList[k].tokenList[cnt].operand[0] or "LENGTH" in self.TokenList[k].tokenList[cnt].operand[0]:
                        f.write("\nM"+str.format("%06X" % (self.TokenList[k].tokenList[cnt].location+1))+"05+"+self.TokenList[k].tokenList[cnt].operand[0])
            f.write("\nE")
            if k == 0:
                f.write("000000")
            start += (i+1)
            f.write("\n\n")





assembler = Assembler("inst.data")
assembler.loadInputFile("input.txt")
assembler.pass1()
assembler.printSymbolTable("symtab_20150286.txt")
assembler.printLiteralTable("literaltab_20150286.txt")
assembler.pass2()
assembler.printObjectCode("output_20150286.txt")

