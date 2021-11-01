#include "qws.h"

int main() {
	qws::Service s;
	// Simple base route
	s.route("/", [](qws::Request& req) {
		return qws::html(
			qws::body(
				qws::b("main page!")
			)
		);
	});
	// Simple route
	s.route("/help", [](qws::Request& req) {
		return qws::html(
			qws::head(
				qws::title("Help")
			),
			qws::body(
				qws::b("help page!")
			)
		);
	});
	// Route with one parameter
	s.route<int>("/test/:help", [](qws::Request& req, int param) {
		return qws::html(
			qws::body(
				qws::b(std::to_string(param) + "main page!")
			)
		);
	});
	
	return s.run();
}
