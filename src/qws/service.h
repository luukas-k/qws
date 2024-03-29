#pragma once

#include "pre.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8080"

namespace qws {

	class Socket {
	public:
		Socket()
			:
			m_Socket(0)
		{
			WSADATA wsaData{};
			WSAStartup(MAKEWORD(2,2), &wsaData);

			m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		Socket(Socket&& rhs) 
			:
			m_Socket(rhs.m_Socket)
		{
			rhs.m_Socket = 0;
		}
		Socket(SOCKET s) {
			m_Socket = s;
		}
		// bind(8080);
		void bind(u32 port) {
			sockaddr_in service;
			
			service.sin_family = AF_INET;
			service.sin_addr.s_addr = INADDR_ANY;
			service.sin_port = htons(port);

			::bind(m_Socket, (sockaddr*)&service, sizeof(service));
		}
		// bind_at("127.0.0.1", 8080);
		void bind_at(std::string point, u32 port) {
			sockaddr_in service;
			
			service.sin_family = AF_INET;
			service.sin_addr.s_addr = inet_addr(point.c_str());
			service.sin_port = htons(port);

			::bind(m_Socket, (sockaddr*)&service, sizeof(service));
		}
		void listen() {
			::listen(m_Socket, SOMAXCONN);
		}
		Socket accept() {
			SOCKET clientSocket = ::accept(m_Socket, 0, 0);
			return Socket(clientSocket);
		}
		u32 recv(char* buf, u64 len) {
			return ::recv(m_Socket, buf, len, 0);
		}
		void send(std::string str) {
			::send(m_Socket, str.data(), str.size(), 0);
		}
		void close() {
			closesocket(m_Socket);
		}
	private:
		SOCKET m_Socket;
	};

	std::vector<std::string> split(std::string str, const std::string& delim) {
		std::vector<std::string> strs;
		while (true) {
			auto end = str.find(delim);
			std::string s = str.substr(0, end);
			strs.push_back(s);
			if (s == str) {
				break;
			}
			str = str.substr(end + delim.size());
		}
		return strs;
	}

	class Request {
	public:
		Request(std::string str)
			:
			m_Request(str)
		{
			auto req_split = split(str, "\r\n");
			auto reqline_split = split(req_split[0], " ");
			m_Path = reqline_split[1];
		}
		const std::string& path() const { return m_Path; }
	private:
		std::string m_Path;
		std::string m_Request;
	};

	struct Cookie {
		std::string name;
		std::string value;
	};

	class ResponseBuilder {
	public:
		ResponseBuilder()
			:
			m_Code(200),
			m_Content("")
		{}

		void set_statuscode(int val) {
			m_Code = val;
		}
		int get_statuscode() const {
			return m_Code;
		}
		void set_content(std::string str) {
			m_Content = str;
		}
		void add_cookie(const Cookie& c) {
			m_Cookies.push_back(c);
		}
		std::string build() {
			std::string res;
			auto add_version_line = [&](int code, std::string status) {
				res.append("HTTP/1.1 ");
				res.append(std::to_string(code));
				res.append(" " + status + "\r\n");
			};
			auto add_hdr = [&](std::string name, std::string value) {
				res.append(create_header(name, value) + "\n");
			};
			auto add_cntnt = [&](std::string cntnt) {
				res.append("\n");
				res.append(cntnt);
				res.append("\r\n\r\n");
			};
			if(m_Code == 200)
				add_version_line(200, "OK");
			else if(m_Code == 404)
				add_version_line(404, "Not found");
			else
				add_version_line(500, "Internal Server Error");

			add_hdr("Content-Length", std::to_string(m_Content.size()));
			for (auto& c : m_Cookies) {
				add_hdr("Set-Cookie", c.name + "=" + c.value);
			}
			add_hdr("Content-Type", "text/html");
			add_hdr("Connection", "Closed");
			add_cntnt(m_Content);
			return res;
		}
		std::string create_header(std::string name, std::string value) {
			return name + ": " + value;
		}
	private:
		int m_Code;
		std::string m_Content;
		std::vector<Cookie> m_Cookies;
	};

	class Service {
	public:
		void route_404(std::function<std::string(Request&)> handler) {
			m_Handler404 = handler;
		}
		void route(const std::string& pathSpec, std::function<std::string(Request&)> handler) {
			m_Handlers[pathSpec] = handler;
		}
		template<typename T>
		void route(const std::string& pathSpec, std::function<std::string(Request&, T)> handler) {
			m_Handlers[pathSpec] = [handler](Request& r) {
				auto p = r.path();
				auto pp = split(p, "/");
				int val = atoi(pp[2].c_str());
				return handler(r, val);
			};
		}
		int run() {
			Socket listenSocket;
			listenSocket.bind(8080);
			listenSocket.listen();
			while (true) {
				Socket clientSocket = listenSocket.accept();
				std::cout << "Client connected." << std::endl;
				launch_handler(std::move(clientSocket));
			}
		}
		void launch_handler(Socket&& s) {
			std::thread st([&, s = std::move(s)]() mutable {
				std::string request;
				auto has_eor = [&]() {
					return 
						(request.size() >= 4) &&
						(request[request.size() - 4] == '\r') && 
						(request[request.size() - 3] == '\n') &&
						(request[request.size() - 2] == '\r') &&
						(request[request.size() - 1] == '\n');
				};
				while (true) {
					char recvbuf[DEFAULT_BUFLEN]{0};
					u32 n = s.recv(recvbuf, DEFAULT_BUFLEN);
					request.append(std::string(recvbuf, n));
					if (has_eor()) {
						break;
					}
 				}
				std::cout << request << std::endl;

				Request r(request);


				auto strip_last = [](auto str) {
					auto a = split(str, "/");
					std::string res;
					for (int i = 0; i < a.size() - 1; i++) {
						if(a[i].size())	
							res += "/" + a[i];
					}
					return res;
				};

				auto parse_int = [](std::string str) -> std::optional<int> {
					try {
						int val = std::stoi(str);
						return val;
					}
					catch (...) {
						return {};
					}
				};

				auto matches = [&](const std::string& pathSpec, const std::string& path) -> bool {
					auto pathSpecSplit = split(pathSpec, "/");
					auto pathSplit = split(path, "/");

					if(pathSpecSplit.size() != pathSplit.size()) 
						return false;

					auto matches_type = [&](std::string type, std::string val) {
						if (type == "int") {
							if (parse_int(val).has_value()) {
								return true;
							}
						}
						return false;
					};
					auto match = [&](std::string spec, std::string pathComp) {
						if (spec[0] == ':') {
							return matches_type(spec.substr(1), pathComp);
						}
						return spec == pathComp;
					};

					for (u32 i = 0; i < pathSpecSplit.size(); i++) {
						auto specPart = pathSpecSplit[i];
						auto pathPart = pathSplit[i];
						if(!match(specPart, pathPart))
							return false;
					}
					return true;
				};
				
				ResponseBuilder rb;
				rb.set_statuscode(404);
				rb.add_cookie({.name = "testCookie", .value = "testValue"});
				for (auto& h : m_Handlers) {
					if (matches(h.first, r.path())) {
						rb.set_statuscode(200);
						rb.set_content(h.second(r));
						break;
					}
				}
				if (rb.get_statuscode() == 404 && m_Handler404.has_value()) {
					rb.set_content((*m_Handler404)(r));
				}
				s.send(rb.build());
				std::cout << "Client served." << std::endl;
				s.close();
			});
			
			st.detach();
		}
	private:
		std::unordered_map<std::string, std::function<std::string(Request&)>> m_Handlers;
		std::optional<std::function<std::string(Request&)>> m_Handler404;
	};

}