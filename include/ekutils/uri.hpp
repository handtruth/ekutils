// Copyright (C) 2015 Ben Lewis <benjf5+github@gmail.com>
// Licensed under the MIT license.

#ifndef URI_HEAD_QPVKFKEQDSFSD
#define URI_HEAD_QPVKFKEQDSFSD

#include <cctype>
#include <map>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>

namespace ekutils {

class uri {
	/* URIs are broadly divided into two categories: hierarchical and
	 * non-hierarchical. Both hierarchical URIs and non-hierarchical URIs have a
	 * few elements in common; all URIs have a scheme of one or more alphanumeric
	 * characters followed by a colon, and they all may optionally have a query
	 * component preceded by a question mark, and a fragment component preceded by
	 * an octothorpe (hash mark: '#'). The query consists of stanzas separated by
	 * either ampersands ('&') or semicolons (';') (but only one or the other),
	 * and each stanza consists of a key and an optional value; if the value
	 * exists, the key and value must be divided by an equals sign.
	 *
	 * The following is an example from Wikipedia of a hierarchical URI:
	 * scheme:[//[user:password@]domain[:port]][/]path[?query][#fragment]
	 */

public:
	enum class scheme_category {
		Hierarchical,
		NonHierarchical
	};

	enum class component {
		Scheme,
		Content,
		Username,
		Password,
		Host,
		Port,
		Path,
		Query,
		Fragment
	};

	enum class query_argument_separator {
		ampersand,
		semicolon
	};

	uri(
		char const *uri_text, scheme_category category = scheme_category::Hierarchical,
		query_argument_separator separator = query_argument_separator::ampersand
	) : m_category(category), m_path_is_rooted(false), m_port(-1), m_separator(separator) {
		setup(std::string(uri_text));
	}

	uri(
		std::string const &uri_text, scheme_category category = scheme_category::Hierarchical,
		query_argument_separator separator = query_argument_separator::ampersand
	) : m_category(category), m_path_is_rooted(false), m_port(-1), m_separator(separator) {
		setup(uri_text);
	}

	uri(
		std::map<component, std::string> const &components,
		scheme_category category,
		bool rooted_path,
		query_argument_separator separator = query_argument_separator::ampersand
	);

	uri(
		uri const &other,
		std::map<component, std::string> const &replacements
	);

	// Copy constructor; just use the copy assignment operator internally.
	uri(uri const &other) {
		*this = other;
	}

	// Copy assignment operator
	uri &operator=(uri const &other) {
		if (this != &other) {
			m_scheme = other.m_scheme;
			m_content = other.m_content;
			m_username = other.m_username;
			m_password = other.m_password;
			m_host = other.m_host;
			m_path = other.m_path;
			m_query = other.m_query;
			m_fragment = other.m_fragment;
			m_query_dict = other.m_query_dict;
			m_category = other.m_category;
			m_port = other.m_port;
			m_path_is_rooted = other.m_path_is_rooted;
			m_separator = other.m_separator;
		}
		return *this;
	}

	~uri() {}

	std::string const &get_scheme() const {
		return m_scheme;
	}

	scheme_category get_scheme_category() const {
		return m_category;
	}

	std::string const &get_content() const {
		if (m_category != scheme_category::NonHierarchical) {
			throw std::domain_error("The content component is only valid for non-hierarchical URIs.");
		}
		return m_content;
	}

	std::string const &get_username() const {
		if (m_category != scheme_category::Hierarchical) {
			throw std::domain_error("The username component is only valid for hierarchical URIs.");
		}
		return m_username;
	}

	std::string const &get_password() const {
		if (m_category != scheme_category::Hierarchical) {
			throw std::domain_error("The password component is only valid for hierarchical URIs.");
		}
		return m_password;
	}

	std::string const &get_host() const {
		if (m_category != scheme_category::Hierarchical) {
			throw std::domain_error("The host component is only valid for hierarchical URIs.");
		}
		return m_host;
	}

	long get_port() const {
		if (m_category != scheme_category::Hierarchical) {
			throw std::domain_error("The port component is only valid for hierarchical URIs.");
		}
		return m_port;
	}

	std::string const &get_path() const {
		if (m_category != scheme_category::Hierarchical) {
			throw std::domain_error("The path component is only valid for hierarchical URIs.");
		}
		return m_path;
	}

	std::string const &get_query() const {
		return m_query;
	}

	std::multimap<std::string, std::string> const &get_query_dictionary() const {
		return m_query_dict;
	}

	std::string const &get_fragment() const {
		return m_fragment;
	}

	std::string to_string() const;

private:
	void setup(std::string_view const &uri_text);

	std::string_view::const_iterator parse_scheme(std::string_view const &uri_text, std::string_view::const_iterator scheme_start);
	std::string_view::const_iterator parse_content(std::string_view const &uri_text, std::string_view::const_iterator content_start);
	std::string::const_iterator parse_username(std::string_view const &uri_text, std::string const &, std::string::const_iterator username_start);
	std::string::const_iterator parse_password(std::string_view const &, std::string const &, std::string::const_iterator password_start);
	std::string::const_iterator parse_host(std::string_view const &uri_text, std::string const &content, std::string::const_iterator host_start);
	std::string::const_iterator parse_port(std::string_view const &uri_text, std::string const &content, std::string::const_iterator port_start);
	std::string_view::const_iterator parse_query(std::string_view const &uri_text, std::string_view::const_iterator query_start);
	std::string_view::const_iterator parse_fragment(std::string_view const &uri_text, std::string_view::const_iterator fragment_start);

	void init_query_dictionary();

	scheme_category m_category;
	bool m_path_is_rooted;
	long m_port;
	query_argument_separator m_separator;

	std::string m_scheme;
	std::string m_content;
	std::string m_username;
	std::string m_password;
	std::string m_host;
	std::string m_path;
	std::string m_query;
	std::string m_fragment;

	std::multimap<std::string, std::string> m_query_dict;
};

} // namespace ekutils

#endif // URI_HEAD_QPVKFKEQDSFSD
