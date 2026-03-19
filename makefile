CFLAGS = -g -fsanitize=address -fno-omit-frame-pointer

helper_functions.o: helper_functions.c helper_functions.h
	gcc $(CFLAGS) -c helper_functions.c

sound_seg_obj.o: sound_seg.c sound_seg.h
	gcc $(CFLAGS) -c sound_seg.c -o sound_seg_obj.o

# Combine into a single relocatable object (without main.o)
sound_seg.o: sound_seg_obj.o helper_functions.o
	gcc -r -o sound_seg.o sound_seg_obj.o helper_functions.o

# Clean up
clean:
	rm -f *.o