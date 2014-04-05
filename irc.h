#ifndef IRC_H 
#define IRC_H 
#include "array.h"
#include "strmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#define die(M, ...) do { fprintf(stderr, M, ##__VA_ARGS__); exit(1); } while(0)

struct irc_message {
	char *sender;
	char *command;
	array args;
	char *data;
};

void irc_parse(struct irc_message *msg, const char *string);
char *new_string(const char *value);
FILE *dial(const char *host, const char *port);
char *chomp_string(char *buf);
void split_string(array *output, const char *string, const char *delim);
void join_string(array *a, size_t i, char *buf, const char *delim);

extern char *owner, *nick;
extern FILE *irc;

struct bot_command {
	const char *cmd;
	char* (*func)(array *args, struct irc_message *msg);
};

extern struct bot_command bot_cmds[];

struct bot_command *bot_command_find(const char *cmd);

extern void (*command_init_funcs[])();
extern void (*command_cleanup_funcs[])();

void init_bot_commands();
void cleanup_bot_commands();

#endif
