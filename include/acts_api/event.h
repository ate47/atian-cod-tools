#ifndef __ACTS_API_EVENTS_H__
#define __ACTS_API_EVENTS_H__
#include "api.h"
/*
 * Atian Tools Event API
 */

 // event type
typedef enum {
    EVT_REGISTER_GSC = 0xc25bddd9c1f2adfc,
    EVT_REGISTER_FASTFILE = 0x17686c4ce86af089,
} ActsAPIEvent_Type;

typedef struct {
    // no data
} ActsAPIEvent_RegisterGscData;

typedef struct {
    // no data
} ActsAPIEvent_RegisterFastFileData;

typedef void ActsAPIEvent_Callback(void* data);


/*
 * Register an event callback, the callback will be called when the event is triggered.
 * @param type event type
 * @param callback event callback.
 */
ACTS_COMMON_API void ActsAPIEvent_RegisterCallback(ActsAPIEvent_Type type, ActsAPIEvent_Callback callback);

/*
 * Trigger an event.
 * @param type event type
 */
ACTS_COMMON_API void ActsAPIEvent_TriggerEvent(ActsAPIEvent_Type type, void* data);

#endif // __ACTS_API_EVENTS_H__