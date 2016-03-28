GSOAP_PREFIX=/usr
GSOAP_BASE=$(GSOAP_PREFIX)/share/gsoap
GSOAP_IMPORT=$(GSOAP_BASE)/import
GSOAP_PLUGINS=$(GSOAP_BASE)/plugin
GSOAP_CFLAGS= -I gen -I $(GSOAP_PREFIX)/include -I $(GSOAP_PLUGINS)
GSOAP_LDFLAGS= -lgsoap++ -L $(GSOAP_PREFIX)/lib/

all: gen/soapC.cpp probe.exe server.exe proxy.exe info.exe media.exe

gen/soapClient.cpp gen/soapC.cpp gen/soapH.h:
	mkdir -p gen
	$(GSOAP_PREFIX)/bin/soapcpp2 -Cx $(GSOAP_IMPORT)/wsdd10.h -I $(GSOAP_IMPORT) -d gen

probe.exe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen/soapClient.cpp gen/soapC.cpp probe.cpp 
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 

server.exe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen/soapClient.cpp gen/soapC.cpp server.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 

proxy.exe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen/soapClient.cpp gen/soapC.cpp proxy.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 


gen2/devicemgmt.h: devicemgmt.wsdl onvif.xsd b-2.xsd bf-2.xsd t-1.xsd
	mkdir -p gen2
	wsdl2h -d -Ntev -z6 -s -o $@ b-2.xsd bf-2.xsd t-1.xsd onvif.xsd devicemgmt.wsdl 

gen2/soapClient.cpp gen2/soapC.cpp gen2/soapH.h: gen2/devicemgmt.h
	mkdir -p gen2
	$(GSOAP_PREFIX)/bin/soapcpp2 -Cx gen2/devicemgmt.h -I$(GSOAP_IMPORT) -I$(GSOAP_BASE) -d gen2

gen2/duration.o: $(GSOAP_BASE)/custom/duration.c 
	c++ -c -g -o $@ $^ -Igen2 

gen2/wsseapi.o: $(GSOAP_PLUGINS)/wsseapi.c
	c++ -c -g -o $@ $^ -Igen2

CXXFLAGS:= -Igen2

info.exe: gen2/soapClient.o gen2/soapC.o gen2/duration.o info.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) -I$(GSOAP_BASE) -I gen2



gen3/media.h: media.wsdl onvif.xsd b-2.xsd t-1.xsd bf-2.xsd 
	mkdir -p gen3
	wsdl2h -o$@ -s media.wsdl onvif.xsd b-2.xsd t-1.xsd bf-2.xsd 

gen3/soapClient.cpp gen3/soapC.cpp gen3/soapH.h: gen3/media.h
	mkdir -p gen3
	$(GSOAP_PREFIX)/bin/soapcpp2 -Cx gen3/media.h -I$(GSOAP_IMPORT) -I$(GSOAP_BASE) -d gen3

gen3/duration.o: $(GSOAP_BASE)/custom/duration.c 
	c++ -c -g -o $@ $^ -I gen3 

media.exe: gen3/soapClient.o gen3/soapC.o gen2/duration.o media.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) -I gen3

clean:
	rm -rf *.exe gen gen2 gen3 *.o


