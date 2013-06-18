## NegDep
import os

class NegDep():

    def __init__(self, trigger_filepath, test_filepath):
        self.trigger_filepath = trigger_filepath
        self.test_filepath = test_filepath
        self.trimmed_filepath = "./data/test_trimmed"
        self.parsed_filepath = "./data/parsed_output.txt"

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
                            if trigger in line:
                                tfout.write(line+"\n\n")
                            break

    def parse(self):
        # run stanford parser to produce denpency trees
        cmd = "/Users/siyuanguo/Documents/nlptool/gdep-beta2/gdep "+self.trimmed_filepath+" > "+self.parsed_filepath
        print "start parsing ..."
        os.system(cmd)

    def run(self):
        self.read_NegTriggers()
        self.read_TestFile()
        self.parse()
        
## test run
trigger_filepath = "./data/negTriggers.txt"
test_filepath = "/Users/siyuanguo/GoogleDrive/Corpora/NegationDetectionCorpora/cleaned/PlainTextNoTag/bioscope_abstracts_cleaned.txt"
bioscope = NegDep(trigger_filepath, test_filepath)
bioscope.run()
