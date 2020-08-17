#ifndef DYNAMIC_HEAD_GCRGBVUZDQXWEVP
#define DYNAMIC_HEAD_GCRGBVUZDQXWEVP

#include <typeinfo>
#include <stdexcept>
#include <map>
#include <string>
#include <memory>
#include <iterator>

namespace ekutils {

class dynamic final {
public:
	struct field {
		const std::type_info & type;
		virtual ~field() {}
	private:
		field(const std::type_info & t) : type(t) {}
	protected:
		virtual void * ptr() = 0;
		virtual const void * ptr() const noexcept = 0;
		template <typename T>
		T & get() {
			if (typeid(T) != type)
				throw std::bad_cast();
			return *reinterpret_cast<T *>(ptr());
		}
		template <typename T>
		const T & get() const {
			if (typeid(T) != type)
				throw std::bad_cast();
			return *reinterpret_cast<const T *>(ptr());
		}

		friend class dynamic;
	};

private:
	template <typename T>
	class field_t final : public field {
		T value;
	public:
		field_t(const T & v) : field(typeid(T)), value(v) {}
		field_t(T && v) : field(typeid(T)), value(std::move(v)) {}
	protected:
		virtual void * ptr() noexcept override {
			return &value;
		}
		virtual const void * ptr() const noexcept override {
			return &value;
		}
	};

	typedef std::map<std::string, std::unique_ptr<field>> fields_map;

	fields_map fields;

public:
	dynamic() = default;
	dynamic(dynamic && other) = default;
	dynamic(const dynamic & other) = delete;

	template <typename T>
	T & put(std::string && name, T && value) {
		auto ptr = std::make_unique<field_t<T>>(std::move(value));
		auto result = fields.insert_or_assign(std::move(name), std::move(ptr));
		return result.first->second->template get<T>();
	}

	template <typename T>
	T & put(std::string && name, const T & value) {
		auto ptr = std::make_unique<field_t<T>>(value);
		auto result = fields.insert_or_assign(std::move(name), std::move(ptr));
		return result.first->second->template get<T>();
	}

	template <typename T>
	T & put(const std::string & name, T && value) {
		auto ptr = std::make_unique<field_t<T>>(std::move(value));
		auto result = fields.insert_or_assign(name, std::move(ptr));
		return result.first->second->template get<T>();
	}

	template <typename T>
	T & put(const std::string & name, const T & value) {
		auto ptr = std::make_unique<field_t<T>>(value);
		auto result = fields.insert_or_assign(name, std::move(ptr));
		return result.first->second->template get<T>();
	}

	bool erase(const std::string & name) {
		return fields.erase(name);
	}

	field & peek(const std::string & name) {
		return *fields.at(name);
	}

	const field & peek(const std::string & name) const {
		return *fields.at(name);
	}

	template <typename T>
	T & get(const std::string & name) {
		return peek(name).get<T>();
	}

	template <typename T>
	const T & get(const std::string & name) const {
		return peek(name).get<T>();
	}

	template <typename T>
	T take(const std::string & name) {
		auto result = fields.extract(name);
		if (result.empty())
			throw std::out_of_range("not found");
		return std::move(result.mapped()->get<T>());
	}

	const std::type_info & type(const std::string & name) const {
		return peek(name).type;
	}

	bool contains(const std::string & name) const noexcept {
		return fields.find(name) != fields.end();
	}

	bool empty() const noexcept {
		return fields.empty();
	}

	typedef fields_map::size_type size_type;

	size_type size() const noexcept {
		return fields.size();
	}

	size_type max_size() const noexcept {
		return fields.max_size();
	}

	struct entry {
		const std::string & key;
		field & value;
	};

	template <typename type, typename parent_type>
	class iterator_template {
		parent_type parent;
		iterator_template(parent_type && p) : parent(p) {}
	public:
		typedef type value_type;
		iterator_template(const iterator_template & other) : parent(other.parent) {}
		iterator_template & operator=(const iterator_template & other) {
			parent = other.parent;
			return *this;
		}
		iterator_template & operator++() {
			++parent;
			return *this;
		}
		iterator_template & operator--() {
			--parent;
			return *this;
		}
		value_type operator*() const {
			auto & pair = *parent;
			return entry { pair.first, *pair.second };
		}
		//friend void swap(iterator& lhs, iterator& rhs);
		template <typename T, typename I>
		friend bool operator==(const iterator_template & a, const iterator_template<T, I> & b) {
			return a.parent == b.parent;
		}
		template <typename T, typename I>
		friend bool operator!=(const iterator_template & a, const iterator_template<T, I> & b) {
			return a.parent != b.parent;
		}
		friend dynamic;
	};

	typedef iterator_template<entry, fields_map::iterator> iterator;
	typedef iterator_template<const entry, fields_map::const_iterator> const_iterator;
	typedef iterator_template<entry, fields_map::reverse_iterator> reverse_iterator;
	typedef iterator_template<const entry, fields_map::const_reverse_iterator> const_reverse_iterator;

	const_iterator cbegin() const {
		return const_iterator(fields.cbegin());
	}
	const_iterator cend() const {
		return const_iterator(fields.cend());
	}
	iterator begin() {
		return iterator(fields.begin());
	}
	iterator end() {
		return iterator(fields.end());
	}
	const_iterator begin() const {
		return cbegin();
	}
	const_iterator end() const {
		return cend();
	}
	const_reverse_iterator crbegin() const {
		return const_reverse_iterator(fields.crbegin());
	}
	const_reverse_iterator crend() const {
		return const_reverse_iterator(fields.crend());
	}
	reverse_iterator rbegin() {
		return reverse_iterator(fields.rbegin());
	}
	reverse_iterator rend() {
		return reverse_iterator(fields.rend());
	}
	const_reverse_iterator rbegin() const {
		return crbegin();
	}
	const_reverse_iterator rend() const {
		return crend();
	}
};

} // namespace ekutils

namespace std {



string to_string(const ekutils::dynamic & obj);

} // namespace std

#endif // DYNAMIC_HEAD_GCRGBVUZDQXWEVP
