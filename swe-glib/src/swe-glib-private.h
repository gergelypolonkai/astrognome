#ifdef __SWE_GLIB_BUILDING__
#ifndef __SWE_GLIB_PRIVATE_H__

#include "gswe-timestamp.h"

extern gchar *gswe_ephe_path;
extern GsweTimestamp *gswe_full_moon_base_date;

#endif /* __SWE_GLIB_PRIVATE_H__ */
#else /* not defined __SWE_GLIB_BUILDING__ */
#error __FILE__ "Can not be included, unless building SWE-GLib"
#endif /* __SWE_GLIB_BUILDING__ */

