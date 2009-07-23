
#include <glib.h>
#include <gtk/gtk.h>
#include "logout-dialog.h"
#include "ck-pk-helper.h"

static LogoutDialogAction type = LOGOUT_DIALOG_LOGOUT;

static gboolean
option_logout (const gchar * arg, const gchar * value, gpointer data, GError * error)
{
	type = LOGOUT_DIALOG_LOGOUT;
	return TRUE;
}

static gboolean
option_shutdown (const gchar * arg, const gchar * value, gpointer data, GError * error)
{
	type = LOGOUT_DIALOG_SHUTDOWN;
	return TRUE;
}

static gboolean
option_restart (const gchar * arg, const gchar * value, gpointer data, GError * error)
{
	type = LOGOUT_DIALOG_RESTART;
	return TRUE;
}

static GOptionEntry options[] = {
	{"logout",     'l',  G_OPTION_FLAG_NO_ARG,  G_OPTION_ARG_CALLBACK,  option_logout,   "Log out of the current session",   NULL},
	{"shutdown",   's',  G_OPTION_FLAG_NO_ARG,  G_OPTION_ARG_CALLBACK,  option_shutdown, "Shutdown the entire system",       NULL},
	{"restart",    'r',  G_OPTION_FLAG_NO_ARG,  G_OPTION_ARG_CALLBACK,  option_restart,  "Restart the system",               NULL},

	{NULL}
};

int
main (int argc, char * argv[])
{
	gtk_init(&argc, &argv);

	GError * error = NULL;
	GOptionContext * context = g_option_context_new(" - logout of the current session");
	g_option_context_add_main_entries(context, options, "gtk-logout-helper");
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	g_option_context_set_help_enabled(context, TRUE);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_debug("Option parsing failed: %s", error->message);
		g_error_free(error);
		return 1;
	}

	GtkWidget * dialog = NULL;
	if (!pk_require_auth(type)) {	
		dialog = logout_dialog_new(type);
	}

	if (dialog != NULL) {
		GtkResponseType response = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_hide(dialog);

		if (response == GTK_RESPONSE_HELP) {
			type = LOGOUT_DIALOG_RESTART;
			response = GTK_RESPONSE_OK;
		}

		if (response != GTK_RESPONSE_OK) {
			return 0;
		}
	}

	return 0;
}
