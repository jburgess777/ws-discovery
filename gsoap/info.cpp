#include "gen2/soapH.h"
#include "gen2/soapStub.h"
#include "gen2/DeviceBinding.nsmap"

void getInfo(struct soap* serv, const char *url, const char *user, const char *password)
{
  int res;
  _tds__GetDeviceInformation info;
  _tds__GetDeviceInformationResponse response;

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

void getCaps(struct soap* serv, const char *url, const char *user, const char *password)
{
  int res;
  _tds__GetCapabilities caps;
  _tds__GetCapabilitiesResponse response;
  enum tt__CapabilityCategory temp_category;

  caps.__sizeCategory = 1;
  temp_category = tt__CapabilityCategory__Media;   
  caps.Category = &temp_category;

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

int main(int argc, char *argv[])
{
  if (argc != 4) {
    fprintf(stderr, "%s <URL> username password\n", argv[0]);
    return 1;
  }

  const char *url = argv[1];
  const char *user = argv[2];
  const char *password = argv[3];

  // create soap instance
  struct soap* serv = soap_new(); 
  getInfo(serv, url, user, password);
  getCaps(serv, url, user, password);

  return 0;
}
