DOC_DIR = html
DOC_TAG = doxygen.tag
DOC_LOG = warnings.log

all: clean
	doxygen Doxyfile

clean:
	rm -rf $(DOC_DIR)
	rm -f $(DOC_TAG)
	rm -f $(DOC_LOG)
