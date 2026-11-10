// Copyright (C) 2019-2026 Marek Zalewski aka Drwalin
//
// This file is part of Drwalin_util project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_COMMAND_PARSER_HPP
#define DRWUTIL_COMMAND_PARSER_HPP

#include <memory>
#include <vector>
#include <string>
#include <functional>

class command_parser
{
public:
	command_parser();
	~command_parser();

	void initialize_commands();

	void parse_single_command(const std::string_view command);
	void parse_commands(const std::string_view commands);
	void parse_commands(std::istream &input);

	void register_custom_command(
		const std::vector<std::string> &command_names, std::string description,
		std::function<void(const std::vector<std::string_view> &args)>
			function);

	void register_custom_command(
		const std::vector<std::string> &command_names, std::string description,
		std::function<void(const std::string_view args)> function);

	void _internal_parse_command(const std::string_view command);

	std::string get_full_help() const;
	std::string get_commands_list() const;

private:
	struct command_storage {
		std::function<void(const std::vector<std::string_view> &args)>
			function_args;
		std::function<void(const std::string_view args)> function_line;
		std::vector<std::string> alternatives;
		std::string description;
	};

	void register_command(std::shared_ptr<command_storage> com,
						  std::string description);

	std::unordered_map<std::string, std::shared_ptr<command_storage>> commands;
};
#endif
