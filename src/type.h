#ifndef TYPE_H
#define TYPE_H

#include <strophe.h>

typedef struct ConnInfo {
	xmpp_ctx_t *ctx;
	char **chatrooms;
	char **chatroomsNick;
	char **chats;
	char *msg;
	int chatroomsNum;
	int chatsNum;
} connInfo;



#endif

