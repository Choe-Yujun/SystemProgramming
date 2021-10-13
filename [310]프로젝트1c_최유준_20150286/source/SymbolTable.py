
class SymbolTable:
    def __init__(self):
        self.symbolList = []
        self.locationList = []

    def putSymbol(self, symbol, location):
        self.symbolList.append(symbol)
        self.locationList.append(location)

    def modifySymbol(self, symbol, newlocation):
        for i in range(len(self.symbolList)):
            if self.symbolList[i] == symbol:
                self.locationList[i] = newlocation
                break
        return -1

    def search(self, symbol):
        for i in range(len(self.symbolList)):
            if self.symbolList[i] == symbol:
                return self.locationList[i]
        return -1
