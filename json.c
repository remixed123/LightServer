/*
 * json.c
 *
 *  Obtained from the JSMN Project - http://zserge.com/jsmn.html
 */

//#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "buf.h"
//#include "log.h"

/* XDCtools Header files */
#include <xdc/runtime/System.h>

#define BUFFER_SIZE 32768
#define JSON_TOKENS 256

//static size_t fetch_data(void *buffer, size_t size, size_t nmemb, void *userp)
//{
//    buf_t *buf = (buf_t *) userp;
//    size_t total = size * nmemb;
//
//    if (buf->limit - buf->len < total)
//    {
//        buf = buf_size(buf, buf->limit + total);
//        log_null(buf);
//    }
//
//    buf_concat(buf, buffer, total);
//
//    return total;
//}
//
//char * json_fetch(char *url)
//{
//    CURL *curl = curl_easy_init();
//    log_null(curl);
//
//    curl_easy_setopt(curl, CURLOPT_URL, url);
//
//    buf_t *buf = buf_size(NULL, BUFFER_SIZE);
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_data);
//    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
//    curl_easy_setopt(curl, CURLOPT_USERAGENT, "jsmn-example (https://github.com/alisdair/jsmn-example, alisdair@mcdiarmid.org)");
//
//    struct curl_slist *hs = curl_slist_append(NULL, "Accept: application/json");
//    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
//
//    CURLcode res = curl_easy_perform(curl);
//    if (res != CURLE_OK)
//        log_die("curl_easy_perform failed: %s", curl_easy_strerror(res));
//
//    curl_easy_cleanup(curl);
//    curl_slist_free_all(hs);
//
//    char *js = buf_tostr(buf);
//    free(buf->data);
//    free(buf);
//
//    return js;
//}

jsmntok_t * json_tokenise(const char *js)
{
    jsmn_parser parser;
    jsmn_init(&parser);

    unsigned int n = JSON_TOKENS;
    jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);
    //log_null(tokens);

	//jsmn_init(&p);
	//r = jsmn_parse(&p, js, strlen(js), tokens, 32);

    int ret = jsmn_parse(&parser, js, strlen(js), tokens, 64);

    while (ret == JSMN_ERROR_NOMEM)
    {
        n = n * 2 + 1;
        tokens = realloc(tokens, sizeof(jsmntok_t) * n);
        //log_null(tokens);
        ret = jsmn_parse(&parser, js, sizeof(js), tokens, n);
    }

    if (ret == JSMN_ERROR_INVAL)
    {
        System_printf("jsmn_parse: invalid JSON string\n");
        System_flush();
    }
    if (ret == JSMN_ERROR_PART)
    {
    	System_printf("jsmn_parse: truncated JSON string\n");
    	System_flush();
    }

    return tokens;
}

bool json_token_streq(const char *js, jsmntok_t *t, char *s)
{
    return (strncmp(js + t->start, s, t->end - t->start) == 0
            && sizeof(s) == (size_t) (t->end - t->start));
}

const char * json_token_tostr(const char *js, jsmntok_t *t)
{
	//*(long *)ptr = 0x11223344L;
    //js[34] = "\0";
    //js[t->end] = "\0";
	const char * nullTerminate;
	nullTerminate = '\0';
	strcpy((char *)&js[t->end],nullTerminate);
    return js + t->start;
}


