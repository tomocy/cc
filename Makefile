cc: cc.c

test: cc
	./test.sh

clean:
	rm -rf cc *.o *~ tmp*