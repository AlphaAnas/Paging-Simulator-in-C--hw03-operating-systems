# Build target
build: paging.c
	gcc paging.c -o paging -Wall

# Run target
run: 
	./paging 1048576 12 1024 "p1.proc" "p2.proc" "p3.proc"

# Clean target
clean:
	rm -f paging
# Generate process files and run tests
test: build p1.proc p2.proc p3.proc
	@echo "Running Tests..."
	./paging 1048576 12 1024 "p1.proc" "p2.proc" "p3.proc"
	@echo "Expected Results:"
	@echo "Process 1:"
	@echo "  Pages: 2, Frames Allocated: 2, Internal Fragmentation: 916 bytes"
	@echo "Process 2:"
	@echo "  Pages: 4, Frames Allocated: 4, Internal Fragmentation: 768 bytes"
	@echo "Process 3:"
	@echo "  Pages: 2, Frames Allocated: 2, Internal Fragmentation: 874 bytes"
	@echo "Total Internal Fragmentation: 2558 bytes"

# # Generate process files
# p1.proc:
# 	@echo "Generating p1.proc..."
# 	gcc process_generator.c -o process_generator 
# p2.proc: 
# 	@echo "Generating p2.proc..."
# 	python3 generate_procs.py

# p3.proc: 
# 	@echo "Generating p3.proc..."
# 	python3 generate_procs.py
