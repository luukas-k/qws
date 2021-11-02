#include "qws.h"


template<typename T>
std::string enumerate(std::vector<T> s, std::function<std::string(const T& s)> fn) {
	std::string res;
	for (auto& a : s) {
		res += fn(a);
	}
	return res;
}

struct Project {
	std::string name;
	std::string description;

	std::string link_text;
	std::string link;
};

std::vector<Project> get_projects() {
	std::vector<Project> projects {
		Project{
			"qws", 
			"qws is a simple web server. You can create simple and quick web sites. This website was created with it.",
			"View on GitHub", 
			"https://github.com/PotatoNoodle/qws"
		},
		
	};
	for (int i = 0; i < 16; i++) {
		Project p{
			"testProject " + std::to_string(i), 
			"lorem ipsum dolor sit amet.", 
			"View on google", 
			"https://www.google.com"
		};
		projects.push_back(p);
	}
	return projects;
}

struct NavTarget {
	std::string name;
	std::string url;
};

std::vector<NavTarget> get_pages() {
	return {
		NavTarget{"Home", "/"},
		NavTarget{"Who am i?", "/who"},
		NavTarget{"What have i done?", "/projects"}
	};
}

std::string styling() {
	/*
	Palette
	Gold Crayoula		- 0xEDCB96
	Apricot				- 0xF7C4A5
	Mountbatten Pink	- 0x9E7682
	Independence		- 0x605770
	Dark Independece	- 0x4D4861
	*/
	return qws::style(
		"html { font-family: Arial, Helvetica, sans-serif; }\n"
		".test { color: red }\n"
		// Gold Crayoula
		".project { \n"
		"   background-color: rgb(77,72,97);\n"
		"   padding: 10px;\n"
		"   margin: 10px;\n"
		"   display: inline-block;\n"
		"   width: 350px;\n"
		"}\n"
		".project_container { \n"
		"   display: flex-wrap;\n"
		"   justify-content: center;\n"
		"}\n"
		// Apricot
		"html { background-color: rgb(96,87,112); }"
		"a { color: rgb(247,196,165); }"
		"h2, p { color: rgb(237,203,150); }"
	);
}

std::string navigation() {
	return qws::div(
		{},
		enumerate<NavTarget>(get_pages(), [](const NavTarget& n){
		return qws::div(
			{},
			qws::link(n.url, n.name)
		);
	}));
}

int main() {
	qws::Service s;

	s.route_404([](qws::Request& req){
		return "404";
	});
	// Simple base route
	s.route("/", [](qws::Request& req) {
		return qws::html(
			qws::head(
				qws::title("Portfolio"),
				styling()
			),
			qws::body(
				navigation(),
				qws::h1("Main page!"),
				qws::link("/bio", qws::p("Bio")),
				qws::link("/contact", qws::p("Contact")),
				qws::link("/projects", qws::p("Projects"))
			)
		);
	});

	// Route with one parameter
	s.route("/who", [](qws::Request& req) {
		return qws::html(
			qws::head(
				qws::title("Who am I"),
				styling()
			),
			qws::body(
				navigation(),
				qws::div(
					{.cls = "test"},
					qws::h1("Who am i?"),
					qws::p("Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet.")
				),
				qws::div(
					{},
					qws::h1("Where am i?"),
					qws::p("Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet.")
				)
			)
		);
	});

	// Route with one parameter
	s.route("/projects", [](qws::Request& req) {
		return qws::html(
			qws::head(
				qws::title("Projects"),
				styling()
			),
			qws::body(
				navigation(),
				qws::h1("Projects made by be"),
				qws::div(
					{.cls = "project_container"},
					enumerate<Project>(get_projects(), [](const Project& p){ 
						return qws::div(
							{.cls = "project"},
							qws::h2(p.name),
							qws::p(p.description),
							qws::link(p.link, qws::p(p.link_text))
						); 
					})
				)
			)
		);
	});
	
	return s.run();
}
