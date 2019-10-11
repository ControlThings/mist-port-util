#pragma once

#include "mist_app.h"

void identity_export_base64(mist_app_t *app, uint8_t local_uid[WISH_ID_LEN]);

void generate_contact_card_base64(mist_app_t *mist_app);