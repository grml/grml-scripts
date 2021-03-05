.DEFAULT_GOAL:=help

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

all: codecheck spellcheck ## Run codecheck and spellcheck

codecheck: shellcheck pythoncheck ## Run shellcheck and pythoncheck

shellcheck: ## Run shellcheck
	for shellfile in $$(ls usr_bin/* usr_sbin/* usr_share/* | grep -v "iimage" | grep -v "make_chroot_jail"); do head -1 "$${shellfile}" | grep -q "/bin/bash\|/bin/sh" && shellcheck -e SC1117 -x "$${shellfile}"; done

.ONESHELL:
pythoncheck: ## Run shellcheck
	for pythonfile in usr_bin/* usr_sbin/* usr_share/*; do
		if head -1 "$${pythonfile}" | grep -q "python"; then
			flake8 "$${pythonfile}"
			isort --check "$${pythonfile}"
			black --check "$${pythonfile}"
		fi
	done

spellcheck: ## Run spellcheck
	spellintian manpages/*
