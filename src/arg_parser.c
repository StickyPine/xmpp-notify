#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"


#define OPT_STRING "j:p:m:C:M:P:"


int
parse_arg(int argc, char** argv, char ***chatrooms, char ***chats, char **msg,
           int *chatroomsNum, int *chatsNum, char **jid, char **passwd, int *port)
{
	int c;
	int j_c = 0, s_c = 0, m_c = 0, C_c = 0, M_c = 0, p_c = 0;

	while ((c = getopt(argc, argv, OPT_STRING)) > 0) {
		switch (c) {
		case 'C':
			++C_c;
			break;
		case 'M':
			++M_c;
			break;
		}
	}

	if (!C_c && !M_c) {
		fprintf(stderr, "Missing destination, use at least one -C or -M\n");
		return -1;
	}

	*chatroomsNum = M_c;
	*chatsNum = C_c;

	if (M_c)
		*chatrooms = (char**)malloc(sizeof(char*) * M_c);
	if (C_c)
		*chats = (char**)malloc(sizeof(char*) * C_c);

	optind = 1; // reset optget

	while ((c = getopt(argc, argv, OPT_STRING)) > 0) {
		switch (c) {
		case 'j':
			if (!j_c) {
				*jid = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
				strcpy(*jid, optarg);
				++j_c;
			} else {
				fprintf(stderr, "Only provide one jabber id\n");
				return -1;
			}
			break;

		case 'p':
			if (!s_c) {
				*passwd = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
				strcpy(*passwd, optarg);
				++p_c;
			} else {
				fprintf(stderr, "Only provide one password\n");
				return -1;
			}
			break;

		case 'm':
			if (!m_c) {
				*msg = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
				strcpy(*msg, optarg);
				++m_c;
			} else {
				fprintf(stderr, "Only provide one message\n");
				return -1;
			}
			break;

		case 'C':
			if (C_c > 0) {
				--C_c;
				*(*chats + C_c) = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
				strcpy(*(*chats + C_c), optarg);
			}
			break;

		case 'M':
			if (M_c) {
				--M_c;
				*(*chatrooms + M_c) = (char*)malloc(sizeof(char) * strlen(optarg) + 1);
				strcpy(*(*chatrooms + M_c), optarg);
			}
			break;

		case 'P':
			if (!p_c) {
				*port = atoi(optarg);
				++p_c;
			} else {
				fprintf(stderr, "specify at most one port number");
				return -1;
			}
		}
	}

	if (!j_c) {
		fprintf(stderr, "set a jabber id -j\n");
		return -1;
	}
	if (!p_c) {
		fprintf(stderr, "set a jabber password\n");
		return -1;
	}
	if (!m_c) {
		fprintf(stderr, "specify the message to be send\n");
		return -1;
	}

	return 0;
}


int main(int argc, char** argv){

	connInfo *info = (connInfo*)malloc(sizeof(connInfo));
	char *jid = NULL, *passwd = NULL;
	int port;

	parse_arg(argc, argv, &info->chatrooms, &info->chats, &info->msg,
              &info->chatroomsNum, &info->chatsNum, &jid, &passwd, &port);

	printf("%s\n", *info->chats+0);
	/* printf("%s\n", *info->chats); */




}

