<<<<<<< HEAD
/*
=======
/* 
>>>>>>> e3dd83e7ca671d47dee0fd918ca3ab2056254b5b
 * webpage_internal.h - glue between webpage.c and webpage_fetch.c
 *
 * DO NOT INCLUDE THIS FILE - it is only for 'internal' use, that is,
 * between webpage.c and webpage_fetch.c.
 *
 * David Kotz - April 2017
 *
 */

#ifndef __WEBPAGE_INTERNAL_H
#define __WEBPAGE_INTERNAL_H

#include <stdlib.h>

/***********************************************************************/
/* webpage_t: structure to represent a web page, and its contents.
 * The innards should not be visible to users of the webpage module.
 */
typedef struct webpage {
  char *url;                               // url of the page
  char *html;                              // html code of the page
  size_t html_len;                         // length of html code
  int depth;                               // depth of crawl
} webpage_t;

#endif // __WEBPAGE_INTERNAL_H
