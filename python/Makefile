%.dot:
	python3 $*.py

%.png:
	dot -Tpng $*.dot -o $@

clean:
	rm *.png *.dot
