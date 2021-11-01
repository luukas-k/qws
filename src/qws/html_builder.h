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

	template<typename...T>
	std::string html(T...content) {
		return 
			"<!DOCTYPE html>" 
			"<html>" + 
			concat<T...>(content...) +
			"</html>";
	}

	
	template<typename...T>
	std::string head(T...content) {
		return 
			"<head>" + 
			concat<T...>(content...) +
			"</head>";
	}
	
	template<typename...T>
	std::string body(T...content) {
		return 
			"<body>" + 
			concat<T...>(content...) +
			"</body>";
	}	
	
	template<typename...T>
	std::string b(T...content) {
		return 
			"<b>" + 
			concat<T...>(content...) +
			"</b>";
	}	
	
	template<typename...T>
	std::string title(T...content) {
		return 
			"<title>" + 
			concat<T...>(content...) +
			"</title>";
	}	
}