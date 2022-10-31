# Компиляция и запуск

``` bash
> g++ server.cpp -o server
> g++ client.cpp -o client

> ./server {port}
> ./client {port}
```

В реализации в репозитории вместо `select` используется `epoll`. 