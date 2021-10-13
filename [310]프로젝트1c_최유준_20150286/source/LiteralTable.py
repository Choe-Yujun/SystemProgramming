
class LiteralTable:
    def __init__(self):
        self.literalList = []
        self.locationList = []

    def putLiteral(self, literal, location):
        self.literalList.append(literal)
        self.locationList.append(location)

    def modifyLiteral(self, literal, newlocation):
        for i in range(len(self.literalList)):
            if self.literalList[i] == literal:
                self.locationList[i] = newlocation
                break

    def search(self, literal):
        for i in range(len(self.literalList)):
            if self.literalList[i] == literal:
                return self.locationList[i]
        return -1
