a.out: file.o hl_sha1.o hl_sha1wrapper.o
	g++ -o a.out file.o hl_sha1.o hl_sha1wrapper.o

hl_md5.o: hl_md5.cpp hl_md5.h
	g++ -c hl_md5.cpp

hl_md5wrapper.o: hl_md5wrapper.cpp hl_md5wrapper.h
	g++ -c hl_md5wrapper.cpp

hl_sha1.o: hl_sha1.cpp hl_sha1.h
	g++ -c hl_sha1.cpp

hl_sha1wrapper.o: hl_sha1wrapper.cpp hl_sha1wrapper.h
	g++ -c hl_sha1wrapper.cpp

clean:
	rm *.o
	rm a.out
