
# SyntaxNode

class SyntaxNode:
    noComment = []

    def addComment (self, text):
        try:
            self.__comment.append (text)
        except AttributeError:
            self.__comment = [text]

    def getComment (self):
        try:
            result = self.__comment
        except AttributeError:
            result = self.noComment
        return result

    def getAttribute (self, name):
        try:
            result = getattr (self, name)
        except AttributeError:
            result = None
        return result

