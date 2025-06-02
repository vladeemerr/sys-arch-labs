# Задание №5

## Условие задания
- [x] 1. Для данных, хранящихся в реляционной базе PostgreSQL реализуйте шаблон
сквозное чтение и сковзная запись;
- [x] 2. В качестве кеша -- используйте Redis/Valkey;
- [x] 3. Замерьте производительность запросов на чтение данных с и без кеша с
использованием утилиты `wrk`, изменяя количество потоков из которых
производятся запросы (1, 5, 10);
- [x] 4. Актуализируйте модель архитектуры в Structurizr DSL;
- [x] 5. Ваши сервисы должны запускаться через docker-compose командой
`docker-compose up` (создайте Docker файлы для каждого сервиса).

## Замеры производительности

Чтобы запустить `scripts/benchmark.sh` скрипт, понадобятся `curl`, `wrk` и `jq`.

Valkey в userver конфигурации использует пул из одного потока, их
можно увеличить при необходимости. (См. файл: `user-service/config.yaml`)

### С кэшем Valkey

```
*** Bench existing user (1 thread)
Running 15s test @ http://localhost:8080/user/tomcat
  1 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.11ms  680.65us  23.35ms   93.59%
    Req/Sec     9.00k     1.21k   13.07k    79.33%
  134474 requests in 15.01s, 84.13MB read
  Non-2xx or 3xx responses: 134474
Requests/sec:   8961.76
Transfer/sec:      5.61MB

*** Bench non-existing user (1 thread)
Running 15s test @ http://localhost:8080/user/dude
  1 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.87ms  367.36us  10.18ms   85.97%
    Req/Sec    11.44k   811.95    12.97k    65.33%
  170683 requests in 15.00s, 106.78MB read
  Non-2xx or 3xx responses: 170683
Requests/sec:  11377.74
Transfer/sec:      7.12MB

*** Bench existing user (5 threads)
Running 15s test @ http://localhost:8080/user/tomcat
  5 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.86ms  346.64us  10.05ms   86.67%
    Req/Sec     2.36k   210.92     4.91k    79.65%
  176653 requests in 15.10s, 110.52MB read
  Non-2xx or 3xx responses: 176653
Requests/sec:  11699.72
Transfer/sec:      7.32MB

*** Bench non-existing user (5 threads)
Running 15s test @ http://localhost:8080/user/dude
  5 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.88ms  404.50us  13.67ms   89.40%
    Req/Sec     2.32k   221.77     2.77k    69.07%
  172827 requests in 15.01s, 108.12MB read
  Non-2xx or 3xx responses: 172827
Requests/sec:  11517.37
Transfer/sec:      7.21MB

*** Bench existing user (10 threads)
Running 15s test @ http://localhost:8080/user/tomcat
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.88ms  388.74us  12.25ms   88.73%
    Req/Sec     1.17k   104.47     1.51k    64.67%
  174257 requests in 15.01s, 109.02MB read
  Non-2xx or 3xx responses: 174257
Requests/sec:  11607.83
Transfer/sec:      7.26MB

*** Bench non-existing user (10 threads)
Running 15s test @ http://localhost:8080/user/dude
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   836.58us  364.77us  13.13ms   91.34%
    Req/Sec     1.22k    92.52     1.39k    73.93%
  182487 requests in 15.01s, 114.17MB read
  Non-2xx or 3xx responses: 182487
Requests/sec:  12160.02
Transfer/sec:      7.61MB
```

### Без кэша

```
*** Bench existing user (1 thread)
Running 15s test @ http://localhost:8080/user/tomcat
  1 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    25.03ms    4.30ms  67.14ms   77.13%
    Req/Sec   401.27     67.13   606.00     64.67%
  5996 requests in 15.01s, 3.75MB read
  Non-2xx or 3xx responses: 5996
Requests/sec:    399.41
Transfer/sec:    255.87KB

*** Bench non-existing user (1 thread)
Running 15s test @ http://localhost:8080/user/dude
  1 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    22.02ms    5.37ms  47.20ms   66.21%
    Req/Sec   455.96    107.54   696.00     55.33%
  6815 requests in 15.01s, 4.26MB read
  Non-2xx or 3xx responses: 6815
Requests/sec:    453.95
Transfer/sec:    290.81KB

*** Bench existing user (5 threads)
Running 15s test @ http://localhost:8080/user/tomcat
  5 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    20.19ms    3.95ms  33.64ms   64.84%
    Req/Sec    99.31     19.85   141.00     62.00%
  7432 requests in 15.02s, 4.65MB read
  Non-2xx or 3xx responses: 7432
Requests/sec:    494.85
Transfer/sec:    317.01KB

*** Bench non-existing user (5 threads)
Running 15s test @ http://localhost:8080/user/dude
  5 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    16.69ms    1.51ms  29.47ms   91.20%
    Req/Sec   120.25     11.14   141.00     81.07%
  8988 requests in 15.01s, 5.62MB read
  Non-2xx or 3xx responses: 8988
Requests/sec:    598.63
Transfer/sec:    383.50KB

*** Bench existing user (10 threads)
Running 15s test @ http://localhost:8080/user/tomcat
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    21.38ms    3.77ms  46.02ms   66.16%
    Req/Sec    46.77      9.15    70.00     75.67%
  7017 requests in 15.02s, 4.39MB read
  Non-2xx or 3xx responses: 7017
Requests/sec:    467.27
Transfer/sec:    299.35KB

*** Bench non-existing user (10 threads)
Running 15s test @ http://localhost:8080/user/dude
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    20.59ms    3.67ms  34.93ms   60.67%
    Req/Sec    48.56      9.58    70.00     70.47%
  7284 requests in 15.02s, 4.56MB read
  Non-2xx or 3xx responses: 7284
Requests/sec:    485.06
Transfer/sec:    310.74KB
```

**Выводы**: с кэшем количество обработанных запросов увеличилась в 30x раз.
Конфигурация userver на данный момент аллоцирует по подному потоку на каждый
компонент, включая и Valkey тоже. Если выкрутить количество потоков, то,
предсказуемо, при чтении количество запросов увеличится.
