
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
WX, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#define	MAX_USER		10
#define	MAX_USER_FILE	100
#define MAX_USER_DIR	5
#define	MAX_PSWD_LEN	12

#define	MAX_FILES		80
#define	MAX_DIRS		50


char location[MAX_FILENAME_LEN] = "root";
char files[MAX_FILES][MAX_FILENAME_LEN];
int  filequeue[MAX_FILES];
int  filecount = 0;
char dirs[MAX_DIRS][MAX_FILENAME_LEN];
int  dirqueue[MAX_FILES];
int  dircount = 0;

void shabby_shell(const char * tty_name);

int isDir(const char * filepath);

void getFilepath(char *filepath, char * filename);
void getDirFilepath(char *filepath, char * filename);
void getDirpathAndFilename(char * dirpath, char * filename, char * filepath);

int getFreeFilePos();
int getFreeDirPos();
int getPosInDirQueue(char * filepath);


void addFileIntoDir(const char * dirpath, char * filename);
void deleteFileFromDir(const char * dirpath, char * filename);

void initFS();
void welcome();
void clear();
void showProcess();
void help();
void colorful();
void createFile(char * filepath, char *filename, char * buf);
void createDir(char * filepath, char *filename);
void readFile(char * filename);
void editAppand(const char * filepath, char * str);
void editCover(const char * filepath, char * str);
void deleteFile(char * filepath);
void deleteDir(char * filepath);
void ls();
void cd(char * dirname);
void cdback();



/*****************************************************************************
*                               kernel_main
*****************************************************************************/
/**
* jmp from kernel.asm::_start.
*
*****************************************************************************/
PUBLIC int kernel_main()

{

	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"

		"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");



	int i, j, eflags, prio;

	u8  rpl;

	u8  priv; /* privilege */



	struct task * t;

	struct proc * p = proc_table;



	char * stk = task_stack + STACK_SIZE_TOTAL;



	for (i = 0; i < NR_TASKS + NR_PROCS; i++, p++, t++) {

		if (i >= NR_TASKS + NR_NATIVE_PROCS) {

			p->p_flags = FREE_SLOT;

			continue;

		}



		if (i < NR_TASKS) {     /* TASK */

			t = task_table + i;

			priv = PRIVILEGE_TASK;

			rpl = RPL_TASK;

			eflags = 0x1202;/* IF=1, IOPL=1, bit 2 is always 1 */

			prio = 15;

		}

		else {                  /* USER PROC */

			t = user_proc_table + (i - NR_TASKS);

			priv = PRIVILEGE_USER;

			rpl = RPL_USER;

			eflags = 0x202;	/* IF=1, bit 2 is always 1 */

			prio = 5;

		}



		strcpy(p->name, t->name);	/* name of the process */

		p->p_parent = NO_TASK;



		if (strcmp(t->name, "INIT") != 0) {

			p->ldts[INDEX_LDT_C] = gdt[SELECTOR_KERNEL_CS >> 3];

			p->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];



			/* change the DPLs */

			p->ldts[INDEX_LDT_C].attr1 = DA_C | priv << 5;

			p->ldts[INDEX_LDT_RW].attr1 = DA_DRW | priv << 5;

		}

		else {		/* INIT process */

			unsigned int k_base;

			unsigned int k_limit;

			int ret = get_kernel_map(&k_base, &k_limit);

			assert(ret == 0);

			init_desc(&p->ldts[INDEX_LDT_C],

				0, /* bytes before the entry point

				   * are useless (wasted) for the

				   * INIT process, doesn't matter

				   */

				(k_base + k_limit) >> LIMIT_4K_SHIFT,

				DA_32 | DA_LIMIT_4K | DA_C | priv << 5);



			init_desc(&p->ldts[INDEX_LDT_RW],

				0, /* bytes before the entry point

				   * are useless (wasted) for the

				   * INIT process, doesn't matter

				   */

				(k_base + k_limit) >> LIMIT_4K_SHIFT,

				DA_32 | DA_LIMIT_4K | DA_DRW | priv << 5);

		}



		p->regs.cs = INDEX_LDT_C << 3 | SA_TIL | rpl;

		p->regs.ds =

			p->regs.es =

			p->regs.fs =

			p->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;

		p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p->regs.eip = (u32)t->initial_eip;

		p->regs.esp = (u32)stk;

		p->regs.eflags = eflags;



		p->ticks = p->priority = prio;



		p->p_flags = 0;

		p->p_msg = 0;

		p->p_recvfrom = NO_TASK;

		p->p_sendto = NO_TASK;

		p->has_int_msg = 0;

		p->q_sending = 0;

		p->next_sending = 0;



		for (j = 0; j < NR_FILES; j++)

			p->filp[j] = 0;



		stk -= t->stacksize;

	}



	k_reenter = 0;

	ticks = 0;



	p_proc_ready = proc_table;



	init_clock();

	init_keyboard();



	restart();



	while (1) {}

}


/*****************************************************************************
*                                get_ticks
*****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


/**
* @struct posix_tar_header
* Borrowed from GNU `tar'
*/
struct posix_tar_header
{				/* byte offset */
	char name[100];		/*   0 */
	char mode[8];		/* 100 */
	char uid[8];		/* 108 */
	char gid[8];		/* 116 */
	char size[12];		/* 124 */
	char mtime[12];		/* 136 */
	char chksum[8];		/* 148 */
	char typeflag;		/* 156 */
	char linkname[100];	/* 157 */
	char magic[6];		/* 257 */
	char version[2];	/* 263 */
	char uname[32];		/* 265 */
	char gname[32];		/* 297 */
	char devmajor[8];	/* 329 */
	char devminor[8];	/* 337 */
	char prefix[155];	/* 345 */
						/* 500 */
};

/*****************************************************************************
*                                untar
*****************************************************************************/
/**
* Extract the tar file and store them.
*
* @param filename The tar file.
*****************************************************************************/
void untar(const char * filename)
{
	printf("[extract `%s'", filename);
	int fd = open(filename, O_RDWR);
	assert(fd != -1);

	char buf[SECTOR_SIZE * 16];
	int chunk = sizeof(buf);

	while (1) {
		read(fd, buf, SECTOR_SIZE);
		if (buf[0] == 0)
			break;

		struct posix_tar_header * phdr = (struct posix_tar_header *)buf;

		/* calculate the file size */
		char * p = phdr->size;
		int f_len = 0;
		while (*p)
			f_len = (f_len * 8) + (*p++ - '0'); /* octal */

		int bytes_left = f_len;
		int fdout = open(phdr->name, O_CREAT | O_RDWR);
		if (fdout == -1) {
			printf("    failed to extract file: %s\n", phdr->name);
			printf(" aborted]");
			return;
		}
		printf("    %s (%d bytes)", phdr->name, f_len);
		while (bytes_left) {
			int iobytes = min(chunk, bytes_left);
			read(fd, buf,
				((iobytes - 1) / SECTOR_SIZE + 1) * SECTOR_SIZE);
			write(fdout, buf, iobytes);
			bytes_left -= iobytes;
		}
		close(fdout);
	}

	close(fd);

	printf(" done]\n");
}


/*****************************************************************************
*                                Init
*****************************************************************************/
/**
* The hen.
*
*****************************************************************************/
void Init()
{
	int fd_stdin = open("/dev_tty0", O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open("/dev_tty0", O_RDWR);
	assert(fd_stdout == 1);

	//printf("Init() is running ...\n");

	/* extract `cmd.tar' */
	untar("/cmd.tar");


	char * tty_list[] = { "/dev_tty0" };

	int i;
	for (i = 0; i < sizeof(tty_list) / sizeof(tty_list[0]); i++) {
		int pid = fork();
		if (pid != 0) { /* parent process */
		}
		else {	/* child process */
			close(fd_stdin);
			close(fd_stdout);

			shabby_shell(tty_list[i]);
			assert(0);
		}
	}

	while (1) {
		int s;
		int child = wait(&s);
		printf("child (%d) exited with status: %d.\n", child, s);
	}

	assert(0);
}


/*======================================================================*
TestA
*======================================================================*/
void TestA()
{
	for (;;);
}

/*======================================================================*
TestB
*======================================================================*/
void TestB()
{
	for (;;);
}

/*======================================================================*
TestB
*======================================================================*/
void TestC()
{
	for (;;);
}

/*****************************************************************************
*                                panic
*****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

/*****************************************************************************
*                                wx_shell
*****************************************************************************/
/**
* A very powerful shell.
*
* @param tty_name  TTY file name.
*****************************************************************************/
void shabby_shell(const char * tty_name)
{


	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];
	char cmd[128];
	char arg1[MAX_FILENAME_LEN];
	char arg2[MAX_FILENAME_LEN];
	char filepath[MAX_FILENAME_LEN];

	colorful();
	clear();
	welcome();
	printf("press any key to start:\n");
	initFS();

	while (1) {

		memset(rdbuf, 0, 128);
		memset(cmd, 0, 128);
		memset(arg1, 0, MAX_FILENAME_LEN);
		memset(arg2, 0, MAX_FILENAME_LEN);

		printf("%s $ ", location);
		int r = read(0, rdbuf, 70);
		rdbuf[r] = 0;


		int argc = 0;
		char * argv[PROC_ORIGIN_STACK];
		char * p = rdbuf;
		char * s;
		int word = 0;
		char ch;
		do {
			ch = *p;
			if (*p != ' ' && *p != 0 && !word) {
				s = p;
				word = 1;
			}
			if ((*p == ' ' || *p == 0) && word) {
				word = 0;
				argv[argc++] = s;
				*p = 0;
			}
			p++;
		} while (ch);
		argv[argc] = 0;

		int fd = open(argv[0], O_RDWR);
		if (fd == -1) {
			if (rdbuf[0]) {
				int i = 0, j = 0;
				/* get command */
				while (rdbuf[i] != ' ' && rdbuf[i] != 0)
				{
					cmd[i] = rdbuf[i];
					i++;
				}
				i++;
				/* get arg1 */
				while (rdbuf[i] != ' ' && rdbuf[i] != 0)
				{
					arg1[j] = rdbuf[i];
					i++;
					j++;
				}
				i++;
				j = 0;
				/* get arg2 */
				while (rdbuf[i] != ' ' && rdbuf[i] != 0)
				{
					arg2[j] = rdbuf[i];
					i++;
					j++;
				}

				/* welcome */
				if (strcmp(cmd, "welcome") == 0)
				{
					welcome();
				}
				/* clear screen */
				else if (strcmp(cmd, "clear") == 0)
				{
					clear();
					welcome();
				}
				/* show process */
				else if (strcmp(cmd, "proc") == 0)
				{
					showProcess();
				}
				/* show help message */
				else if (strcmp(cmd, "help") == 0)
				{
					help();
				}
				/* create a file */
				else if (strcmp(cmd, "mkfile") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}

					strcpy(filepath, location);
					getFilepath(filepath, arg1);
					printf("%s  %s\n", arg1, arg2);
					createFile(filepath, arg1, arg2);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* create a dir */
				else if (strcmp(cmd, "mkdir") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular dirname!");
						continue;
					}

					strcpy(filepath, location);
					getDirFilepath(filepath, arg1);
					createDir(filepath, arg1);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* read a file */
				else if (strcmp(cmd, "read") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					readFile(arg1);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* edit a file cover */
				else if (strcmp(cmd, "edit") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					strcpy(filepath, location);
					getFilepath(filepath, arg1);
					editCover(filepath, arg2);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* edit a file appand */
				else if (strcmp(cmd, "edit+") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					strcpy(filepath, location);
					getFilepath(filepath, arg1);
					editAppand(filepath, arg2);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* delete a file */
				else if (strcmp(cmd, "delete") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					strcpy(filepath, location);
					getFilepath(filepath, arg1);
					deleteFile(filepath);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* delete a directory */
				else if (strcmp(cmd, "deletedir") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					strcpy(filepath, location);
					getDirFilepath(filepath, arg1);
					deleteDir(filepath);
					memset(filepath, 0, MAX_FILENAME_LEN);
				}
				/* ls */
				else if (strcmp(cmd, "ls") == 0)
				{
					ls();
				}
				/* cd */
				else if (strcmp(cmd, "cd") == 0)
				{
					if (arg1[0] == '#')
					{
						printf("Irregular filename!");
						continue;
					}
					else if (strcmp(arg1, "..") == 0)
					{
						cdback();
					}
					else
					{
						cd(arg1);
					}
				}
				else if (strcmp(cmd, "information") == 0)

				{

					information();

				}

				/* print */

				else if (strcmp(cmd, "print") == 0)

				{

					printf("%s\n", arg1);

				}
				else
				{
					printf("Command not found\n");
				}
			}
		}
		else {
			close(fd);
			int pid = fork();
			if (pid != 0) { /* parent */
				int s;
				wait(&s);
			}
			else {	/* child */
				execv(argv[0], argv);
			}
		}
	}

	close(1);
	close(0);
}

/*****************************************************************************
*								Welcome
*****************************************************************************/
void welcome()
{

	printf("=============================================================================\n");
	printf("       ooooo     ooooooo         ooo      oooo     ooo      ooooo    oooooooo\n");
	printf("    oooo  oooo   ooo  oooo      ooooo     ooooo    ooo   ooooo  ooo  ooo     \n");
	printf("   ooo      ooo  ooo   ooo      oo ooo    oooooo   ooo  ooo          ooo     \n");
	printf("   ooo      ooo  oooooooo      oo   ooo   ooo oooo ooo  ooo  oooooo  oooooooo\n");
	printf("   ooo      ooo  ooo oooo     ooooooooo   ooo   oooooo  ooo     ooo  ooo     \n");
	printf("   oooo    oooo  ooo   ooo   ooo     ooo  ooo    ooooo  oooo    ooo  ooo     \n");
	printf("     oooooooo    ooo    ooo ooo      ooo  ooo     oooo    oooooooo   oooooooo\n");
	printf("=============================================================================\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n");
}

/*****************************************************************************
*								Clear
*****************************************************************************/
void clear()
{
	int i = 0;
	for (i = 0; i < 20; i++)
		printf("\n");
}

/*****************************************************************************
*								Quit
*****************************************************************************/
void off()
{
	return 0;
}
/*****************************************************************************
*							  Show Process
*****************************************************************************/
void showProcess()
{
	int i = 0;
	printf("********************************************************************************\n");
	printf("        name        |        priority        |        f_flags(0 is runable)        \n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	for (i = 0; i < NR_TASKS + NR_PROCS; i++)
	{
		printf("        %s                   %d                      %d\n", proc_table[i].name, proc_table[i].priority, proc_table[i].p_flags);
	}
	printf("********************************************************************************\n");
}

/*****************************************************************************
*							Show Help Message
*****************************************************************************/
void help()
{
	printf("********************************************************************************\n");
	printf("        name                   |                      function                      \n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("        welcome                |       Welcome the users\n");
	printf("        clear                  |       Clean the screen\n");
	printf("        ls                     |       List all files in current file path\n");
	printf("        help                   |       List all commands\n");
	printf("        proc                   |       List all process's message\n");
	printf("        print	  [str]        |       Print a string\n");
	printf("        mkfile	  [file][str]  |       Create a file\n");
	printf("        mkdir	  [file]	   |       Create a directory\n");
	printf("        read	  [file]       |       Read a file\n");
	printf("        delete	  [file]       |       Delete a file\n");
	printf("        deletedir [file]       |       Delete a directory\n");
	printf("        edit      [file][str]  |       Edit file, cover the content\n");
	printf("        edit+     [file][str]  |       Edit file, appand after the content\n");
	printf("        add       [user][pass] |       Create a new user\n");
	printf("        move      [user][pass] |       Remove a user and delete his files\n");
	printf("        login     [user][pass] |       Login \n");
	printf("        logout                 |       Loginout\n");
	printf("        cd                     |       Change directory\n");
	printf("        quit                   |       Power off\n");
	printf("===============================================================================\n");

}

/*****************************************************************************
*								Colorful
*****************************************************************************/
void colorful()
{
	int j = 0;
	for (j = 0; j < 2800; j++) { disp_str(" "); }
	disp_color_str("                       oo", YELLOW);
	disp_color_str("oooooooooooo", LIGHT);
	disp_color_str("oooo", LIGHT);
	disp_color_str("oooooooooo", LIGHT);
	disp_color_str("oo                   \n", YELLOW);
	disp_color_str("                       oo", YELLOW);
	disp_color_str("oooooooooooooooooooooooooo", LIGHT);
	disp_color_str("oo                   \n", YELLOW);
	disp_color_str("                        oo", YELLOW);
	disp_color_str("oooooooooooooooooooooooo", LIGHT);
	disp_color_str("oo                   \n", YELLOW);
	disp_color_str("                         oo", YELLOW);
	disp_color_str("oooooooooooooooooooooo", LIGHT);
	disp_color_str("oo                    \n", YELLOW);
	disp_color_str("                          oo", YELLOW);
	disp_color_str("ooooooooooooooooooooo", LIGHT);
	disp_color_str("o                      \n", YELLOW);
	disp_color_str("                           oo", YELLOW);
	disp_color_str("ooooooooooooooooo", LIGHT);
	disp_color_str("oo                       \n", YELLOW);
	disp_color_str("                             oo", YELLOW);
	disp_color_str("ooooooooooooo", LIGHT);
	disp_color_str("oo                          \n", YELLOW);
	disp_color_str("                               ooooooooooooo                              \n", YELLOW);
	for (j = 0; j < 3; j++)
		disp_color_str("oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo", BLUE);
	disp_color_str("        ooooo     ooooooo         ooo      oooo     ooo      ooooo    oooooooo\n", RED);
	disp_color_str("     oooo  oooo   ooo  oooo      ooooo     ooooo    ooo   ooooo  ooo  ooo     \n", RED);
	disp_color_str("    ooo      ooo  ooo   ooo      oo ooo    oooooo   ooo  ooo          ooo     \n", RED);
	disp_color_str("    ooo      ooo  oooooooo      oo   ooo   ooo oooo ooo  ooo  oooooo  oooooooo\n", RED);
	disp_color_str("    ooo      ooo  ooo oooo     ooooooooo   ooo   oooooo  ooo     ooo  ooo     \n", RED);
	disp_color_str("    oooo    oooo  ooo   ooo   ooo     ooo  ooo    ooooo  oooo    ooo  ooo     \n", RED);
	disp_color_str("      oooooooo    ooo    ooo ooo      ooo  ooo     oooo    oooooooo   oooooooo\n", RED);

	for (j = 0; j < 3; j++)
		disp_color_str("oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo", BLUE);
	for (j = 0; j < 300; j++)
		disp_str(" ");
	milli_delay(80000);

}
/*****************************************************************************

*								Information

*****************************************************************************/

void information()

{

	printf(" MEMORYSIZE:%dMB\n", memory_size / (1024 * 1024));

	printf(" STACK_SIZE_TOTAL:%dMB\n", STACK_SIZE_TOTAL / (1024 * 1024));

	printf(" MMBUF_SIZE:%dMB\n", MMBUF_SIZE / (1024 * 1024));

	printf(" FSBUF_SIZE:%dMB\n", FSBUF_SIZE / (1024 * 1024));



}
/*****************************************************************************
*							File System
*****************************************************************************/

/*****************************************************************************
*								Init FS
*****************************************************************************/
void initFS()
{
	int fd = -1, n = 0;
	char bufr[1024];
	char filepath[MAX_FILENAME_LEN];
	char dirpath[MAX_FILENAME_LEN];
	char filename[MAX_FILENAME_LEN];

	memset(filequeue, 0, MAX_FILES);
	memset(dirqueue, 0, MAX_DIRS);

	fd = open("root", O_CREAT | O_RDWR);
	close(fd);

	fd = open("root", O_RDWR);
	write(fd, bufr, 1024);
	close(fd);

	/*fd = open("root", O_RDWR);
	n = read(fd, bufr, 1024);
	close(fd);

	int i, k;
	for (i = 0, k = 0; i < n; i++)
	{

		if (bufr[i] != ' ')
		{
			filepath[k] = bufr[i];
			k++;
		}
		else
		{
			while (bufr[i] == ' ')
				i++;

			if (strcmp(filepath, "") == 0)
				continue;

			getDirpathAndFilename(dirpath, filename, filepath);
			if (filename[0] == '#')
			{
				strcpy(dirs[dircount], filepath);
				dirqueue[dircount] = 1;
				dircount++;
			}
			else
			{
				strcpy(dirs[dircount], filepath);
				filequeue[filecount] = 1;
				filecount++;
			}

			fd = open(filepath, O_CREAT | O_RDWR);
			close(fd);

			k = 0;

			if (bufr[i] == 0)
				break;

			i--;
		}
	}*/
}

/*****************************************************************************
*							Identity a Directory
*****************************************************************************/
int isDir(const char * filepath)
{
	int pos = getPosInDirQueue(filepath);
	if (pos != -1)
	{
		return 1;
	}
	return 0;
}

/*****************************************************************************
*                             Get Filepath
*****************************************************************************/
void getFilepath(char *filepath, char * filename)
{
	strjin(filepath, filename, '_');
}

/*****************************************************************************
*                         Get Directory Filepath
*****************************************************************************/
void getDirFilepath(char *filepath, char * filename)
{
	strcat(filepath, "_");
	strjin(filepath, filename, '#');
}

/*****************************************************************************
*                   Get Dirpath And Filename/Dirname From Filepath
*****************************************************************************/
void getDirpathAndFilename(char * dirpath, char * filename, char * filepath)
{

	char str[MAX_FILENAME_LEN];
	int i, k;

	memset(dirpath, 0, MAX_FILENAME_LEN);
	memset(filename, 0, MAX_FILENAME_LEN);

	for (i = 0, k = 0; filepath[i] != 0; i++)
	{
		if (filepath[i] != '_')
		{
			str[k] = filepath[i];
			k++;
		}
		else
		{
			strcat(dirpath, str);
			strcat(dirpath, "_");
			memset(str, 0, MAX_FILENAME_LEN);
			k = 0;
		}
	}
	dirpath[strlen(dirpath) - 1] = 0;
	strcpy(dirpath, dirpath);
	strcpy(filename, str);

}

/*****************************************************************************
*						Get a Free Pos in FileQueue
*****************************************************************************/
int getFreeFilePos()
{
	int i = 0;
	for (i = 0; i < MAX_FILES; i++)
	{
		if (filequeue[i] == 0)
			return i;
	}
	printf("The number of files is full!!\n");
	return -1;
}

/*****************************************************************************
*						Get a Free Pos in DirQueue
*****************************************************************************/
int getFreeDirPos()
{
	int i = 0;
	for (i = 0; i < MAX_DIRS; i++)
	{
		if (dirqueue[i] == 0)
			return i;
	}
	printf("The number of folders is full!!\n");
	return -1;
}

/*****************************************************************************
*						Get Dir's Pos in FileQueue
*****************************************************************************/
int getPosInDirQueue(char * filepath)
{
	int i = 0;
	for (i = 0; i < MAX_FILES; i++)
	{
		if (strcmp(dirs[i], filepath) == 0)
			return i;
	}
	return -1;
}


/*****************************************************************************
*						Add Filename Into Dir
*****************************************************************************/
void addFileIntoDir(const char * dirpath, char * filename)
{
	int fd = -1;

	if (strcmp(dirpath, "root") == 0)
	{
		fd = open("root", O_RDWR);
		
	}
	else
	{
		fd = open(dirpath, O_RDWR);
	}

	if (fd == -1)
	{
		printf("%s has not been found!\n", dirpath);
		return;
	}

	strcat(filename, " ");
	editAppand(dirpath, filename);
}

/*****************************************************************************
*						Delete Filename From Dir
*****************************************************************************/
void deleteFileFromDir(const char * dirpath, char * filename)
{

	/*char bufr[MAX_USER_FILE * MAX_FILENAME_LEN];
	char bufw[MAX_USER_FILE * MAX_FILENAME_LEN];*/
	char bufr[1024];
	char bufw[1024];
	char buf[MAX_FILENAME_LEN];
	int fd = -1, n = 0;

	fd = open(dirpath, O_RDWR);

	if (fd == -1)
	{
		printf("%s has not been found!!\n", dirpath);
		return;
	}

	n = read(fd, bufr, 1024);

	int i, k;
	for (i = 0, k = 0; i < n; i++)
	{
		if (bufr[i] != ' ')
		{
			buf[k] = bufr[i];
			k++;
		}
		else
		{
			buf[k] = 0;
			k = 0;

			if (strcmp(buf, filename) == 0)
				continue;

			strcat(bufw, buf);
			strcat(bufw, " ");
		}
	}
	printf("%s\n", bufw);
	
	editCover(dirpath, bufw);

	close(fd);
}

/*****************************************************************************
*							 Create File
*****************************************************************************/
void createFile(char * filepath, char *filename, char * buf)
{
	int fd = -1, pos = -1;
	
	fd = open(filepath, O_CREAT | O_RDWR);
	printf("file name: %s\n content: %s\n", filename, buf);
	if (fd == -1)
	{
		printf("New file failed. Please check and try again!!\n");
		return;
	}
	else if (fd == -2)
	{
		printf("File already exist!!\n");
		return;
	}

	write(fd, buf, strlen(buf));
	close(fd);

	pos = getFreeFilePos();
	filequeue[pos] = 1;
	strcpy(files[pos], filepath);
	filecount++;

	addFileIntoDir(location, filename);
}

/*****************************************************************************
*							 Create Directory
*****************************************************************************/
void createDir(char * filepath, char *filename)
{
	int fd = -1, pos = -1;

	fd = open(filepath, O_CREAT | O_RDWR);
	printf("Folder name: %s\n", filename);
	if (fd == -1)
	{
		printf("New folder failed. Please check and try again!!\n");
		return;
	}
	else if (fd == -2)
	{
		printf("Folder already exists!!\n");
		return;
	}

	close(fd);

	pos = getFreeDirPos();
	dirqueue[pos] = 1;
	strcpy(dirs[pos], filepath);
	dircount++;


	char str[MAX_FILENAME_LEN] = "#";
	strcat(str, filename);
	addFileIntoDir(location, str);
}

/*****************************************************************************
*								Read File
*****************************************************************************/
void readFile(char * filename)
{
	char filepath[MAX_FILENAME_LEN];
	strcpy(filepath, location);
	getDirFilepath(filepath, filename);
	if (isDir(filepath))
	{
		printf("Cannot read folder!!\n");
		return;
	}

	int fd = -1;
	int n;
	char bufr[1024] = "";

	strcpy(filepath, location);
	getFilepath(filepath, filename);
	fd = open(filepath, O_RDWR);
	if (fd == -1)
	{
		printf("Opening file error. Please check and try again!\n");
		return;
	}

	n = read(fd, bufr, 1024);
	bufr[n] = 0;
	printf("%s(fd=%d) : %s\n", filepath, fd, bufr);
	close(fd);
}

/*****************************************************************************
*							Edit File Cover
*****************************************************************************/
void editCover(const char * filepath, char * str)
{
	char empty[1024];
	int fd = -1;
	fd = open(filepath, O_RDWR);
	if (fd == -1)
	{

		printf("Opening file error. Please check and try again!!\n");
		return;
	}
	memset(empty, 0, 1024);
	write(fd, empty, 1024);
	close(fd);
	fd = open(filepath, O_RDWR);
	write(fd, str, strlen(str));
	close(fd);
}

/*****************************************************************************
*							Edit File Appand
*****************************************************************************/
void editAppand(const char * filepath, char * str)
{
	int fd = -1;
	char bufr[1024];
	char empty[1024];

	fd = open(filepath, O_RDWR);
	if (fd == -1)
	{
		printf("Opening file error. Please check and try again!!\n");
		return;
	}

	read(fd, bufr, 1024);
	close(fd);

	fd = open(filepath, O_RDWR);
	write(fd, empty, 1024);
	close(fd);

	strcat(bufr, str);

	fd = open(filepath, O_RDWR);
	write(fd, bufr, strlen(bufr));
	close(fd);
}
/*****************************************************************************
*							   Delete File
*****************************************************************************/
void deleteFile(char * filepath)
{
	if (filecount == 0)
	{
		printf("Error, no file to delete!\n");
		return;
	}

	if (unlink(filepath) != 0)
	{
		printf("Deleting file error. Please check and try again!\n");
		return;
	}

	int i;
	for (i = 0; i < filecount; i++)
	{
		if (strcmp(files[i], filepath) == 0)
		{
			memset(files[i], 0, MAX_FILENAME_LEN);
			filequeue[i] = 0;
			filecount--;
			break;
		}
	}

	/* delete filename from user's dir */
	char dirpath[MAX_FILENAME_LEN];
	char filename[MAX_FILENAME_LEN];
	getDirpathAndFilename(dirpath, filename, filepath);

	deleteFileFromDir(dirpath, filename);
}

/*****************************************************************************
*							 Delete Directory
*****************************************************************************/
void deleteDir(char * filepath)
{
	if (dircount == 0)
	{
		printf("Error, no folder to delete!!\n");
		return;
	}

	char dirfile[MAX_FILENAME_LEN];
	char rdbuf[1024];
	int fd = -1, n = 0;
	char filename[MAX_FILENAME_LEN];
	fd = open(filepath, O_RDWR);
	if (fd == -1)
	{
		printf("Deleting folder error. Please check and try again!!\n");
		return;
	}

	n = read(fd, rdbuf, 1024);

	int i, k;
	for (i = 0, k = 0; i < n; i++)
	{

		if (rdbuf[i] != ' ')
		{
			dirfile[k] = rdbuf[i];
			k++;
		}
		else
		{
			dirfile[k] = 0;
			k = 0;

			char path[MAX_FILENAME_LEN];
			strcpy(path, filepath);
			strjin(path, filename, '_');

			if (dirfile[0] == '#')
			{
				deleteDir(path);
			}
			else
			{
				deleteFile(path);
			}
		}
	}
	close(fd);

	if (unlink(filepath) != 0)
	{
		printf("Deleting folder error. Please check and try again!\n");
		return;
	}

	for (i = 0; i < dircount; i++)
	{
		if (strcmp(dirs[i], filepath) == 0)
		{
			memset(dirs[i], 0, MAX_FILENAME_LEN);
			dirqueue[i] = 0;
			dircount++;
			break;
		}
	}

	char dirpath[MAX_FILENAME_LEN];

	getDirpathAndFilename(dirpath, filename, filepath);
	deleteFileFromDir(dirpath, filename);
}

/*****************************************************************************
*						List All Files in the Directory
*****************************************************************************/
void ls()
{
	int fd = -1;
	char bufr[1024];

	fd = open(location, O_RDWR);

	if (fd == -1)
	{
		printf("Error opening file\n");
		return;
	}

	read(fd, bufr, 1024);
	printf("%s\n", bufr);
	close(fd);
}

/*****************************************************************************
*									cd
*****************************************************************************/
void cd(char * dirname)
{
	char filepath[MAX_FILENAME_LEN];
	strcpy(filepath, location);
	getDirFilepath(filepath, dirname);
	if (!isDir(filepath))
	{
		printf("NO folder %s!\n", dirname);
		return;
	}

	strcat(location, "_");
	strcat(location, dirname);
}

/*****************************************************************************
*							Go Back To Previous Directory
*****************************************************************************/
void cdback()
{
	if (strcmp(location, "root") == 0)
	{
		printf("ROOT");
		return;
	}

	char dirpath[MAX_FILENAME_LEN];
	char filename[MAX_FILENAME_LEN];

	getDirpathAndFilename(dirpath, filename, location);
	strcpy(location, dirpath);
}
