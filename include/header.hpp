// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <nlohmann/json.hpp>
#include <mutex>
#include <vector>
#include <fstream>
#include <thread>
#include <iostream>
#include <string>

const unsigned int time_for_sleep = 900;
const char rootDirectory[]  = "/v1/api/suggest";

namespace beast = boost::beast;
namespace http = beast::http;

using nlohmann::json;
using tcp = boost::asio::ip::tcp;

struct JSON
{
  std::string id = "";
  std::string name = "";
  int cost = 0;
};

template<class Stream>
struct send_lambda
{
  Stream& stream_;
  bool& close_;
  beast::error_code& ec_;
  explicit
  send_lambda(
      Stream& stream,
      bool& close,
      beast::error_code& ec)
      : stream_(stream)
      , close_(close)
      , ec_(ec)
  {
  }

  template<bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields>&& msg) const
  {
    close_ = msg.need_eof();
    http::serializer<isRequest, Body, Fields> sr{msg};
    http::write(stream_, sr, ec_);
  }
};

class HTTP_Server
{
 public:
  HTTP_Server();
  ~HTTP_Server();
  void start();
  void create_session(tcp::socket& socket);
  void create_suggestion();
  std::string update_js_vec(std::string str);
  template<class Body, class Allocator, class Send>
  void handle_request(
      http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send);
  void fail(beast::error_code ec, char const* what);
 private:
  json JS;
  std::vector<struct JSON> vec_sug;
  std::mutex mut;
  unsigned int number_js = 0;
};

#endif // INCLUDE_HEADER_HPP_
