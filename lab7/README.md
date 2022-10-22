# Требования

В `fcntl(pipe_fd, ...)` запрещён `O_NONBLOCK`. 

В этом репозитории не так... 

# Компиляция и запуск

# Отдельные файлы

``` bash
> bash ./compile.sh
> ./parent
```

# Все в одном

``` bash
> g++ main.cpp -o main
> ./main
```