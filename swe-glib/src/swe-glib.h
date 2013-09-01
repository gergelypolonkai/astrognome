#ifndef __SWE_GLIB_H__
#define __SWE_GLIB_H__

#include <glib.h>
#include "gswe-types.h"
#include "gswe-timestamp.h"
#include "gswe-moment.h"

extern GHashTable *gswe_planet_info_table;
extern GHashTable *gswe_house_system_info_table;

void gswe_init(gchar *sweph_path);

#endif /* __SWE_GLIB_H__ */

