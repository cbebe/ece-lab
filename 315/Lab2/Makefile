SRC := part1/lab2_part1.c part1/morseTranslator.h \
	   part2/initialization.c part2/initialization.h \
	   part2/part2_lab2_main.c part2/uart_driver.h

lab_2.zip: $(SRC) report.pdf 
	zip -r $@ $^

# Pandoc is pretty bloated, up to you guys if you wanna use it
%.pdf: %.md
	pandoc $< -o $@

.PHONY: all