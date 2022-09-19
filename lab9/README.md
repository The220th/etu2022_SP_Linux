# Компиляция и запуск

``` bash
> g++ main.cpp -o main

# Следующие 2 строки запускать в любом порядке
> ./main 0 {shared_mem_key_1} {shared_mem_key_2}
> ./main 1 {shared_mem_key_2} {shared_mem_key_1}
# {shared_mem_key_1} - это положительно число
# {shared_mem_key_2} - это положительно число
```