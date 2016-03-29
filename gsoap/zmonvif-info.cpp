#include "soapH.h"
#include "soapStub.h"
#include "DeviceBinding.nsmap"

#include "wsseapi.h"

int verbose;


void addSecurity(struct soap *soap, const char *user, const char *pass)
{
  if (user) {
    soap_wsse_add_Timestamp(soap, "Time", 600);
    soap_wsse_add_UsernameTokenDigest(soap, "User",user , pass);
  }
}


void getInfo(struct soap* serv, const char *url, const char *user, const char *pass)
{
  int res;
  _tds__GetDeviceInformation info;
  _tds__GetDeviceInformationResponse response;

  addSecurity(serv, user, pass);
  res = soap_call___tds__GetDeviceInformation(
					      serv,
					      url,
					      NULL, // char *action = NULL selects default action for this operation
					      // input parameters:
					      &info,
					      // output parameters:
					      &response
  );
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  } 
  
  std::cout << "   Manufacturer: " << response.Manufacturer << std::endl;
  std::cout << "          Model: " << response.Model << std::endl;
  std::cout << "FirmwareVersion: " << response.FirmwareVersion << std::endl;
  std::cout << "   SerialNumber: " << response.SerialNumber << std::endl;
  std::cout << "     HardwareId: " << response.HardwareId << std::endl;
}

void displayMedia(tt__MediaCapabilities *m)
{
  if (m->StreamingCapabilities) {
    tt__RealTimeStreamingCapabilities *s = m->StreamingCapabilities;
    std::cout << "Streaming media capabilities: ";

    if (s->RTPMulticast && *s->RTPMulticast)
      std::cout << "RTPMulticast ";
    if (s->RTP_USCORETCP && *s->RTP_USCORETCP)
      std::cout << "RTP_TCP ";
    if (s->RTP_USCORERTSP_USCORETCP && *s->RTP_USCORERTSP_USCORETCP)
      std::cout << "RTP_RTSP_TCP ";

    std::cout << "\n";
  }

  if (m->Extension)
    std::cout << "Extension present\n";

}

void getCaps(struct soap* serv, const char *url, const char *user, const char *pass)
{
  int res;
  _tds__GetCapabilities caps;
  _tds__GetCapabilitiesResponse response;
  enum tt__CapabilityCategory temp_category;

  caps.__sizeCategory = 1;
  temp_category = tt__CapabilityCategory__Media;   
  caps.Category = &temp_category;

  addSecurity(serv, user, pass);
  res = soap_call___tds__GetCapabilities(
					 serv,
					 url,
					 NULL, // char *action = NULL selects default action for this operation
					 // input parameters:
					 &caps,
					 // output parameters:
					 &response
					 );
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  } 

  if (response.Capabilities) {
    tt__Capabilities *c = response.Capabilities;
#define DISPLAY(X) \
    if (c->X) \
      std::cout << #X <<  ": " << c->X->XAddr << std::endl

    DISPLAY(Analytics);
    DISPLAY(Device);
    DISPLAY(Events);
    DISPLAY(Imaging);
    DISPLAY(Media);
    DISPLAY(PTZ);
    //DISPLAY(Extension);
    //DISPLAY();
#undef DISPLAY

    if (c->Media)
      displayMedia(c->Media);
  }
}

void usage(char **argv)
{
    fprintf(stderr, "%s <URL> username password\n", argv[0]);
    exit(1);
}

int main(int argc, char *argv[])
{
  const char *url = NULL;
  const char *user = NULL;
  const char *pass = NULL;

  if (argc < 2)
    usage(argv);

  int arg = 1;

  if (!strcmp(argv[arg], "-v")) {
    verbose = 1;
    arg++;
  }

  if (arg < argc)
    url = argv[arg++];
  else
    usage(argv);

  if (arg < argc)
    user = argv[arg++];

  if (arg < argc)
    pass = argv[arg++];

  struct soap* soap = soap_new();
  getInfo(soap, url, user, pass);
  getCaps(soap, url, user, pass);

  return 0;
}
