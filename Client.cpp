#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"
#include "Birza.h"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const std::string& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);
        std::cout << "Menu:\n"
                     " 1) Hello Request\n"
                     " 2) Exit\n"
                     " 3) Create request\n"
                     " 4) Show balance\n"
                     " 5) Delete request\n"
                     << std::endl;
        while (true)
        {
            // Тут реализовано "бесконечное" меню.


            short menu_option_num;
            std::string input;
            std::cout<<">";
            std::cin>>input;
            try //Обрабатываем неправильный ввод.
            {
                menu_option_num=std::stoi(input);
            }
            catch(const std::exception& e)
            {
                std::cout<<" Invalid input."<<std::endl;
                std::cout << "Menu:\n"
                     " 1) Hello Request\n"
                     " 2) Exit\n"
                     " 3) Create request\n"
                     " 4) Show balance\n"
                     " 5) Delete request\n"
                     << std::endl;
                     continue;
            }
            

            
            switch (menu_option_num)
            {
                case 1:
                {
                    // Для примера того, как может выглядить взаимодействие с сервером
                    // реализован один единственный метод - Hello.
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    exit(0);
                    break;
                }
                
                case 3:
                {
                    //Запрос на добавление заявки.
                    char c;
                    int amount,price;
                    std::cout<<" Want buy or sell? [b/s] : ";
                    std::cin>>c;
                    std::cout<<"    enter amount of $:";
                    std::cin>>amount;
                    std::cout<<"    enter price in RUB:";
                    std::cin>>price;
                    std::string message;
                    message=std::to_string(amount)+ " " + std::to_string(price) + " " + c;
                    
                    SendMessage(s,my_id,Requests::Request,message);
                    auto rec_=ReadMessage(s);
                    break;
                    
                }
                case 4:
                {
                    //Запрос на показ баланса.
                    SendMessage(s,my_id,Requests::Show_Balance," ");
                    std::cout<<ReadMessage(s);
                    break;
                }
                case 5:
                {
                    //Запрос на удаление заявки
                    int del_id;
                    std::string response;
                    SendMessage(s,my_id,Requests::Send_my_requests," ");
                    response=ReadMessage(s);
                    if (response=="Error") {std::cout<< " Requests list is empty"<<std::endl;break;}
                    std::cout<<response;
                    std::cout<<" Enter id of request, you want to delete: ";
                    std::cin>>del_id;
                    SendMessage(s,my_id,Requests::Delete_request,std::to_string(del_id));
                    std::cout<<ReadMessage(s);
                    break;
                }
                default:
                {
                    std::cout<<"Unknow user command\n";
                                    std::cout << "Menu:\n"
                     " 1) Hello Request\n"
                     " 2) Exit\n"
                     " 3) Create request\n"
                     " 4) Show balance\n"
                     " 5) Delete request\n"
                     << std::endl;

                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}