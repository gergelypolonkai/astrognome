namespace Ag {

    class HeaderBar: Gtk.HeaderBar {
        public void clear()
        {
            custom_title = null;

            foreach (Gtk.Widget w in get_children()) {
                w.hide();
            }
        }
    }
}
