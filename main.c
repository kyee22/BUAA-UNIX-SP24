/*  @author: 匡亦煊 Kuang Yixuan (学号: 22371092)
             计算机学院 School of Computer Science and Engineering
	May 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <ctype.h>
#include <string.h>


#define NTOKEN 1024		/* Token     数量上限 */
#define NCMD 8			/* Command   数量上限 */
#define NBUF 32			/* Buf char  数量上限 */
#define BUF_SIZE (NBUF * sizeof(int))
#define panic(errStr) do {{ printf("\033[1;31m%s\n\033[0m", errStr); exit(-1); } } while (0)
char buf[NBUF];

typedef struct {
	char *cmd_file_in,*cmd_file_out,*cmd_file_apd,*cmd_target;
	char **cmd_args;
} Cmd;

typedef enum {
    TOKEN_SYM, TOKEN_STR, TOKEN_PIPE,TOKEN_END,TOKEN_REDIR_IN, TOKEN_REDIR_OUT, TOKEN_REDIR_APD  
} Token_t;

typedef struct {
	Token_t type;
	char *str;
	unsigned int len;
} Token;



void greet_hello();							/*  启动界面  */
void print_current_directory();				/*  打印当前路径  */
void CMD_INIT(Cmd * cmd, int nargc);		/*  初始化 Cmd 结构体  */
void makeToken(Token *token, Token_t type, char *str, unsigned int len);	/*  解析: buf   ->  Token  */
void parseTokens(Token *tokens, char *buf);									/*  解析: Token ->  Cmd    */
void parseCmds(Cmd * cmds, Token *tokens);
void printTokens(Token *tokens);	// for debug
void printCmds(Cmd *cmds); 			// for debug
void bufcpy(char **s, char *buf, size_t len);
void execCmd(Cmd *cmd, int fd_in, int fd_out);
void execCmdAll(Cmd *cmds);

int main() {

	greet_hello();

	while (1) {
		print_current_directory();
		if (fgets(buf, BUF_SIZE, stdin) != NULL) {
			if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
			// if (strcmp(buf, "exit") == 0) {
    		// 	printf("\033[1;33m | ____|_  _(_) |_  |  \\/  |_   _/ ___|| |__   ___| | | \n |  _| \\ \\/ / | __| | |\\/| | | | \\___ \\| '_ \\ / _ \\ | | \n | |___ >  <| | |_  | |  | | |_| |___) | | | |  __/ | | \n |_____/_/\\_\\_|\\__| |_|  |_|\\__, |____/|_| |_|\\___|_|_| \n                            |___/                       \033[0m\n");
			// 	break;
			// }
			Cmd * cmds = (Cmd *) malloc(sizeof(Cmd) * NCMD);
			Token * tokens = (Token *) malloc(sizeof(Token) * NTOKEN);
			parseTokens(tokens, buf);
			// printTokens(tokens);
			parseCmds(cmds, tokens);
			// printCmds(cmds);
			execCmdAll(cmds);
		} else {
			panic("Error Occurred in Input Buffer!");
		}
	}
    return 0;
}

void greet_hello() {
	printf("\033[1;36m           Welcome to MyShell   \033[1;32m       ___                 ___   ___  \n");
	printf("\033[1;36m           author: Kuang Yixuan   \033[1;32m    (   )               (   ) (   ) \n");
    printf("\033[1;36m        studentID: 22371092      \033[1;32m     (   )               (   ) (   ) \033[0m\n");
	printf("\033[1;32m ___ .-. .-.    \033[1;36m___  ___\033[1;32m      .--.     | | .-.     .--.    | |   | |  \n");
    printf("\033[1;32m(   )   '   \\  \033[1;36m(   )(   )\033[1;32m   /  _  \\    | |/   \\   /    \\   | |   | |  \n");
	printf("\033[1;32m |  .-.  .-. ;  \033[1;36m| |  | |\033[1;32m   . .' `. ;   |  .-. .  |  .-. ;  | |   | |  \n");
	printf("\033[1;32m | |  | |  | |  \033[1;36m| |  | |\033[1;32m   | '   | |   | |  | |  |  | | |  | |   | |  \n");
    printf("\033[1;32m | |  | |  | |  \033[1;36m| '  | |\033[1;32m   _\\_`.(___)  | |  | |  |  |/  |  | |   | |  \n");
	printf("\033[1;32m | |  | |  | |  \033[1;36m'  `-' |\033[1;32m  (   ). '.    | |  | |  |  ' _.'  | |   | |  \n");
	printf("\033[1;32m | |  | |  | |  \033[1;36m `.__. |\033[1;32m   | |  `\\ |   | |  | |  |  .'.-.  | |   | |  \n");
    printf("\033[1;32m | |  | |  | |  \033[1;36m ___ | |\033[1;32m   ; '._,' '   | |  | |  '  `-' /  | |   | |  \n");
	printf("\033[1;32m(___)(___)(___) \033[1;36m(   )' |\033[1;32m    '.___.'   (___)(___)  `.__.'  (___) (___) \n");
	printf("\033[1;32m                 \033[1;36m; `-' '\033[1;32m                                              \n");
	printf("\033[1;32m                  \033[1;36m.__.'\033[1;32m                                               \n\033[31m");
}

void print_current_directory(){
    char *path = getcwd(NULL, 0);
    printf("\033[0;36m%s\033[0m%s\033[1;36m%s\033[0m $ ","myShell",":", path);
    free(path);
}

void CMD_INIT(Cmd * cmd, int nargc) {
	cmd->cmd_file_in = NULL;
	cmd->cmd_file_out = NULL;
	cmd->cmd_file_apd = NULL;
	cmd->cmd_target = NULL;
	cmd->cmd_args = (char **)malloc(nargc * sizeof(char *));
}
void makeToken(Token *token, Token_t type, char *str, unsigned int len) {
	token->type = type;
	token->str = str;
	token->len = len;
}
void parseTokens(Token *tokens, char *buf) {
	char *start;
	while (*buf != '\0')
	{
		while ((*buf != '\0') && isspace(*buf)) ++buf;
		if (*buf == '\0') break;

		switch (*buf)
		{
			case '<':
				makeToken(tokens, TOKEN_REDIR_IN, NULL, 0);
				++buf;	// to next...
				break;
			case '>':
				++buf;	// check whether is '>>' or '>'
				if (*buf == '>') makeToken(tokens, TOKEN_REDIR_APD, NULL, 0),++buf;	// Remember move to next...
				else makeToken(tokens, TOKEN_REDIR_OUT, NULL, 0);
				break;
			case '|':
				makeToken(tokens, TOKEN_PIPE, NULL, 0);
				++buf;	// to next...
				break;
			case '\'':
			case '\"':
				start = buf;
				++buf;	// skip 左引号
				while ((*buf != '\0') && (*buf != *start)) {
					if ((*buf == '\\') && (*(buf+1) != '\0')) {
						if (*(buf + 1) != '\0') {
							size_t len = strlen(buf);
							memmove(buf, buf+1, len);
						} else panic("Error Occurred in Escape Character near Your String!");
					} 
					++buf;
                }
				if (*buf == '\0') panic("Error Occurred in Quotation Marks near Your String!");
				makeToken(tokens, TOKEN_STR, start+1, buf-start-1);
				++buf;	// to next...
				break;
			default:
				start = buf;
				while ((*buf != '\0') && (!isspace(*buf)) && (*buf != '|') && (*buf != '<') && (*buf != '>')) {
					++buf;
				}
				makeToken(tokens, TOKEN_SYM, start, buf-start); // Here do not move to next!!!
				break;
		}
		++tokens;
	}
	makeToken(tokens, TOKEN_END, NULL, 0);
}
void parseCmds(Cmd *cmds, Token *tokens) {
	int nargc;
	char **args, **dstp;
	Token *tmp;
	while (tokens->type != TOKEN_END) {
		nargc = 0;
		tmp = tokens;
		while (tmp->type != TOKEN_END && tmp->type != TOKEN_PIPE) ++nargc,++tmp;
		CMD_INIT(cmds, nargc + 1); // we alloc 1 more arg space with 0-padding to indicate END of args
		args = cmds->cmd_args;
		int hit_target = 0;
		
		while (tokens->type != TOKEN_END && tokens->type != TOKEN_PIPE) {
			switch (tokens->type)
			{
				case TOKEN_STR:
				case TOKEN_SYM:
					if (!hit_target) {
						if (tokens->type == TOKEN_SYM) {
							hit_target = 1;
							bufcpy(&(cmds->cmd_target), tokens->str, tokens->len);
						} else panic("Error Occurred in your command format!");
					}
					bufcpy(args, tokens->str, tokens->len);
					++args;
					++tokens;
					break;
				case TOKEN_REDIR_APD:
				case TOKEN_REDIR_IN:
				case TOKEN_REDIR_OUT:
					dstp = (tokens->type == TOKEN_REDIR_IN) ? &cmds->cmd_file_in:
						   (tokens->type == TOKEN_REDIR_OUT) ? &cmds->cmd_file_out: &cmds->cmd_file_apd;
					++tokens;	// skip to get the redirection path
					if (tokens->type != TOKEN_SYM && tokens->type != TOKEN_STR) panic("Error Occurred in your redirection path!");
					bufcpy(dstp, tokens->str, tokens->len);
					++tokens;	// skip the redirection path to next...
					break;
				default:
					break;
			}
		}
		*args = (char *)malloc(sizeof(char));
		*args = 0;
		++cmds;
		if (tokens->type == TOKEN_PIPE) {
			++tokens;
			if (tokens->type == TOKEN_END) panic("Error Occurred near you pipe! \nEmpty command after pipe!");
		}
	}
	cmds->cmd_target = NULL;
}

void printTokens(Token *tokens) {
	int tot = 0;
	while (tokens->type != TOKEN_END) {
		char *s;
		switch (tokens->type)
		{
		case TOKEN_PIPE:
			s = "|";
			break;
		case TOKEN_REDIR_APD:
			s = ">>";
			break;
		case TOKEN_REDIR_OUT:
			s = ">";
			break;
		case TOKEN_REDIR_IN:
			s = "<";
			break;
		case TOKEN_SYM:
		case TOKEN_STR:
		default:
			s = (char *) malloc(sizeof(char) * tokens->len + 1);
			memcpy(s, tokens->str, tokens->len);
			s[tokens->len] = '\0';
		}
		char* type_list[] = {"TOKEN_SYM", "TOKEN_STR", "TOKEN_PIPE","TOKEN_END","TOKEN_REDIR_IN", "TOKEN_REDIR_OUT", "TOKEN_REDIR_APD" } ;

		printf("(token %d) %s : %s\n", ++tot, type_list[tokens->type], s);
		++tokens;
	}
}
void printCmds(Cmd *cmds) {
	int tot = 0;
	while (cmds->cmd_target != NULL)
	{
		printf("(command %d) [%s]: ",++tot, cmds->cmd_target);
		char **args = cmds->cmd_args;
		while (*args != 0) {
			printf("[%s]; ", *args);
			++args;
		}
		if (cmds->cmd_file_in) printf(" < [%s]",cmds->cmd_file_in);
		if (cmds->cmd_file_out) printf(" > [%s]",cmds->cmd_file_out);
		if (cmds->cmd_file_apd) printf(" >> [%s]",cmds->cmd_file_apd);
		printf("\n");
		++cmds;
	}
	
}
void bufcpy(char **s, char *buf, size_t len) {
	*s = (char *)malloc(sizeof(char) * (len + 1));
	memcpy(*s, buf, len);
	(*s)[len] = '\0';
}
void execCmd(Cmd *cmd, int fd_in, int fd_out) {
	if (strcmp(cmd->cmd_target, "exit") == 0) {
    	printf("\033[1;33m | ____|_  _(_) |_  |  \\/  |_   _/ ___|| |__   ___| | | \n |  _| \\ \\/ / | __| | |\\/| | | | \\___ \\| '_ \\ / _ \\ | | \n | |___ >  <| | |_  | |  | | |_| |___) | | | |  __/ | | \n |_____/_/\\_\\_|\\__| |_|  |_|\\__, |____/|_| |_|\\___|_|_| \n                            |___/                       \033[0m\n");
		panic("My Shell has closed.");
	} else if (strcmp(cmd->cmd_target, "cd") == 0) {
		if (chdir(cmd->cmd_args[1]) != 0) panic("Error Occurred in you cd path! \nCheck your path!");
	} else {
		pid_t pid = fork();		// fork 失败
		if (pid < 0) panic("Fork Failed!");
		else if (pid == 0) {	// 子进程
			if (cmd->cmd_file_in) 		dup2(open(cmd->cmd_file_in, O_RDONLY, 0666), STDIN_FILENO);
			else if (fd_in > 0) 		dup2(fd_in, STDIN_FILENO);

			if (cmd->cmd_file_out)		dup2(open(cmd->cmd_file_out, O_RDWR | O_CREAT, 0666), STDOUT_FILENO);
			else if (cmd->cmd_file_apd)	dup2(open(cmd->cmd_file_apd, O_WRONLY | O_CREAT | O_APPEND, 0666), STDOUT_FILENO);
			else if (fd_out > 0)		dup2(fd_out, STDOUT_FILENO);
			
			execvp(cmd->cmd_target, cmd->cmd_args);

			panic("Unreachable Code! \nError Occurred in the EXECUTION of your command!");
		} else {				// 父进程
			int status;
			waitpid(0, &status, 0);
			if (!WIFEXITED(status)) panic("Error! \nForked subprocess doesn't exit normal!");
		}
	}
}
void execCmdAll(Cmd *cmds) {
	size_t ncmds = 0, cmdno = 0;
	Cmd *tmp = cmds;
	while (tmp->cmd_target != NULL) ++ncmds, ++tmp;

	int fd[2];
	int prev_out_fd = -1;

	while (cmds->cmd_target != NULL) {
		pipe(fd);
		execCmd(cmds, (cmdno == 0) ? -1 : prev_out_fd, (cmdno == ncmds - 1) ? -1 : fd[1]);
		
		close(fd[1]);
        if (prev_out_fd > 0) close(prev_out_fd);
        prev_out_fd = fd[0];
		++cmds,++cmdno;
	}
	if (prev_out_fd > 0) close(prev_out_fd);
}
