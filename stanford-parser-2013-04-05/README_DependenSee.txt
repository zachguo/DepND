# Readme for DependenSee
# In *nix machines, using following command
# otherwise, change colon into semicolon 

*make sure that input text only contains dependency output file, do not include PCFG output file, and one sentence at a time*

java -cp DependenSee.jar:stanford-parser.jar:stanford-parser-2.0.5-models.jar com.chaoticity.dependensee.Main "Example isn't another way to teach, it is the only way to teach." out.png

java -cp DependenSee.jar:stanford-parser.jar:stanford-parser-2.0.5-models.jar com.chaoticity.dependensee.Main -t input.txt out.png
