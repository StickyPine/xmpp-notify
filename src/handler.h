#ifndef HANDLER_H
#define HANDLER_H

#include "common.h"
#include "stanza.h"

#include <strophe.h>


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
	int i;

	(void)error;
	(void)stream_error;

	if (status == XMPP_CONN_CONNECT){

		stanza = xmpp_presence_new(info->ctx);
		xmpp_send(conn, stanza);
		xmpp_stanza_release(stanza);

		if (info->chatrooms){
			for(i = 0; i < info->chatroomsNum; ++i){
				chatName = *((info->chatrooms) + i);

				stanza = stanza_create_room_join_presence(info->ctx, chatName, NULL);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);

				stanza = xmpp_message_new(info->ctx, "groupchat", chatName, uuid);
				xmpp_message_set_body(stanza, info->msg);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
			}
		}

		if (info->chats){
			for(i = 0; i < info->chatsNum; ++i){
				stanza = xmpp_message_new(info->ctx, "chat", *((info->chats) + i), uuid);
				xmpp_message_set_body(stanza, info->msg);
				xmpp_send(conn, stanza);
				xmpp_stanza_release(stanza);
		}
	}
	xmpp_disconnect(conn);
}

#endif

