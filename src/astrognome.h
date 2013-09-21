#ifndef __ASTROGNOME_H__
#define __ASTROGNOME_H__

typedef struct {
    gboolean version;
    gboolean quit;
    gboolean new_window;
} AstrognomeOptions;

extern GtkFileFilter *filter_all;
extern GtkFileFilter *filter_chart;

#endif /* __ASTROGNOME_H__ */

