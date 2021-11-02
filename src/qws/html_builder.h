#pragma once

#include <string>

namespace qws {
	template<typename First>
	std::string concat(First first) {
		return first;
	}

	template<typename First, typename...Rest>
	std::string concat(First first, Rest...rest) {
		return first + concat<Rest...>(rest...);
	}

	std::string tag_pair(std::string tag, std::string content) {
		return "<" + tag + ">" + content + "</" + tag + ">";
	}

	template<typename...T>
	std::string html(T...content) {
		return "<!DOCTYPE html>" + tag_pair("html", concat<T...>(content...));
	}
	
	template<typename...T>
	std::string head(T...content) {
		return tag_pair("head", concat<T...>(content...));
	}

	template<typename...T>
	std::string style(T...content) {
		return tag_pair("style", concat<T...>(content...));
	}
	
	template<typename...T>
	std::string body(T...content) {
		return tag_pair("body", concat<T...>(content...));
	}	
	
	template<typename...T>
	std::string b(T...content) {
		return tag_pair("b", concat<T...>(content...));
	}

	template<typename...T>
	std::string h1(T...content) {
		return tag_pair("h1", concat<T...>(content...));
	}

	template<typename...T>
	std::string h2(T...content) {
		return tag_pair("h2", concat<T...>(content...));
	}

	template<typename...T>
	std::string p(T...content) {
		return tag_pair("p", concat<T...>(content...));
	}

	std::string br() {
		return "<br>";
	}
	
	template<typename...T>
	std::string title(T...content) {
		return tag_pair("title", concat<T...>(content...));
	}

	struct Attributes {
		std::optional<std::string> cls;
	};

	template<typename...T>
	std::string div(Attributes attr, T...content) {
		return std::string("<div") + (attr.cls.has_value() ? (" class=\"" + attr.cls.value() + "\"") : "") + ">" + concat<T...>(content...) + "</div>";
	}

	template<typename...T>
	std::string link(std::string href, T...content) {
		return 
			"<a href=" + href + ">" +
			concat<T...>(content...) + 
			"</a>";
	}

}