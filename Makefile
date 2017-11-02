all: test

CFLAG= -maes -msse2 -msse4.2 -march=corei7-avx `libgcrypt-config --cflags --libs`  -lm -O3 -I src
JIMUFile=client.c server.c util.c OTExtension.c PermutationOperation.c WireLabelOperation.c RandomMatrix.c RSEncoding.c galois.c IHash.c BatchGateGeneration.c BatchBucketProcess.c InputProcess.c BatchMuxGeneration.c BatchMuxBucketProcess.c
JIMUHeader=aes.h block.h client.h server.h util.h OTExtension.h PRG.h OTExtension.h PermutationOperation.h WireLabelOperation.h RandomMatrix.h RSEncoding.h galois.h IHash.h BatchGateGeneration.h BatchBucketProcess.h InputProcess.h BatchMuxGeneration.h BatchMuxBucketProcess.h

test: testG.out testE.out
	
testG.out:	$(addprefix src/, $(JIMUFile)) $(addprefix src/, $(JIMUHeader)) test/test.c
	gcc  $(addprefix src/, $(JIMUFile))  test/test.c -o testG.out $(CFLAG) -D ALICE

testE.out:	$(addprefix src/, $(JIMUFile)) $(addprefix src/, $(JIMUHeader)) test/test.c
	gcc  $(addprefix src/, $(JIMUFile))  test/test.c -o testE.out $(CFLAG) -D BOB

pool: poolG.o poolE.o

poolG.o:	$(addprefix src/, $(JIMUFile)) $(addprefix src/, $(JIMUHeader)) PoolWarpper/PoolWarpper.h
	gcc  $(addprefix src/, $(JIMUFile))  PoolWarpper/PoolWarpper.h -c -o poolG.o $(CFLAG) -D ALICE

poolE.o:	$(addprefix src/, $(JIMUFile)) $(addprefix src/, $(JIMUHeader)) PoolWarpper/PoolWarpper.h
	gcc  $(addprefix src/, $(JIMUFile))  PoolWarpper/PoolWarpper.h -c -o poolE.o $(CFLAG) -D BOB

clean: 
	rm -f *.out 
