## find negation triggers

with open("/Users/siyuanguo/GoogleDrive/Megaputer/NegDetection/DepNegEx/data/negex_triggers_raw.txt",'r') as fin:
    with open("/Users/siyuanguo/GoogleDrive/Megaputer/NegDetection/DepNegEx/data/negex_triggers_trimmed.txt", "w") as fout:
        for line in fin:
            if "[PREN]" in line:
                fout.write(line)
