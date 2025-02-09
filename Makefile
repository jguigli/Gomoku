all: 
	docker-compose up --build -d

re: clean
	docker-compose up --build -d

clean:

fclean: clean
	docker-compose down


phony: all re clean fclean