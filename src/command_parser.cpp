// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <algorithm>
#include <string>
#include <iostream>
#include <streambuf>

#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/string_util.hpp"
#include "../include/drwalin_util/command_parser.hpp"

namespace
{
class byte_buf : public std::streambuf
{
public:
	byte_buf(const std::string_view sv)
	{
		char *p = const_cast<char *>(sv.data());
		setg(p, p, p + sv.size());
	}
};

struct istream_buf {
	inline istream_buf(const std::string_view sv) : buf(sv), stream(&buf) {}

	byte_buf buf;
	std::istream stream;
};
} // namespace

namespace dutl
{
command_parser::command_parser() {}

command_parser::~command_parser() {}

void command_parser::parse_single_command(const std::string_view command)
{
	_internal_parse_command(command);
}

void command_parser::parse_commands(const std::string_view commands)
{
	size_t pos = 0, next = 0;
	while (pos > commands.size()) {
		pos = commands.find_first_not_of("\n\r", pos);
		next = commands.find_first_of("\n\r", pos);
		_internal_parse_command(commands.substr(pos, next - pos));
		pos = next;
	}
}

void command_parser::parse_commands(std::istream &input)
{
	input >> std::ws;
	std::string line;
	while (input.eof() == false && input.good()) {
		line = "";
		std::getline(input, line);
		input >> std::ws;
		parse_single_command(line);
	}
}

std::string command_parser::get_full_help() const
{
	std::string ret;
	for (const auto &it : commands) {
		if (it.first == it.second->alternatives[0]) {
			if (ret.size() > 0) {
				ret += "\n\n";
			}
			ret += it.second->description;
		}
	}
	return ret;
}

std::string command_parser::get_commands_list() const
{
	std::string ret;
	for (const auto &it : commands) {
		if (it.first == it.second->alternatives[0]) {
			ret += "\n  ";
			ret += it.first;
		}
	}
	return ret;
}

void command_parser::register_command(
	std::shared_ptr<command_storage> cmd_storage, std::string description)
{
	std::string conflict = "";
	for (int i = 0; i < cmd_storage->alternatives.size(); ++i) {
		std::string &cmd = cmd_storage->alternatives[i];
		std::string old_cmd = cmd;
		for (int j = 1; j <= 1000; ++j) {
			std::transform(cmd.begin(), cmd.end(), cmd.begin(),
						   [](auto c) { return std::tolower(c); });
			auto it = commands.find(cmd);
			if (it != commands.end()) {
				cmd = old_cmd + "_" + std::to_string(j);
				cmd_storage->alternatives[i] = cmd;
				conflict = it->second->alternatives[0];
			} else {
				if (j > 1) {
					LOG_ERROR("there is conflict between `%s` and `%s` of: "
							  "`%s`, replacing with: `%s` for `%s`",
							  conflict.c_str(),
							  cmd_storage->alternatives[0].c_str(),
							  old_cmd.c_str(), cmd.c_str(),
							  cmd_storage->alternatives[0].c_str());
				}
				commands[cmd] = cmd_storage;
				break;
			}
		}
	}

	cmd_storage->description = "";
	for (auto cmd : cmd_storage->alternatives) {
		if (cmd_storage->description.size() > 0) {
			cmd_storage->description += ", ";
		}
		cmd_storage->description += cmd;
	}
	cmd_storage->description +=
		"\n  " + std::replace_all(description, "\n", "\n  ");
}

void command_parser::register_custom_command(
	const std::vector<std::string> &command_names, std::string description,
	std::function<void(const std::vector<std::string_view> &args)> function)
{
	std::shared_ptr<command_storage> cmd_storage =
		std::make_shared<command_storage>();
	cmd_storage->alternatives = command_names;
	cmd_storage->function_args = function;
	register_command(cmd_storage, description);
}

void command_parser::register_custom_command(
	const std::vector<std::string> &command_names, std::string description,
	std::function<void(const std::string_view args)> function)
{
	std::shared_ptr<command_storage> cmd_storage =
		std::make_shared<command_storage>();
	cmd_storage->alternatives = command_names;
	cmd_storage->function_line = function;
	register_command(cmd_storage, description);
}

void command_parser::_internal_parse_command(const std::string_view _command)
{
	std::string_view command = std::trim(_command, " \n\r\t\v");
	size_t pos = command.find_first_of(" \n\r\t\v");
	std::string_view prog = command.substr(0, pos);
	std::string_view args_str = std::trim(command.substr(pos), " \n\r\t\v");

	/*
	std::vector<std::string> args;
	args.resize(_args.size());
	for (int i = 0; i < args.size(); ++i) {
		args[i] = std::to_string(_args[i]);
		if (i > 0) {
			// todo: substitute variablies like in bash: ${varname}
// 			std::string v = std::to_string(_args[i]);
// 			int64_t ni = 0;
// 			double nf = 0;
// 			if (server_core->config_storage.get_string(v, &(args[i]))) {
// 				continue;
// 			} else if (server_core->config_storage.get_integer(v, &ni)) {
// 				args[i] = std::to_string(ni);
// 			} else if (server_core->config_storage.get_float(v, &nf)) {
// 				args[i] = std::to_string(nf);
// 			} else {
// 				args[i]
// 			}
		}
	}
	*/

	std::string cmd = std::to_string(prog);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(),
				   [](auto c) { return std::tolower(c); });
	auto it = commands.find(cmd);
	if (it != commands.end()) {
		if (it->second->function_args) {
			auto _args =
				std::convert_to_args_list({command.data(), command.size()});
			if (_args.size() == 0) {
				return;
			}
			it->second->function_args(_args);
		} else if (it->second->function_args) {
			it->second->function_line(args_str);
		}
	} else {
		auto _args =
			std::convert_to_args_list({command.data(), command.size()});
		if (_args.size() == 0) {
			return;
		}
		std::string s;
		for (std::string_view sv : _args) {
			s += "`";
			s += std::to_string(sv);
			s += "`";
		}
		LOG_ERROR("command not found: %s", s.c_str());
	}
}
} // namespace dutl
