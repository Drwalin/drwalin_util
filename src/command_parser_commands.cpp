// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <fstream>

#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/string_util.hpp" // IWYU pragma: keep
#include "../include/drwalin_util/command_parser.hpp"

namespace dutl
{
void command_parser::initialize_commands()
{
	register_custom_command(
		{"help", "h", "?"}, "Shows help for commands",
		[this](const std::vector<std::string_view> &args) {
			std::string help_string;
			if (args.size() > 1) {
				for (int i = 1; i < args.size(); ++i) {
					auto it = commands.find(std::to_string(args[i]));
					if (it == commands.end()) {
						LOG_INFO("No command `%s` exists",
								 std::to_string(args[i]).c_str());
					} else {
						if (help_string.size() > 0) {
							help_string += "\n\n";
						}
						help_string += it->second->description;
					}
				}
			} else {
				help_string = get_full_help();
			}
			printf("%s\n", help_string.c_str());
			fflush(stdout);
		});

	register_custom_command(
		{"list_commands", "list"}, "Lists available commands.",
		[this](const std::string_view com) {
			std::string msg = get_commands_list();
			printf("Available commands:\n  %s\n", get_commands_list().c_str());
			fflush(stdout);
		});

	register_custom_command({"source", "run"},
							"Executes contents of given file as configuration"
							"file\n"
							"arguments:\n"
							"  - file path with script)",
							[this](const std::vector<std::string_view> &args) {
								std::ifstream file(std::to_string(args[1]));
								parse_commands(file);
							});
}
} // namespace dutl
