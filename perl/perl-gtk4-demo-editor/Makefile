# dependencies: perl-Glib-Object-Introspection, gtk4

PREFIX ?= /usr/local
DESTDIR ?=
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/$(EXE)
SCHEMADIR = $(PREFIX)/share/glib-2.0/schemas
EXE = perl-gtk4-demo-editor

install:
	install -d -m 755 $(DESTDIR)$(BINDIR) $(DESTDIR)$(DATADIR) $(DESTDIR)$(SCHEMADIR)
	cat $(EXE).pl |sed -e "s,DATADIR =.*,DATADIR = '$(DATADIR)';," > $(DESTDIR)$(BINDIR)/$(EXE)
	chmod 755 $(DESTDIR)$(BINDIR)/$(EXE)
	install -m 644 com.example.MyApp.gschema.xml $(DESTDIR)$(SCHEMADIR)
	install -m 644 *.ui $(DESTDIR)$(DATADIR)
	install -m 644 *.css $(DESTDIR)$(DATADIR)
	if test "x$(DESTDIR)" = "x"; then glib-compile-schemas $(SCHEMADIR); fi

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(EXE)
	rm -f $(DESTDIR)$(SCHEMADIR)/com.example.MyApp.gschema.xml
	rm -rf $(DESTDIR)$(DATADIR)
	if test "x$(DESTDIR)" = "x"; then glib-compile-schemas $(SCHEMADIR); fi

.PHONY: install uninstall
