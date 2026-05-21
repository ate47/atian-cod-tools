#ifndef __ACTS_API_INTERNAL_UI_H__
#define __ACTS_API_INTERNAL_UI_H__
#include "api_ui.h"
/*
 * Internal API used by Atian Tools UI
 * IMPORTANT: None of these functions are meant to be used by external systems, they are not ABI safe and may change without any notice.
 * If you want to use ACTS API, please refer to api_ui.h
 */

 /*
  * Init ACTS common API, remove ACTS options from argc/argv and update them if needed
  * @param argc argc
  * @param argv argv
  * @return ret code
  */
ACTS_COMMON_UI_API int MainActsUI(int argc, const char** argv);

#endif // __ACTS_API_INTERNAL_UI_H__