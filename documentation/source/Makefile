# Makefile for Sphinx documentation
#
SPHINXBUILD = sphinx-build
PAPER       = letter
BUILDDIR    = _build

.PHONY: html all epub latex pdf clean

html:
	$(SPHINXBUILD) -a . $(BUILDDIR)/html

epub:
	$(SPHINXBUILD) -a -b epub . $(BUILDDIR)/html

all: html pdf

latex:
	$(SPHINXBUILD) -a -b latex . $(BUILDDIR)/latex

pdf: latex
	cd $(BUILDDIR)/latex && make all-pdf
	cp -pr $(BUILDDIR)/latex/xspress3epics.pdf $(BUILDDIR)/html/xspress3epics.pdf

clean:
	-rm -rf $(BUILDDIR)/*

tarball: html
	cd $(BUILDDIR)/html &&  tar cvzf ../../../../_doc.tgz *


publish: tarball
	cd ../..  &&  git checkout gh-pages &&  tar xvzf _doc.tgz && git commit -m "update docs" && git push && git checkout master && cd documentation/sources
