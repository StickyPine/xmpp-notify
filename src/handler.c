#include <strophe.h>
#include <string.h>
#include <stdlib.h>

#include "type.h"
#include "handler.h"
#include "stanza.h"


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

	if (status == XMPP_CONN_CONNECT) {
		stanza = xmpp_presence_new(info->ctx);
		xmpp_send(conn, stanza);
		xmpp_stanza_release(stanza);

		if (info->chatrooms) {
			for(i = 0; i < info->chatroomsNum; ++i){
				chatName = *(info->chatrooms + i);

				chatroomPresence = (char*)malloc(sizeof(char)
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

		if (info->chats) {
			for(i = 0; i < info->chatsNum; ++i){
				stanza = xmpp_message_new(info->ctx, "chat", *(info->chats + i), uuid);
				xmpp_message_set_body(stanza, info->msg);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
			}
		}
		xmpp_disconnect(conn);
	} else
		xmpp_stop(info->ctx);
}

