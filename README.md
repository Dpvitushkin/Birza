Витушкин Денис 23.09.2022.

Тестовое задание "Биржа".

1.Принцип работы биржи:
    У нас есть два списка: список активных заявок на продажу(Sell) и на покупку (Buy). Список Sell упорядочен по возрастанию, то есть первой стоит заявка с наименьшей ценой, а список Buy упорядочен по убыванию, то есть первой стоит заявка с наибольшей ценой. Такой подход позволяет мне, как руководителю биржи, зарабатывать деньги на разнице этих цен. Так же этот подход удобен с точки зрения реализации, ведь я знаю что наилучшей для меня сделкой будет сделка между двумя лидерами списков Buy и Sell(ведь они всегда отсортированы), и мне не надо придумывать пути обхода этих списков, я просто сматчиваю заявки лидеров и удаляю неактивные. После удаления неактивной заявки из какого-либо списка, я вновь рассматриваю элементы, которые заняли первые позиции.
    Я взял на себя смелость немного изменить само задание, тк сначала не заметил, что биржа должна быть бездоходной. С самого начала я подумал, что неправильно отдавать продавцу излишки покупателей, или продавать валюту по цене, ниже заявленной покупателем(то есть мне выгодно, если кто-то решит продать доллар по 20 при курсе 60 или купить по 100 при том же курсе).
2.Инструция по сборке:
    Программа представляет собой консольное приложение.
    -Прописываем в терминале в директории с исходниками cmake с указанием пути сборки.
    -Прописываем make.
    -Запускаем сервер -> ./Server.
    -Запускаем клиент -> ./Client.
3.Инструкция по работе программы:
    После запуска сервера и хотя бы одного клиента нам доступны 5 опций:
    1) Приветствие.
    2) Выход.
    3) Добавление заявки.
    4) Просмотр баланса.
    5) Удаление заявки.
    Сервер никакие команды не принемает, работает постоянно и выводит различную информацию о состоянии заявок и запросах клиентов.
4:Описания функций и реализации:
    4.1 Клиент-серверная модель:
        Я использовал предложенный вами каркас клиент-серверной модели, добавил только обработку нового функционала клиентов и инициализацию биржи.
    4.2 Биржа:
        Есть класс биржи, содержащий списки заявок, балансы пользователей и другое. Мы инициализируем экземпляр этого класса в сервере и реализуем его функционал, все функции закомментированы.
5:



