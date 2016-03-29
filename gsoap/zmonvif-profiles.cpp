#include "soapH.h"
#include "soapStub.h"
#include "MediaBinding.nsmap"

#include "wsseapi.h"

int verbose;

//$ zmonvif-probe.pl profiles http://192.168.1.153:8080/onvif/devices 1.2 admin no_pass
//MainProfileToken, MainProfile, H264, 1280, 720, 25, rtsp://192.168.1.153:554/11
//SubProfileToken, SubProfile, H264, 640, 352, 10, rtsp://192.168.1.153:554/12


void addSecurity(struct soap *soap, const char *user, const char *pass)
{
  if (user) {
    soap_wsse_add_Timestamp(soap, "Time", 600);
    soap_wsse_add_UsernameTokenDigest(soap, "User",user , pass);
  }
}

void getProfileStream(struct soap* serv, const char *url, char *token, const char *user, const char *pass)
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

  addSecurity(serv, user, pass);
  res = soap_call___trt__GetStreamUri(serv, url, NULL, &req, response);
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  }
  tt__MediaUri *mu = response.MediaUri;
  if (mu)
    std::cout << ", " << mu->Uri;
}

void displayProfile(struct soap* serv, const char *url, const tt__Profile *p, const char *user, const char *pass)
{
    std::cout << p->token << ", " << p->Name << ", ";
    
    if (p->VideoEncoderConfiguration) {
      tt__VideoEncoderConfiguration *vec = p->VideoEncoderConfiguration;
      switch(vec->Encoding) { // enum tt__VideoEncoding
	case tt__VideoEncoding__JPEG: 
	  std::cout << "JPEG";
	  break;
	case tt__VideoEncoding__MPEG4: 
	  std::cout << "MPEG4";
	  break;
	case tt__VideoEncoding__H264: 
	  std::cout << "H264";
	  break;
      }
      std::cout << ", ";

      if (vec->Resolution) {
	tt__VideoResolution *res = vec->Resolution;
	std::cout << res->Width << ", " << res->Height;
      }
      if (vec->RateControl) {
	tt__VideoRateControl *rate = vec->RateControl;
	std::cout << ", " << rate->FrameRateLimit;
      }
    }

    getProfileStream(serv, url, p->token, user, pass);

    std::cout << "\n";
}


void getMedia(struct soap* serv, const char *url, const char *user, const char *pass)
{
  int res, i;
  _trt__GetProfiles req;
  _trt__GetProfilesResponse response;

  addSecurity(serv, user, pass);
  res = soap_call___trt__GetProfiles(serv, url, NULL, &req, response);
  if (res != SOAP_OK) {
    soap_print_fault(serv, stderr);
    return;
  } 

  if (verbose)
    std::cout << "Device has " << response.__sizeProfiles << " profiles\n";

  for(i=0; i < response.__sizeProfiles; i++)
    displayProfile(serv, url, response.Profiles[i], user, pass);
}


void usage(char **argv)
{
    fprintf(stderr, "%s [-v] <URL> [username] [password]\n", argv[0]);
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

  struct soap *soap = soap_new();

  getMedia(soap, url, user, pass);

  return 0;
}
