## NegDep
import os
import re

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
                            pattern = re.compile(r'\b'+trigger+r'\b')
                            if pattern.search(line):
                                tfout.write(line+"\n\n")
                                break
                        ## something wrong here, write some sents containing no negation cues

    def parse(self):
        # run stanford parser to produce denpency trees
        cmd = "./stanford-parser-2013-04-05/lexparser.sh "+self.trimmed_filepath+" > "+self.parsed_filepath
        print "start parsing ..."
        os.system(cmd)

    def visualize_ParsedTree(self, i_start, i_end):
        # visualize the parsed output using DependenSee, one sent by one sent
        # save the visualization into .png
        # !!Use carefully!! DependenSee has minor bugs (cannot read some parsed tree correctly)
        print "starting visualizing parsed trees ..."
        fout_tmp = open("./data/parsed_output.tmp","w")
        i_sent = 1
        with open(self.parsed_filepath,'r') as fin:
            for line in fin:
                line = line.strip()
                if line:
                    fout_tmp.write(line+"\n")
                else:
                    fout_tmp.close()
                    if i_sent >= i_start and i_sent <= i_end:
                        # call DependenSee to visualize the parsed tree
                        cmd = "java -cp ./stanford-parser-2013-04-05/DependenSee.jar:./stanford-parser-2013-04-05/stanford-parser.jar:./stanford-parser-2013-04-05/stanford-parser-2.0.5-models.jar com.chaoticity.dependensee.Main -t ./data/parsed_output.tmp ./data/parsed_output_visualization/"+str(i_sent)+".png"
                        os.system(cmd)
                    fout_tmp = open("./data/parsed_output.tmp","w")
                    i_sent += 1
        fout_tmp.close()

    def run(self):
        self.read_NegTriggers()
        self.read_TestFile()
        self.parse()
        
## test run
trigger_filepath = "./data/negTriggers.txt"
test_filepath = "/Users/siyuanguo/GoogleDrive/Corpora/NegationDetectionCorpora/cleaned/PlainTextNoTag/bioscope_abstracts_cleaned.txt"
bioscope = NegDep(trigger_filepath, test_filepath)
bioscope.run()
bioscope.visualize_ParsedTree(50,100) #visualize parsed sentences with indice from 50 to 100
