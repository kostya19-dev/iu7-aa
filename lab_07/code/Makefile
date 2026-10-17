# CXX = g++
# CXXFLAGS_COMMON = -std=c++17 -Wall -Wextra -pthread -Iinc
# COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
# UNIT_FLAGS = -lcheck -lsubunit -lm -lpthread -lrt --coverage

# SRC_DIR = src
# TEST_DIR = tests

# TARGET = prog
# DEBUG_TARGET = prog_debug
# TEST_TARGET = unit_tests

# .PHONY: all build test coverage clean clean_objs clean_bin clean_test_bin release debug unit report clean-report 

# all: release

# build: release


# release: CXXFLAGS = $(CXXFLAGS_COMMON) -O2
# release: clean_bin $(TARGET)


# debug: CXXFLAGS = $(CXXFLAGS_COMMON) -g -O0 -DDEBUG
# debug: clean_bin $(DEBUG_TARGET)


# $(TARGET): $(SRC_DIR)/main.cpp \
#            $(SRC_DIR)/graph.cpp \
#            $(SRC_DIR)/tsp_bruteforce.cpp \
#            $(SRC_DIR)/tsp_ant.cpp \
#            $(SRC_DIR)/stats.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ $^


# $(DEBUG_TARGET): $(SRC_DIR)/main.cpp \
#                 $(SRC_DIR)/graph.cpp \
#                 $(SRC_DIR)/tsp_bruteforce.cpp \
#                 $(SRC_DIR)/tsp_ant.cpp \
#                 $(SRC_DIR)/stats.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ $^


# test: CXXFLAGS_TEST = $(CXXFLAGS_COMMON) -g -O0
# test: clean_test_bin $(TEST_TARGET)
# 	./$(TEST_TARGET)


# unit: CXXFLAGS_TEST = $(CXXFLAGS_COMMON) -g -O0 $(COVERAGE_FLAGS)
# unit: clean_test_bin $(TEST_TARGET)

# $(TEST_TARGET): $(TEST_DIR)/unit_tests.cpp \
#                 $(SRC_DIR)/graph.cpp \
#                 $(SRC_DIR)/tsp_bruteforce.cpp \
#                 $(SRC_DIR)/tsp_ant.cpp \
#                 $(SRC_DIR)/stats.cpp
# 	$(CXX) $(CXXFLAGS_TEST) $^ -o $@ $(UNIT_FLAGS)

# coverage: unit
# 	@./$(TEST_TARGET) > $(TEST_DIR)/test_output.txt 2>&1; \
# 	test_result=$$?; \
# 	checks=$$(grep -m1 -o "Checks: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
# 	fails=$$(grep -m1 -o "Failures: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
# 	errors=$$(grep -m1 -o "Errors: [0-9]\+" $(TEST_DIR)/test_output.txt | awk '{print $$2}' || echo 0); \
# 	passed=$$((checks - fails - errors)); \
# 	cov=$$(gcovr -r . --filter='src/' --filter='tests/' --txt 2>/dev/null | grep TOTAL | awk '{gsub(/%/, "", $$4); print $$4}' || echo 0); \
# 	ts=$$(date -u +"%Y-%m-%dT%H:%M:%S+00:00"); \
# 	echo "{ \"timestamp\": \"$$ts\", \"coverage\": $$cov, \"passed\": $$passed, \"failed\": $$fails }" > $(TEST_DIR)/stud-unit-test-report-prev.json; \
# 	cat $(TEST_DIR)/stud-unit-test-report-prev.json; \
# 	exit $$test_result


# clean_objs:
# 	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o


# clean_bin:
# 	rm -f $(TARGET) $(DEBUG_TARGET)


# clean_test_bin:
# 	rm -f $(TEST_TARGET)


# clean: clean_objs clean_bin clean_test_bin
# 	rm -f $(SRC_DIR)/*.gcda $(SRC_DIR)/*.gcno
# 	rm -f $(TEST_DIR)/*.gcda $(TEST_DIR)/*.gcno
# 	rm -f *.gcda *.gcno *.gcov
# 	rm -f $(TEST_DIR)/test_output.txt $(TEST_DIR)/stud-unit-test-report-prev.json


# REPORT_DIR := ../report

# report:
# 	cd $(REPORT_DIR) && \
# 	pdflatex -interaction=batchmode report.tex && \
# 	pdflatex -interaction=batchmode report.tex && \
# 	gs -sDEVICE=pdfwrite \
# 		-dCompatibilityLevel=1.4 \
# 		-dNOPAUSE \
# 		-dOptimize=true \
# 		-dQUIET \
# 		-dBATCH \
# 		-dRemoveUnusedFonts=true \
# 		-dRemoveUnusedImages=true \
# 		-dOptimizeResources=true \
# 		-dDetectDuplicateImages \
# 		-dCompressFonts=true \
# 		-dEmbedAllFonts=true \
# 		-dSubsetFonts=true \
# 		-dPreserveAnnots=true \
# 		-dPreserveMarkedContent=true \
# 		-dPreserveOverprintSettings=true \
# 		-dPreserveHalftoneInfo=true \
# 		-dPreserveOPIComments=true \
# 		-dPreserveDeviceN=true \
# 		-dMaxInlineImageSize=0 \
# 		-sOutputFile=report_compressed.pdf \
# 		report.pdf && \
# 	mv report_compressed.pdf report.pdf



# clean-report:
# 	cd $(REPORT_DIR) && \
# 	rm -f *.aux *.log *.toc *.lof *.lot *.out *.synctex.gz *.bbl *.blg *.fls *.fdb_latexmk report.pdf