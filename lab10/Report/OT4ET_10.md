# ВВЕДЕНИЕ

Цель работы: знакомство с организацией семафоров, системными функциями, обеспечивающими управление семафорами, и их использованием для решения задач взаимоисключения и синхронизации. 

Задание:

1. Написать две программы, экземпляры которых запускаются параллельно и с различной частотой обращаются к общему файлу. Каждый процесс из первой группы (Писатель) пополняет файл определенной строкой символов и выводит ее на экран вместе с именем программы. Процессы второй группы (Читатели) считывают весь файл и выводят его на экран. Писатели имеют приоритет перед Читателями. Пока один Писатель записывает строку в файл, другим Писателям и всем Читателям запрещено обращение к файлу. Читатели могут одновременно читать файл, если нет Писателей, готовых к записи в файл. Писатель заканчивает работу, после того как выполнит N-кратную запись строки в файл. Читатель заканчивает работу после прочтения текущего содержимого файла. Синхронизация процессов должна выполняться с помощью семафоров.

2. Откомпилировать программы Читатель и Писатель. Запустить на разных терминалах несколько Писателей и Читателей.

# Выполнение работы

Пример протокола выполнения представлен в приложении А.

Как видно, записи писателей не колизят друг с другом. Также видно, что то, что читают читатели, со временем увеличивается. Настраивать режим работы программы можно, меняя `PARALLEL_READING` в исходнике `reader.cpp`.

Исходный код программы представлен в приложении Б.

# ВЫВОД

В результате выполнение лабораторной работы было проведено знакомство с организацией семафоров, системными функциями, обеспечивающими управление семафорами, также они были применены на практике для решения задач взаимоисключения и синхронизации в ОС GNU/Linux. 

# Приложение А

Скрипт `compare_and_run.sh`:

``` bash
#!/bin/bash

g++ writer.cpp -o writer
g++ reader.cpp -o reader

rm ./out.txt

SEM_KEY=1234567;

index_r=1; 
index_w=0;

while (( $index_w < 5 )) # writers
do
    ./writer $index_w $SEM_KEY &
    let "index_w=index_w+1"
done

while (( $index_r < 7 )) # readers
do
    ./reader -$index_r $SEM_KEY &
    let "index_r=index_r+1"
done

```

Пример протокола выполнения:

``` bash
the220th@mainPC ~/git/etu2022_SP_Linux/lab10> bash ./compare_and_run.sh 
[1666856916954] Semaphores with key = 1234567 has been created. Its id = 38. 
[1666856916954] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916954] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916954] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916954] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 
[1666856916955] There was an attachment to the semaphores with key = 1234567. Its id = 38. 

writer 2 want to write 5 lines. 

writer 2 writting in file... 
writer 0 want to write 7 lines. 

writer 4 want to write 11 lines. 

writer 3 want to write 9 lines. 

writer 1 want to write 12 lines. 
OK! 

writer 0 writting in file... OK! 

writer 4 writting in file... OK! 

writer 3 writting in file... OK! 

writer 1 writting in file... OK! 

writer 2 writting in file... OK! 

writer 0 writting in file... OK! 

writer 4 writting in file... OK! 

writer 3 writting in file... OK! 

writer 1 writting in file... OK! 

writer 2 writting in file... OK! 

writer 4 writting in file... OK! 

writer 0 writting in file... OK! 

writer 3 writting in file... OK! 

writer 1 writting in file... OK! 

writer 2 writting in file... OK! 

writer 4 writting in file... OK! 

writer 3 writting in file... OK! 

writer 0 writting in file... OK! 

writer 1 writting in file... OK! 

writer 2 writting in file... OK! 

writer 4 writting in file... OK! 

writer 3 writting in file... OK! 

writer 0 writting in file... OK! 

writer 1 writting in file... OK! 

writer 4 writting in file... OK! 

writer 3 writting in file... OK! 

writer 0 writting in file... OK! 

writer 1 writting in file... OK! 

writer 3 writting in file... OK! 

writer 4 writting in file... OK! 

writer 0 writting in file... OK! 

writer 3 writting in file... OK! 

writer 1 writting in file... OK! 

writer 4 writting in file... writer 0 waiting others readers and writers... OK! 

writer 3 writting in file... OK! 

writer 1 writting in file... OK! 

writer 4 writting in file... OK! 

reader -5 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
"""

writer 1 writting in file... OK! 

writer 4 writting in file... OK! 

writer 1 writting in file... OK! 

reader -6 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
"""

reader -4 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
"""

reader -1 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
"""

reader -2 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
"""

writer 1 writting in file... OK! 

writer 4 writting in file... OK! 

reader -3 read: 
"""
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
[1666856923877] writer 1 write: 185 231 716 899 260 6 116 124 586 829 520 50 98 
[1666856923944] writer 4 write: 201 833 589 461 
"""

writer 1 writting in file... OK! 
Destroying semaphores with key = 1234567 and id = 38... OK! 


```

Файл `out.txt`:

``` txt
[1666856918135] writer 2 write: 476 233 276 
[1666856918231] writer 0 write: 853 252 916 803 982 543 626 752 933 496 887 91 345 251 
[1666856918354] writer 4 write: 783 505 229 395 622 525 400 511 413 446 163 330 112 
[1666856918605] writer 3 write: 965 448 598 850 574 996 552 861 244 360 189 
[1666856918725] writer 1 write: 295 973 908 651 403 967 220 187 209 360 175 767 103 
[1666856918891] writer 2 write: 716 689 730 791 864 333 149 410 265 110 37 978 681 
[1666856919118] writer 0 write: 726 219 632 298 740 146 380 601 197 976 434 798 
[1666856919237] writer 4 write: 32 360 143 321 490 823 158 294 
[1666856919312] writer 3 write: 101 549 837 218 86 192 743 987 37 236 270 842 165 691 
[1666856919467] writer 1 write: 924 511 968 268 378 322 
[1666856919587] writer 2 write: 736 163 543 97 786 353 334 613 723 967 
[1666856919749] writer 4 write: 298 399 454 587 764 
[1666856919782] writer 0 write: 246 453 711 543 757 594 
[1666856919910] writer 3 write: 969 54 
[1666856919968] writer 1 write: 574 515 754 520 623 389 753 767 34 212 82 656 972 
[1666856920213] writer 2 write: 214 648 60 279 732 503 856 308 26 330 836 922 
[1666856920436] writer 4 write: 32 619 441 905 999 217 476 419 570 847 224 
[1666856920566] writer 3 write: 342 736 783 780 49 822 938 305 203 
[1666856920750] writer 0 write: 653 100 588 475 609 231 685 
[1666856920886] writer 1 write: 826 448 527 970 700 724 724 363 445 792 
[1666856921138] writer 2 write: 240 88 367 789 577 956 447 698 457 
[1666856921270] writer 4 write: 795 413 
[1666856921426] writer 3 write: 216 969 41 73 
[1666856921514] writer 0 write: 992 469 43 208 387 540 19 830 512 
[1666856921684] writer 1 write: 436 13 580 142 152 648 999 55 540 64 
[1666856921793] writer 4 write: 602 424 287 53 171 743 182 
[1666856921935] writer 3 write: 88 
[1666856921988] writer 0 write: 909 508 220 379 855 585 609 270 370 477 441 700 716 795 
[1666856922097] writer 1 write: 204 540 976 559 318 230 529 734 790 
[1666856922228] writer 3 write: 260 22 327 10 
[1666856922300] writer 4 write: 135 
[1666856922359] writer 0 write: 930 19 22 413 132 402 547 734 433 148 444 568 187 307 
[1666856922560] writer 3 write: 607 90 
[1666856922623] writer 1 write: 291 704 908 3 3 67 635 620 163 
[1666856922802] writer 4 write: 338 873 686 663 406 217 915 285 880 
[1666856923011] writer 3 write: 259 379 487 975 68 
[1666856923149] writer 1 write: 925 384 768 285 
[1666856923316] writer 4 write: 906 141 547 98 442 304 
[1666856923425] writer 1 write: 443 
[1666856923526] writer 4 write: 917 832 807 358 778 
[1666856923628] writer 1 write: 340 651 814 302 150 105 985 110 848 26 153 899 531 
[1666856923877] writer 1 write: 185 231 716 899 260 6 116 124 586 829 520 50 98 
[1666856923944] writer 4 write: 201 833 589 461 
[1666856924173] writer 1 write: 587 666 585 609 886 700 811 925 383 590 

```

# Приложение Б

Исходный код программы `writer`:

``` cpp
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

// Readers nums in negative, writers num is not negarive.
#define CREATOR_PROG_NUM 0
#define FILE_NAME "out.txt"

void sleep_ms(unsigned ms);
std::string get_cur_time();
/*including min and including max*/
int rnd(int min, int max);

int create_shsem(int prog_num, int sem_key);

void write_to_file(int prog_num, const char* file_name);

/*
Всего планируется создать простых 4 семафора:
    (1) mutex для доступа к файлу на запись
    (2) кол-во желающих записать писателей
    (3) кол-во желающих прочитать читателей
    (4) кол-во всего работающих читателей и писателей вместе

Алгоритм:
    Для писателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Пока есть, что записать:
            3. Инкрементирует (2). 
            4. Ждёт, пока (3) упадёт до нуля. 
            5. Пытается сквозь мьютекс (1) записать в файл очередную строку. 
            6. Декрементирует (2). 
            7. Ждёт рандомное время. Вернуться в пункт 2. 
        8. Декрементирует (4).
        // 9. Если писатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        9. Если писатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        10. Конец
    
    Для читателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Ждёт, пока (2) упадёт до нуля. Как только (2) упало до нуля, то сразу инкрементировать (3). 
                То есть получается 2 операции за раз одновременно: ждать (2) упадёт до нуля и инкрементировать (3). 
        3. Читает. 
        4. Декрементирует (3). 
        // 5. Если читатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        5. Если читатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        6. Конец. 

    На деле нумерация мьютексов начинается с нуля. 
*/

/*
./writer {prog_num} {sem_key}
*/
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "Syntax error. Expected: \"> ./writer {prognum_num} {sem_key}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int prog_num = atoi(argv[1]);
    const int sem_key = atoi(argv[2]);
    if(sem_key <= 0)
    {
        std::cout << "{sem_key} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL) + 13*prog_num);

    const int sem_id = create_shsem(prog_num, sem_key);
    struct sembuf sembuff_buff;

    sembuff_buff = {3, 1, 0};
    semop(sem_id, &sembuff_buff, 1);

    sleep_ms(rnd(1150, 1300));
    

    
        const unsigned times_count = rnd(5, 15);
        std::cout << "\nwriter " << prog_num << " want to write " << times_count << " lines. " << std::endl; 
        for(unsigned gi = 0; gi < times_count; ++gi)
        {
            sembuff_buff = {1, 1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {2, 0, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {0, -1, 0};
            semop(sem_id, &sembuff_buff, 1);

            write_to_file(prog_num, FILE_NAME);

            sembuff_buff = {0, 1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sembuff_buff = {1, -1, 0};
            semop(sem_id, &sembuff_buff, 1);

            sleep_ms(rnd(50, 300));
        }



    sembuff_buff = {3, -1, 0};
    semop(sem_id, &sembuff_buff, 1);

    if(prog_num == CREATOR_PROG_NUM)
    {
        std::cout << "writer " << prog_num << " waiting others readers and writers... " << std::flush; 
        sembuff_buff = {3, 0, 0};
        semop(sem_id, &sembuff_buff, 1);
        std::cout << "Destroying semaphores with key = " << sem_key << " and id = " << sem_id << "... " << std::flush; 
        if(semctl(sem_id, -1, IPC_RMID, 0) < 0)
        {
            perror("Destroy semaphores in shmctl failed");
        }
        else
            std::cout << "OK! " << std::endl; 
    }
    return 0;
}



std::string get_cur_time()
{
    long long long_long_long_long_very_long_value_long_long = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
    return "[" + std::to_string(long_long_long_long_very_long_value_long_long) + "] ";
}

void sleep_ms(unsigned ms)
{
    // mdelay(write_delay_after); // активное ожидание, хз где: <linux/delay.h>? <ams/delay.h>? <sys/delay.h>? 

    // std::this_thread::sleep_for(std::chrono::milliseconds(x)); //   #include <chrono>    and    #include <thread>

    if(usleep(1000*ms) < 0)
        perror("usleep error");
}

int rnd(int min, int max)
{
    return random() % (max-min+1) + min;
}



int create_shsem(int prog_num, int sem_key)
{
    int buff;
    int shsem_id;

    if(prog_num == CREATOR_PROG_NUM)
    {
        buff = semget(sem_key, 4, 0666 | IPC_CREAT | IPC_EXCL);
        if(buff == -1)
        {
            std::cout << "Cannot create semaphores with key = " << sem_key << ", because it is already exist, choose another one. " << std::endl;
            perror("semget failed");
            exit(EXIT_FAILURE);
        }
        shsem_id = buff;
        std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " has been created. Its id = " << shsem_id << ". " << std::endl;
    }
    else
    {
        do
        {
            buff = semget(sem_key, 4, IPC_EXCL);
            if(buff == -1)
            {
                std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " doesn't exist yet. Waiting..." << std::endl;
                sleep(1);
            }
        }while(buff == -1);
        shsem_id = buff;
        std::cout << get_cur_time() << "There was an attachment to the semaphores with key = " << sem_key << ". Its id = " << shsem_id << ". " << std::endl;
    }

    if(prog_num == CREATOR_PROG_NUM)
    {
        struct sembuf sembuff_buff = {0, 1, 0};
        semop(shsem_id, &sembuff_buff, 1);
    }

    return shsem_id;
}

void write_to_file(int prog_num, const char* file_name)
{
    std::cout << "\nwriter " << prog_num << " writting in file... " << std::flush;

    unsigned long write_delay_after = rnd(1, 100);
    int fd = open(file_name, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);

    std::string buffS;
    buffS = get_cur_time() + "writer " + std::to_string(prog_num) + " write: ";
    write(fd, buffS.c_str(), buffS.length());

    unsigned N = rnd(1, 15);
    for(unsigned i = 0; i < N; ++i)
    {
        buffS = std::to_string(rnd(0, 1000)) + " ";
        write(fd, buffS.c_str(), buffS.length());
        if(rnd(0, 1) == 0)
            sleep_ms(random() % 50);
    }

    write(fd, "\n", 1);
    
    sleep_ms(write_delay_after);

    if(close(fd) < 0)
        perror("Cannot close file");

    std::cout << "OK! " << std::endl;
}
```

Исходный код программы `reader`:

``` cpp
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

// Readers nums in negative, writers num is not negarive.
#define CREATOR_PROG_NUM 0
#define PARALLEL_READING 0       // Если PARALLEL_READING == 0, тогда чтение будет по очереди. 
#define FILE_NAME "out.txt"

void sleep_ms(unsigned ms);
std::string get_cur_time();
/*including min and including max*/
int rnd(int min, int max);

int create_shsem(int prog_num, int sem_key);

void read_from_file(int prog_num, const char* file_name);

/*
Всего планируется создать простых 4 семафора:
    (1) mutex для доступа к файлу на запись
    (2) кол-во желающих записать писателей
    (3) кол-во желающих прочитать читателей
    (4) кол-во всего работающих читателей и писателей вместе

Алгоритм:
    Для писателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Пока есть, что записать:
            3. Инкрементирует (2). 
            4. Ждёт, пока (3) упадёт до нуля. 
            5. Пытается сквозь мьютекс (1) записать в файл очередную строку. 
            6. Декрементирует (2). 
            7. Ждёт рандомное время. Вернуться в пункт 2. 
        8. Декрементирует (4).
        // 9. Если писатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        9. Если писатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        10. Конец
    
    Для читателей: 
        1. Инкрементирует (4). Ждёт рандомное время. 
        2. Ждёт, пока (2) упадёт до нуля. Как только (2) упало до нуля, то сразу инкрементировать (3). 
                То есть получается 2 операции за раз одновременно: ждать (2) упадёт до нуля и инкрементировать (3). 
        3. Читает. 
        4. Декрементирует (3). 
        // 5. Если читатель последний из всех читателей и писателей, то уничтожить все семафоры. 
        5. Если читатель создавал семафоры, то он должен дождаться пока все закончат и уничтожить все созданные семафоры. 
        6. Конец. 

    На деле нумерация мьютексов начинается с нуля. 
*/

/*
./reader {prog_num} {sem_key}
*/
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "Syntax error. Expected: \"> ./reader {prognum_num} {sem_key}\"" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int prog_num = atoi(argv[1]);
    const int sem_key = atoi(argv[2]);
    if(sem_key <= 0)
    {
        std::cout << "{sem_key} must be possitive. " << std::endl;
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL) + 13*prog_num);

    const int sem_id = create_shsem(prog_num, sem_key);
    struct sembuf sembuff_buff;

    sembuff_buff = {3, 1, 0};
    semop(sem_id, &sembuff_buff, 1);

    sleep_ms(rnd(1350, 4000));
    
    int buff_ret;    



        #if PARALLEL_READING == 0
            struct sembuf sembuff_buff3[3];
            sembuff_buff3[0] = {1, 0, 0}; sembuff_buff3[1] = {2, 1, 0}; sembuff_buff3[2] = {0, -1, 0};
            buff_ret = semop(sem_id, sembuff_buff3, 3);
        #else
            struct sembuf sembuff_buff2[2];
            sembuff_buff2[0] = {1, 0, 0}; sembuff_buff2[1] = {2, 1, 0};
            semop(sem_id, sembuff_buff2, 2);
        #endif

        read_from_file(prog_num, FILE_NAME);

        #if PARALLEL_READING == 0
            sembuff_buff = {0, 1, 0};
            semop(sem_id, &sembuff_buff, 1);
        #endif

        sembuff_buff = {2, -1, 0};
        semop(sem_id, &sembuff_buff, 1);



    sembuff_buff = {3, -1, 0};
    semop(sem_id, &sembuff_buff, 1);

    if(prog_num == CREATOR_PROG_NUM)
    {
        std::cout << "writer " << prog_num << " waiting others readers and writers... " << std::flush; 
        sembuff_buff = {3, 0, 0};
        semop(sem_id, &sembuff_buff, 1);
        std::cout << "Destroying semaphores with key = " << sem_key << " and id = " << sem_id << "... " << std::flush; 
        if(shmctl(sem_id, IPC_RMID, 0) < 0)
        {
            perror("Destroying semaphores in shmctl failed");
        }
        else
            std::cout << "OK! " << std::endl; 
    }
    return 0;
}



std::string get_cur_time()
{
    long long long_long_long_long_very_long_value_long_long = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
    return "[" + std::to_string(long_long_long_long_very_long_value_long_long) + "] ";
}

void sleep_ms(unsigned ms)
{
    // mdelay(write_delay_after); // активное ожидание, хз где: <linux/delay.h>? <ams/delay.h>? <sys/delay.h>? 

    // std::this_thread::sleep_for(std::chrono::milliseconds(x)); //   #include <chrono>    and    #include <thread>

    if(usleep(1000*ms) < 0)
        perror("usleep error");
}

int rnd(int min, int max)
{
    return random() % (max-min+1) + min;
}



int create_shsem(int prog_num, int sem_key)
{
    int buff;
    int shsem_id;

    if(prog_num == CREATOR_PROG_NUM)
    {
        buff = semget(sem_key, 4, 0666 | IPC_CREAT | IPC_EXCL);
        if(buff == -1)
        {
            std::cout << "Cannot create semaphores with key = " << sem_key << ", because it is already exist, choose another one. " << std::endl;
            perror("semget failed");
            exit(EXIT_FAILURE);
        }
        shsem_id = buff;
        std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " has been created. Its id = " << shsem_id << ". " << std::endl;
    }
    else
    {
        do
        {
            buff = semget(sem_key, 4, IPC_EXCL);
            if(buff == -1)
            {
                std::cout << get_cur_time() << "Semaphores with key = " << sem_key << " doesn't exist yet. Waiting..." << std::endl;
                sleep(1);
            }
        }while(buff == -1);
        shsem_id = buff;
        std::cout << get_cur_time() << "There was an attachment to the semaphores with key = " << sem_key << ". Its id = " << shsem_id << ". " << std::endl;
    }

    if(prog_num == CREATOR_PROG_NUM)
    {
        struct sembuf sembuff_buff = {0, 1, 0};
        semop(shsem_id, &sembuff_buff, 1);
    }

    return shsem_id;
}

void read_from_file(int prog_num, const char* file_name)
{
    int fd = open(file_name, O_RDONLY, S_IRUSR | S_IWUSR);

    std::cout << "\nreader " << prog_num << " read: \n" << "\"\"\"" << std::endl;

    unsigned char c;
    while(read(fd, &c, 1) != 0)
    {
        write(1, &c, 1);
    }

    std::cout << "\"\"\"" << std::endl;

    if(close(fd) < 0)
        perror("Cannot close file");

    sleep_ms(rnd(10, 50));
}
```