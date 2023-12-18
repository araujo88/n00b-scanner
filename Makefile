all:
	go build

run:
	go run .

test:
	go test -cover -race .
