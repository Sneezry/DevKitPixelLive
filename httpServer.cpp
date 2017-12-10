/**
 ******************************************************************************
 * @file    app_https.c
 * @author  QQ DING
 * @version V1.0.0
 * @date    1-September-2015
 * @brief   The main HTTPD server initialization and wsgi handle.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include <httpd.h>
#include "mico.h"
#include "httpServer.h"
#include "OledDisplay.h"
#include "Base64.h"

#define app_httpd_log(...) 

#define HTTPD_HDR_DEFORT (HTTPD_HDR_ADD_SERVER|HTTPD_HDR_ADD_CONN_CLOSE|HTTPD_HDR_ADD_PRAGMA_NO_CACHE)

extern OLEDDisplay Screen;

bool is_http_init;
bool is_handlers_registered;

int web_send_live_res(httpd_request_t *req)
{
  char row[2] = { 0 };
  char encoded_bmp[128] = { 0 };
  int err = kNoErr;

  err = httpd_send_all_header(req, HTTP_RES_200, 17, HTTP_CONTENT_JS_STR);
  require_noerr_action( err, exit, app_httpd_log("ERROR: Unable to send http wifisetting headers.") );
  err = httpd_send_body(req->sock, (const unsigned char*)"void({\"code\":0});", 17);
  require_noerr_action( err, exit, app_httpd_log("ERROR: Unable to send http wifisetting body.") );
  if (httpd_get_tag_from_url(req, "r", row, sizeof(row)) == 0 &&
        httpd_get_tag_from_url(req, "b", encoded_bmp, sizeof(encoded_bmp)) == 0)
  {
    int encode_bmp_length = strlen(encoded_bmp);
    int decode_bmp_length = base64_dec_len(encoded_bmp, encode_bmp_length);
    char decoded_bmp[decode_bmp_length];
    base64_decode(decoded_bmp, encoded_bmp, encode_bmp_length);
    Screen.draw((row[0] - 'A') % 2 * 64, (row[0] - 'A') / 2, (row[0] - 'A') % 2 * 64 + decode_bmp_length, (row[0] - 'A') / 2 + 1, (unsigned char*)decoded_bmp);
	}

exit:
  return err; 
}

struct httpd_wsgi_call g_app_handlers[] = {
  {"/", HTTPD_HDR_DEFORT, 0, web_send_live_res, NULL, NULL, NULL},
};

int g_app_handlers_no = sizeof(g_app_handlers)/sizeof(struct httpd_wsgi_call);

void app_http_register_handlers()
{
  int rc;
  rc = httpd_register_wsgi_handlers(g_app_handlers, g_app_handlers_no);
  if (rc) 
  {
    app_httpd_log("failed to register test web handler");
  }
}

int _app_httpd_start()
{
  OSStatus err = kNoErr;
  app_httpd_log("initializing web-services");
  
  /*Initialize HTTPD*/
  if (is_http_init == false) 
  {
    err = httpd_init();
    require_noerr_action( err, exit, app_httpd_log("failed to initialize httpd") );
    is_http_init = true;
  }
  
  /*Start http thread*/
  err = httpd_start();
  if (err != kNoErr) 
  {
    app_httpd_log("failed to start httpd thread");
    httpd_shutdown();
  }
exit:
  return err;
}

int httpd_server_start()
{
  int err = kNoErr;
  err = _app_httpd_start();
  require_noerr( err, exit ); 
  
  if (is_handlers_registered == false) 
  {
    app_http_register_handlers();
    is_handlers_registered = true;
  }

  Screen.print(1, "  DevKit is\r\n");
  Screen.print(2, "  listening...\r\n");
  
exit:
  return err;
}

int app_httpd_stop()
{
  OSStatus err = kNoErr;
  
  /* HTTPD and services */
  app_httpd_log("stopping down httpd");
  err = httpd_stop();
  require_noerr_action( err, exit, app_httpd_log("failed to halt httpd") );
  
exit:
  return err;
}
