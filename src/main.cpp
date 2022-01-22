#include "qws.h"

int main() {
	qws::Service service;

	// A simple index page with the text 'Hello world!'
	service.route("/", [](qws::Request& request) {
		return "Hello world!";
	});

	/*
	A simple 404 page. If page was not found route them to this page
	*/
	service.route_404([](qws::Request& request) {
		return "Page not found. (404)";
	});

	/*
	A more complex page using the html builder.
	*/
	service.route("/test", [](qws::Request& request) {
		return qws::html(
			qws::head(
				qws::title("Title")
			),
			qws::body(
				qws::h1("Test page")
			)
		);
	});

	service.run();
}