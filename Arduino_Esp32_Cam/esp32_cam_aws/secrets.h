#include <pgmspace.h>

#define SECRET
#define THINGNAME "esp32-cam"

const char WIFI_SSID[] = "Khoa";
const char WIFI_PASSWORD[] = "ledangkhoa123";
const char AWS_IOT_ENDPOINT[] = "a2uypdxh8qqj8q-ats.iot.ap-southeast-2.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUDDN1Sda5mBfDAnhLi4ugYTNJURwwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTExNjEyMzQx
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM6iH7g9plmiX2rfv0tP
yaJ37E8HVJmol9sJgmaa6gGaRM4oSVNjTE+aIt2ZMtiyV079Jah1EEiErYi9vGEk
KLBoh0wP0xwOBpr1rGDtiwr4Xi3uev4aIyskoCtoKuzC9+iix5wCtGWiX7FIs8uI
7ZJHZa6xYMyxUqJvp27QhhP0+VKVhh0/07jlV0Z2Y2n1gx6FG847l6zcM4DF9IZ3
e3edboZQonDvwXN6PSQu5JwuLzEHm0G51rGDyR/LLTVxvAAK2wB0gXKOJ/xf8Y/I
pkar5B1LK6WyewdTsXqIby3V+pZmFVHvFesndSIbqcqbi7h9Lnjakj2nCLkr6OFw
xj8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUffinHmZTJaQnIntxxWKjsv0XgYYwHQYD
VR0OBBYEFC/5FbvK+ZvB1iedbdLnUW7c23XHMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCAzrZ3tpmWxXjvA0UoG5KCVXqz
GTl+mPS4bMsbEWOdpKf0ipwDCzhiMB2vCgyme5Hm2KkPyslKJaKs8gGqhha/1cua
KhqLYHGVwAv492u0eaZu6nEh0OGe+WKtn5hTPTaOMJtZrw/nzN9giT3cBGnLnrWL
tSukx2CZpEn6anu696PPJAz7bX7sSNpvGH0qcXTW0U1sHbB6XF8nWNp4CPP4Mw61
K/bXY0ryiQe7gIAiNgMlJH5aJXJYRezkwO7IPw3GleKH6BtXTmHPXde1vEUq6lsK
onTSBHXd9hOb9u2VmwGgyze4WpgQJxuq+yeswNLV8cgwMhkgSKcns2/O8R6M
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAzqIfuD2mWaJfat+/S0/JonfsTwdUmaiX2wmCZprqAZpEzihJ
U2NMT5oi3Zky2LJXTv0lqHUQSIStiL28YSQosGiHTA/THA4GmvWsYO2LCvheLe56
/hojKySgK2gq7ML36KLHnAK0ZaJfsUizy4jtkkdlrrFgzLFSom+nbtCGE/T5UpWG
HT/TuOVXRnZjafWDHoUbzjuXrNwzgMX0hnd7d51uhlCicO/Bc3o9JC7knC4vMQeb
QbnWsYPJH8stNXG8AArbAHSBco4n/F/xj8imRqvkHUsrpbJ7B1OxeohvLdX6lmYV
Ue8V6yd1IhupypuLuH0ueNqSPacIuSvo4XDGPwIDAQABAoIBAGXGxQjilMUhjQ3/
FyHpMs+rceGbIrhsqh9hF7GiLBCOA8XnYfaIulWa/gFYLAtADHEDj0DiKBsP/smT
au4N+Vyqn7l/MeRnpCk/SKCVYB8PJpRJGpt+FERzzdCThlEH10PmqZrWWjSVPbz8
qlJeJ2YddKAPDjbEQIdxp26VlvTMmZzDK+vNgrag+X0edieDDi0ucTJjzw4Ropcr
jQBm1hCWuyRaYkASDarATew5XNz1NfBqpRkaCvHUNODb8arxQn6Q/lZAOdfMJP8/
nRB5S0Zn0PhoaxO4Jc4EadpOEd/IVSh3PPNuC+MZoqj40h/dhLhTBw+bOKGzYmb9
del4AkECgYEA+MPVMZlHWMdvNcsTm2zdub3DU4QYRjyWxDr/EfXX4V5NH5QdqvHH
U9CXTJCO+vxszD11ReGLYGrcP3ZyUi2dUJSwaTHhFvVbYz3XZVg6bNkWT1LrvanB
C08qaRtWTZxdEGhLTTAC9JK4A+RewG2woT9m8n3MWrL2kPV87u12S50CgYEA1KSa
EUWoCz9XrSOkW3NYM5UwcW1PZp0/h3BMuHg67VfVEaJHN348TfBuo9X+X5TTkn2P
PpG9Hg6Tb5vj6BvrJzeueFkGH5+6cGYufDpscSKRZhfkMSvRgvUteJmhECNQhk/C
S/Mjxt0ufhmL+JWTpJLpMX3MGwQXR4cIA5rrGIsCgYEA2/hW6Y/PTgq0JQ4YnWQr
wBWzxYLV8C2dZ5F/gQMxzOAVNGpFvaYkCtI1J4fgVsGk9wkVTSC+nRiA/0Een9oj
9GA2ih6IGkQFdoD1mXDl4OApYSVCvaiAwh5TszNwIhq1FFAU3gEpa9jdDNwAiQm1
mTA2ORDSBhWf/9iDi8bAxuUCgYEAwuKY0Xulp6lGEci4uZfdHml9HHeybblwoBCs
P+TxHNWC0caIvrNTUZspy740hCVCQuAQiKfJS+LP/VpCZlgwDKFbpbDxi8o4dQsx
T2m8OXQVDhyXZvlh60QJiJ5EnyW+h1pSGImeIIRWN/vx7MIprot2bYV9qsT1EPkQ
vz0cekECgYEApblvhjDDApjoajTSRGma1JDlT4/zd3JD3LujkrLFM7oj36wJgdzG
Li52ckQERurITTrcol5VKS1dJwV4n/D5uh0ivyGFN2X69gk5N5CSTj9jF6P4qkzx
B8uZ/qa61Bs6RomokW3zIVm0Ydunx1Mk5A1a7x6LJ5BhzwllRhqaIpg=
-----END RSA PRIVATE KEY-----
)KEY";
