DAYS := $(wildcard day*)

GREEN := \033[0;32m
RED := \033[0;31m
NC := \033[0m

.PHONY: test
test:
	@echo "Running all tests... $(DAYS)"
	@for day in $(DAYS); do \
		echo "Running test for $$day"; \
		if $(MAKE) -C $$day test; then \
			echo  "    $(GREEN)Test passed for $$day$(NC)"; \
		else \
			echo  "    $(RED)Test failed for $$day$(NC)"; \
		fi; \
	done
	@echo "All tests completed"
