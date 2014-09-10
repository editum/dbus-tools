
#include <string>
#include <iostream>
#include <map>

#include <unistd.h>

namespace dbus_tools {
	namespace send_signal {

		struct send_signal_options_t {
			std::string bus;
			std::string iface;
			std::string member;
			std::string obj_path;
			std::multimap<std::string, std::string> args;
		} options;

		void parse_options(int argc, char** argv);
		void print_usage();
	}
}

int main(int argc, char** argv)
{
	dbus_tools::send_signal::parse_options(argc, argv);

	return 0;
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
				dbus_tools::send_signal::options.bus = std::string(optarg);
				b_present = true;
				break;
			case 'i':
				dbus_tools::send_signal::options.iface = std::string(optarg);
				i_present = true;
				break;
			case 'm':
				dbus_tools::send_signal::options.member = std::string(optarg);
				m_present = true;
				break;
			case 'o':
				dbus_tools::send_signal::options.obj_path = std::string(optarg);
				break;
			default:
				dbus_tools::send_signal::print_usage(), exit(1);
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
					dbus_tools::send_signal::print_usage(), exit(1);

			} else
				dbus_tools::send_signal::print_usage(), exit(1);

			dbus_tools::send_signal::options.args.emplace(type, val);
		}
	}

	if(!b_present || !i_present || !m_present)
		dbus_tools::send_signal::print_usage(), exit(1);
}

void dbus_tools::send_signal::print_usage() {
	std::cout << "Usage: send-signal -b bus -i interface -m member [-o obj_path]"
		<< " [type:val]*"
		<< std::endl;
}
