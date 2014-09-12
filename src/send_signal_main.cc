
#include <string>
#include <iostream>
#include <map>

#include <om/ipc/dbus/connection.h>
#include <om/ipc/dbus/message.h>
#include <om/async/epoll_wrapper.h>

#include <unistd.h>

namespace dbus_tools {
	namespace send_signal {

		om::async::EPollWrapper epoll_wrapper;
		om::ipc::dbus::Connection dbus_connection;

		struct send_signal_options_t {
			std::string bus;
			std::string iface;
			std::string member;
			std::string obj_path;
			std::multimap<std::string, std::string> args;
		} options;

		void connect();
		void connected(om::ipc::dbus::Connection* c);
		void send_signal(om::ipc::dbus::Connection* c);

		DBusHandlerResult default_handler(om::ipc::dbus::Connection* c, DBusMessage* m);

		void parse_options(int argc, char** argv);
		void print_usage();		
	}
}

int main(int argc, char** argv)
{
	dbus_tools::send_signal::parse_options(argc, argv);
	dbus_tools::send_signal::connect();

	return 0;
}

void dbus_tools::send_signal::connect()
{
	
	using namespace std::placeholders;

	dbus_connection.set_default_signal_handler(
		std::bind(dbus_tools::send_signal::default_handler, _1, _2)
	);

	dbus_connection.set_default_method_call_handler(
		std::bind(dbus_tools::send_signal::default_handler, _1, _2)
	);

	dbus_connection.set_default_method_return_handler(
		std::bind(dbus_tools::send_signal::default_handler, _1, _2)
	);

	dbus_connection.set_default_error_handler(
		std::bind(dbus_tools::send_signal::default_handler, _1, _2)
	);
		
	try {

		dbus_connection.open(
			options.bus, "de.editum.dbus_tools.SendSignal",
			std::bind(dbus_tools::send_signal::connected, _1)
		);

	} catch(std::runtime_error& e) {
		std::cerr << "error occured" << std::endl;
	}

}

void dbus_tools::send_signal::connected(om::ipc::dbus::Connection* c)
{
	send_signal(c);
}

void dbus_tools::send_signal::send_signal(om::ipc::dbus::Connection* c)
{
	try {

		om::ipc::dbus::Message m(DBUS_MESSAGE_TYPE_SIGNAL);

		m.set_interface(options.iface);
		m.set_member(options.member);
		m.set_path(options.obj_path);

		c->send(m);

	} catch(std::runtime_error& e) {
		std::cerr << "error occured" << std::endl;
	}
}

DBusHandlerResult dbus_tools::send_signal::default_handler(om::ipc::dbus::Connection* c, DBusMessage* m)
{
	return DBUS_HANDLER_RESULT_HANDLED ;
}

void dbus_tools::send_signal::parse_options(int argc, char** argv)
{
	int opt = 0;
	opterr = 0;

	bool b_present = false, i_present = false, m_present = false;

	dbus_tools::send_signal::options.obj_path = "/";

	while((opt = getopt(argc, argv, "b:i:m:o:")) != -1) {
		switch(opt) {
			case 'b':
				options.bus = std::string(optarg);
				b_present = true;
				break;
			case 'i':
				options.iface = std::string(optarg);
				i_present = true;
				break;
			case 'm':
				options.member = std::string(optarg);
				m_present = true;
				break;
			case 'o':
				options.obj_path = std::string(optarg);
				break;
			default:
				print_usage(), exit(1);
		}
	}

	if(argc > optind) {

		for(int i = 0; i < argc - optind; i++) {

			std::string arg = argv[optind + i], type = "", val = "";
			int sep = -1;

			if((sep = arg.find(":")) != -1) {

				type = arg.substr(0, sep);
				val = arg.substr(sep+1, arg.length()-1);

				if(type != "str" && type != "int")
					print_usage(), exit(1);

			} else
				print_usage(), exit(1);

			options.args.emplace(type, val);
		}
	}

	if(!b_present || !i_present || !m_present)
		print_usage(), exit(1);
}

void dbus_tools::send_signal::print_usage() {
	std::cout << "Usage: send-signal -b bus -i interface -m member [-o obj_path]"
		<< " [type:val]*"
		<< std::endl;
}
