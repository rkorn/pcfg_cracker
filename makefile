CC=g++
HD=.\/hash_src\/
CFLAG =
main: checkPass pcfg_manager

checkPass: checkPass.o hl_md5.o hl_md5wrapper.o hl_sha1.o hl_sha1wrapper.o
	$(CC) $(CFLAG) checkPass.o -o checkPass hl_md5.o hl_md5wrapper.o hl_sha1.o hl_sha1wrapper.o

checkPass.o: checkPass.cpp
	$(CC) $(CFLAG) -c  checkPass.cpp

hl_md5.o: $(HD)hl_md5.cpp $(HD)hl_md5.h
	$(CC) -c $(HD)hl_md5.cpp

hl_md5wrapper.o: $(HD)hl_md5wrapper.cpp $(HD)hl_md5wrapper.h
	$(CC) -c $(HD)hl_md5wrapper.cpp

hl_sha1.o: $(HD)hl_sha1.cpp $(HD)hl_sha1.h
	$(CC) -c $(HD)hl_sha1.cpp

hl_sha1wrapper.o: $(HD)hl_sha1wrapper.cpp $(HD)hl_sha1wrapper.h
	$(CC) -c $(HD)hl_sha1wrapper.cpp
	
pcfg_manager: pcfg_manager.o tty.o brown_grammar.o standard_grammar.o
	$(CC) $(CFLAG) pcfg_manager.o tty.o brown_grammar.o standard_grammar.o -O3 -o pcfg_manager
	
pcfg_manager.o: pcfg_manager.cpp
	$(CC) $(CFLAG) -c pcfg_manager.cpp

tty.o: tty.c
	$(CC) $(CFLAG) -c tty.c

brown_grammar.o: brown_grammar.cpp
	$(CC) $(CFLAG) -c brown_grammar.cpp

standard_grammar.o: standard_grammar.cpp
	$(CC) $(CFLAG) -c standard_grammar.cpp

clean:
	rm -f pcfg_manager 	
	rm -f checkPass 
	rm -f *.o
