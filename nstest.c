#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <grp.h>

static int verbose;

//TODO: Check failure on mount command if directory doesn't exist or target directory is not empty

struct child_args {
	char **argv;      /* Command to be executed by child, with args */
	int pipe_fd[2];  /* Pipe used to synchronize parent and child */
	char *local;
	char *command; //Comand to run
	char *options; //Comand command line options
	char *optionsa; //Comand command line options
	int opt_share_dev;
	int opt_share_tmp;
	int opt_share_network;
};

static int pivot_root(const char *new_root, const char *put_old){
	return syscall(SYS_pivot_root, new_root, put_old);
} 
int child(void *arg){
	struct child_args *args = arg;
	char ch;
	int bufferaddlength=1000;
    //printf("pid as seen in the child: %lu\n", (unsigned long)getpid());
	close(args->pipe_fd[1]);    /* Close our descriptor for the write
	                              // end of the pipe so that we see EOF
	                            //   when parent closes its descriptor. */
	if (read(args->pipe_fd[0], &ch, 1) != 0) {
	//	fprintf(stderr, "Failure in child: read from pipe returned != 0\n");
		exit(EXIT_FAILURE);
	}
	close(args->pipe_fd[0]);
	char *desta=malloc(strlen(args->local)+bufferaddlength);
	desta[0]='\0';
	strcat(desta, args->local);
	strcat(desta, "/sys/\0");
	char *destb=malloc(strlen(args->local)+bufferaddlength);
	destb[0]='\0';
	strcat(destb, args->local);
	strcat(destb, "/proc/\0");
	char *destc=malloc(strlen(args->local)+bufferaddlength);
	destc[0]='\0';
	strcat(destc, args->local);
	strcat(destc, "/dev/\0");
	char *destd=malloc(strlen(args->local)+bufferaddlength);
	destd[0]='\0';
	strcat(destd, args->local);
	strcat(destd, "/tmp/\0");


	char *destdmedia=malloc(strlen(args->local)+bufferaddlength);
	destdmedia[0]='\0';
	strcat(destdmedia, args->local);
	strcat(destdmedia, "/tmp/\0");
	mount("/media/", destdmedia, NULL, MS_BIND| MS_REC, NULL); 

	mount(args->local, args->local, NULL, MS_BIND, NULL);
	if(args->opt_share_network==0){// Can mount /sys only if network is not shared
		mount("none", desta, "sysfs", 0, NULL);
	}
	mount("none", destb, "proc", 0, NULL);
	// mount("none", "ROOT-scidavis/dev", "devfs", 0, NULL); Não funciona?????
	if(args->opt_share_dev==1){
		mount("/dev/", destc, NULL, MS_BIND| MS_REC, NULL); //compartilha com o host para o xserver conectar
		//perror("mount");
	}
	if(args->opt_share_tmp==1){
		mount("/tmp", destd, NULL, MS_BIND | MS_REC, NULL); //compartilha com o host para o xserver conectar
	}
	char *destckkt=malloc(strlen(args->local)+bufferaddlength);
	destckkt[0]='\0';
	strcat(destckkt, args->local);
	strcat(destckkt, "/usr/share/themes/\0");
	mount("/usr/share/themes/", destckkt, NULL, MS_BIND | MS_REC, NULL); //Ajuste do tema do Host para o guest


	char *destckktb=malloc(strlen(args->local)+bufferaddlength);
	destckktb[0]='\0';
	strcat(destckktb, args->local);
	strcat(destckktb, "/usr/share/icons/\0");
	mount("/usr/share/icons/", destckktb, NULL, MS_BIND | MS_REC, NULL); //Ajuste do tema do Host para o guest


char *destckktbaaa=malloc(strlen(args->local)+bufferaddlength);
	destckktbaaa[0]='\0';
	strcat(destckktbaaa, args->local);
	strcat(destckktbaaa, "/run/\0");
	mount("/run/", destckktbaaa, NULL, MS_BIND | MS_REC, NULL); //Monta o /run para o dconf funcionar.

	char *destckkhome=malloc(strlen(args->local)+bufferaddlength);
	destckkhome[0]='\0';
	strcat(destckkhome, args->local);
	strcat(destckkhome, "/home/\0");
	mount("/home/", destckkhome, NULL, MS_BIND | MS_REC, NULL); //Compartilha a Home.

	char *destckksr=malloc(strlen(args->local)+bufferaddlength);
	destckksr[0]='\0';
	strcat(destckksr, args->local);
	strcat(destckksr, "/opt/SpectraRead/\0");
	mount("/usr/lib/luisvmf.com-spectraread/SpectraRead", destckksr, NULL, MS_BIND | MS_REC, NULL); //Compartilha o SpectraRead.

	char *destckksrqimon=malloc(strlen(args->local)+bufferaddlength);
	destckksrqimon[0]='\0';
	strcat(destckksrqimon, args->local);
	strcat(destckksrqimon, "/opt/qimon/\0");
	mount("/usr/lib/luisvmf.com-qimon", destckksrqimon, NULL, MS_BIND | MS_REC, NULL); //Compartilha o Qimon.


	char *dest=malloc(strlen(args->local)+bufferaddlength);
	dest[0]='\0';
	strcat(dest, args->local);
	strcat(dest, "/put_old/\0");
	pivot_root(args->local, dest);
	chdir("/");
	umount2("/put_old/", MNT_DETACH);
	//printf("pid as seen in the child: %lu\n", (unsigned long)getpid());
    // unmount all
    // chroot (bind mount/pivot root dance)
    // remove capabilities? or switch user
	//printf("%s\n",args->local);
    char *newargv[] = { args->command, NULL };
    //execv(args->command, newargv);
	system(args->command);
}



static void update_map(char *mapping, char *map_file){
	int fd;
	size_t map_len;     /* Length of 'mapping' */
	/* Replace commas in mapping string with newlines. */
	map_len = strlen(mapping);
	for (int j = 0; j < map_len; j++)
		if (mapping[j] == ',')
			mapping[j] = '\n';
	fd = open(map_file, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "ERROR: open %s: %s\n", map_file, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (write(fd, mapping, map_len) != map_len) {
		fprintf(stderr, "ERROR: write %s: %s\n", map_file, strerror(errno));
		exit(EXIT_FAILURE);
	}
	close(fd);
}

static void proc_setgroups_write(pid_t child_pid, char *str){
	char setgroups_path[PATH_MAX];
	int fd;
	snprintf(setgroups_path, PATH_MAX, "/proc/%jd/setgroups",(intmax_t) child_pid);
	fd = open(setgroups_path, O_RDWR);
	if (fd == -1) {
		/* We may be on a system that doesn't support
		   /proc/PID/setgroups. In that case, the file won't exist,
		   and the system won't impose the restrictions that Linux 3.19
		   added. That's fine: we don't need to do anything in order
		   to permit 'gid_map' to be updated.
		   However, if the error from open() was something other than
		   the ENOENT error that is expected for that case,  let the
		   user know. */
		if (errno != ENOENT)
			fprintf(stderr, "ERROR: open %s: %s\n", setgroups_path,strerror(errno));
		return;
	}
	if (write(fd, str, strlen(str)) == -1)
		fprintf(stderr, "ERROR: write %s: %s\n", setgroups_path,strerror(errno));
	close(fd);
}


int main(int argc, char *argv[]){
	int flags, opt, map_zero;
	struct child_args args;
	const int MAP_BUF_SIZE = 100;
	char map_buf[MAP_BUF_SIZE];
	char map_path[PATH_MAX];
	int opt_share_IPC=0;
	int opt_share_network=0;
	args.opt_share_dev=0;
	args.opt_share_tmp=0;
	if(argc<7){
		printf("Error! \n     Usage: nstest [location] [bool share_dev] [bool share_tmp] [bool share_IPC] [bool share_network] command [argument]\n\n");
		return 0;
	}
	if(argc>=7){
		args.local=argv[1];
		args.command=argv[6];
		if(argv[2][0]=='1'){
			 args.opt_share_dev=1;
		}
		if(argv[3][0]=='1'){
			 args.opt_share_tmp=1;
		}
		if(argv[4][0]=='1'){
			 opt_share_IPC=1;
		}
		if(argv[5][0]=='1'){
			 opt_share_network=1;
		}
	}
	args.options=NULL;
	args.optionsa="";
	if(argc>=8){
		args.options=argv[7];
	}
	if(argc>=9){
		args.optionsa=argv[8];
		if(argv[7][0]=='\0'){
			args.options=NULL;
		}
	}
	args.opt_share_network=opt_share_network;
	flags = 0;
	verbose = 0;
	map_zero = 0;
	char *uidstringint=(char *)malloc(300*sizeof(char));
	uidstringint[0]='\0';
	char *uidstringintb=(char *)malloc(300*sizeof(char));
	uidstringintb[0]='\0';
	snprintf(uidstringint, 300, "0 %i 1",getuid());
	snprintf(uidstringintb, 300, "0 %i 1",getgid());
	char *uid_map=uidstringint;
	char *gid_map=uidstringintb;
	/* We use a pipe to synchronize the parent and child, in order to
	   ensure that the parent sets the UID and GID maps before the child
	   calls execve(). This ensures that the child maintains its
	   capabilities during the execve() in the common case where we
	   want to map the child's effective user ID to 0 in the new user
	   namespace. Without this synchronization, the child would lose
	   its capabilities if it performed an execve() with nonzero
	   user IDs (see the capabilities(7) man page for details of the
	   transformation of a process's capabilities during execve()). */
		if (pipe(args.pipe_fd) == -1)
			perror("pipe");
	//CLONE_NEWIPC faz com que o xserver não conecte, mais deixa mais isolado.
	int namespaces;
	if(opt_share_IPC==1){
		if(opt_share_network==1){
			namespaces = CLONE_NEWUSER|CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWUTS;
		}else{
			namespaces = CLONE_NEWUSER|CLONE_NEWPID|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWUTS;
		}
	}else{
		if(opt_share_network==1){
			namespaces = CLONE_NEWUSER|CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWUTS|CLONE_NEWIPC;
		}else{
			namespaces = CLONE_NEWUSER|CLONE_NEWPID|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWUTS|CLONE_NEWIPC;
		}
	}
    pid_t child_pid = clone(child, malloc(4096) + 4096, SIGCHLD|namespaces, &args);
    if (child_pid == -1) {
        perror("clone");
        exit(1);
    }
	if (uid_map != NULL || map_zero) {
		snprintf(map_path, PATH_MAX, "/proc/%jd/uid_map",(intmax_t) child_pid);
		if (map_zero) {
			snprintf(map_buf, MAP_BUF_SIZE, "0 %jd 1",(intmax_t) getuid());
			uid_map = map_buf;
		}
		update_map(uid_map, map_path);
	}
	if (gid_map != NULL || map_zero) {
		proc_setgroups_write(child_pid, "deny");
		snprintf(map_path, PATH_MAX, "/proc/%jd/gid_map",(intmax_t) child_pid);
		if (map_zero) {
			snprintf(map_buf, MAP_BUF_SIZE, "0 %ld 1",(intmax_t) getgid());
			gid_map = map_buf;
		}
		update_map(gid_map, map_path);
	}
	/* Close the write end of the pipe, to signal to the child that we
	   have updated the UID and GID maps. */
	close(args.pipe_fd[1]);
    printf("child pid: %lu\n", child_pid);
	sleep(10);
    waitpid(child_pid, NULL, 0);
    return 0;
}
