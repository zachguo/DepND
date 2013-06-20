CFLAGS = -O2 -DNDEBUG -Wno-deprecated 
CPP = g++
OBJS = maxent.o tokenize.o bidir.o morph.o chunking.o namedentity.o blmvm.o ksdep.o

gdep: $(OBJS) ksdep.h
	$(CPP) -o gdep $(CFLAGS) $(OBJS)
clean:
	/bin/rm -r -f $(OBJS) gdep *.o *~ *.flc
.cpp.o:
	$(CPP) -c $(CFLAGS) $<
