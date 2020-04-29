#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#define CMD_LINE_SIZE 32
#define HIST_BUF_SIZE 8
#define CACHE_LINE_SIZE 64
#define MAX_ROW_SIZE 12

static char inputline[CMD_LINE_SIZE+1];
static int memfd;

bool is_dec(const char* str){

    if(strlen(str) < 1){
        return false;
    }

    const char *ptr = str;
     while(*ptr) {
        if(!isdigit(*ptr)){
            return false;
        }
        ptr++;
    }

    return true;
}

bool is_hex(const char* str){

    if(strlen(str) < 3 || str[0] != '0' || str[1] != 'x'){
        return false;
    }

    const char *ptr = &str[2];

    while(*ptr){
        if(!isxdigit(*ptr)){
            return false;
        }
        ptr++;
    } 

    return true;
}

static inline bool is_number(const char* str){
    return is_dec(str) || is_hex(str);
}

static void cache_flush(uintptr_t addr, size_t len){

    if(len == 0) return;

    size_t lines = (len/CACHE_LINE_SIZE) + (((addr+len)%CACHE_LINE_SIZE) ? 1 : 0);
    uint64_t* vaddr = mmap_device_memory(NULL, lines * CACHE_LINE_SIZE, 
        PROT_NOCACHE | PROT_WRITE, 0, addr & ~(CACHE_LINE_SIZE-1));
    //TODO check vaddr

    uintptr_t tmp_vaddr = (uintptr_t)vaddr;
    for(size_t i = 0; i < lines; i++){

        asm volatile(
            "dc civac, %0\n\t"
            "dsb sy\n\t"
            "isb sy\n\t" :: "r"(tmp_vaddr));
        tmp_vaddr += CACHE_LINE_SIZE; 
    }
    munmap((void*)vaddr, len);
}

static void load(uintptr_t addr)
{
    uint8_t val;
    
    cache_flush(addr, 1);

    if (pread(memfd, &val, sizeof(val), addr) != sizeof(val))
    {
        //todo check errno and its string
        printf("0x%016lx: 0x00\n\n", addr);
    }
    else
    {
        printf("0x%016lx: 0x%02x\n\n", addr, val);
    }

    cache_flush(addr, 1);
}

static void store(uintptr_t addr, uint64_t val, size_t len)
{
    if (pwrite(memfd, &val, len, addr) != len){
        //todo check errno and its string
        printf("x%016lx: can't write\n\n");
    } else {
        printf("0x%016lx: 0x%x\n\n", addr, val);
    }
    cache_flush(addr, len);
}

static void dump(uintptr_t addr, size_t len)
{
    static char row[MAX_ROW_SIZE];
    uint8_t val;
    uintptr_t tmp_addr = addr;
    cache_flush(addr, len);
    for(int j = 0; j < len; j+=MAX_ROW_SIZE){
        printf("0x%016lx: ", tmp_addr);
        size_t row_size = (len-j)/MAX_ROW_SIZE ? MAX_ROW_SIZE : len%MAX_ROW_SIZE;

        for(int i = 0; i < row_size; i++ ){
            if (pread(memfd, &val, sizeof(val), tmp_addr + i) != sizeof(val)){
                row[i] = 0;
            } else {
                row[i] = val;
            }
        }

        for(int i = 0; i < row_size; i++ ){
            printf("%02x ", row[i]);
        }
        for(int i = 0; i < MAX_ROW_SIZE-row_size; i++ ){
            printf("   ");
        }

        printf("|");
        for(int i = 0; i < row_size; i++ ){
            printf("%c", isalnum(row[i]) ? row[i] : '.');
        }
        for(int i = 0; i < MAX_ROW_SIZE-row_size; i++ ){
            printf(" ");
        }
        printf("|\n");

        tmp_addr += row_size;
    }
    printf("\n");
    cache_flush(addr, len);
}

static void erase(uintptr_t addr, size_t len)
{   
    const uint8_t zero = 0;
    for(int i = 0; i < len; i++) {
        if(pwrite(memfd, &zero, 1, addr+i) != 1) {
            printf("failed\n\n");
            return;
        }
    }
    cache_flush(addr, len);
}

void prompt(){
    write(STDIN_FILENO, "\rQNX > ", 7);
}

int readline() {


	static int p=0;
	static int esc=0;
	static char history[HIST_BUF_SIZE][sizeof(inputline)];
    static int h=-1;
    static char c;
	while ( ((c = getchar()) != EOF) || esc!=0 ) {

        if (c=='\e'){
            esc=1;

        } else if (esc==1 && c=='['){
            esc=2;

        } else if (esc==2 && c=='3'){
            esc=3;

        } else if (esc==3 && c=='~'){ // del key
            for (int i=p; i<strlen(inputline); i++) inputline[i]=inputline[i+1];
            write(STDIN_FILENO, "\e7", 2); // save curs pos
            write(STDIN_FILENO, "\e[K", 3); // clear line from curs pos
            write(STDIN_FILENO, &inputline[p], strlen(inputline)-p);
            write(STDIN_FILENO, "\e8", 2); // restore curs pos
            esc=0;

        } else if (esc==2 && c=='C'){ // right arrow
            esc=0;
            if (p < strlen(inputline)){
                p++;
                write(STDIN_FILENO, "\e[C", 3);
            }

        } else if (esc==2 && c=='D'){ // left arrow
            esc=0;
            if (p>0){
                p--;
                write(STDIN_FILENO, "\e[D", 3);
            }

        } else if (esc==2 && c=='A'){ // up arrow (history)
            esc=0;
            if (h<8-1 && strlen(history[h+1])>0){
                h++;
                strcpy(inputline, history[h]);
                write(STDIN_FILENO, "\e[2K", 4); // 2K clear entire line - cur pos dosn't change
                prompt();
                write(STDIN_FILENO, inputline, strlen(inputline));
                p=strlen(inputline);
            }

        } else if (esc==2 && c=='B'){ // down arrow (history)
            esc=0;
            if (h>0 && strlen(history[h-1])>0){
                h--;
                strcpy(inputline, history[h]);
                write(STDIN_FILENO, "\e[2K", 4); // 2K clear entire line - cur pos dosn't change
                prompt();
                write(STDIN_FILENO, inputline, strlen(inputline));
                p=strlen(inputline);
            }

        } else if ((c=='\b' || c=='\x7f') && p>0 && esc==0){ // backspace
            p--;
            for (int i=p; i<strlen(inputline); i++) inputline[i]=inputline[i+1];
            write(STDIN_FILENO, "\e[D", 3);
            write(STDIN_FILENO, "\e7", 2);
            write(STDIN_FILENO, "\e[K", 3);
            write(STDIN_FILENO, &inputline[p], strlen(inputline)-p);
            write(STDIN_FILENO, "\e8", 2);

        } else if (c>=' ' && c<='~' && p < sizeof(inputline)-1 && esc==0 && strlen(inputline)!=CMD_LINE_SIZE){
            for (int i = sizeof(inputline)-1-1; i > p; i--) inputline[i]=inputline[i-1]; // make room for 1 ch
            inputline[p]=c;
            write(STDIN_FILENO, "\e7", 2); // save curs pos
            write(STDIN_FILENO, "\e[K", 3); // clear line from curs pos
            write(STDIN_FILENO, &inputline[p], strlen(inputline)-p); p++;
            write(STDIN_FILENO, "\e8", 2); // restore curs pos
            write(STDIN_FILENO, "\e[C", 3); // move curs right 1 pos

        } else if (c=='\r' || c=='\n') {
            p=0; esc=0;
            write(STDIN_FILENO, "\n", 1);
            for (int i = sizeof(inputline)-1; i > 0; i--) if (inputline[i]==' ') inputline[i]='\0'; else break;

            if (strlen(inputline)>0 && strcmp(inputline, history[0])!=0){
                for (int i = 8-1; i > 0; i--) strcpy(history[i], history[i-1]);
                strcpy(history[0], inputline);
            }
            h = -1;

            return 1;

        } else esc=0;
    }

	return 0;

}


void cmd_handler(){

	char * tk1 = strtok (inputline, " ");
	char * tk2 = strtok (NULL, " ");
	char * tk3 = strtok (NULL, " ");
    char * tk4 = strtok (NULL, " ");

	if (tk1 == NULL) tk1 = "help";

	// --------------------------------------------------------------------
	if (strcmp(tk1, "load")==0){
	// --------------------------------------------------------------------
		if (tk2 != NULL && is_hex(tk2) && tk3 == NULL){
			const uintptr_t addr = strtoull(tk2, NULL, 16);
			load(addr);
		} else printf("Syntax: load address \n\n");

	// --------------------------------------------------------------------
	} 

	// --------------------------------------------------------------------
	else if (strcmp(tk1, "store")==0){
	// --------------------------------------------------------------------
		if (tk2 != NULL && is_hex(tk2) && tk3 != NULL && is_number(tk3) && tk4 == NULL){
			const uintptr_t addr = strtoull(tk2, NULL, 16);
            const uint64_t val = strtoull(tk3, NULL, is_hex(tk3) ? 16 : 10);
            size_t len;
            if(val < (1ULL << 8)) len = 1;
            else if(val < (1ULL << 16)) len = 2;
            else if(val < (1ULL << 32)) len = 4;
            else len = 8;

			store(addr, val, len);
		} else printf("Syntax: store address data\n\n");

	// --------------------------------------------------------------------
	} 

	// --------------------------------------------------------------------
	else if (strcmp(tk1, "dump")==0){
	// --------------------------------------------------------------------
		if (tk2 != NULL && is_hex(tk2) && tk3 != NULL && is_number(tk3) && tk4 == NULL){
			const uintptr_t addr = strtoull(tk2, NULL, 16);
            const uint64_t len = strtoull(tk3, NULL, is_hex(tk3) ? 16 : 10);
			dump(addr, len);
		} else printf("Syntax: dump address length\n\n");

	// --------------------------------------------------------------------
	} 

	// --------------------------------------------------------------------
	else if (strcmp(tk1, "erase")==0){
	// --------------------------------------------------------------------
		if (tk2 != NULL && is_hex(tk2) && tk3 != NULL && is_number(tk3) && tk4 == NULL){
			const uintptr_t addr = strtoull(tk2, NULL, 16);
            const uint64_t len = strtoull(tk3, NULL, is_hex(tk3) ? 16 : 10);
			erase(addr, len);
		} else printf("Syntax: erase address length\n\n");

	// --------------------------------------------------------------------
	} 

    // --------------------------------------------------------------------
	else printf("Commands: load store dump erase \n\n");

}


int main(void)
{   
	printf(
        "\e[2J\e[H" // clear screan
        "=====================================================================\n"
        "      	         Hex Five MultiZone® QNX Application                  \n"
        "    Copyright© 2020 Hex Five Security, Inc. - All Rights Reserved    \n"
        "=====================================================================\n"
        "This version of MultiZone® QNX App is meant for evaluation purposes  \n"
        "only. As such, use of this software is governed by the Evaluation    \n"
        "License. There may be other functional limitations as described in   \n"
        "the evaluation SDK documentation. The commercial version of the      \n"
        "software does not have these restrictions.                           \n"
        "=====================================================================\n"
    );

    printf(
        "QNX Neutrino    : QNX700 \n"
        "NXP i.MX        : i.MX8QM-MEK \n\n"
    );

    struct termios old_tio, new_tio;
	tcgetattr(STDIN_FILENO,&old_tio);
	new_tio=old_tio;
	new_tio.c_lflag &=(~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

    memfd = open("/dev/mem", O_RDWR | O_SYNC | O_DSYNC | O_RSYNC );

    if(memfd < 0){
        printf("\nWARNING: No permissions to access physical memory!\n");
    }
	
    while(1){
        prompt();
		if (readline()){
			cmd_handler();
            inputline[0] = '\0';
		}
    }
}
