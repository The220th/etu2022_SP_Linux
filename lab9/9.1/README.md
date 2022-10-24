# Компиляция и запуск

## Раздельно

``` bash
> g++ prog1.cpp -o prog1
> g++ prog2.cpp -o prog2

# Следующие 2 строки запускать в любом порядке
> ./prog1 {shared_mem_key_1} {shared_mem_key_2}
> ./prog2 {shared_mem_key_2} {shared_mem_key_1}
# {shared_mem_key_1} - это положительно число
# {shared_mem_key_2} - это положительно число
```

## Всё в одном

``` bash
> g++ main.cpp -o main

# Следующие 2 строки запускать в любом порядке
> ./main 0 {shared_mem_key_1} {shared_mem_key_2}
> ./main 1 {shared_mem_key_2} {shared_mem_key_1}
# {shared_mem_key_1} - это положительно число
# {shared_mem_key_2} - это положительно число
```