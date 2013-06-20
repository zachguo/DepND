## NegDep
import os
import re

class DepNeg():

    def __init__(self, trigger_filepath, test_filepath, result_filepath="./data/result.txt"):
        self.trigger_filepath = trigger_filepath
        self.test_filepath = test_filepath
        self.result_filepath = result_filepath
        self.trimmed_filepath = "./data/test_trimmed"
        self.parsed_filepath = "./data/parsed"
        self.read_NegTriggers()

    def read_NegTriggers(self):
        print "start reading negation triggers ..."
        # read triggers, one trigger per line
        self.NegTriggers = []
        with open(self.trigger_filepath,'r') as fin:
            for line in fin:
                line = line.strip()
                if line:
                    self.NegTriggers.append(line)
        print "All negation triggers are\n", self.NegTriggers

    def read_TestFile(self):
        print "start reading test file ..."
        # read text file for analysis
        # finding whether a sentence containing negation triggers
        # aggregate all sentences containing negation into a single text file, one sentence per line
        with open(self.trimmed_filepath,'w') as tfout:
            with open(self.test_filepath,'r') as tfin:
                for line in tfin:
                    line = line.strip()
                    if line:
                        for trigger in self.NegTriggers:
                            pattern = re.compile(r'\b'+trigger+r'\b')
                            if pattern.search(line):
                                tfout.write(line+"\n\n")
                                break

    def parse(self):
        # run gDep to produce denpency trees from plain text sentence
        cmd = "./gdep "+self.trimmed_filepath+" > "+self.parsed_filepath
        print "start parsing ... (this may take several minutes, please wait.)"
        os.system(cmd)

    def run_parse(self):
        self.read_TestFile()
        self.parse()

# a wrapper for sentence
class SENT():
    
    def __init__(self):
        self.indice = []
        self.words = []
        self.POS = []
        self.arc_end = [] # another end of the incoming arc, every word has only one incoming arc
        self.dep = []
        self.NegIndice = []

    def add_Row(self, row):
        # add one row of CoNLL output into the sentence
        row = row.strip()
        cols = row.split('\t')
        self.indice.append(int(cols[0]))
        self.words.append(cols[1])
        self.POS.append(cols[4])
        self.arc_end.append(int(cols[6]))
        self.dep.append(cols[7])

    def add_NegIndex(self, i):
        self.NegIndice.append(i)

    def get_indice(self):
        return self.indice

    def get_words(self):
        return self.words

    def get_POS(self):
        return self.POS

    def get_arc_end(self):
        return self.arc_end

    def get_dep(self):
        return self.dep

    def get_NegIndice(self):
        return self.NegIndice

    def whether_empty(self):
        return self.indice == []
    

class DepNegEx(DepNeg):

    def findNeg(self, sentwrapper):
        for i in sentwrapper.get_indice():
            if sentwrapper.get_words()[i-1] in self.NegTriggers:
                sentwrapper.add_NegIndex(i)
        return sentwrapper.get_NegIndice()
                
    def MST(self, i_root, sentwrapper):
        # maximal spanning tree
        indice = []
        openlist = []
        indice.append(i_root)
        openlist.append(i_root)
        while openlist:
            i_now = openlist.pop()
            for j in sentwrapper.get_indice():
                if sentwrapper.get_arc_end()[j-1] == i_now:
                    openlist.append(j)
                    indice.append(j)
        indice.sort()
        return indice

    def indice2result(self, indice, sentwrapper):
        words = sentwrapper.get_words()
        words.insert(indice[0]-1, '<NegScope>')
        words.insert(indice[-1]+1, '</NegScope>')
        return ' '.join(words)

    def getNegScope(self, sentwrapper):
        result = ''
        indice_neg = self.findNeg(sentwrapper)
        tagset_gMST = set(['RB','DT','JJ','CC'])
        tagset_sMST = set(['NN','IN','VB','VBD','VBG','VBN','VBP','VBZ'])
        for i_neg in indice_neg:
            if sentwrapper.get_POS()[i_neg-1] in tagset_sMST:
                i_root = i_neg
                result += self.indice2result(self.MST(i_root,sentwrapper), sentwrapper)+'\n'
            if sentwrapper.get_POS()[i_neg-1] in tagset_gMST:
                i_root = sentwrapper.get_arc_end()[i_neg-1]
                result += self.indice2result(self.MST(i_root,sentwrapper), sentwrapper)+'\n'
        return result

    def run_DepNegEx(self):
        # 1. read one sentence, find negation trigger words in the sentence;
        # 2. run corresponding rules to determine scope, return the index range for the scope.
        print "start generating scope for negation triggers ..."
        sent_tmp = SENT()
        with open(self.result_filepath,'w') as fout:
            with open(self.parsed_filepath,'r') as fin:
                for line in fin:
                    line = line.strip()
                    if line:
                        sent_tmp.add_Row(line)
                    else:
                        if not sent_tmp.whether_empty():
                            fout.write(self.getNegScope(sent_tmp)+'\n')
                        sent_tmp = SENT()
        print "all processes finished, please see results at "+self.result_filepath



## test run
trigger_filepath = "./data/negTriggers.txt"
test_filepath = "./data/test_toy.txt"
#test_filepath = "/Users/siyuanguo/GoogleDrive/Corpora/NegationDetectionCorpora/cleaned/PlainTextNoTag/bioscope_abstracts_cleaned.txt"
toy = DepNegEx(trigger_filepath, test_filepath)
#toy.run_parse()
toy.run_DepNegEx()
