#pragma once

#include <optional>
#include <string_view>

static const int DEFAULT_EXIT_CODE = -1;

void halt(
  std::optional<std::string_view> reason = std::nullopt,
  int exit_code = DEFAULT_EXIT_CODE
);