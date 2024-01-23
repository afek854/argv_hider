lib_hider:
	gcc lib_hider.c -o lib_hider.so -fPIC -shared -ldl	
clean:
	rm -f lib_hider.so