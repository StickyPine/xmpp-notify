#include "iq.h"

#include <strophe.h>
#include <stdlib.h>
#include <string.h>


int
_http_upload_response_id_handler(xmpp_stanza_t* const stanza, void* const userdata)
{
    HTTPUpload* upload = (HTTPUpload*)userdata;
    const char* from = xmpp_stanza_get_from(stanza);
    const char* type = xmpp_stanza_get_type(stanza);

    if (from) {
        log_info("Received http_upload response from: %s", from);
    } else {
        log_info("Received http_upload response");
    }

    // handle error responses
    if (g_strcmp0(type, STANZA_TYPE_ERROR) == 0) {
        char* error_message = stanza_get_error_message(stanza);
        if (from) {
            cons_show_error("Uploading '%s' failed for %s: %s", upload->filename, from, error_message);
        } else {
            cons_show_error("Uploading '%s' failed: %s", upload->filename, error_message);
        }
        free(error_message);
        return 0;
    }

    xmpp_stanza_t* slot = xmpp_stanza_get_child_by_name(stanza, STANZA_NAME_SLOT);

    if (slot && g_strcmp0(xmpp_stanza_get_ns(slot), STANZA_NS_HTTP_UPLOAD) == 0) {
        xmpp_stanza_t* put = xmpp_stanza_get_child_by_name(slot, STANZA_NAME_PUT);
        xmpp_stanza_t* get = xmpp_stanza_get_child_by_name(slot, STANZA_NAME_GET);

        if (put && get) {
            const char* put_url = xmpp_stanza_get_attribute(put, "url");
            upload->put_url = strdup(put_url);
            const char* get_url = xmpp_stanza_get_attribute(get, "url");
            upload->get_url = strdup(get_url);

            // Optional "authorization", "cookie", "expires" headers
            upload->authorization = upload->cookie = upload->expires = NULL;
            xmpp_stanza_t* header = xmpp_stanza_get_children(put);
            for (; header; header = xmpp_stanza_get_next(header)) {
                if (g_strcmp0(xmpp_stanza_get_name(header), STANZA_NAME_HEADER) == 0) {
                    const char* header_name = xmpp_stanza_get_attribute(header, STANZA_ATTR_NAME);
                    if (g_strcmp0(header_name, STANZA_HEADER_AUTHORIZATION) == 0) {
                        upload->authorization = xmpp_stanza_get_text(header);
                    } else if (g_strcmp0(header_name, STANZA_HEADER_COOKIE) == 0) {
                        upload->cookie = xmpp_stanza_get_text(header);
                    } else if (g_strcmp0(header_name, STANZA_HEADER_EXPIRES) == 0) {
                        upload->expires = xmpp_stanza_get_text(header);
                    } else {
                        log_warning("[HTTP upload] unknown header: %s", header_name);
                    }
                }
            }

            pthread_create(&(upload->worker), NULL, &http_file_put, upload);
            http_upload_add_upload(upload);
        } else {
            log_error("Invalid XML in HTTP Upload slot");
            return 1;
        }
    }

    return 0;
}

