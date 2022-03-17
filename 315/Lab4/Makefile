SRC := \
		lab4_main.c \
	
ZIP := lab_4.zip

$(ZIP): $(SRC) report.pdf 
	zip -r $@ $^

# Pandoc is pretty bloated, up to you guys if you wanna use it
%.pdf: %.md
	pandoc $< -o $@

clean:
	rm -rf $(ZIP) report.pdf

.PHONY: all