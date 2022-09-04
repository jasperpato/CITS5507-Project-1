all: driver results

driver: driver.c
	gcc -o $@ $^

results: results.c
	gcc -o $@ $^

clean:
	rm driver
	rm results