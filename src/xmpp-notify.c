#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strophe.h>

#include "type.h"
#include "handler.h"

#define OPT_STRING "j:p:m:C:M:P:"


int
parse_arg(int argc, char** argv, xmpp_ctx_t *ctx, char ***chatrooms, char ***chats, char **msg,
           int *chatroomsNum, int *chatsNum, char **jid, char **passwd, int *port)
{
	int c;
	short j_c = 0, m_c = 0, C_c = 0, M_c = 0, p_c = 0, P_c = 0;

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
			if (!p_c) {
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
			if (C_c) {
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
			if (!P_c) {
				*port = atoi(optarg);
				++P_c;
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

	return 1;
}


int
main(int argc, char** argv)
{
	xmpp_conn_t *conn;
	xmpp_log_t *log;

	connInfo info;
	char *jid, *pass;
	int port = 5222;
	xmpp_initialize();
	/* log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG); */
	info.ctx = xmpp_ctx_new(NULL, NULL);

	if (parse_arg(argc, argv, info.ctx, &info.chatrooms, &info.chats,
                  &info.msg, &info.chatroomsNum, &info.chatsNum,
				  &jid, &pass, &port) > 0) {

		conn = xmpp_conn_new(info.ctx);

		xmpp_conn_set_jid(conn, jid);
		xmpp_conn_set_pass(conn, pass);

		if(xmpp_connect_client(conn, NULL, port, conn_handler_message_on_connect, &info) >= 0) {
			xmpp_run(info.ctx);

			xmpp_ctx_free(info.ctx);
			xmpp_shutdown();
			return 0;
		} else {
			fprintf(stderr, "connection failed");
			return -1;
		}

	} else {
		return -1;
	}

}

