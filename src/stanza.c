#include <strophe.h>

#include "stanza.h"


#define STANZA_NAME_X "x"
#define STANZA_NS_MUC "http://jabber.org/protocol/muc"


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

