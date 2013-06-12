## NegDep

# read triggers
NegTriggers = []
with open("/Users/siyuanguo/GoogleDrive/Megaputer/NegDetection/DepNegEx/data/negTriggers.txt",'r') as fin:
    for line in fin:
        NegTriggers.append(line.strip())
print "All negation triggers are\n", NegTriggers


# read text file for analysis
# finding whether a sentence containing negation triggers
# aggregate all sentences containing negation into a single text file, one sentence per line
# parse this text file

import os
filepath = "./data/test.txt"
# parse function
def parse(filepath):
    cmd = "./stanford-parser-2013-04-05/lexparser.sh "+filepath+" > ./data/parsed_output.txt"
    print "start parsing ..."
    os.system(cmd)
parse(filepath)    
