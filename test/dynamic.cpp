#include <iostream>
#include "ekutils/dynamic.hpp"

#include "test.hpp"

test {
    using namespace handtruth::ekutils;

    dynamic obj;

    assert_true(obj.empty());

    obj.put("hello", 23);
    assert_equals(1u, obj.size());

    obj.put("there", std::string("lol kek"));
    assert_equals(2u, obj.size());

    assert_equals(23, obj.get<int>("hello"));
    assert_equals("lol kek", obj.take<std::string>("there"));
    assert_fails_with(std::bad_cast, {
        obj.get<char>("hello");
    });

    assert_true(obj.contains("hello"));
    assert_false(obj.contains("there"));
    assert_false(obj.contains("nothing"));

    obj.put("popka", std::string("zopka"));
    obj.put("lol", 23u);
    obj.put("kek", 'c');
    obj.put<const char *>("consta", "kotlinbergh");

    for (auto entry : obj) {
        std::cout << entry.key << " : " << entry.value.type.name() << std::endl;
    }

    const dynamic & const_obj = obj;

    for (auto entry : const_obj) {
        std::cout << entry.key << " : " << entry.value.type.name() << std::endl;
    }

    dynamic other = std::move(obj);
}
