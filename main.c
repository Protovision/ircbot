#include "irc.h"

FILE *irc;
char *owner, *nick;

int main(int argc, const char **argv) {
	char line[512];
	struct irc_message msg;
	array args;
	char *cmd, *output;
	struct bot_command *botcmd;

	if (argc != 5) {
		die(
			"An IRC bot in C\n"
			"Usage: [SERVER] [PORT] [OWNER] [NICK]\n"
		);
		return 1;
	}

	init_bot_commands();

	irc = dial(argv[1], argv[2]);
	if (irc == NULL) die("Failed to connect\n");
	setbuf(irc, NULL);

	owner = new_string(argv[3]);
	nick = new_string(argv[4]);
	fprintf(irc, "USER %s * * :%s\r\nNICK %s\r\n", nick, owner, nick);

	array_init(&args, sizeof(char*));	
	while (fgets(line, 512, irc)) {
		fprintf(stdout, "%s\n", chomp_string(line));
		irc_parse(&msg, line);
		if (strcmp(msg.command, "001") == 0) {
			fprintf(irc, "MODE %s +B\r\n", nick);
		} else if (strcmp(msg.command, "PING") == 0) {
			fprintf(irc, "PONG %s\r\n", *(char**)array_at(&msg.args, 0));
		} else if (
			strcmp(msg.command, "PRIVMSG") == 0 ||
			strcmp(msg.command, "NOTICE") == 0
		) {
			split_string(&args, msg.data, " ");
			if (strcmp(msg.command, "PRIVMSG") == 0) {
				if (strcmp(*(char**)array_at(&args, 0), nick) != 0) {
					array_clear(&args);
					continue;
				} else array_remove(&args, 0, 1);
			}
			cmd = new_string(*(char**)array_at(&args, 0));
			array_remove(&args, 0, 1);
		
			botcmd = bot_command_find(cmd);
			if (botcmd != NULL) {
				if (botcmd->cmd[0] == '@') {
					if (strcmp(msg.sender, owner)) {
						free(cmd);
						array_clear(&args);
						continue;
					}
				}
				output = botcmd->func(&args, &msg);
				if (output != NULL) { 
					if (strcmp(msg.command, "NOTICE") == 0)
						fprintf(irc, "NOTICE %s :%s\r\n", msg.sender, output);
					else
						fprintf(irc, "PRIVMSG %s :%s\r\n", *(char**)array_at(&msg.args, 0), output);
					free(output);
				}
			}
			free(cmd);
			array_clear(&args);
		}
	}
	fclose(irc);
	cleanup_bot_commands();
}
