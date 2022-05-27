#!/usr/bin/perl
# vim: ts=4 sw=4

use strict;
use warnings;
use v5.16;

use lib '.';

use DemoWidget;

sub activate_cb
{
	my $app = shift;

	my $window = Gtk4::Window->new ();
	my $box = Gtk4::Box->new ('vertical', 6);
	my $btnbox = Gtk4::Box->new ('horizontal', 0);
	my $demo = DemoWidget->new ();
	my $label = Gtk4::Label->new ('Move cursor:');
	my $left_btn = Gtk4::Button->new_with_mnemonic ('_Left');
	my $right_btn = Gtk4::Button->new_with_mnemonic ('_Right');

	$app->add_window ($window);

	$demo->set ('text',
		'Blah blah blah asdf asdf asdf asdf adsf');

	$left_btn->signal_connect ('clicked' => sub {
			$demo->set ('cursor-pos', $demo->get('cursor-pos') - 1);
		}
	);
	$right_btn->signal_connect ('clicked' => sub {
			$demo->set ('cursor-pos', $demo->get('cursor-pos') + 1);
		}
	);

	$btnbox->append ($left_btn);
	$btnbox->append ($right_btn);

	$box->append ($demo);
	$label->set_halign ('start');
	$box->append ($label);
	$box->append ($btnbox);

	$window->set_default_size (640, 480);
	$window->set_child ($box);

	$window->show ();
}

my $app = Gtk4::Application->new ("com.example.CustomWidgetDemo", "flags-none");
my $status;

$app->signal_connect (activate => \&activate_cb);

$app->run (\@ARGV);
