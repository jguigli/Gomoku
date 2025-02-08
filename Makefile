python := python3

define venvWrapper
	{\
	. bin/activate; \
	$1; \
	}
endef


install:
	@{ \
		echo "Setting up..."; \
		python3 -m venv .; \
		. bin/activate; \
		if [ -f requirements.txt ]; then \
			pip install -r requirements.txt; \
			echo "Installing dependencies...DONE"; \
		fi; \
	}

freeze:
	$(call venvWrapper, pip freeze > requirements.txt)

front:
	$(call venvWrapper, python3 frontend/main.py)

all: 
	docker-compose up --build -d

re: clean
	docker-compose up --build -d

clean:
	@rm -rf  bin/ include/ lib/ lib64 pyvenv.cfg share/

fclean: clean
	docker-compose down


phony: all re clean fclean