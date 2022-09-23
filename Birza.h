#pragma once

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <vector>

struct request //Структура заявки
{
    int id;
    char type;
    std::string user;
    int amount;
    int price;
};

class Birza
{
public:
    //Члены
    std::list<request> Buy;
    std::list<request> Sell;
    std::list<int> Quotation;
    int Earned_money;
    int ids;
    std::map<std::string,std::vector<int>> Balance;
    //Конструктор
    Birza(){ 
        Earned_money=0;
        ids=0;
    };
    //Методы
    void add_request(request);
    void Sell_insert(std::list<request> &A,request req);
    void Buy_insert(std::list<request> &A,request req);
    void make_deal();
    void print_requests();
    std::vector<int> Show_balance(std::string name);
    std::string Send_requests(std::string name);
    bool Delete_request(int del_id);


};