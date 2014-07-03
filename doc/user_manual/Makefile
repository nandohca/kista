NAME=kista_manual

pdf:
	pdflatex $(NAME).tex

# To generate HTML version
# Note: to remove the cover, you should currently use the option \pdffalse in kista_manual.tex file
html:
	rm -rf kista_um_html
	mkdir -p kista_um_html
#	htlatex $(NAME).tex "html,index=2,5" "" ""
# To dump into a specific directory (must end with /)
#	htlatex $(NAME).tex "" "" "-d./kista_um_html/"
	htlatex $(NAME).tex "html,index=2,5" "" "-d./kista_um_html/"	
# Without processing
	htlatex $(NAME).tex "html,index=2,5" "" "-d./kista_um_html/ -p"	
# For some reason, the previous command does not create a -figs subdir, nor it moves figures to it, so we do it
# It is maybe a question of configuration
	mkdir ./kista_um_html/figs
	mv ./kista_um_html/*.jpg ./kista_um_html/figs
	mv ./kista_um_html/*.png ./kista_um_html/figs
	
clean:
	rm -f $(NAME).dvi
	rm -f $(NAME).lg
	rm -f $(NAME).log
	rm -f $(NAME).tog
	rm -f $(NAME).tmp
	rm -f $(NAME).toc
	rm -f $(NAME).aux
	rm -f $(NAME).css
	rm -f $(NAME).4ct
	rm -f $(NAME).4tc
	rm -f $(NAME).xref
	rm -f $(NAME).idv
	rm -f texput.log
	rm -f *.html
	rm -f *~

ultraclean: clean
	rm -f *.pdf
	rm -rf kista_um_html
