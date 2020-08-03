
          
	Заготовка для проектов на базе STM32H743Z ( плата NUCLEO-H743ZI ).

    Запущена FreeRTOS 10.3.1 куча закинута в RAM_D3 (64K)
	Подключена отладка по UART (printf)
	Подключен простой терминал для отправки команд просмотра статистики FreeRTOS


Пример листринга терминала:

______________________________________________

   NUCLEO-H743ZI 
   DATA: Aug  3 2020 
   TIME: 22:19:46 
   CPU FREQ = 480000000 Hz 
______________________________________________

>freertos all

Time work  00:00:00:06:500 ms

FreeHeapMemSize     59616 byte

Number of tasks      4

Number   Name     Pri  FStсk State Time% Time tick
 1      SysTask   28     144   B    <1%     364
 2         IDLE    0     108   R    99%     6499221
 3      Tmr Svc    2     212   B    <1%     11
 4     CmdTrmnl   28     526   X    <1%     417
