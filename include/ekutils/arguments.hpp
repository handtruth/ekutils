#ifndef ARGUMENTS_HEAD_PQOOVKKDKGM
#define ARGUMENTS_HEAD_PQOOVKKDKGM

#include <string>
#include <string_view>
#include <forward_list>
#include <vector>
#include <limits>
#include <cinttypes>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace ekutils {

struct argument {
	std::string name;
	std::string hint;
	virtual std::string build_help() const {
		return hint;
	}
	virtual ~argument() {}
};

struct value_argument : public argument {
	virtual void parse_value(const std::string_view & str) = 0;
	virtual const char * value_type_str() const noexcept = 0;
};

struct invalid_argument_value : public std::runtime_error {
	invalid_argument_value(const std::string & message) : std::runtime_error(message) {}
};

struct arguments_parse_error : public std::runtime_error {
	arguments_parse_error(const std::string & message) : std::runtime_error(message) {}
};

struct arguments {
	struct flag : public argument {
		typedef bool value_type;
		value_type value = false;
		char c = '\0';
	};

	struct integer : public value_argument {
		typedef std::intmax_t value_type;
		static constexpr value_type possible_max = std::numeric_limits<value_type>::max();
		static constexpr value_type possible_min = std::numeric_limits<value_type>::min();
		value_type value = 0;
		value_type max = possible_max;
		value_type min = possible_min;
		virtual void parse_value(const std::string_view & str) override;
		virtual const char * value_type_str() const noexcept override {
			return "INTEGER";
		}
		virtual std::string build_help() const override;
	};

	struct string : public value_argument {
		typedef std::string value_type;
		value_type value;
		virtual void parse_value(const std::string_view & str) override;
		virtual const char * value_type_str() const noexcept override {
			return "STRING";
		}
		virtual std::string build_help() const override;
	};

	struct multistring : public value_argument {
		typedef std::vector<std::string> value_type;
		value_type value;
		virtual void parse_value(const std::string_view & str) override;
		virtual const char * value_type_str() const noexcept override {
			return "STRING";
		}
		virtual std::string build_help() const override;
	};

	struct fractional : public value_argument {
		typedef double value_type;
		value_type value;
		virtual void parse_value(const std::string_view & str) override;
		virtual const char * value_type_str() const noexcept override {
			return "NUMBER";
		}
	};

	template <typename E>
	struct variant : public value_argument {
		typedef E value_type;
		value_type value;
		std::vector<std::pair<std::string, value_type>> variants;
	
	private:
		void append_variants(std::string & result) const {
			if (variants.size() == 0) {
				result += '-';
			} else {
				auto iter = variants.begin();
				auto end = variants.end();
				result += iter->first;
				iter++;
				for (; iter != end; ++iter) {
					result += '|';
					result += iter->first;
				}
			}
		}

	public:
		virtual void parse_value(const std::string_view & str) override {
			auto iter = std::find_if(variants.begin(), variants.end(), [&str](const std::pair<std::string, value_type> & it) {
				return it.first == str;
			});
			if (iter == variants.end()) {
				if (variants.size() == 0)
					throw invalid_argument_value("malformed option");
				std::string err("unknown variant '");
				err += str;
				err += "' (expected: ";
				append_variants(err);
				err += ")";
				throw invalid_argument_value(err);
			}
			value = iter->second;
		}
		virtual const char * value_type_str() const noexcept override {
			return "VARIANT";
		}
		virtual std::string build_help() const override {
			std::string result;
			if (hint.empty()) {
				result = "variants: ";
				append_variants(result);
			} else {
				result = hint + " (variants: ";
				append_variants(result);
				result += ')';
			}
			for (const auto & pair : variants) {
				if (pair.second == value) {
					result += " (default: ";
					result += pair.first;
					result += ')';
					break;
				}
			}
			return result;
		}
	};

	void parse(int argc, char * argv[]);
	std::string build_help(const std::string_view & program) const;

	std::vector<std::string> positional;

private:
	typedef std::forward_list<std::unique_ptr<argument>> arg_list_type;

	arg_list_type all_args;
	arg_list_type::iterator arg_last = all_args.before_begin();

protected:
	std::string positional_hint;

	bool allow_positional = true;

	std::size_t width = 40;

	template <typename T, typename F>
	typename T::value_type & add(const std::string_view & name, F block) {
		std::unique_ptr ptr = std::make_unique<T>();
		T & ref = *ptr;
		ref.name = name;
		block(ref);
		arg_last = all_args.emplace_after(arg_last, std::move(ptr));
		return ref.value;
	}

	template <typename T>
	typename T::value_type & add(const std::string_view & name) {
		std::unique_ptr ptr = std::make_unique<T>();
		T & ref = *ptr;
		ref.name = name;
		arg_last = all_args.emplace_after(arg_last, std::move(ptr));
		return ref.value;
	}

private:
	void fill_from(const arguments & other);

public:
	template <typename T>
	friend void operator<<(T & receiver, const T & other) {
		static_assert(std::is_base_of_v<arguments, T>);
		receiver.fill_from(other);
	}
};



} // namespace ekutils

#endif // ARGUMENTS_HEAD_PQOOVKKDKGM
