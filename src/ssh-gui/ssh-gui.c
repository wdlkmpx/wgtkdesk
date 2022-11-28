
#include "common.h"

GtkWidget *window;
GtkWidget *notebook[1];
int parent=1;

void notebook_init(void);
void ssh_widget_init(void);
void ssh_widget_pack(void);
void ssh_window_show(void);

char command_line[200] = {"xterm -e"};

int main(int argc, char *argv[])
{
    w_gtk_check_version (GTK_MAJOR_VERSION);

#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
    bindtextdomain("ssh-gui", "/usr/share/locale");
    textdomain("ssh-gui"); 
#endif

	gtk_init(&argc, &argv);

	// main window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "SSH GUI");
	g_signal_connect(G_OBJECT(window), "destroy",
					 G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	notebook_init();
	ssh_widget_init();
	ssh_widget_pack();

	gtk_widget_show_all (window);

	gtk_main();
	
	if (!parent) {
		system(command_line);
	}

	return(0);
}


// notebook

void notebook_init(void)
{
	notebook[0] = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(window), notebook[0]);
}

// ssh_widget

/*
	+---------------------------------------------+
	| ______________ @ ______________ : _______   | Table
	| Username         Host             Port      |
	+------------------------+--------------------+
	| < RSA Authent            _________________  | Table
	| < X11 Forwarding                            |
	| > Agent Forwarding                          |
	+------------------------+--------------------+
	| < Verbose              | < Quiet            | Table
	+---------------------------------------------+
	| < Forward (selected)   | ____:______:_____  | HBox <- Table, Separator, Table
	|   port to (non-select) | Lis  Host   Port   |
	|   address.             |                    |
	| ( )Remote ( )Local     |                    |
	+---------------------------------------------+
	| < Set Esc Char    __________                | VBOX <- Table, Toggle, HBox
	| < Set Cipher      __________                |
	| < Compression                               |
	| ( )IPv4 only ( )IPv6 only (*)Not set        |
	+---------------------------------------------+
	|[                Start SSH                  ]| Button
	+---------------------------------------------+
	* 130516 robwoj44 internationalized
*/

#define MAX_ARGS  256
struct EXEC_COMMAND {
   char *program;
   char *args[MAX_ARGS];
};
typedef struct EXEC_COMMAND EXEC_COMMAND;

GtkWidget *box[4];
GtkWidget *table[5];
GtkWidget *separator[7];

GtkWidget *button[1];

GtkWidget *base_entry[3];	//Username,Host,Port,
GtkWidget *base_label[5];
GtkWidget *base_toggle[1];

GtkWidget *auth_toggle[3];	//X11 frwd,agent frwd,RSA auth
GtkWidget *auth_entry[1];

GtkWidget *verb_toggle[2];	//Verbose, Quiet

GtkWidget *forw_entry[3];	//Forward (remote/local) port to (local/remote) addr.
GtkWidget *forw_toggle[3];
GtkWidget *forw_label[6];

GtkWidget *inte_entry[2];	//IPv?, Esc Char, Cipher, Compression
GtkWidget *inte_toggle[6];

static gboolean tb_get_active(GtkWidget *w)
{
	GtkToggleButton *tb = GTK_TOGGLE_BUTTON (w);
	return (gtk_toggle_button_get_active (tb));
}

gint start_ssh(GtkWidget *widget,
		GdkEvent  *event,
		gpointer   data )
{
   EXEC_COMMAND *ec;
   int i;
   char str[1024];
   char *username, *host;

   ec = malloc(sizeof(EXEC_COMMAND));
   if ( ec == NULL ) {
      printf(_("Out of memory\n"));
   }

   ec->program = "xterm";
   i = 0;
   ec->args[i++] = "xterm";
   ec->args[i++] = "-e";

   ec->args[i++] = "ssh";
   
	if (tb_get_active (inte_toggle[3]))
		ec->args[i++] = "-4";
	else if (tb_get_active (inte_toggle[4]))
		ec->args[i++] = "-6";

	if (tb_get_active (inte_toggle[2]))
		ec->args[i++] = "C";

	if (tb_get_active (inte_toggle[1])) {
		ec->args[i++] = "-c";
		ec->args[i++] = gtk_editable_get_chars(GTK_EDITABLE(inte_entry[1]), 0, -1);
	}
	if (tb_get_active (inte_toggle[0])) {
		ec->args[i++] = "-e";
		ec->args[i++] = gtk_editable_get_chars(GTK_EDITABLE(inte_entry[0]), 0, 1);
	}
   
	if (tb_get_active (forw_toggle[0])) {
		if (tb_get_active (forw_toggle[1])) ec->args[i++] = "-L";
		else if (tb_get_active (forw_toggle[2])) ec->args[i++] = "R";

		sprintf(str, "%s:%s:%s", 
         gtk_editable_get_chars(GTK_EDITABLE(forw_entry[0]), 0, -1),
			gtk_editable_get_chars(GTK_EDITABLE(forw_entry[1]), 0, -1),
			gtk_editable_get_chars(GTK_EDITABLE(forw_entry[2]), 0, -1));
      ec->args[i++] = strdup(str);
	}
	
	if (tb_get_active (verb_toggle[0])) ec->args[i++] = "-v";
	if (tb_get_active (verb_toggle[1])) ec->args[i++] = "-q";
	
	if (!tb_get_active (auth_toggle[2])) ec->args[i++] = "-a";
	if (tb_get_active (auth_toggle[1])) ec->args[i++] = "-X";
	else ec->args[i++] = "-x";
	
	if (tb_get_active (auth_toggle[0])) {
      ec->args[i++] = "-i";
      ec->args[i++] = gtk_editable_get_chars(GTK_EDITABLE(auth_entry[0]), 0, -1);
   }
																		
	if (strcmp(gtk_editable_get_chars(GTK_EDITABLE(base_entry[2]), 0, -1), "")) {
      ec->args[i++] = "-p";
      ec->args[i++] = gtk_editable_get_chars(GTK_EDITABLE(base_entry[2]), 0, -1);
   }
      
   username = gtk_editable_get_chars(GTK_EDITABLE(base_entry[0]), 0, -1);
   host = gtk_editable_get_chars(GTK_EDITABLE(base_entry[1]), 0, -1);

   if ( strcmp(username, "") != 0 ) {
      ec->args[i++] = "-l";
      ec->args[i++] = username;
   }
   ec->args[i++] = host;
   ec->args[i++] = NULL;

	parent = fork();
   if ( parent == -1 ) {
      perror("ssh_widget.c");
      return FALSE;
   }
   if ( parent == 0 ) {
      execvp(ec->program, ec->args);
		exit(2);
   }

	if (tb_get_active (base_toggle[0])) {
		exit(2);
	}
	
   free(ec);
	return TRUE;
}

//Configures All Widgets
void ssh_widget_init(void)
{

	//Boxes
	box[0] = gtk_vbox_new(FALSE, 0);
	box[1] = gtk_hbox_new(FALSE, 0);
	box[2] = gtk_vbox_new(FALSE, 0);
	box[3] = gtk_hbox_new(FALSE, 0);

	//Tables
	table[0] = gtk_table_new(2, 5, FALSE);
	table[1] = gtk_table_new(3, 2, FALSE);
	table[2] = gtk_table_new(1, 3, FALSE);
	table[3] = gtk_table_new(3, 5, FALSE);
	table[4] = gtk_table_new(2, 2, FALSE);

	//Separators
	separator[0] = gtk_hseparator_new();
	separator[1] = gtk_hseparator_new();
	separator[2] = gtk_vseparator_new();
	separator[3] = gtk_hseparator_new();
	separator[4] = gtk_vseparator_new();
	separator[5] = gtk_hseparator_new();
	separator[6] = gtk_hseparator_new();

	//Button
	button[0] = gtk_button_new_with_label(_("Start SSH"));
	g_signal_connect (G_OBJECT(button[0]), "clicked",
					G_CALLBACK(start_ssh), NULL);
	
	//Base
	base_entry[0] = gtk_entry_new();
	gtk_widget_set_size_request(base_entry[0], 158, -1);

	base_entry[1] = gtk_entry_new();
	gtk_widget_set_size_request(base_entry[1], 158, -1);

	base_entry[2] = gtk_entry_new();
	gtk_widget_set_size_request(base_entry[2], 35, -1);

	//Labels
	base_label[0] = gtk_label_new(" @  ");
	gtk_misc_set_alignment(GTK_MISC(base_label[0]), 0, 0);
	
	base_label[1] = gtk_label_new(" : ");
	gtk_misc_set_alignment(GTK_MISC(base_label[1]), 0, 0);
	
	base_label[2] = gtk_label_new(_("Username"));
	gtk_misc_set_alignment(GTK_MISC(base_label[2]), 0, 0);
	
	base_label[3] = gtk_label_new(_("Host"));
	gtk_misc_set_alignment(GTK_MISC(base_label[3]), 0, 0);
	
	base_label[4] = gtk_label_new(_("Port"));
	gtk_misc_set_alignment(GTK_MISC(base_label[4]), 0, 0);
	
	base_toggle[0] = gtk_check_button_new_with_label(_("Close SSH-GUI On Connection Attempt"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(base_toggle[0]), FALSE);
	
	//Authentication
	auth_toggle[0] = gtk_check_button_new_with_label(_("RSA Authentification"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(auth_toggle[0]), FALSE);
	
	auth_toggle[1] = gtk_check_button_new_with_label(_("X11 Connection Forwarding"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(auth_toggle[1]), FALSE);
	
	auth_toggle[2] = gtk_check_button_new_with_label(_("Authentication Agent Forwarding"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(auth_toggle[2]), TRUE);
	
	auth_entry[0] = gtk_entry_new();
	gtk_widget_set_size_request(auth_entry[0], 35, -1);
	
	//Verbosity
	verb_toggle[0] = gtk_check_button_new_with_label(_("Verbose"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(verb_toggle[0]), FALSE);
	
	verb_toggle[1] = gtk_check_button_new_with_label(_("Quiet"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(verb_toggle[1]), FALSE);
	
	//Forwarding Ports to Addresses
	forw_entry[0] = gtk_entry_new();
	gtk_widget_set_size_request(forw_entry[0], 158, -1);
	
	forw_entry[1] = gtk_entry_new();
	gtk_widget_set_size_request(forw_entry[1], 158, -1);
	
	forw_entry[2] = gtk_entry_new();
	gtk_widget_set_size_request(forw_entry[2], 35, -1);
	
	forw_toggle[0] = gtk_check_button_new_with_label(_("Forward port to address:"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(forw_toggle[0]), FALSE);
	
	forw_toggle[1] = gtk_radio_button_new_with_label(NULL, _("Local"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(forw_toggle[1]), FALSE);

	forw_toggle[2] = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(forw_toggle[1])), _("Remote"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(forw_toggle[2]), FALSE);
	
	forw_label[0] = gtk_label_new(" : ");
	gtk_misc_set_alignment(GTK_MISC(forw_label[0]), 0, 0);
	
	forw_label[1] = gtk_label_new(" : ");
	gtk_misc_set_alignment(GTK_MISC(forw_label[1]), 0, 0);
	
	forw_label[2] = gtk_label_new(_("Listen-Port"));
	gtk_misc_set_alignment(GTK_MISC(forw_label[2]), 0, 0);
	
	forw_label[3] = gtk_label_new(_("Host"));
	gtk_misc_set_alignment(GTK_MISC(forw_label[3]), 0, 0);
	
	forw_label[4] = gtk_label_new(_("Port"));
	gtk_misc_set_alignment(GTK_MISC(forw_label[4]), 0, 0);
	
	forw_label[5] = gtk_label_new(_("Port: "));
	gtk_misc_set_alignment(GTK_MISC(forw_label[5]), 0, 0);
	
	//Interface
	inte_toggle[0] = gtk_check_button_new_with_label(_("Set Escape Character"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[0]), FALSE);
	
	inte_toggle[1] = gtk_check_button_new_with_label(_("Set Cipher"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[1]), FALSE);
	
	inte_toggle[2] = gtk_check_button_new_with_label(_("Compression"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[2]), FALSE);
	
	inte_toggle[3] = gtk_radio_button_new_with_label(NULL, _("IPv4"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[3]), FALSE);

	inte_toggle[4] = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(inte_toggle[3])), _("IPv6"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[4]), FALSE);
	
	inte_toggle[5] = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(inte_toggle[4])), _("None Set"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inte_toggle[5]), TRUE);
	
		
	inte_entry[0] = gtk_entry_new();
	gtk_widget_set_size_request(inte_entry[0], 158, -1);
	
	inte_entry[1] = gtk_entry_new();
	gtk_widget_set_size_request(inte_entry[1], 158, -1);
}

void ssh_widget_pack(void)
{
	
	//Notebook0.Box0
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook[0]), box[0], gtk_label_new(_("SSH Config")));
	
	//Box0.Table0
	gtk_box_pack_start(GTK_BOX(box[0]), table[0], FALSE,FALSE, 0);
	
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_entry[0], 0,1, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_label[0], 1,2, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_entry[1], 2,3, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_label[1], 3,4, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_entry[2], 4,5, 0,1);
	
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_label[2], 0,1, 1,2);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_label[3], 2,3, 1,2);
	gtk_table_attach_defaults(GTK_TABLE(table[0]), base_label[4], 4,5, 1,2);
	
	//Box0.Separator0
	gtk_box_pack_start(GTK_BOX(box[0]), separator[0], FALSE,FALSE, 0);
	
	//Box0.Table1
	gtk_box_pack_start(GTK_BOX(box[0]), table[1], FALSE,FALSE, 0);
	
	gtk_table_attach_defaults(GTK_TABLE(table[1]), auth_toggle[0], 0,1, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[1]), auth_entry[0],  1,2, 0,1);

	gtk_table_attach_defaults(GTK_TABLE(table[1]), auth_toggle[1], 0,1, 1,2);

	gtk_table_attach_defaults(GTK_TABLE(table[1]), auth_toggle[2], 0,1, 2,3);
	
	//Box0.Separator1
	gtk_box_pack_start(GTK_BOX(box[0]), separator[1], FALSE,FALSE, 0);
	
	//Box0.Table2
	gtk_box_pack_start(GTK_BOX(box[0]), table[2], FALSE,FALSE, 0);
	
	gtk_table_attach_defaults(GTK_TABLE(table[2]), verb_toggle[0], 0,1, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[2]), separator[2],   1,2, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[2]), verb_toggle[1], 2,3, 0,1);
	
	//Box0.Separator3
	gtk_box_pack_start(GTK_BOX(box[0]), separator[3], FALSE,FALSE, 0);

	//Box0.Box1
	gtk_box_pack_start(GTK_BOX(box[0]), box[1], FALSE,FALSE, 0);

	//Box1	
	gtk_box_pack_start(GTK_BOX(box[1]), forw_toggle[0], FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(box[1]), separator[4], FALSE, FALSE, 0);

	//Box1.Table3
	gtk_box_pack_start(GTK_BOX(box[1]), table[3], FALSE, FALSE, 0);

	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_entry[0], 0,1, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[0], 1,2, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_entry[1], 2,3, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[1], 3,4, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_entry[2], 4,5, 0,1);

	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[2], 0,1, 1,2);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[3], 2,3, 1,2);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[4], 4,5, 1,2);
	
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_label[5],  0,1, 2,3);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_toggle[1], 1,2, 2,3);
	gtk_table_attach_defaults(GTK_TABLE(table[3]), forw_toggle[2], 2,3, 2,3);
	
	//Box0.Separator5
	gtk_box_pack_start(GTK_BOX(box[0]), separator[5], FALSE,FALSE, 0);
	
	//Box0.Box2
	gtk_box_pack_start(GTK_BOX(box[0]), box[2], FALSE,FALSE, 0);
	
	//Box2.Table4
	gtk_box_pack_start(GTK_BOX(box[2]), table[4], FALSE, FALSE, 0);
	
	gtk_table_attach_defaults(GTK_TABLE(table[4]), inte_toggle[0], 0,1, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[4]), inte_entry[0],  1,2, 0,1);
	gtk_table_attach_defaults(GTK_TABLE(table[4]), inte_toggle[1], 0,1, 1,2);
	gtk_table_attach_defaults(GTK_TABLE(table[4]), inte_entry[1],  1,2, 1,2);
	
	gtk_box_pack_start(GTK_BOX(box[2]), inte_toggle[2], FALSE, FALSE, 0);
	
	//Box2.Box3
	gtk_box_pack_start(GTK_BOX(box[2]), box[3], FALSE,FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(box[3]), inte_toggle[3], FALSE, FALSE,0);
	gtk_box_pack_start(GTK_BOX(box[3]), inte_toggle[4], FALSE, FALSE,0);
	gtk_box_pack_start(GTK_BOX(box[3]), inte_toggle[5], FALSE, FALSE,0);

	//Box0.Separator6
	gtk_box_pack_start(GTK_BOX(box[0]), separator[6], FALSE,FALSE, 0);

	//Box0.toggle0
	gtk_box_pack_start(GTK_BOX(box[0]), base_toggle[0], FALSE, FALSE, 0);
	
	//Box0.Button
	gtk_box_pack_start(GTK_BOX(box[0]), button[0], FALSE,FALSE, 0);
}

