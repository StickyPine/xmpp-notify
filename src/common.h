#include <strophe.h>

#ifndef COMMON_H
#define COMMON_H

typedef struct ConnInfo {
	xmpp_ctx_t *const ctx;
	char **chatrooms;
	char **chats;
	char *msg;
	int chatroomsNum;
	int chatsNum;
} connInfo;


#endif

