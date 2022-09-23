#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Hello = "Hel";
    static std::string Request="Req";
    static std::string Show_Balance="Bal";
    static std::string Delete_request="Del";
    static std::string Send_my_requests="Mreq";

}

#endif //CLIENSERVERECN_COMMON_HPP