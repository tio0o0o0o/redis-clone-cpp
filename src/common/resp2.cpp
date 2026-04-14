#include "../../include/resp2.h"
#include "../../include/logger.h"
#include <charconv>
#include <execution>
#include <fmt/base.h>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

std::string encode_simple_string(std::string input) {
  return "+" + input + "\r\n";
}

std::string encode_error(std::string input) { return "-" + input + "\r\n"; }

std::string encode_integer(int input) {
  return ":" + std::to_string(input) + "\r\n";
}

std::string encode_bulk_string(std::string input) {
  return "$" + std::to_string(input.length()) + "\r\n" + input + "\r\n";
}

std::string encode_null() { return "$-1\r\n"; }

std::string encode_array(std::vector<std::shared_ptr<RespValue>> input);

std::string serialize(RespValue resp_value) {
  if (resp_value.type == simple_string) {
    return encode_simple_string(std::get<std::string>(resp_value.value));
  } else if (resp_value.type == bulk_string) {
    return encode_bulk_string(std::get<std::string>(resp_value.value));
  } else if (resp_value.type == integer) {
    return encode_integer(std::get<int>(resp_value.value));
  } else if (resp_value.type == error) {
    return encode_error(std::get<std::string>(resp_value.value));
  } else if (resp_value.type == null) {
    return encode_null();
  } else if (resp_value.type == array) {
    return encode_array(
        std::get<std::vector<std::shared_ptr<RespValue>>>(resp_value.value));
  } else {
    return "";
  }
}

std::string encode_array(std::vector<std::shared_ptr<RespValue>> input) {
  std::string output = "*" + std::to_string(input.size()) + "\r\n";

  for (int i = 0; i < input.size(); i++) {
    output += serialize(*input[i]);
  }
  return output;
}

int find_crlf_index(std::string_view input) {
  if (input.size() < 2) {
    return -1;
  }
  for (int i = 0; i < input.size() - 1; i++) {
    if (input[i] == '\r' && input[i + 1] == '\n') {
      return i;
    }
  }
  return -1;
}

// +string\r\n
ParseResult parse_simple_string(std::string_view input) {

  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  std::string_view value = input.substr(1, crlf_index - 1);
  RespValue resp_value;
  resp_value.type = simple_string;
  resp_value.value = std::string(value);
  ParseResult parse_result;
  parse_result.resp_value = resp_value;
  parse_result.consumed = crlf_index + 2;

  return parse_result;
}

// $11\r\nbulk string\r\n
ParseResult parse_bulk_string(std::string_view input) {
  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  if (input.length() < crlf_index + 3) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  int crlf_index_end = find_crlf_index(input.substr(crlf_index + 2));

  if (crlf_index_end == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  int length;
  auto from_chars_res =
      std::from_chars(input.data() + 1, input.data() + crlf_index, length);
  if (from_chars_res.ec != std::errc{}) {
    LOG_ERROR("Failed to parse bulk string");
    return parse_failed();
  }
  if (length < 0) {
    LOG_ERROR("Failed to parse bulk string");
    return parse_failed();
  }

  RespValue resp_value;
  resp_value.value = std::string(input.substr(crlf_index + 2, crlf_index_end));

  resp_value.type = bulk_string;

  ParseResult parse_result;
  parse_result.resp_value = resp_value;
  parse_result.consumed = crlf_index + crlf_index_end + 4;

  return parse_result;
}

// :integer\r\n
ParseResult parse_integer(std::string_view input) {
  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  int value;
  RespValue resp_value;
  resp_value.type = integer;
  auto from_chars_res =
      std::from_chars(input.data() + 1, input.data() + crlf_index, value);

  if (from_chars_res.ec != std::errc{}) {
    LOG_ERROR("Failed to convert string to int");
    return parse_failed();
  }

  resp_value.value = value;
  ParseResult parse_result;
  parse_result.resp_value = resp_value;
  parse_result.consumed = crlf_index + 2;

  return parse_result;
}

// -error\r\n
ParseResult parse_error(std::string_view input) {
  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  std::string_view value = input.substr(1, crlf_index - 1);
  RespValue resp_value;
  resp_value.type = error;
  resp_value.value = std::string(value);
  ParseResult parse_result;
  parse_result.resp_value = resp_value;
  parse_result.consumed = crlf_index + 2;

  return parse_result;
}

// *2\r\n$5\r\nhello\r\n$5\r\nworld\r\n
ParseResult parse_array(std::string_view input) {
  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  int total_consumed = crlf_index + 2;

  int array_length;
  auto from_chars_res = std::from_chars(
      input.data() + 1, input.data() + crlf_index, array_length);
  if (from_chars_res.ec != std::errc{}) {
    LOG_ERROR("Would not parse array");
    return parse_failed();
  }

  input = input.substr(crlf_index + 2);

  std::vector<std::shared_ptr<RespValue>> output_vector;

  for (int i = 0; i < array_length; i++) {
    ParseResult parse_result = parse(input);
    total_consumed += parse_result.consumed;
    if (parse_result.consumed == -1) {
      return parse_failed();
    } else if (parse_result.consumed == 0) {
      return parse_incomplete();
    } else {

      output_vector.push_back(
          std::make_shared<RespValue>(parse_result.resp_value));
      input = input.substr(parse_result.consumed);
    }
  }
  RespValue output_resp_value;
  output_resp_value.value = output_vector;
  output_resp_value.type = array;
  ParseResult parse_result;
  parse_result.resp_value = output_resp_value;
  parse_result.consumed = total_consumed;
  return parse_result;
}

// $-1\r\n
ParseResult parse_null(std::string_view input) {
  int crlf_index = find_crlf_index(input);

  if (crlf_index == -1) {
    LOG_INFO("Parse incomplete");
    return parse_incomplete();
  }

  if (input.substr(0, 5) == "$-1\r\n") {
    RespValue resp_value;
    resp_value.type = null;
    resp_value.value = "";
    ParseResult parse_result;
    parse_result.resp_value = resp_value;
    parse_result.consumed = 5;
    return parse_result;
  } else {
    return parse_failed();
  }
}

ParseResult parse_incomplete() {
  RespValue resp_value;
  resp_value.type = incomplete;
  resp_value.value = "";
  ParseResult parse_result;
  parse_result.consumed = 0;
  parse_result.resp_value = resp_value;
  return parse_result;
}

ParseResult parse_failed() {
  RespValue resp_value;
  resp_value.type = failed;
  resp_value.value = "";
  ParseResult parse_result;
  parse_result.consumed = -1;
  parse_result.resp_value = resp_value;
  return parse_result;
}

ParseResult parse(std::string_view input) {
  if (input.length() >= 1) {
    switch (input[0]) {
    case '$':
      if (input.length() >= 2) {
        if (input[1] == '-') {
          return parse_null(input);
        } else {
          return parse_bulk_string(input);
        }
      } else {
        return parse_incomplete();
      }
    case '+':
      return parse_simple_string(input);
    case '-':
      return parse_error(input);
    case ':':
      return parse_integer(input);
    case '*':
      return parse_array(input);
    default:
      LOG_ERROR("Failed to parse");
      return parse_failed();
    }
  } else {
    return parse_incomplete();
  }
}
