#include <stdint.h>
#include <string.h>

#include "wish_debug.h"
#include "wish_app.h"
#include "bson.h"
#include "bson_visit.h"
#include "mbedtls/base64.h"

#include "export_contact_base64.h"


static void base64_print_formatted(uint8_t *raw_doc, size_t raw_doc_len) {
	size_t base64_str_max_len = raw_doc_len * 2;
	char* base64_str = malloc(base64_str_max_len);
	memset(base64_str, 0, base64_str_max_len);
	size_t olen = 0;
	if (mbedtls_base64_encode((unsigned char*) base64_str, base64_str_max_len, &olen, (const unsigned char*) raw_doc, raw_doc_len) != 0) {
		WISHDEBUG(LOG_CRITICAL, "base64 encode error");
	}
	else {
		//WISHDEBUG(LOG_CRITICAL, "Base64 (%i): %s", olen, base64_str);
		WISHDEBUG(LOG_CRITICAL, "Base64, total len = %i:", olen);
		/* Print the data, 'per_line' characters per line */
		size_t i = 0;
		const int per_line = 72;
		while (i < olen) {
			char buf[per_line + 2];
			memset(buf, 0, per_line+2);
			memcpy(buf, base64_str+i, per_line);
			i+=per_line;
			WISHDEBUG(LOG_CRITICAL, "%s", buf);
		}

	}

	free(base64_str);
}

static void identity_export_cb(struct wish_rpc_entry* req, void* ctx, const uint8_t* data, size_t data_len) {
    bson_visit("identity_export_cb", data);

    bson_iterator it;

    BSON_ITERATOR_FROM_BUFFER(&it, data);
    bson_find_fieldpath_value("data.data", &it);
    bson_visit("Contact data.data", (const uint8_t*) bson_iterator_bin_data(&it));

    BSON_ITERATOR_FROM_BUFFER(&it, data);
    bson_find_fieldpath_value("data", &it);
    if (bson_iterator_type(&it) == BSON_OBJECT) {
    	bson export_bs;
    	bson_iterator_subobject(&it, &export_bs);
    	bson_visit("data doc", (const uint8_t*) bson_data(&export_bs));

    	WISHDEBUG(LOG_CRITICAL, "Here is the the local contact card, base64 encoded:");

    	base64_print_formatted((uint8_t *) bson_data(&export_bs), bson_size(&export_bs));

    	WISHDEBUG(LOG_CRITICAL, "(Remember to strip out the newlines before pasting it into wish-cli!)");
    }
    else {
    	WISHDEBUG(LOG_CRITICAL, "No subobject!");
    }
}

void identity_export_base64(mist_app_t *mist_app, uint8_t local_uid[WISH_ID_LEN]) {
    bson bs;
    const size_t buf_sz = 100;
    uint8_t buf[buf_sz];
    bson_init_buffer(&bs, (char *) buf, buf_sz);
    bson_append_string(&bs, "op", "identity.export");
    bson_append_start_array(&bs, "args");
    bson_append_binary(&bs, "0", (char *) local_uid, WISH_ID_LEN);
    bson_append_finish_array(&bs);
    bson_append_int(&bs, "id", 0);
    bson_finish(&bs);

    wish_app_request(mist_app->app,  &bs, identity_export_cb, NULL);
}

static void core_list_identities_cb(struct wish_rpc_entry* req, void* ctx, const uint8_t* data, size_t data_len) {
    bson_visit("core_list_identities_cb", data);
    uint8_t local_uid[WISH_ID_LEN];

    mist_app_t *mist_app = ctx;

    bson_iterator it;
    BSON_ITERATOR_FROM_BUFFER(&it, data);
    bson_find_fieldpath_value("data.0.uid", &it);
    if (bson_iterator_type(&it) == BSON_BINDATA) {
    	memcpy(local_uid, bson_iterator_bin_data(&it), bson_iterator_bin_len(&it));
    }
    else {
    	WISHDEBUG(LOG_CRITICAL, "Could not recover local uid!");
    	return;
    }

    identity_export_base64(mist_app, local_uid);
}

void generate_contact_card_base64(mist_app_t *mist_app) {
    bson bs;
    const size_t buf_sz = 100;
    char buf[buf_sz];
    
    bson_init_buffer(&bs, buf, buf_sz);
    bson_append_string(&bs, "op", "identity.list");
    bson_append_start_array(&bs, "args");
    bson_append_finish_array(&bs);
    bson_append_int(&bs, "id", 0);
    bson_finish(&bs);
    wish_app_request(mist_app->app,  &bs, core_list_identities_cb, NULL);
}