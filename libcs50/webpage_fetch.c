/* 
 * webpage_fetch - utility functions for downloading web pages.
 *           See webpage.h and webpage.c.
 *
 * This function logically belongs inside webpage.c, but that causes any 
 * user of this module - even if they don't use the webpage_fetch function - 
 * to require the inclusion of libcurl in their compilation.  Separating this
 * function into a separate .c file (and hence separate .o file) allows the 
 * linker to skip this file in programs that do not call webpage_fetch - and
 * thus succeed even when libcurl is not provided to the linker. 
 * 
 * Ira Ray Jenkins - April 2014
 * 
 * Updated by David Kotz - April 2016, 2017
 * Updated by Xia Zhou - July 2016
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>	      // curl to fetch web pages

#include "webpage.h"	      // definition of this module
#include "webpage_internal.h" // internal-use only


/* *********************************************************************** */
/* Private function prototypes */

static size_t WriteMemoryCallback(void* contents, 
				  size_t size, size_t nmemb, void* userp);


/* ************* webpage_fetch ******************** */
/* see webpage.h for usage documentation.
 *
 * Pseudocode:
 *     1. check for valid page pointer
 *     2. allocate buffer to page->html, set page->html_len to 0
 *     3. setup curl
 *     4. curl the page->url
 *     5. check return status
 *     6. cleanup
 */
bool webpage_fetch(webpage_t *page)
{
  const int MAX_TRY = 3;               // maximum attempts to fetch
  static char errbuf[CURL_ERROR_SIZE]; // buffer for error messages
  int tries = 0;		       // number of attempts at curl
  bool status = true;		       // return value
  CURL* curl_handle;		       // curl handle
  CURLcode res;		               // curl response code

  // check webpage structure - must have URL and not yet have HTML
  if (page == NULL || page->url == NULL || page->html != NULL) {
    return false;
  }

  // allocate space for the html, curl will realloc as needed
  page->html = calloc(1, sizeof(char));
  page->html_len = 0;

  // init curl session
  curl_handle = curl_easy_init();

  // specify url
  curl_easy_setopt(curl_handle, CURLOPT_URL, page->url);

  // send all data to this function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  // pass page struct to callback function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)page);

  // add a user agent just in case servers need it
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  // make 404+ an error
  curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);

  // save error messages
  curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, &errbuf);

  // get the page; repeat MAX_TRY times
  do {
    res = curl_easy_perform(curl_handle);
#ifndef NOSLEEP // CS50 students: please don't turn off the sleep!
    sleep(1);   // sleep one second between fetches, to lighten load on server
#endif
  } while (res != CURLE_OK && ++tries < MAX_TRY);

  // check response code
  if (res != CURLE_OK) {
    // we're going to return the curl error message on failure
    free(page->html);
    page->html = calloc(strlen(errbuf) + 1, sizeof(char));
    page->html_len = strlen(errbuf);
    strcpy(page->html, errbuf);

    status = false;                          // signal failure
  }

  // cleanup curl stuff
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return status;
}


/******************************************************************
 * WriteMemoryCallback - curl callback for writing retrieved data
 *
 * For implementation details see:
 * http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTWRITEFUNCTION
 *
 * Should have no use outside of this file, thus declared static.
 */
static size_t 
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  webpage_t *page = (webpage_t*) userp;

  page->html = realloc(page->html, page->html_len + realsize + 1);
  if (page->html == NULL) {
    return 0;
  }

  memcpy(&(page->html[page->html_len]), contents, realsize);
  page->html_len += realsize;
  page->html[page->html_len] = 0;

  return realsize;
}
