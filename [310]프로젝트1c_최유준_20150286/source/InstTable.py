

class InstTable:
    def __init__(self, filename):
        self.instMap = {}#instMap dictionary
        f = open(filename, 'r')
        while True:
            line = f.readline()
            if not line: break
            arr = line.split('\t')
            inst = Instruction(arr[1])
            self.instMap[arr[0]] = inst

        f.close()

    def hash_get(self, line):#if line is instruction, return format of line
        if line[0] == '+':
            newline = line[1:]
            return self.instMap[newline].format
        else:
            if line in self.instMap.keys():
                return self.instMap[line].format
            else:
                return -1


class Instruction:
    def __init__(self, line):
        self.format = 0
        self.opcode = ""
        self.numberOfOperand = 0
        self.parsing(line)

    def parsing(self, line):
        arr = line.split(",")
        self.format = int(arr[0])
        self.opcode = arr[1]
        self.numberOfOperand = int(arr[2])

