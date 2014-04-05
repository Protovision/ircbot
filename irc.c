#include "irc.h"

char *new_string(const char *value) {
	char *s = (char*)malloc(strlen(value) + 1);
	strcpy(s, value);
	return s;
}

void irc_parse(struct irc_message *msg, const char *line) {
	char buf[512];
	char *s, *saveptr, *token, *arg;

	strcpy(buf, line);
	array_init(&msg->args, sizeof(char*));
	token = strtok_r(buf, " ", &saveptr);
	if (token[0] != ':') {
		msg->command = new_string(token);
		while (token = strtok_r(NULL, " ", &saveptr)) {
			arg = new_string(token);
			array_push(&msg->args, &arg);
		}
		msg->sender = NULL;
		msg->data = NULL;
	} else {
		s = strchr(++token, '!');
		if (s != NULL) *s = 0;
		msg->sender = new_string(token);
		msg->command = new_string(strtok_r(NULL, " ", &saveptr));
		while (token = strtok_r(NULL, " ", &saveptr)) {
			if (token[0] == ':') break;
			arg = new_string(token);
			array_push(&msg->args, &arg);
		}
		if (token) {
			token[strlen(token)] = ' ';
			msg->data = new_string(++token);	
		} else msg->data = NULL;
	}
}

FILE* dial(const char *host, const char *port) {
	int sockfd;
	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(host, port, &hints, &ai))
		return NULL;	
	sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sockfd == -1) return NULL;
	if (connect(sockfd, ai->ai_addr, ai->ai_addrlen)) return NULL;
	freeaddrinfo(ai);
	return fdopen(sockfd, "r+");
}

char *chomp_string(char *buf) {
	register char *s;
	s = strpbrk(buf, "\r\n");
	if (s != NULL) *s = 0;
	return buf;	
}

void split_string(array *output, const char *string, const char *delim) {
	char buf[512];
	char *token, *saveptr, *arg;
	strcpy(buf, string);
	for (
		token = strtok_r(buf, delim, &saveptr);
		token != NULL;
		token = strtok_r(NULL, delim, &saveptr)
	) {
		arg = new_string(token);
		array_push(output, &arg);
	}
}

void join_string(array *a, size_t i, char *buf, const char *delim) {
	char *s;

	buf[0] = 0;
	for (s = buf; i != array_count(a); ++i) {
		strcat(buf, *(char**)array_at(a, i));
		strcat(buf, delim);
	}
	s = buf + strlen(buf);
	if (strcmp(s - strlen(delim), delim) == 0)
		*(s - strlen(delim)) = 0;
}

struct bot_command *bot_command_find(const char *cmd) {
	struct bot_command *p;
	for (p = bot_cmds; p->cmd != NULL; ++p) {
		if (strcmp(p->cmd, cmd) == 0) return p;
	}
	return NULL;
}

