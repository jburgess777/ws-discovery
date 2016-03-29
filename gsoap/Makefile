GSOAP_PREFIX=/usr
GSOAP_BASE=$(GSOAP_PREFIX)/share/gsoap
GSOAP_IMPORT=$(GSOAP_BASE)/import
GSOAP_PLUGINS=$(GSOAP_BASE)/plugin
GSOAP_CFLAGS=-I$(GSOAP_PREFIX)/include -I$(GSOAP_PLUGINS)
GSOAP_LDFLAGS=-lgsoap++ -L $(GSOAP_PREFIX)/lib/

WSSE_SRC=$(GSOAP_PLUGINS)/wsseapi.c $(GSOAP_PLUGINS)/smdevp.c $(GSOAP_PLUGINS)/mecevp.c $(GSOAP_PLUGINS)/wsaapi.c

PROG:=zmonvif-probe12 zmonvif-probe11 zmonvif-profiles12 zmonvif-info12

PREFIX?=/usr/local

# Disable warnings of variable length arrays in generated code
CXXFLAGS+= -Wno-vla

all: $(PROG)

install: $(PROG)
	install -d $(PREFIX)/bin
	install -s $(PROG) $(PREFIX)/bin
	install zmonvif-probe.pl $(PREFIX)/bin


gen_probe11/.build:
	mkdir -p gen_probe11
	$(GSOAP_PREFIX)/bin/soapcpp2 -1 -Cx $(GSOAP_IMPORT)/wsdd10.h -I $(GSOAP_IMPORT) -d gen_probe11
	touch $@

gen_probe11/soapClient.cpp gen_probe11/soapC.cpp gen_probe11/soapH.h: gen_probe11/.build

zmonvif-probe11: $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen_probe11/soapClient.cpp gen_probe11/soapC.cpp zmonvif-probe.cpp
	c++ -g -o $@ $^ -Igen_probe11 $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) -DPROBE_VERSION="1.1"




gen_probe12/.build:
	mkdir -p gen_probe12
	$(GSOAP_PREFIX)/bin/soapcpp2 -2 -Cx $(GSOAP_IMPORT)/wsdd10.h -I $(GSOAP_IMPORT) -d gen_probe12
	touch $@

gen_probe12/soapClient.cpp gen_probe12/soapC.cpp gen_probe12/soapH.h: gen_probe12/.build

zmonvif-probe12: $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen_probe12/soapClient.cpp gen_probe12/soapC.cpp zmonvif-probe.cpp
	c++ -g -o $@ $^ -Igen_probe12 $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) -DPROBE_VERSION="1.2"




gen_info12/devicemgmt.h: devicemgmt.wsdl onvif.xsd b-2.xsd bf-2.xsd t-1.xsd
	mkdir -p gen_info12
	wsdl2h -d -z6 -s -o $@ $^

gen_info12/.build: gen_info12/devicemgmt.h
	mkdir -p gen_info12
	$(GSOAP_PREFIX)/bin/soapcpp2 -Cx gen_info12/devicemgmt.h -I$(GSOAP_IMPORT) -I$(GSOAP_BASE) -d gen_info12
	touch $@

gen_info12/soapClient.cpp gen_info12/soapC.cpp gen_info12/soapH.h: gen_info12/.build


gen_info12/duration.o: $(GSOAP_BASE)/custom/duration.c gen_info12/soapH.h
	c++ -c -g -o $@ $< -Igen_info12

zmonvif-info12: gen_info12/soapClient.o gen_info12/soapC.o gen_info12/duration.o $(WSSE_SRC) zmonvif-info.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) -I$(GSOAP_PLUGINS) -I$(GSOAP_BASE) -Igen_info12 -DWITH_OPENSSL -fpermissive  -lgsoapssl++ -lssl -lcrypto -lz




gen_profiles12/media.h: media.wsdl onvif.xsd b-2.xsd t-1.xsd bf-2.xsd 
	mkdir -p gen_profiles12
	wsdl2h  -z6 -o$@ -s $^

gen_profiles12/.build: gen_profiles12/media.h
	mkdir -p gen_profiles12
	$(GSOAP_PREFIX)/bin/soapcpp2 -Cx gen_profiles12/media.h -I$(GSOAP_IMPORT) -I$(GSOAP_BASE) -d gen_profiles12
	touch $@

gen_profiles12/soapClient.cpp gen_profiles12/soapC.cpp gen_profiles12/soapH.h: gen_profiles12/.build

gen_profiles12/duration.o: $(GSOAP_BASE)/custom/duration.c gen_profiles12/soapH.h
	c++ -c -g -o $@ $< -Igen_profiles12 

zmonvif-profiles12: gen_profiles12/soapClient.o gen_profiles12/soapC.o gen_info12/duration.o $(WSSE_SRC) zmonvif-profiles.cpp
	c++ -g -o $@ $^ $(GSOAP_LDFLAGS) -I$(GSOAP_PLUGINS) -Igen_profiles12 -DWITH_OPENSSL -fpermissive  -lgsoapssl++ -lssl -lcrypto -lz




clean:
	rm -fR *.o $(PROG)
	rm -fR gen_probe11 gen_probe12 gen_info12 gen_profiles12 

