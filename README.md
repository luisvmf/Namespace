# Namespace

Simple c program for using Linux Namespaces.

Unpack Runtime for demo container (File runtime-build.tar).

Compile:

		make
Usage:
   		 
       		Usage: nstest [location] [bool share_dev] [bool share_tmp] [bool share_IPC] [bool share_network] command [argument]

Example:

		./nstest Runtime/ 0 0 0 0 /bin/bash
	
And on other terminal connect to existing namespace:

		a=$(pidof nstest);for pid in $a; do ./enterns /proc/$pid/ns/user /proc/$pid/ns/pid /proc/$pid/ns/uts /proc/$pid/ns/mnt /bin/bash; done
