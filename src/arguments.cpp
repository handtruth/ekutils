#include "ekutils/arguments.hpp"

#include <cstring>

namespace ekutils {

void arguments::integer::parse_value(const std::string_view & str) {
	if (str.empty())
		throw invalid_argument_value("integer argument should not be empty");
	auto iter = str.begin();
	bool minus = false;
	switch (*iter) {
		case '-':
			minus = true;
			[[fallthrough]];
		case '+':
			++iter;
			break;
		default:
			break;
	}
	value = 0;
	for (auto end = str.end(); iter != end; ++iter) {
		char digit = *iter;
		if (digit < '0' || digit > '9')
			throw invalid_argument_value("not an integer");
		value = value*10 + (digit - '0');
	}
	if (value > max)
		throw invalid_argument_value("value exceeded allowed maximum (" + std::to_string(max) + ")");
	if (value < min)
		throw invalid_argument_value("value exceeded allowed minimum (" + std::to_string(min) + ")");
	if (minus)
		value = -value;
}

std::string arguments::integer::build_help() const {
	std::string result;
	if (hint.empty() && min == possible_min && max == possible_max)
		result = {};
	else if (hint.empty() && min == possible_min)
		result = "should be lesser than " + std::to_string(max);
	else if (hint.empty() && max == possible_max)
		result = "should be bigger than " + std::to_string(min);
	else if (hint.empty())
		result = "should be in range [" + std::to_string(min) + ':' + std::to_string(max) + ']';
	else if (min == possible_min)
		result = hint + " (<= " + std::to_string(max) + ')';
	else if (max == possible_max)
		result = hint + " (>= " + std::to_string(min) + ')';
	else
		result = hint + " [" + std::to_string(min) + ':' + std::to_string(max) + ']';
	if (value != 0) {
		if (result.empty()) {
			return "default: " + std::to_string(value);
		} else {
			result += " (default: ";
			result += std::to_string(value);
			result += ')';
		}
	}
	return result;
}

void arguments::string::parse_value(const std::string_view & str) {
	value = str;
}

std::string arguments::string::build_help() const {
	if (value.empty()) {
		return hint;
	} else {
		if (hint.empty())
			return "default: " + value;
		else
			return hint + " (default: " + value + ')';
	}
}

void arguments::multistring::parse_value(const std::string_view & str) {
	value.emplace_back(str);
}

std::string arguments::multistring::build_help() const {
	if (hint.empty())
		return "multivalue";
	return hint + " (multivalue)";
}

void arguments::fractional::parse_value(const std::string_view & str) {
	value = std::stod(std::string(str));
}

[[noreturn]] void forbid_positional() {
	throw arguments_parse_error("positional arguments not expected");
}

void arguments::parse(int argc, char * argv[]) {
	// prepare options
	std::unordered_map<std::string_view, std::reference_wrapper<argument>> options;
	std::unordered_map<char, std::reference_wrapper<flag>> flags;
	for (std::unique_ptr<argument> & option : all_args) {
		options.emplace(option->name, *option);
		try {
			flag & f = dynamic_cast<flag &>(*option);
			if (f.c)
				flags.emplace(f.c, f);
		} catch (const std::bad_cast &) {
			// do nothing
		}
	}

	char **iter = argv + 1;
	char **end = argv + argc;
	for (; iter != end; ++iter) {
		const char * word = *iter;
		if (!std::strncmp(word, "--", 2)) {
			if (word[2] == '\0') {
				// positional args
				if (!allow_positional)
					forbid_positional();
				++iter;
				break;
			}
			// named argument
			std::string_view name = word + 2;
			bool value_at_next = true;
			for (std::size_t name_size = 0; name_size < name.size(); ++name_size) {
				if (name[name_size] == '=') {
					name.remove_suffix(name.size() - name_size);
					value_at_next = false;
					break;
				}
			}
			// find parameter
			auto found = options.find(name);
			if (found == options.end()) {
				std::string err = "unknown option \"";
				err += name;
				err += '"';
				throw arguments_parse_error(err);
			}
			argument & parameter = found->second;
			try {
				auto & option = dynamic_cast<flag &>(parameter);
				option.value = true;
			} catch (const std::bad_cast &) {
				auto & option = dynamic_cast<value_argument &>(parameter);
				std::string_view value;
				if (value_at_next) {
					// value at the next command line argument
					++iter;
					value = *iter;
				} else {
					// value is there after '='
					value = word + name.size() + 3;
				}
				try {
					option.parse_value(value);
				} catch (const invalid_argument_value & e) {
					std::string err("option '");
					err += name;
					err += "': ";
					err += e.what();
					throw arguments_parse_error(err);
				}
			}
		} else if (word[0] == '-') {
			// short flags
			for (const char * ptr = word + 1; *ptr; ++ptr) {
				char c = *ptr;
				auto found = flags.find(c);
				if (found == flags.end())
					throw arguments_parse_error(std::string("unknown flag '") + c + '\'');
				flag & f = found->second;
				f.value = true;
			}
		} else {
			// positional argument
			if (!allow_positional)
				forbid_positional();
			positional.emplace_back(word);
		}
	}
	for (; iter != end; ++iter) {
		// only positional
		if (!allow_positional)
			forbid_positional();
		positional.emplace_back(*iter);
	}
}

std::string arguments::build_help(const std::string_view & program) const {
	std::string result = "Usage: ";
	result += program;
	if (!all_args.empty())
		result += " [options]";
	if (allow_positional) {
		if (positional_hint.empty())
			result += " parameters...";
		else
			result += ' ' + positional_hint;
	}
	if (!all_args.empty()) {
		result += "\n\nOptions:\n";
		std::forward_list<std::string> options_agenda;
		auto options_agenda_iter = options_agenda.before_begin();
		std::size_t max_size = 0;
		for (const std::unique_ptr<argument> & option_ptr : all_args) {
			const argument & option = *option_ptr;
			std::string agenda = "  ";
			char c;
			const char * value_type;
			try {
				auto & f = dynamic_cast<const flag &>(option);
				c = f.c;
				value_type = nullptr;
			} catch (const std::bad_cast &) {
				c = '\0';
				auto & arg = dynamic_cast<const value_argument &>(option);
				value_type = arg.value_type_str();
			}
			agenda.reserve(8);
			if (c) {
				agenda += '-';
				agenda += c;
				agenda += ", --";
			} else {
				agenda += "    --";
			}
			agenda += option.name;
			if (value_type) {
				agenda += ' ';
				agenda += value_type;
			}
			if (agenda.size() > max_size)
				max_size = agenda.size();
			options_agenda_iter = options_agenda.emplace_after(options_agenda_iter, std::move(agenda));
		}
		auto iter_agenda = options_agenda.begin();
		auto agenda_end = options_agenda.end();
		auto iter_option = all_args.begin();
		max_size += 2;
		for (; iter_agenda != agenda_end; ++iter_agenda, ++iter_option) {
			const std::string & agenda = *iter_agenda;
			const argument & option = *(*iter_option);
			result += agenda;
			std::size_t space_size = max_size - agenda.size();
			std::size_t offset = 0;
			auto helpstr = option.build_help();
			bool done;
			do {
				std::size_t part_size;
				if (width >= helpstr.size() - offset) {
					part_size = helpstr.size() - offset;
					done = true;
				} else {
					part_size = width - 1;
					while (!std::isspace(helpstr[offset + part_size])) {
						part_size--;
						if (part_size == 0) {
							part_size = width - 1;
							break;
						}
					}
					++part_size;
					done = false;
				}
				result.reserve(result.size() + max_size);
				for (std::size_t i = 0; i < space_size; ++i)
					result += ' ';
				result.append(helpstr, offset, part_size);
				space_size = max_size;
				offset += part_size;
				result += '\n';
			} while (!done);
		}
	}
	return result;
}

template <typename T>
bool copy_as(argument & receiver, const argument & source) {
	try {
		T & a = dynamic_cast<T &>(receiver);
		const T & b = dynamic_cast<const T &>(source);
		a.value = b.value;
		return true;
	} catch (const std::bad_cast &) {
		return false;
	}
}

void arguments::fill_from(const arguments & other) {
	auto a = all_args.begin();
	auto a_end = all_args.end();
	auto b = other.all_args.begin();
	auto b_end = other.all_args.end();
	for (; a != a_end && b != b_end; ++a, ++b) {
		argument & receiver = *(*a);
		const argument & source = *(*b);
		copy_as<flag>(receiver, source) || copy_as<integer>(receiver, source) || copy_as<string>(receiver, source)
		|| copy_as<multistring>(receiver, source) || copy_as<fractional>(receiver, source);//|| copy_as<variant>(receiver, source);
	}
}

} // namespace ekutils
