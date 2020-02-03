/**
 * Copyright (C) 2020, ControlThings Oy Ab
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * @license Apache-2.0
 */
#pragma once

#include "mist_app.h"

void identity_export_base64(mist_app_t *app, uint8_t local_uid[WISH_ID_LEN]);

void generate_contact_card_base64(mist_app_t *mist_app);