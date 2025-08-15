.DEFAULT_GOAL:=help

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

allcheck: codecheck spellcheck ## Run codecheck and spellcheck

codecheck: shellcheck pythoncheck ## Run shellcheck and pythoncheck

.ONESHELL:
shellcheck: ## Run shellcheck
	@RETURN=0
	@for shellfile in $$(ls usr_bin/* usr_sbin/* usr_share/*); do
	@	file "$${shellfile}" | grep -q shell && (shellcheck -x "$${shellfile}" || RETURN=1)
	@done
	@exit $${RETURN}

.ONESHELL:
pythoncheck: ## Run pythoncheck (flakecheck, isortcheck + blackcheck)
	@RETURN=0
	@for pythonfile in usr_bin/* usr_sbin/* usr_share/*; do
	@	if head -1 "$${pythonfile}" | grep -q "python"; then
	@		flake8 --max-line-length 88 "$${pythonfile}" || RETURN=1
	@		isort --check "$${pythonfile}" || RETURN=1
	@		black --check "$${pythonfile}" || RETURN=1
	@	fi
	@done
	@exit $${RETURN}

.ONESHELL:
flakecheck: ## Run flake8 only
	@RETURN=0
	@for pythonfile in usr_bin/* usr_sbin/* usr_share/*; do
	@	if head -1 "$${pythonfile}" | grep -q "python"; then
	@		flake8 --max-line-length 88 "$${pythonfile}" || RETURN=1
	@	fi
	@done
	@exit $${RETURN}

.ONESHELL:
isortcheck: ## Run isort --check only
	@RETURN=0
	@for pythonfile in usr_bin/* usr_sbin/* usr_share/*; do
	@	if head -1 "$${pythonfile}" | grep -q "python"; then
	@		isort --check "$${pythonfile}" || RETURN=1
	@	fi
	@done
	@exit $${RETURN}

.ONESHELL:
blackcheck: ## Run black --check only
	@RETURN=0
	@for pythonfile in usr_bin/* usr_sbin/* usr_share/*; do
	@	if head -1 "$${pythonfile}" | grep -q "python"; then
	@		black --check "$${pythonfile}" || RETURN=1
	@	fi
	@done
	@exit $${RETURN}

.ONESHELL:
spellcheck: ## Run spellcheck
	@OUTPUT="$$(spellintian manpages/*)"
	@echo $$OUTPUT
	@if [ -n "$$OUTPUT" ]; then
	@	false
	@fi
