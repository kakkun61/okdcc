.PHONY: check
check:
	cspell --color "**"

.PHONY: format
format:
	treefmt
