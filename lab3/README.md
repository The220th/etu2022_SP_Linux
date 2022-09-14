# Компиляция и запуск

``` bash
> g++ -O2 main.cpp -o main
> g++ -O2 vfork_exec.cpp -o vfork_exec
> ./main {parent_delay} {fork_delay} {vfork_delay}
```