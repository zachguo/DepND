## NegDep
import os
import re

class DepNeg():

    def __init__(self, trigger_filepath, test_filepath, result_filepath):
        self.trigger_filepath = trigger_filepath
        self.test_filepath = test_filepath
        self.result_filepath = "./data/result.txt"
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
        cmd = "cd /Users/siyuanguo/Documents/nlptool/gdep-beta2/ && ./gdep "+self.trimmed_filepath+" > "+self.parsed_filepath
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

    def addRow(self, row):
        # add one row of CoNLL output into the sentence
        row = row.strip()
        cols = row.split('\t')
        self.indice.append(cols[0])
        self.words.append(cols[1])
        self.POS.append(cols[4])
        self.arc_end.append(cols[6])
        self.dep.append(cols[7])

    def addNegIndex(self, i):
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

    def findNeg(sentwrapper):
        for i in sentwrapper.get_indice():
            if sentwrapper.get_words()[i-1] in self.NegTriggers:
                sentwrapper.addNegIndex(i)

    def getNegScope(sentwrapper):
        self.findNeg(sentwrapper)
        

    def run_DepNegEx(self):
        # 1. read one sentence, find negation trigger words in the sentence;
        # 2. run corresponding rules to determine scope, return the index range for the scope.
        print "start generating scope for negation triggers ..."
        sent_tmp = SENT()
        with open(self.result_filepath,'w') as fout:
            with open(self.parsed_filepath,'r') as fin:
                for line in fin:
                    if line:
                        sent_tmp.addRow(line)
                    else:
                        if not sent_tmp.whether_empty():
                            fout.write(self.getNegScope(sent_tmp)+'\n')
                        sent_tmp = SENT()
        print "all processes finished, please see results at "+self.result_filepath





    
## test run
trigger_filepath = "./data/negTriggers.txt"
test_filepath = "/Users/siyuanguo/GoogleDrive/Corpora/NegationDetectionCorpora/cleaned/PlainTextNoTag/bioscope_abstracts_cleaned.txt"
bioscope = DepNeg(trigger_filepath, test_filepath)
bioscope.run()
