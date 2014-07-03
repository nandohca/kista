
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

EX_NAME=sample2

SD_FILE= $(EX_NAME).xml

run:
	kista-xml $(SD_FILE)

sketch:
	pdflatex $(EX_NAME)_sketch.tex

all:run sketch

clean:
	rm -f $(EX_NAME)_sketch.tex
	rm -f $(EX_NAME)_sketch.aux
	rm -f $(EX_NAME)_sketch.log

ultraclean:clean
	rm -f output.xml
	rm -f $(EX_NAME)_sketch.pdf
		

