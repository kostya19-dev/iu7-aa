CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O0 -Iinc
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
UNIT_FLAGS = -lcheck -lsubunit -lm -lpthread -lrt --coverage

SRC_DIR = src
TEST_DIR = tests

TARGET = print_numbers
TEST_TARGET = unit_tests

.PHONY: all build test coverage clean report clean-report

all: build

build: $(TARGET)

$(TARGET): $(SRC_DIR)/main.cpp $(SRC_DIR)/cpu_time.cpp $(SRC_DIR)/print_numbers.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST_TARGET)
	./$(TEST_TARGET)

unit: CXXFLAGS += $(COVERAGE_FLAGS)
unit: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_DIR)/unit_tests.cpp $(SRC_DIR)/print_numbers.cpp
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $^ -o $@ $(UNIT_FLAGS)

coverage: unit
	@./$(TEST_TARGET) > $(TEST_DIR)/test_output.txt 2>&1; \
	test_result=$$?; \
	checks=$$(grep -m1 -o "Checks: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
	fails=$$(grep -m1 -o "Failures: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
	errors=$$(grep -m1 -o "Errors: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
	passed=$$((checks - fails - errors)); \
	cov=$$(gcovr -r . --filter='src/print_numbers.cpp' --filter='tests/unit_tests.cpp' --txt 2>/dev/null | grep TOTAL | awk '{gsub(/%/, "", $$4); print $$4}' || echo 0); \
	ts=$$(date -u +"%Y-%m-%dT%H:%M:%S+00:00"); \
	echo "{ \"timestamp\": \"$$ts\", \"coverage\": $$cov, \"passed\": $$passed, \"failed\": $$fails }" > $(TEST_DIR)/stud-unit-test-report-prev.json; \
	cat $(TEST_DIR)/stud-unit-test-report-prev.json; \
	exit $$test_result

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o
	rm -f $(SRC_DIR)/*.gcda $(SRC_DIR)/*.gcno
	rm -f $(TEST_DIR)/*.gcda $(TEST_DIR)/*.gcno
	rm -f *.gcda *.gcno *.gcov
	rm -f $(TEST_DIR)/test_output.txt $(TEST_DIR)/stud-unit-test-report-prev.json



REPORT_DIR := ../report

report:
	cd $(REPORT_DIR) && \
	pdflatex -interaction=batchmode report.tex && \
	pdflatex -interaction=batchmode report.tex && \
	gs -sDEVICE=pdfwrite \
		-dCompatibilityLevel=1.4 \
		-dNOPAUSE \
		-dOptimize=true \
		-dQUIET \
		-dBATCH \
		-dRemoveUnusedFonts=true \
		-dRemoveUnusedImages=true \
		-dOptimizeResources=true \
		-dDetectDuplicateImages \
		-dCompressFonts=true \
		-dEmbedAllFonts=true \
		-dSubsetFonts=true \
		-dPreserveAnnots=true \
		-dPreserveMarkedContent=true \
		-dPreserveOverprintSettings=true \
		-dPreserveHalftoneInfo=true \
		-dPreserveOPIComments=true \
		-dPreserveDeviceN=true \
		-dMaxInlineImageSize=0 \
		-sOutputFile=report_compressed.pdf \
		report.pdf && \
	mv report_compressed.pdf report.pdf

clean-report:
	cd $(REPORT_DIR) && \
	rm -f *.aux *.log *.toc *.lof *.lot *.out *.synctex.gz *.bbl *.blg *.fls *.fdb_latexmk report.pdf