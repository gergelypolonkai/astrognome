#ifndef __ASTROGNOME_H__
#define __ASTROGNOME_H__

typedef struct {
    gboolean version;
    gboolean quit;
    gboolean new_window;
} AstrognomeOptions;

extern GtkFileFilter *filter_all,
                     *filter_chart;

#endif /* __ASTROGNOME_H__ */

