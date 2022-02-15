PARTS := 1 2 3
SRC := $(PARTS:%=part%_lab_1.c)

lab_1.zip: $(SRC) report.pdf
	zip $@ $^

# Pandoc is pretty bloated, up to you guys if you wanna use it
%.pdf: %.md
	pandoc $< -o $@

.PHONY: all