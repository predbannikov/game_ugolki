Пытался следовать модели MVC, все данные и алгоритмы помещал в модель GameModel. Всё что должно быть визуализировано, вынесено в отдельные Классы View , которые получают уведомление о том, что нужно перерисоваться. Управление состоянием в самой игре(модели) выполняется контроллерами. В процессе выполнения задания, я вспомнил ещё про метод комбинаторики, и хотел применить его для различных комбинаций типа
	1 Запомнить состояние пешек врага в интересуемой области
	2 Найти пути, результат поместить в общий стек путей
	3 Сбросить состояние своих пешек которые добрались до цели
	4 Проверить целесообразность  хода, удалить, все нецелесообразные ходы, из списка путей
	12324
	1242
	1324
	и так далее...	
поэтому часть алгоритмов вынесена в Контроллер. Ещё в данной задаче возможно пригодился бы метод обнаружения образовавшихся внутри пешек пустот, или обнаружение замкнутых границ к пустотам.
На данный момент есть проблемы с зацикливанием. Но, что хотел сделал, можно дальше поиграться с добиванием цели к лучшему результату.

Для рендеринга использовал двигатель HGE, так как в задании была и ссылка на его русскую документацию. Сильно копаться в изучении его документации я не стал. В описание на github.com , инструкция по созданию VS проекта не сработала, разбирать почему я тоже не стал, просто использовал настроенный cmake проект из туториалов. Думаю если вы скинули и документацию, значит вы имели дело с ним раньше и запустите исходный код, который я вам прислал. Либо можно извлечь из содержимое архива в папку src/fonted и запустить нужный cmake_studio*.bat из любой временной папки, где и будет расположен файл HGE.sln, а в нём проект fonted.

После запуска, нужно выделить место на игровой доске куда будут расположены пешки врага. Я решил одной статической расстановкой пешек не ограничиваться, можно создавать место прямоугольной формы. Для решения задачи, придумал мифическую точку в расположении пешек, назвал её MIP (most interesting point), наиболее интересная точка, на которую и ориентируюсь при расчёте алгоритмов для поисков маршрутов. Эта точка рассчитывается  после создания места расположения пешек. 
Правая кнопка пропуск хода.
