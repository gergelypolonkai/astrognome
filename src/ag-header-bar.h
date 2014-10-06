#ifndef __AG_HEADER_BAR_H__
#define __AG_HEADER_BAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AG_TYPE_HEADER_BAR                                              \
   (ag_header_bar_get_type())
#define AG_HEADER_BAR(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_CAST((obj),                                   \
                               AG_TYPE_HEADER_BAR,                      \
                               AgHeaderBar))
#define AG_HEADER_BAR_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_CAST((klass),                                    \
                            AG_TYPE_HEADER_BAR,                         \
                            AgHeaderBarClass))
#define IS_AG_HEADER_BAR(obj)                                           \
   (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                   \
                               AG_TYPE_HEADER_BAR))
#define IS_AG_HEADER_BAR_CLASS(klass)                                   \
   (G_TYPE_CHECK_CLASS_TYPE((klass),                                    \
                            AG_TYPE_HEADER_BAR))
#define AG_HEADER_BAR_GET_CLASS(obj)                                    \
   (G_TYPE_INSTANCE_GET_CLASS((obj),                                    \
                              AG_TYPE_HEADER_BAR,                       \
                              AgHeaderBarClass))

typedef struct _AgHeaderBar      AgHeaderBar;
typedef struct _AgHeaderBarClass AgHeaderBarClass;

struct _AgHeaderBarClass {
    GtkHeaderBarClass parent_class;
};

struct _AgHeaderBar{
    GtkHeaderBar parent;
};

typedef enum {
    AG_HEADER_BAR_MODE_LIST,
    AG_HEADER_BAR_MODE_CHART,
    AG_HEADER_BAR_MODE_SELECTION
} AgHeaderBarMode;

GType ag_header_bar_get_type(void) G_GNUC_CONST;

void ag_header_bar_set_mode(AgHeaderBar *header_bar, AgHeaderBarMode mode);

AgHeaderBarMode ag_header_bar_get_mode(AgHeaderBar *header_bar);

G_END_DECLS

#endif /* __AG_HEADER_BAR_H__ */
