#ifndef HANDLER_H
#define HANDLER_H

#include <strophe.h>

void
conn_handler_message_on_connect(xmpp_conn_t *conn,
                            xmpp_conn_event_t status,
                            int error,
                            xmpp_stream_error_t *stream_error,
                            void *userdata);


#endif

