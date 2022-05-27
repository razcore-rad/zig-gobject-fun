# vim: ts=4 sw=4

package DemoWidget;

use strict;
use warnings;
use v5.16;

use Glib::Object::Introspection;
use Glib::Object::Subclass;

Glib::Object::Introspection->setup(
	basename => 'Gtk', version => '4.0', package => 'Gtk4');
Glib::Object::Introspection->setup(
	basename => 'Gio', version => '2.0', package => 'Glib::IO');

Glib::Type->register_object ('Gtk4::Widget', __PACKAGE__,
	properties => [
		Glib::ParamSpec->string ("text", '', '',
			undef,
			['readable', 'writable']),
		Glib::ParamSpec->uint ("cursor-pos", '', '',
			0, 9999,
			0,
			['readable', 'writable']),
	]
);

sub new
{
	my ($class, $self) = @_;

	$self = Glib::Object::new ($class);

	return bless $self, $class;
}

sub INIT_INSTANCE
{
	my $self = shift;

	$self->set_layout_manager (Gtk4::BoxLayout->new ('vertical'));

	$self->set_hexpand (1);
	$self->set_vexpand (1);

	$self->{cursor_on} = 1;
	$self->{cursor_timeout} = Glib::Timeout->add_seconds (1,
		sub {
			$self->{cursor_on} = !$self->{cursor_on};
			$self->queue_draw ();
			return Glib::SOURCE_CONTINUE;
		}
	);
}

#sub FINALIZE_INSTANCE
#{
#	my $self = shift;
#}

sub GET_PROPERTY
{
  my ($self, $pspec) = @_;
  my $pname = $pspec->get_name;

  return (exists $self->{$pname} ? $self->{$pname}
          : $pspec->get_default_value);
}

sub SET_PROPERTY
{
  my ($self, $pspec, $newval) = @_;
  my $pname = $pspec->get_name;

  $self->{$pname} = $newval;

  if ($pname eq 'cursor_pos')
  {
	  $self->queue_draw ();
  }
}

sub SNAPSHOT
{
	my ($self, $snapshot) = @_;
	my $context = $self->get_style_context ();
	my $layout = $self->create_pango_layout ($self->get ('text'));

	$snapshot->render_background ($context, 0, 0, $self->get_width, $self->get_height);
	$snapshot->render_layout ($context, 0, 0, $layout);

	if ($self->{cursor_on}) {
		$snapshot->render_insertion_cursor ($context, 0, 0, $layout, $self->get ('cursor-pos'), 'ltr');
	}
}

1;
