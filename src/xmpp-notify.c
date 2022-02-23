
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strophe.h>

#define OPT_STRING "j:p:m:C:M:P:"

#define STANZA_NAME_X "x"
#define STANZA_NS_MUC "http://jabber.org/protocol/muc"

typedef struct ConnInfo {
	xmpp_ctx_t *ctx;
	char **chatrooms;
	char **chatroomsNick;
	char **chats;
	char *msg;
	int chatroomsNum;
	int chatsNum;
} connInfo;


xmpp_stanza_t*
stanza_create_room_join_presence(xmpp_ctx_t* const ctx,
                                 const char* const full_room_jid, const char* const passwd){
	xmpp_stanza_t* presence = xmpp_presence_new(ctx);
    xmpp_stanza_set_to(presence, full_room_jid);
    /* _stanza_add_unique_id(presence); */
	xmpp_stanza_set_id(presence, xmpp_uuid_gen(ctx));

    xmpp_stanza_t* x = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(x, STANZA_NAME_X);
    xmpp_stanza_set_ns(x, STANZA_NS_MUC);

    if (passwd) {
        xmpp_stanza_t* pass = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(pass, "password");
        xmpp_stanza_t* text = xmpp_stanza_new(ctx);
        xmpp_stanza_set_text(text, passwd);
        xmpp_stanza_add_child(pass, text);
        xmpp_stanza_add_child(x, pass);
        xmpp_stanza_release(text);
        xmpp_stanza_release(pass);
    }

    xmpp_stanza_add_child(presence, x);
    xmpp_stanza_release(x);

    return presence;
}

int
parse_arg(int argc, char** argv, char ***chatrooms, char ***chats, char **msg,
           int *chatroomsNum, int *chatsNum, char **jid, char **passwd, int *port)
{
	int c;
	short j_c = 0, s_c = 0, m_c = 0, C_c = 0, M_c = 0, p_c = 0, P_c = 0;
	char *token;

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


void
conn_handler_message_on_connect(xmpp_conn_t *conn,
                            xmpp_conn_event_t status,
                            int error,
                            xmpp_stream_error_t *stream_error,
                            void *userdata)
{
	const connInfo *const info = (connInfo*)userdata;
	const char *const uuid = xmpp_uuid_gen(info->ctx);
	xmpp_stanza_t *stanza;
	const char *chatName;
	char *chatroomPresence;
	const char *const jidNode = xmpp_jid_node(info->ctx, xmpp_conn_get_jid(conn));
	int i;

	(void)error;
	(void)stream_error;

	if (status == XMPP_CONN_CONNECT){
		/* xmpp_handler_add(conn, version_handler, "jabber:iq:version", "iq", NULL, */
                         /* info->ctx); */
		stanza = xmpp_presence_new(info->ctx);
		xmpp_send(conn, stanza);
		xmpp_stanza_release(stanza);

		if (info->chatrooms){
			for(i = 0; i < info->chatroomsNum; ++i){
				chatName = *(info->chatrooms + i);

				chatroomPresence =
					(char*)malloc(sizeof(char)
                    * (strlen(jidNode) + strlen(chatName)) + 2);
				strcpy(chatroomPresence, chatName);
				strcat(chatroomPresence, "/");
				strcat(chatroomPresence, jidNode);

				stanza = stanza_create_room_join_presence(info->ctx,
                                                          chatroomPresence, NULL);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
				free(chatroomPresence);

				stanza = xmpp_message_new(info->ctx, "groupchat", chatName, uuid);
				xmpp_message_set_body(stanza, info->msg);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
			}
		}

		if (info->chats){
			for(i = 0; i < info->chatsNum; ++i){
				stanza = xmpp_message_new(info->ctx, "chat", *(info->chats + i), uuid);
				xmpp_message_set_body(stanza, info->msg);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
			}
		}
	}
	/* xmpp_stop(info->ctx); */
}



int
main(int argc, char** argv)
{
	xmpp_conn_t *conn;
	xmpp_log_t *log;

	connInfo info;
	char *jid, *pass;
	int port = 5222;

	if (parse_arg(argc, argv, &info.chatrooms, &info.chats,
                  &info.msg, &info.chatroomsNum, &info.chatsNum,
				  &jid, &pass, &port) > 0) {

		printf("\n\n%i\n\n", port);
		xmpp_initialize();
		log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
		info.ctx = xmpp_ctx_new(NULL, log);
		conn = xmpp_conn_new(info.ctx);

		xmpp_conn_set_jid(conn, jid);
		xmpp_conn_set_pass(conn, pass);

		xmpp_connect_client(conn, NULL, port, conn_handler_message_on_connect, &info);
		xmpp_run(info.ctx);

		xmpp_conn_release(conn);
		xmpp_ctx_free(info.ctx);
		xmpp_shutdown();

	} else {
		return -1;
	}

}

