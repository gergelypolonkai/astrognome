#ifdef __SWE_GLIB_BUILDING__
#ifndef __SWE_GLIB_PRIVATE_H__

extern gchar *gswe_ephe_path;

#endif /* __SWE_GLIB_PRIVATE_H__ */
#else /* not defined __SWE_GLIB_BUILDING__ */
#error __FILE__ "Can not be included, unless building SWE-GLib"
#endif /* __SWE_GLIB_BUILDING__ */

