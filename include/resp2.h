#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <variant>
#include <vector>

enum RespType {
  simple_string,
  error,
  integer,
  bulk_string,
  array,
  null,
  failed
};

class RespValue {
public:
  RespType type;
  std::variant<std::string, int, std::vector<std::string>,
               std::vector<std::shared_ptr<RespValue>>>
      value;
};

class ParseResult {
public:
  RespValue resp_value;
  size_t consumed;
};

std::string encode_simple_string(std::string input);
std::string encode_error(std::string input);
std::string encode_integer(std::string input);
std::string encode_bulk_string(std::string input);
std::string encode_array(std::vector<std::string> input);
std::string encode_null();

RespValue parse_simple_string(std::string input);
RespValue parse_bulk_string(std::string input);
RespValue parse_integer(std::string input);
RespValue parse_error(std::string input);
RespValue parse_array(std::string input);
RespValue parse_null();
RespValue parse_failed();
ParseResult parse(std::string input);
