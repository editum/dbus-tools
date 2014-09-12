
SEND_SIGNAL_EXE = send-signal
SEND_SIGNAL_OBJS = 

OM_LIBS = lib/om-lib/libom-ipc.a lib/om-lib/libom-async.a

DBUS_I = $(shell pkg-config --cflags-only-I dbus-1)
DBUS_L = $(shell pkg-config --libs dbus-1)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Ilib/om-lib $(DBUS_I)
LDFLAGS = $(DBUS_L)

all: $(SEND_SIGNAL_EXE)

$(OM_LIBS):
	$(MAKE) -C lib/om-lib

$(SEND_SIGNAL_EXE): src/send_signal_main.o $(SEND_SIGNAL_OBJS) $(OM_LIBS)
	$(CXX) -o $@ $(LDFLAGS) $^

%.o: %.cc %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) src/send_signal_main.o
	$(RM) $(SEND_SIGNAL_OBJS)

spotless: clean
	$(RM) $(SEND_SIGNAL_EXE)

.PHONY: all clean spotless
