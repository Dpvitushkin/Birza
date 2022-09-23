#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include "Common.hpp"
#include "Birza.h"

using boost::asio::ip::tcp;

class Core
{
public:
    // "Регистрирует" нового пользователя и возвращает его ID.
    std::string RegisterNewUser(const std::string& aUserName)
    {
        size_t newUserId = mUsers.size();
        mUsers[newUserId] = aUserName;

        return std::to_string(newUserId);
    }

    // Запрос имени клиента по ID
    std::string GetUserName(const std::string& aUserId)
    {
        const auto userIt = mUsers.find(std::stoi(aUserId));
        if (userIt == mUsers.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second;
        }
    }

private:
    // <UserId, UserName>
    std::map<size_t, std::string> mUsers;
};

Core& GetCore()
{
    static Core core;
    return core;
}

Birza& GetBirza()
{
    static Birza B;
    return B;
}

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    request parse_str(std::string mes)
    {
        try
        {
            request req;
            int spaces=0;
            std::string amount,price;
            char c;
            for(int i=0;i<mes.size();i++)
            {
                c=mes[i];
                if(c==' ') spaces++;
                else if(spaces==0) amount+=c;
                else if(spaces==1) price+=c;
                else if(spaces==2) {req.type=c; break;}
                else break;
            }
            req.amount=std::stoi(amount);
            req.price=std::stoi(price);

            return req;
        }
        catch(const std::exception& e)
        {
            std::cerr <<"Exception: "<<e.what() << '\n';
        }
        
        
    }
    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';

            // Парсим json, который пришёл нам в сообщении.
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];
            auto message=j["Message"];

            std::string reply = " ";//"Error! Unknown request type"
            if (reqType == Requests::Registration)
            {
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                reply = GetCore().RegisterNewUser(j["Message"]);
            }
            else if (reqType == Requests::Hello)
            {
                // Это реквест на приветствие.
                // Находим имя пользователя по ID и приветствуем его по имени.
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            }
            else if (reqType==Requests::Request)
            {
                request req;
                req=parse_str(message);
                
                req.user=GetCore().GetUserName(j["UserId"]);
                GetBirza().add_request(req);
                reply=" ";
                //reply="Request form " + GetCore().GetUserName(j["UserId"]) +" exectuted by server, this is reply.\n";
            }
            else if(reqType==Requests::Show_Balance)
            {
                //Запрос на показ баланса
                std::vector<int> curr_bal;
                curr_bal=GetBirza().Show_balance(GetCore().GetUserName(j["UserId"]));
                reply=std::to_string(curr_bal[0]) + " $ , " + std::to_string(curr_bal[1]) + " RUB.\n";
            }
            else if(reqType==Requests::Send_my_requests)
            {
                //Запрос на показ активных заявок, используется клиентом для выбора удаляемой заявки.
                std::string my_requets=GetBirza().Send_requests(GetCore().GetUserName(j["UserId"]));
                if(!my_requets.empty()) reply=my_requets;
                else reply="Error";
            }
            else if(reqType==Requests::Delete_request)
            {
                //Запрос на удаление заявки.
                std::string mes_str=message;
                int del_id=std::stoi(mes_str);
                bool responce=GetBirza().Delete_request(del_id);
                std::cout<<" "<<del_id<<" Deleted\nCurrent requests:\n";
                GetBirza().print_requests();
                if(responce) reply="Request deleted\n";
                else reply="Fail to delete\n";
            }

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        Birza B;

        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        static Core core;

        server s(io_service);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}