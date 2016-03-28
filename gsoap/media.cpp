#include "gen3/soapH.h"
#include "gen3/soapStub.h"
#include "gen3/MediaBinding.nsmap"

void getProfileStream(struct soap* serv, const char *url, char *token)
{
  int res;

  _trt__GetStreamUri req;
  _trt__GetStreamUriResponse response;
  tt__StreamSetup setup;
  tt__Transport trans;

  setup.Stream = tt__StreamType__RTP_Unicast;
  req.StreamSetup = &setup;

  trans.Protocol = tt__TransportProtocol__RTSP;
  req.StreamSetup->Transport = &trans;

  req.ProfileToken = token;

  res = soap_call___trt__GetStreamUri(serv, url, NULL, &req, response);
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  }
  tt__MediaUri *mu = response.MediaUri;
  if (mu)
    std::cout << " " << mu->Uri;
}

void displayProfile(struct soap* serv, const char *url, const tt__Profile *p)
{
    std::cout << p->Name << ": ";
    
    if (p->VideoEncoderConfiguration) {
      tt__VideoEncoderConfiguration *vec = p->VideoEncoderConfiguration;
      switch(vec->Encoding) { // enum tt__VideoEncoding
	case tt__VideoEncoding__JPEG: 
	  std::cout << " JPEG";
	  break;
	case tt__VideoEncoding__MPEG4: 
	  std::cout << " MPEG4";
	  break;
	case tt__VideoEncoding__H264: 
	  std::cout << " H264";
	  break;
      }
      if (vec->Resolution) {
	tt__VideoResolution *res = vec->Resolution;
	std::cout << " " << res->Width << "*" << res->Height;
      }
      if (vec->RateControl) {
	tt__VideoRateControl *rate = vec->RateControl;
	std::cout << ", " << rate->FrameRateLimit << " fps";
      }
    }

    getProfileStream(serv, url, p->token);

    std::cout << "\n";
}


void getMedia(struct soap* serv, const char *url)
{
  int res, i;
  _trt__GetProfiles req;
  _trt__GetProfilesResponse response;

  //soap_call___trt__GetProfiles(struct soap *soap, const char *soap_endpoint, const char *soap_action, _trt__GetProfiles *trt__GetProfiles, _trt__GetProfilesResponse &trt__GetProfilesResponse);
  res = soap_call___trt__GetProfiles(serv, url, NULL, &req, response);
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  } 
  std::cout << "Device has " << response.__sizeProfiles << " profiles\n";
  for(i=0; i < response.__sizeProfiles; i++)
    displayProfile(serv, url, response.Profiles[i]);
}


int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "%s <URL>\n", argv[0]);
    return 1;
  }

  const char * url = argv[1];

  // create soap instance
  struct soap* serv = soap_new(); 
  getMedia(serv, url);

  return 0;
}
