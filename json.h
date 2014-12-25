/*
 * json.h
 *
 *  Created on: 23/07/2014
 *      Author: glenn
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "jsmn.h"

char * json_fetch(char *url);
jsmntok_t * json_tokenise(const char *js);
bool json_token_streq(const char *js, jsmntok_t *t, char *s);
const char * json_token_tostr(const char *js, jsmntok_t *t);
