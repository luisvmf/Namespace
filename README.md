# Namespace

Simple c program for using Linux user Namespaces (no root required).

Unpack Runtime for demo container (fedora 30) (File runtime-build.tar).

Compile:

	make
Usage:
 
	Usage: nstest [location] [bool share_dev] [bool share_tmp] [bool share_IPC] [bool share_network] command [argument]

Example:

	./nstest Runtime/ 0 0 0 0 /bin/bash

 Now you are root inside containner and can use yum to install programs...
	
To  connect to existing namespace, while nstest is running, from other terminal run:

	a=$(pidof nstest);for pid in $a; do ./enterns /proc/$pid/ns/user /proc/$pid/ns/pid /proc/$pid/ns/uts /proc/$pid/ns/mnt /bin/bash; done
