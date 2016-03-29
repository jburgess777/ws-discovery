/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** probe.cpp
** 
** WS-Discovery probe/resolve
**
** -------------------------------------------------------------------------*/

#include "soapH.h"
#include "soapStub.h"

#include "wsdd.nsmap"
#include "wsddapi.h"

struct my_state {
  const char *probe_id;
} my_state;

int verbose;

void usage(void)
{
	  std::cout << "Usage: /usr/bin/zmonvif-probe [-v]\n"
	    "    -v        - increase verbosity\n";
}

int main(int argc, char* argv[])
{
	const char * type = 
	  "\"http://www.onvif.org/ver10/network/wsdl\":NetworkVideoTransmitter"
	  " \"http://www.onvif.org/ver10/device/wsdl\":Device";
	const char * scope = NULL;
	std::string url("soap.udp://239.255.255.250:3702");
	

	if ((argc == 2) && !strcmp(argv[1], "-v")) {
	  verbose = 1;
	} else if (argc != 1) {
	  usage();
	  exit(1);
	}

      	int res = 0;

	// create soap instance
	struct soap* serv = soap_new1(SOAP_IO_UDP); 
	if (!soap_valid_socket(soap_bind(serv, NULL, 0, 1000))) {
	  soap_print_fault(serv, stderr);
	  exit(1);
	}	

	const char *id = soap_wsa_rand_uuid(serv);
	serv->user = (void*)&my_state;
	my_state.probe_id = id;

	// call probe
	res = soap_wsdd_Probe(serv,
			      SOAP_WSDD_ADHOC,      // mode
			      SOAP_WSDD_TO_TS,      // to a TS
			      "soap.udp://239.255.255.250:3702",         // address of TS
			      id,                   // message ID
			      NULL, // ReplyTo
			      type,
			      scope,
			      NULL);

	if (res != SOAP_OK) {
	  std::cout << "Got bad return " << res << "\n";
	  soap_print_fault(serv, stderr);
	  exit(1);
	}

	time_t start = time(NULL);
	while (1) {
	  // listen for answers
	  res = soap_wsdd_listen(serv, 1);
	  if (verbose) {
	    if (res != SOAP_OK) {
	      std::cout << "Got bad return " << res << "\n";
	      soap_print_fault(serv, stderr);
	    } else {
	      // soap_wsdd_listen ignores faults returned by clients, check again
	      res = soap_recv_fault(serv, 1);
	      if (res != SOAP_OK) {
		soap_print_fault(serv, stderr);
		printf("%s\n", serv->buf);
	      }
	    }
	  }
	  time_t now = time(NULL);
	  if (now - start > 2)
	    break;
	}

	return 0;
}



template <class T> 
void printMatch(const T & match)
{
	std::cout << "===================================================================" << std::endl;
	if (match.wsa__EndpointReference.Address)
	{
		std::cout << "Endpoint:\t"<< match.wsa__EndpointReference.Address << std::endl;
	}
	if (match.Types)
	{
		std::cout << "Types:\t\t"<< match.Types<< std::endl;
	}
	if (match.Scopes)
	{
		if (match.Scopes->__item )
		{
			std::cout << "Scopes:\t\t"<< match.Scopes->__item << std::endl;
		}
		if (match.Scopes->MatchBy)
		{
			std::cout << "MatchBy:\t"<< match.Scopes->MatchBy << std::endl;
		}
	}
	if (match.XAddrs)
	{
		std::cout << "XAddrs:\t\t"<< match.XAddrs << std::endl;
	}
	std::cout << "MetadataVersion:\t\t" << match.MetadataVersion << std::endl;
	std::cout << "-------------------------------------------------------------------" << std::endl;
}



void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
        if (verbose)
	  printf("wsdd_event_ProbeMatches tid:%s RelatesTo:%s nbMatch:%d\n", MessageID, RelatesTo, matches->__sizeProbeMatch);
        for (int i=0; i < matches->__sizeProbeMatch; ++i)
        {
                wsdd__ProbeMatchType& elt = matches->ProbeMatch[i];
		if (verbose)
		  printMatch(elt);
		
		// http://192.168.1.153:8080/onvif/devices, 1.1, (Profile='Streaming', model='C6F0SeZ0N0P0L0', name='IPCAM', location/country='china')
		if (elt.XAddrs) {
		  std::cout << elt.XAddrs << ", " << PROBE_VERSION << ", (";
		  if (elt.Scopes && elt.Scopes->__item) {
		    // TODO: convert scopes like zmonvif-probe.pl
		    // if($scope =~ m|onvif://www\.onvif\.org/(.+)/(.*)|)
		  }
		  std::cout << ")\n";
		}
        }
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
	printf("wsdd_event_ResolveMatches tid:%s RelatesTo:%s\n", MessageID, RelatesTo);
	printMatch(*match);
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
}

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
	return SOAP_WSDD_ADHOC;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	return SOAP_WSDD_ADHOC;
}
