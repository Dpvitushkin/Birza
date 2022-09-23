#include "Birza.h"
#include <iostream>
#include <list>
#include <string>
#include <map>
#include <vector>



std::vector<int> Birza::Show_balance(std::string name)
{
    //Показывает баланс, если пользователь не добавлял заявки, создаем запись в массив балансов пользователей.
    try
    {
        
        auto it = Balance.at(name);
    }
    catch(const std::exception& e)
    {
        Balance.insert({name,{0,0}});
    }
    
    return Balance.at(name);

}

void Birza::Sell_insert(std::list<request> &A,request req)
{
    //Добавляем заявку в очередь продажи
    try
    {
        int key=req.price;
        int l=0;
        int r=A.size()-1;
        std::list<request>::iterator tmp;
        int mid,val,result;

        if(key<(*(A.begin())).price) {A.push_front(req);return;}
        if(key>(*(--A.end())).price) {A.push_back(req);return;}
        
        while(true)
        {
            mid=(l+r)/2;
            tmp=A.begin();
            std::advance(tmp,mid);
            val=(*tmp).price;
            if(key>val) l=mid;
            else r=mid;
            if(r-l<=1) {result=r;break;}
        }
        tmp=A.begin();
        std::cout<<"pos= "<<result<<std::endl;
        std::advance(tmp,result);
        auto it_result = tmp;
        bool simmilar_price = false;
        while((*++tmp).price == (*it_result).price) {simmilar_price=true; }
        if(simmilar_price) {A.insert(tmp,req);}
        else A.insert(it_result,req);
    }
    catch(const std::exception& e)
    {
        std::cout<<"Exception: "<<e.what()<<std::endl;
        exit(0);
    }
}

void Birza::Buy_insert(std::list<request> &A,request req)
{
    //Добавляем заявку в очередь покупки.
    try
    {
        int key=req.price;
        int l=0;
        int r=A.size()-1;
        std::list<request>::iterator tmp;
        int mid,val,result;

        if(key>(*(A.begin())).price) {A.push_front(req);return;}
        if(key<(*(--A.end())).price) {A.push_back(req);return;}
        
        while(true)
        {
            mid=(l+r)/2;
            tmp=A.begin();
            std::advance(tmp,mid);
            val=(*tmp).price;
            if(key<val) l=mid;
            else r=mid;
            if(r-l<=1) {result=r;break;}
        }
        tmp=A.begin();
        std::advance(tmp,result);
        auto it_result = tmp;
        bool simmilar_price = false;
        while((*++tmp).price == (*it_result).price) {simmilar_price=true; }
        if(simmilar_price) {A.insert(tmp,req);}
        else A.insert(it_result,req);
    }
    catch(const std::exception& e)
    {
        std::cout<<"Exception: "<<e.what()<<std::endl;
        exit(0);
    }
}

void Birza::print_requests()
{
    //Печатаем все заявки.
    try
    {
        std::cout<<"Sell:"<<std::endl;
        for(auto it=Sell.begin();it!=Sell.end();it++)
        {
            std::cout<<"   "<<it->id<<" "<<it->user<<" "<<it->amount<<" "<<it->price<<std::endl;
        }
        std::cout<<"Buy:"<<std::endl;
        for(auto it=Buy.begin();it!=Buy.end();it++)
        {
            std::cout<<"   "<<it->id<<" "<<it->user<<" "<<it->amount<<" "<<it->price<<std::endl;
        }

    }
    catch(const std::exception& e)
    {
        std::cout<<"Exception: "<<e.what()<<std::endl;
        exit(0);
    }
}

void Birza::add_request(request req)
{
    // Добавляем заявку, устанавливаем начальный баланс пользователя.
    try
    {
        req.id=ids++;
        try
        {
            auto it = Balance.at(req.user);
        }
        catch(const std::exception& e)
        {
            Balance.insert({req.user,{0,0}});
        }
        if(req.type=='b') Buy_insert(Buy,req);
        if(req.type=='s') Sell_insert(Sell,req);
        make_deal();
    }
    catch(const std::exception& e)
    {
        std::cerr<<"Exception: "<<e.what()<<std::endl;
        exit(0);
    }



}

void Birza::make_deal()
{
    //Реализуем механизм биржи. Он активируется каждый раз при добавлении новой заявки.
    try
    {
        print_requests();
        std::cout<<"-------------------------"<<std::endl;
        if(!Sell.size() || !Buy.size()) {std::cout<<"Empty Buy or Sell"<<std::endl<<"-------------------------"<<std::endl;return;}
        int b_price,s_price;
        int b_amount,s_amount;
        std::string b_user,s_user;
        bool deal_complete=false;
        while(Buy.begin()->price>=Sell.begin()->price && Sell.size() && Buy.size()) 
        {
            deal_complete=true;
            b_price=Buy.begin()->price;
            s_price=Sell.begin()->price;
            b_amount=Buy.begin()->amount;
            s_amount=Sell.begin()->amount;
            s_user=Sell.begin()->user;
            b_user=Buy.begin()->user;
            int profit;

            if(b_amount>=s_amount)
            {
                std::cout<<"user "<<Sell.begin()->user<<"\n"<<"   sold "<<s_amount<<" $ by "<<s_price<<std::endl;
                std::cout<<"   Balance+="<<s_amount*s_price<<std::endl;
                Balance.at(s_user)[1]+=s_amount*s_price;
                Balance.at(s_user)[0]-=s_amount;
                std::cout<<"user "<<Buy.begin()->user<<"\n"<<"   bought "<<s_amount<<" $ by "<<b_price<<std::endl;
                std::cout<<"   Balance-="<<s_amount*b_price<<std::endl;
                Balance.at(b_user)[1]-=s_amount*b_price;
                Balance.at(b_user)[0]+=s_amount;
                Buy.begin()->amount-=s_amount;
                Sell.erase(Sell.begin());
                if(s_amount==b_amount) Buy.erase(Buy.begin());
                profit=s_amount*(b_price-s_price);
                Earned_money+=profit;
                std::cout<<"Birza earned "<<profit<<std::endl<<"Totaly earned "<<Earned_money<<std::endl;
            }
            else
            {
                std::cout<<"user "<<Sell.begin()->user<<"\n"<<"   sold "<<b_amount<<" $ by "<<s_price<<std::endl;
                std::cout<<"   Balance+="<<b_amount*s_price<<std::endl;
                Balance.at(s_user)[1]+=b_amount*s_price;
                Balance.at(s_user)[0]-=b_amount;
                std::cout<<"user "<<Buy.begin()->user<<"\n"<<"   bought "<<b_amount<<" $ by "<<b_price<<std::endl;
                std::cout<<"   Balance-="<<b_amount*b_price<<std::endl;
                Balance.at(b_user)[1]-=b_amount*b_price;
                Balance.at(b_user)[0]+=b_amount;
                Sell.begin()->amount-=b_amount;
                Buy.erase(Buy.begin());
                profit=b_amount*(b_price-s_price);
                Earned_money+=profit;
                std::cout<<"Birza earned "<<profit<<std::endl<<"Totaly earned "<<Earned_money<<std::endl;
            }    
        }
        if(!deal_complete) std::cout<<"No matching deals"<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"Exception: "<<e.what()<<std::endl;
        exit(0);
    }
}

std::string Birza::Send_requests(std::string name)
{
    //Запихиваем все заявки в строку, чтобы отправить сообщением.
    std::string result;
    for(auto it=Buy.begin();it!=Buy.end();it++)
    {
        if (it->user == name) result+=std::to_string(it->id) + " " + it->type + " " + std::to_string(it->amount) + " " + std::to_string(it->price) + "\n";
    }
    for(auto it=Sell.begin();it!=Sell.end();it++)
    {
        if (it->user == name) result+=std::to_string(it->id) + " " + it->type + " " + std::to_string(it->amount) + " " + std::to_string(it->price) + "\n";
    }
    return result;

}

bool Birza::Delete_request(int del_id)
{
    //Удаляем заявку с данным id.
    for(auto it=Buy.begin();it!=Buy.end();it++)
    {
        if (it->id==del_id) {Buy.erase(it); return true;}
    }
    for(auto it=Sell.begin();it!=Sell.end();it++)
    {
        if (it->id==del_id) {Sell.erase(it); return true;}
    }
    return false;
}