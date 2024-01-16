all: $(OBJS)
	gcc -o nstest nstest.c
	gcc -o enterns enterns.c
	#cp client Runtime/bin/notify_send (build inside container)
