#ifndef STANZA_H
#define STANZA_H

#include <strophe.h>

xmpp_stanza_t*
stanza_create_room_join_presence(xmpp_ctx_t* const ctx,
                                 const char* const full_room_jid, const char* const passwd);


#endif

