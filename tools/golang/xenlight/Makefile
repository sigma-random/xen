XEN_ROOT=$(CURDIR)/../../..
include $(XEN_ROOT)/tools/Rules.mk

# Standing boldly against convention, we insist on installing the
# package source under $(prefix)/share/gocode
GOCODE_DIR ?= $(prefix)/share/gocode/
GOXL_PKG_DIR = /src/$(XEN_GOCODE_URL)/xenlight/
GOXL_INSTALL_DIR = $(GOCODE_DIR)$(GOXL_PKG_DIR)

GO ?= go

.PHONY: all
all: build

.PHONY: package
package: $(XEN_GOPATH)$(GOXL_PKG_DIR)

$(XEN_GOPATH)/src/$(XEN_GOCODE_URL)/xenlight/: xenlight.go types.gen.go helpers.gen.go
	$(INSTALL_DIR) $(XEN_GOPATH)$(GOXL_PKG_DIR)
	$(INSTALL_DATA) xenlight.go $(XEN_GOPATH)$(GOXL_PKG_DIR)
	$(INSTALL_DATA) types.gen.go $(XEN_GOPATH)$(GOXL_PKG_DIR)
	$(INSTALL_DATA) helpers.gen.go $(XEN_GOPATH)$(GOXL_PKG_DIR)

%.gen.go: gengotypes.py $(XEN_ROOT)/tools/libxl/libxl_types.idl $(XEN_ROOT)/tools/libxl/idl.py
	XEN_ROOT=$(XEN_ROOT) $(PYTHON) gengotypes.py ../../libxl/libxl_types.idl

# Go will do its own dependency checking, and not actuall go through
# with the build if none of the input files have changed.
#
# NB that because the users of this library need to be able to
# recompile the library from source, it needs to include '-lxenlight'
# in the LDFLAGS; and thus we need to add -L$(XEN_XENLIGHT) here
# so that it can find the actual library.
.PHONY: build
build: package
	CGO_CFLAGS="$(CFLAGS_libxenlight) $(CFLAGS_libxentoollog)" CGO_LDFLAGS="$(LDLIBS_libxenlight) $(LDLIBS_libxentoollog) -L$(XEN_XENLIGHT) -L$(XEN_LIBXENTOOLLOG)" GOPATH=$(XEN_GOPATH) $(GO) install -x $(XEN_GOCODE_URL)/xenlight

.PHONY: install
install: build
	$(INSTALL_DIR) $(DESTDIR)$(GOXL_INSTALL_DIR)
	$(INSTALL_DATA) $(XEN_GOPATH)$(GOXL_PKG_DIR)xenlight.go $(DESTDIR)$(GOXL_INSTALL_DIR)
	$(INSTALL_DATA) $(XEN_GOPATH)$(GOXL_PKG_DIR)types.gen.go $(DESTDIR)$(GOXL_INSTALL_DIR)
	$(INSTALL_DATA) $(XEN_GOPATH)$(GOXL_PKG_DIR)helpers.gen.go $(DESTDIR)$(GOXL_INSTALL_DIR)

.PHONY: uninstall
	rm -rf $(DESTDIR)$(GOXL_INSTALL_DIR)

.PHONY: clean
clean:
	$(RM) -r $(XEN_GOPATH)$(GOXL_PKG_DIR)
	$(RM) $(XEN_GOPATH)/pkg/*/$(XEN_GOCODE_URL)/xenlight.a
	$(RM) *.gen.go

.PHONY: distclean
distclean: clean

-include $(DEPS_INCLUDE)
