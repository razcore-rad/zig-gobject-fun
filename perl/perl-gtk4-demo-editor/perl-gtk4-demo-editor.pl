#!/usr/bin/perl

# vim: ts=4 sw=4

use v5.16;
use strict;
use warnings;

use Glib::Object::Introspection;

Glib::Object::Introspection->setup(
	basename => 'Gtk', version => '4.0', package => 'Gtk4');
Glib::Object::Introspection->setup(
	basename => 'Gio', version => '2.0', package => 'Glib::IO');

our $DATADIR = '.';
our $app_prettyname;

# work around lack of real bindings for gtk4.
our $priority_application = 800;

sub dialog_response_id_to_string
{
	my $raw_id = shift;
	my $retval;

	eval {
		$retval = Glib::Object::Introspection->convert_enum_to_sv (
			'Gtk4::ResponseType', $raw_id);
	};
	if ($@) {
		die ("Programmer error; invalid dialog response ID:\n\t$@");
	}

	return $retval;
}

sub string_to_dialog_response_id
{
	my $string = shift;
	my $retval;

	eval {
		$retval = Glib::Object::Introspection->convert_sv_to_enum (
			'Gtk4::ResponseType', $string);
	};
	if ($@) {
		die ("Programmer error; invalid dialog response string:\n\t$@");
	}

	return $retval;
}
# </workarounds>

sub update_titlebar
{
	my $widgetsref = shift;
	my $gfile = $$widgetsref{appwindow}{file};

	$$widgetsref{appwindow}->set_title (sprintf "%s - %s",
		$gfile ? $gfile->get_basename () : "Untitled",
		$app_prettyname
	);
}

sub create_confirmation_dialog
{
	my $parent = shift;
	my $dialog = Gtk4::Dialog->new ();

	$dialog->set_transient_for ($parent);
	$dialog->set_modal (1);
	$dialog->set_title ("Confirm");
	$dialog->get_content_area->append (Gtk4::Label->new (
			"The buffer has been modified.\n\n" . 
			"If you proceed, the contents will be discarded.\n\n" .
			"Are you sure?")
	);
	$dialog->add_button ("_Yes, discard changes", string_to_dialog_response_id ('accept'));
	$dialog->add_button ("_No, go back", string_to_dialog_response_id ('reject'));
	$dialog->set_default_response (string_to_dialog_response_id ('reject'));

	return $dialog;
}

sub dark_mode_action
{
	my ($action, $state, $app) = @_;

	my $settings = Gtk4::Settings::get_default ();
	$settings->set ("gtk-application-prefer-dark-theme", $state->get_boolean ());

	$action->set_state ($state);
}

sub bold_action
{
	my ($action, $state, $widgetsref) = @_;
	my $textview = $$widgetsref{textview};

	my $css = sprintf (
		"textview {
		  font-weight: %s;
		 }\n",	# gtk complains if we don't have the \n for some reason
		 $state->get_boolean () ? "bold" : "normal");

	my $provider = Gtk4::CssProvider->new ();
	$provider->load_from_data ($css);

	$textview->get_style_context->add_provider ($provider, $priority_application);

	$action->set_state ($state);
}

sub quit_action
{
	my $widgetsref = shift;
	my $app = $$widgetsref{appwindow}->get_application ();
	my $buffer = $$widgetsref{textview}->get_buffer ();

	if ($buffer->get_modified ())
	{
		my $dialog = create_confirmation_dialog ($$widgetsref{appwindow});

		$dialog->signal_connect ("response" => sub
			{
				shift;	# ignore `dialog'; we have it anyway.
				my $response = dialog_response_id_to_string (shift);

				if ($response eq 'accept') {
					$app->quit ();
				}
				$dialog->destroy ();
			}	
		);
		$dialog->show ();
	}
	else
	{
		$app->quit ();
	}

	# let this double up as a close-request signal handler.
	# Neither Perl nor GTK seem to complain about incompatible return types, so... yay?
	return 1;
}

sub about_action
{
	my $win = shift;
	my $about = Gtk4::AboutDialog->new ();

	$about->set_program_name ("Sample Application");
	$about->set_version ("0.1.0");
	$about->set_authors (["Joe Blow", "John Smith"]);
	$about->set_transient_for ($win);
	$about->show ();
}

sub new_action
{
	my $widgetsref = shift;
	my $buffer = $$widgetsref{textview}->get_buffer ();

	my $do_new = sub {
		$buffer->set_text ("", 0);
		$buffer->set_modified (0);
		$$widgetsref{appwindow}{file} = undef;
		update_titlebar ($widgetsref);
	};

	if ($buffer->get_modified ())
	{
		my $dialog = create_confirmation_dialog ($$widgetsref{appwindow});

		$dialog->signal_connect ("response" => sub
			{
				shift;	# ignore `dialog'; we have it anyway.
				my $response = dialog_response_id_to_string (shift);

				if ($response eq 'accept') {
					$do_new->();
				}
				$dialog->destroy ();
			}	
		);
		$dialog->show ();
	}
	else
	{
		$do_new->();
	}
}

sub open_action
{
	my $widgetsref = shift;

	my $buffer = $$widgetsref{textview}->get_buffer ();
	my $chooser = Gtk4::FileChooserNative->new ("Open file", $$widgetsref{appwindow}, 'open');

	$chooser->signal_connect (response => sub
		{
			shift;	# ignore `chooser`; we have it anyway
			my $response = dialog_response_id_to_string (shift);

			if ($response eq 'accept')
			{
				my $gfile = $chooser->get_file ();
				my $contents;

				open(FILE, $gfile->get_path ());
				# temporarily enable 'slurp mode' and put whole file in a string.
				$contents = do {local $/ = undef; <FILE> };
				close(FILE);

				$buffer->set_text ($contents, -1);
				$buffer->set_modified (0);
				$$widgetsref{appwindow}{file} = $gfile;
				update_titlebar ($widgetsref);
			}
		} 
	);

	if ($buffer->get_modified ())
	{
		my $dialog = create_confirmation_dialog ($$widgetsref{appwindow});

		$dialog->signal_connect ("response" => sub
			{
				shift;	# ignore `dialog'; we have it anyway.
				my $response = dialog_response_id_to_string (shift);

				if ($response eq 'accept') {
					$chooser->show ();
				}
				$dialog->destroy ();
			}	
		);
		$dialog->show ();
	}
	else
	{
		$chooser->show ();
	}
}

sub buffer_to_string
{
	my $buffer = shift;

	return $buffer->get_text (
		$buffer->get_start_iter (),
		$buffer->get_end_iter (),
		0);
}

sub write_content_to_gfile
{
	my ($content, $gfile) = @_;

	open(FILE, '>', $gfile->get_path ());
	print FILE $content;
	close(FILE);
}

sub save_action
{
	my $widgetsref = shift;

	if ($$widgetsref{appwindow}{file})
	{
		my $content = buffer_to_string ($$widgetsref{textview}->get_buffer ());
		write_content_to_gfile ($content, $$widgetsref{appwindow}{file});
		$$widgetsref{textview}->get_buffer->set_modified (0);
		update_titlebar ($widgetsref);
	}
	else
	{
		save_as_action ($widgetsref);
	}
}

sub save_as_action
{
	my $widgetsref = shift;

	my $buffer = $$widgetsref{textview}->get_buffer ();
	my $chooser = Gtk4::FileChooserNative->new (
		"Save as", $$widgetsref{appwindow}, 'save');

	$chooser->signal_connect (response => sub
		{
			shift;	# ignore `chooser`; we have it anyway
			my $response = dialog_response_id_to_string (shift);

			if ($response eq 'accept')
			{
				my $gfile = $chooser->get_file ();
				my $content = buffer_to_string ($$widgetsref{textview}->get_buffer ());

				write_content_to_gfile ($content, $gfile);
				$$widgetsref{appwindow}{file} = $gfile;
				$$widgetsref{textview}->get_buffer->set_modified (0);
				update_titlebar ($widgetsref);
			}
		} 
	);
	$chooser->show ();
}

sub startup_cb
{
	my $app = shift;
	my $builder = Gtk4::Builder->new_from_file ("$DATADIR/menus.ui");

	$app->set_menubar ($builder->get_object ("menubar"));
}

sub setup_css
{
	my $widgetsref = shift;
	my ($context, $provider);

	$provider = Gtk4::CssProvider->new ();
	$provider->load_from_path ("$DATADIR/style.css");
	$context = $$widgetsref{textview}->get_style_context ();
	$context->add_provider ($provider, $priority_application);
}

sub setup_settings
{
	my $app = shift;

	my $settings = Glib::IO::Settings->new ("com.example.MyApp");
	$app->add_action ($settings->create_action ("color"));

	# WORKAROUND https://gitlab.gnome.org/GNOME/gtk/-/issues/4880 - focus on save-as dialogs
	Gtk4::Settings::get_default()->set ("gtk-dialogs-use-header", 1);
}

sub setup_actions
{
	my ($app, $widgetsref) = @_;
	my (%app_actions, %win_actions);

	# setup app action handlers

	for ("new", "open", "save", "save-as", "quit") {
		$app_actions{$_} = Glib::IO::SimpleAction->new ($_);
	}

	$app_actions{new}->signal_connect_swapped (activate => \&new_action, $widgetsref);
	$app_actions{open}->signal_connect_swapped (activate => \&open_action, $widgetsref);
	$app_actions{'save-as'}->signal_connect_swapped (activate => \&save_as_action, $widgetsref);
	$app_actions{quit}->signal_connect_swapped (activate => \&quit_action, $widgetsref);

	# only show save button when document is saveable.

	$app_actions{save}->signal_connect_swapped (activate => \&save_action, $widgetsref);
	$app_actions{save}->set_enabled (0);
	$$widgetsref{textview}->get_buffer->signal_connect ('modified-changed' => sub
		{
			my $buffer = shift;
			$app_actions{save}->set_enabled ($buffer->get_modified ());
		}
	);
		
	# special case: need checkmarks in menu for dark mode

	$app_actions{dark} = Glib::IO::SimpleAction->new_stateful (
		'dark', undef, Glib::Variant->new_boolean (0));
	$app->add_action ($app_actions{dark});
	$app_actions{dark}->signal_connect ('change-state' => \&dark_mode_action, $app);

	# setup win action handlers

	for ("about") {
		$win_actions{$_} = Glib::IO::SimpleAction->new ($_);
	}

	$win_actions{about}->signal_connect_swapped (
		activate => \&about_action, $$widgetsref{appwindow});

	# special case: shape action
	
	$win_actions{shape} = Glib::IO::SimpleAction->new_stateful (
		'shape', 's', Glib::Variant->new_string ("oval"));
	$$widgetsref{appwindow}->add_action ($win_actions{shape});

	# special case: bold action
	$win_actions{bold} = Glib::IO::SimpleAction->new_stateful (
		'bold', undef, Glib::Variant->new_boolean (0));
	$win_actions{bold}->signal_connect ('change-state' => \&bold_action, $widgetsref);

	# add all actions

	for my $key (keys %app_actions) {
		$app->add_action ($app_actions{$key});
	}
	for my $key (keys %win_actions) {
		$$widgetsref{appwindow}->add_action ($win_actions{$key});
	}
}

sub setup_accels
{
	my $app = shift;
	my %accels = (
		'app.new' => ['<Ctrl>n'],
		'app.save' => ['<Ctrl>s'],
		'app.save-as' => ['<Ctrl><Shift>s'],
		'app.quit' => ['<Ctrl>q'],
		'app.color::red' => ['<Ctrl>r'],
		'app.color::green' => ['<Ctrl>g'],
		'app.color::blue' => ['<Ctrl>b'],
		'win.shape::square' => ['<Ctrl><Shift>s'],
		'win.shape::rectangle' => ['<Ctrl><Shift>r'],
		'win.shape::oval' => ['<Ctrl><Shift>o'],
		'win.bold' => ['<Ctrl><Shift>b'],
		'win.about' => ['<Ctrl><Shift>a'],
	);

	for my $action (keys %accels)
	{
		$app->set_accels_for_action ($action, $accels{$action});
	}
}

sub activate_cb
{
	my $app = shift;
	my $builder = Gtk4::Builder->new_from_file ("$DATADIR/application.ui");

	# create widgets hash from GtkBuilder XML file that will be passed around
	# to other subs as a reference.

	my %widgets;
	for ("appwindow", "status", "textview") {
		$widgets{$_} = $builder->get_object ($_);
	}

	# cache prettyname from .ui file
	$app_prettyname = $widgets{appwindow}->get_title ();

	setup_css (\%widgets);
	setup_settings ($app);
	setup_actions ($app, \%widgets);
	setup_accels ($app);

	# add window to app and display.
	
	$widgets{appwindow}->signal_connect_swapped ('close-request' => \&quit_action, \%widgets);
	$app->add_window ($widgets{appwindow});
	$widgets{appwindow}->show ();
}

my $app = Gtk4::Application->new ("com.example.MyApp", "flags-none");
my $status;

$app->signal_connect (startup => \&startup_cb);
$app->signal_connect (activate => \&activate_cb);

$app->run (\@ARGV);
