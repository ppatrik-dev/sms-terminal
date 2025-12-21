SHELL := /bin/bash

.PHONY: zip latex latex-clean

zip:
	zip -r xprochp00.zip doc/ project/ 

latex:
	cd doc && pdflatex main.tex
	cd doc && pdflatex main.tex
	$(MAKE) latex-clean

latex-clean:
	rm -f doc/main.{aux,bcf,log,out,toc,run.xml}